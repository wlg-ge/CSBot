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

// IRCOutputThread.h: interface for the IRCOutputThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IRCOUTPUTTHREAD_H__DF99C4EF_27B5_436B_80FC_4255A049A3B3__INCLUDED_)
#define AFX_IRCOUTPUTTHREAD_H__DF99C4EF_27B5_436B_80FC_4255A049A3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>

// Struct that represents one IRC output line (not only chat)
struct IRCOutputMsg
{
	// The raw IRC command
	std::string sRawCmd;
	// The time when the message was created.
	CTime		tOrigin;
};

// define a new type: a linked list of pointers to IRC output messages
typedef std::list<IRCOutputMsg*> OUTPUTMSGLIST;
class CMainFrame;

// The IRC output thread object. requires locking the queue, so it derives from 
// my generic Lockable object (mutex)
class IRCOutputThread : public CIGCLockable  
{
public:
	// Constructor / Destructor
	IRCOutputThread(CMainFrame* pMainFrame);
	virtual ~IRCOutputThread();

private:
	// For internal use only:

	// A list of output messages, used as a queue of delayable messages
	OUTPUTMSGLIST	m_queueDelayed;
	// A list of output messages, used as a queue of more important no-delay messages
	OUTPUTMSGLIST	m_queueImmediate;

	// Flag to stop the thread
	bool			m_bStop;
	// Our thread's handle
	HANDLE			m_hThread;
	// Pointer to the main frame, where some important variables are kept
	CMainFrame*		m_pMainFrame;

	// Clears all queues
	void			ClearQueue();
	// Static function for the thread, that serves as callback from the OS
	static DWORD WINAPI IRCOutputThreadFunc(LPVOID lpThis);
	// Main work-thread method
	DWORD			Work();
	// Retrieves the next message to process, according to priorities and delays
	IRCOutputMsg*	GetNextMsg();
	// Message handler
	void			HandleOutputMsg(IRCOutputMsg* pMsg);

public:
	// For outside use:

	// Place a new message in the delayable queue
	bool QueueOutputMsg(const char* pRawCmd, CTime tOrigin = CTime::GetCurrentTime());

	// Place a new message in the no-delay queue
	bool QueueOutputMsgImmediate(const char* pRawCmd);

	// Calculate approximate delay of the next message
	CTimeSpan GetTotalDelay();

	// Calculate total amount of queued messages (on both queues)
	int GetTotalQueuedMsgs();
};

#endif // !defined(AFX_IRCOUTPUTTHREAD_H__DF99C4EF_27B5_436B_80FC_4255A049A3B3__INCLUDED_)
