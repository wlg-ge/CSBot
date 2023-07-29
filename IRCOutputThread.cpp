/*
	CSBot - An IRC bot for publishing CS and NS games
	Copyright (C) 2001, 2002, 2003, 2004 Ilan Tayary ([NoCt]Yonatanz)
	Email: yonatanz@unitedadmins.com
	Website: http://www.csbot.org

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// IRCOutputThread.cpp: implementation of the IRCOutputThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IGCLockable.h"
#include "IRCOutputThread.h"
#include "Settings.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRCOutputThread::IRCOutputThread(CMainFrame* pMainFrame) : CIGCLockable("IRCOutputThread")
{
	// initialize members
	m_pMainFrame = pMainFrame;
	m_bStop = false;
	// start the work thread
	DWORD dwThreadId = ::GetCurrentThreadId();
	m_hThread = CreateThread(NULL, 0, &IRCOutputThreadFunc, (LPVOID)this, 0, &dwThreadId);
}

IRCOutputThread::~IRCOutputThread()
{
	// stop the work thread
	m_bStop = true;
	DWORD dwCode;
	do {
		if (!::GetExitCodeThread(m_hThread, &dwCode))
			break;
	} while (dwCode == STILL_ACTIVE);
	// get rid of handle
	::CloseHandle(m_hThread);
	m_hThread = NULL;
	// and clear all left queue items
	ClearQueue();
}

void IRCOutputThread::ClearQueue()
{
	// acquire lock on the queue
	IGCLock lock(this, "AddOutputMsg");
	// delete all items in the delayed queue
	for (OUTPUTMSGLIST::iterator i = m_queueDelayed.begin(); i != m_queueDelayed.end(); i++)
		delete (*i);
	m_queueDelayed.clear();
	// delete all items in the immediate queue
	for (i = m_queueImmediate.begin(); i != m_queueImmediate.end(); i++)
		delete (*i);
	m_queueImmediate.clear();
}

bool IRCOutputThread::QueueOutputMsg(const char* pRawCmd, CTime tOrigin /* = CTime::GetCurrentTime() */)
{
	// make a new irc output message struct and fill it
	IRCOutputMsg* pMsg = new IRCOutputMsg;
	pMsg->sRawCmd = pRawCmd;
	pMsg->tOrigin = tOrigin;

	// acquire lock on the queue
	IGCLock lock(this, "QueueOutputMsg");
	// do not place in queue, if there are already 100 messages there
	if (m_queueDelayed.size() >= 100)
	{
		delete pMsg;
		return false;
	}
	// place in queue for delayed output
	m_queueDelayed.push_back(pMsg);
	return true;
}

bool IRCOutputThread::QueueOutputMsgImmediate(const char* pRawCmd)
{
	// make a new irc output message struct and fill it
	IRCOutputMsg* pMsg = new IRCOutputMsg;
	pMsg->sRawCmd = pRawCmd;
	pMsg->tOrigin = 0;

	// acquire lock on the queue
	IGCLock lock(this, "QueueOutputMsgImmediate");
	// do not place in queue, if there are already 100 messages there
	if (m_queueImmediate.size() >= 100)
	{
		delete pMsg;
		return false;
	}
	// place in queue for immediate output
	m_queueImmediate.push_back(pMsg);
	return true;
}

CTimeSpan IRCOutputThread::GetTotalDelay()
{
	// acquire lock on the queue
	IGCLock lock(this, "GetTotalDelay");
	// empty queue means no delay
	if (m_queueDelayed.empty())
		return CTimeSpan(0,0,0,0);
	// get the next message that is about to be outputted (oldest one)
	IRCOutputMsg* pMsg = m_queueDelayed.front();
	// and get its timespan value
	return CTime::GetCurrentTime() - pMsg->tOrigin;
}

int IRCOutputThread::GetTotalQueuedMsgs()
{
	// acquire lock on the queue
	IGCLock lock(this, "GetTotalDelay");
	// calculate total = delayed queue size + immediate queue size
	return m_queueDelayed.size() + m_queueImmediate.size();
}

IRCOutputMsg* IRCOutputThread::GetNextMsg()
{
	// acquire lock on the queue
	IGCLock lock(this, "GetNextMsg");
	IRCOutputMsg* pMsg = NULL;
	// immediate queue ALWAYS has higher priority than delayed queue
	// only check the delayed queue if the immediate is empty
	if (m_queueImmediate.empty())
	{
		// nothing in both queued - quit
		if (m_queueDelayed.empty())
			return NULL;
		// get the next message in queue
		pMsg = m_queueDelayed.front();
		// make sure we delayed enough. if not - play as if the queue is still empty
		if ((CTime::GetCurrentTime() - pMsg->tOrigin).GetTotalSeconds() < settings.GetIRCDelay())
			return NULL;
		// remove it from the queue and return it
		m_queueDelayed.pop_front();
		return pMsg;
	}

	// take next message from the immediate queue
	pMsg = m_queueImmediate.front();
	// remove it from the queue and return it
	m_queueImmediate.pop_front();
	return pMsg;
}

DWORD WINAPI IRCOutputThread::IRCOutputThreadFunc(LPVOID lpThis)
{
	// thread's static function.
	// get out object's pointer and call its Work() method
	IRCOutputThread* pThis = (IRCOutputThread*)lpThis;
	return pThis->Work();
}

DWORD IRCOutputThread::Work()
{
	IRCOutputMsg* pMsg;
	// loop until we are told to stop.
	while (!m_bStop)
	{
		// get us a message to work on.
		pMsg = GetNextMsg();
		// no such message? wait a bit and try again
		if (pMsg == NULL)
		{
			Sleep(250);
			continue;
		}
		// handle the next message
		HandleOutputMsg(pMsg);
	}
	return 0;
}

void IRCOutputThread::HandleOutputMsg(IRCOutputMsg* pMsg)
{
	// only do stuff if we are connected to IRC, ofcourse.
	// otherwise queue will empty itself at once, since there's no speed limit.
	if (m_pMainFrame->IsConnectedIRC())
	{
		// if it's a channel message, only output it if we're in the channel
		if ((pMsg->tOrigin == 0) || m_pMainFrame->IsJoinedIRC()) {
			// sleep until the speed limiter allows us to continue
			settings.m_limiter.BeforeBuf();
			// send this message
			g_ircSession << pMsg->sRawCmd.c_str();
			// inform speed limiter that we are done sending
			settings.m_limiter.AfterBuf();
		}
	}
	// delete the struct. it is not needed anymore
	delete pMsg;
}
