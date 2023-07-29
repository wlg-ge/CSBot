/*
	This file is adopted from Adi Degani's IRC library
	(c) Adi Degani
	For more details, see: http://www.codeguru.com/network/irc.html

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
// irc.cpp

#include "stdafx.h"
#include "irc.h"
#include "Settings.h"

using namespace irc;

////////////////////////////////////////////////////////////////////

CIrcMessage::CIrcMessage(const char* lpszCmdLine, bool bIncoming)
	: m_bIncoming(bIncoming)
{
	sOriginalMessage = lpszCmdLine;
	ParseIrcCommand(lpszCmdLine);
	m_pOrigin = NULL;
}

CIrcMessage::CIrcMessage(const CIrcMessage& m)
	:	sCommand(m.sCommand),
		sOriginalMessage(m.sOriginalMessage),
		parameters(m.parameters),
		m_bIncoming(m.m_bIncoming)
{
	prefix.sNick = m.prefix.sNick;
	prefix.sUser = m.prefix.sUser;
	prefix.sHost = m.prefix.sHost;
	m_pOrigin = m.m_pOrigin;
}

void CIrcMessage::Reset()
{
	prefix.sNick = prefix.sUser = prefix.sHost = sCommand = "";
	m_bIncoming = false;
	parameters.clear();
}

CIrcMessage& CIrcMessage::operator = (const CIrcMessage& m)
{
	if( &m != this )
	{
		sCommand = m.sCommand;
		parameters = m.parameters;
		prefix.sNick = m.prefix.sNick;
		prefix.sUser = m.prefix.sUser;
		prefix.sHost = m.prefix.sHost;
		m_bIncoming = m.m_bIncoming;
	}
	return *this;
}

CIrcMessage& CIrcMessage::operator = (const char* lpszCmdLine)
{
	Reset();
	ParseIrcCommand(lpszCmdLine);
	return *this;
}

void CIrcMessage::ParseIrcCommand(const char* lpszCmdLine)
{
	const char* p1 = lpszCmdLine;
	const char* p2 = lpszCmdLine;

	ASSERT(lpszCmdLine != NULL);
	ASSERT(*lpszCmdLine);

	// prefix exists ?
	if( *p1 == ':' )
	{ // break prefix into its components (nick!user@host)
		p2 = ++p1;
		while( *p2 && !strchr(" !", *p2) )
			++p2;
		prefix.sNick.assign(p1, p2 - p1);
		if( *p2 != '!' )
			goto end_of_prefix;
		p1 = ++p2;
		while( *p2 && !strchr(" @", *p2) )
			++p2;
		prefix.sUser.assign(p1, p2 - p1);
		if( *p2 != '@' )
			goto end_of_prefix;
		p1 = ++p2;
		while( *p2 && !isspace(*p2) )
			++p2;
		prefix.sHost.assign(p1, p2 - p1);
end_of_prefix :
		while( *p2 && isspace(*p2) )
			++p2;
		p1 = p2;
	}

	// get command
	ASSERT(*p1 != '\0');
	p2 = p1;
	while( *p2 && !isspace(*p2) )
		++p2;
	sCommand.assign(p1, p2 - p1);
	_strupr((char*)sCommand.c_str());
	while( *p2 && isspace(*p2) )
		++p2;
	p1 = p2;

	// get parameters
	while( *p1 )
	{
		if( *p1 == ':' )
		{
			++p1;
			// seek end-of-message
			while( *p2 )
				++p2;
			parameters.push_back(String(p1, p2 - p1));
			break;
		}
		else
		{
			// seek end of parameter
			while( *p2 && !isspace(*p2) )
				++p2;
			parameters.push_back(String(p1, p2 - p1));
			// see next parameter
			while( *p2 && isspace(*p2) )
				++p2;
			p1 = p2;
		}
	} // end parameters loop
}

String CIrcMessage::AsString() const
{
	String s;

	if( prefix.sNick.length() )
	{
		s += ":" + prefix.sNick;
		if( prefix.sUser.length() && prefix.sHost.length() )
			s += "!" + prefix.sUser + "@" + prefix.sHost;
		s += " ";
	}

	s += sCommand;

	for(int i=0; i < (int)parameters.size(); i++)
	{
		s += " ";
		if( i == (int)parameters.size() - 1 ) // is last parameter ?
			s += ":";
		s += parameters[i];
	}

	s += endl;

	return s;
}

////////////////////////////////////////////////////////////////////

CIrcSession::CIrcSession(IIrcSessionMonitor* pMonitor)
	:	m_hThread(NULL)
{
	m_nPingCount = 0;
	m_bDCC = false;
	InitializeCriticalSection(&m_cs);
}

CIrcSession::~CIrcSession()
{
	Disconnect();
	DeleteCriticalSection(&m_cs);
}

bool CIrcSession::Connect(const CIrcSessionInfo& info)
{
	m_nPingCount = 0;
	ASSERT(m_hThread==NULL && !m_socket.IsValid());

	try
	{
		if( !m_socket.Create() )
			throw "Failed to create socket!";

		InetAddr addr(info.sServer.c_str(), info.iPort);
		if( !m_socket.Connect(addr) )
		{
			m_socket.Close();
			throw "Failed to connect to host!";
		}

		m_info = info;

		// start receiving messages from host
		DWORD dwID = GetCurrentThreadId();
		m_hThread = ::CreateThread(NULL, 0, &IRCThreadProc, (LPVOID)this, 0, &dwID);
		if (m_hThread == NULL) {
			int nErr = ::GetLastError();
			m_socket.Close();
			throw "Failed to create thread!";
		}

		Sleep(500);
	}
	catch( const char* )
	{
		Disconnect();
	}
	catch( ... )
	{
		Disconnect();
	}

	return m_socket.IsValid();
}

void CIrcSession::Disconnect(const char* lpszMessage)
{
	static const DWORD dwServerTimeout = 5 * 1000;

	if( !m_hThread )
		return;

	if (m_bDCC) {
		m_socket.Send("%s\n", lpszMessage ? lpszMessage : "Bye!");
		m_socket.Close();
	} else {
		m_socket.Send("QUIT :%s\n", lpszMessage ? lpszMessage : "Bye!");
	}

	if( m_hThread && WaitForSingleObject(m_hThread, dwServerTimeout) != WAIT_OBJECT_0 )
	{
		m_socket.Close();
		Sleep(100);
		if( m_hThread && WaitForSingleObject(m_hThread, dwServerTimeout) != WAIT_OBJECT_0 )
		{
			TerminateThread(m_hThread, 1);
			CloseHandle(m_hThread);
			m_hThread = NULL;
			m_info.Reset();
		}
	}
}

void CIrcSession::Notify(const CIrcMessage* pmsg)
{
	// forward message to monitor objects
	EnterCriticalSection(&m_cs);
	for(std::set<IIrcSessionMonitor*>::iterator it = m_monitors.begin();
			it != m_monitors.end();
			it++
			)
	{
		(*it)->OnIrcMessage(pmsg);
	}
	LeaveCriticalSection(&m_cs);
}

void CIrcSession::DoReceive()
{
	CIrcIdentServer m_identServer;
	char chBuf[1024*4+1];
	int cbInBuf = 0;

	if( m_info.bIdentServer )
		m_identServer.Start(m_info.sUserID.c_str(), m_info.bCloseIdentAfterQuery, m_info.pIdentMonitor);

	while( m_socket.IsValid() )
	{
		int cbRead;
		int nLinesProcessed = 0;

		cbRead = m_socket.Receive((unsigned char*)chBuf+cbInBuf, sizeof(chBuf)-cbInBuf-1);
		if( cbRead <= 0 )
			break;
		cbInBuf += cbRead;
		chBuf[cbInBuf] = '\0';

		char* pStart = chBuf;
		while( *pStart )
		{
			char* pEnd;

			// seek end-of-line
			for(pEnd=pStart; *pEnd && *pEnd != '\r' && *pEnd != '\n'; ++pEnd)
				;
			if( *pEnd == '\0' )
				break; // uncomplete message. stop parsing.

			++nLinesProcessed;

			// replace end-of-line with NULLs and skip
			while( *pEnd == '\r' || *pEnd == '\n' )
				*pEnd++ = '\0';

			if( *pStart )
			{
				if (m_bDCC) {
#if _TRACES >= 8
					TRACE("DCC: %s\n", pStart);
#endif
					// process single message by monitor objects
					std::string sMsg = "DCCMSG ";
					sMsg += pStart;
					CIrcMessage msg(sMsg.c_str(), true);
					msg.m_pOrigin = this;
					Notify(&msg);
				} else {
					// process single message by monitor objects
#if _TRACES >= 8
					TRACE("IRC: %s\n", pStart);
#endif
					CIrcMessage msg(pStart, true);
					Notify(&msg);
				}
			}

			cbInBuf -= pEnd - pStart;
			ASSERT(cbInBuf >= 0);

			pStart = pEnd;
		}

		// discard processed messages
		if( nLinesProcessed != 0 )
			memmove(chBuf, pStart, cbInBuf+1);
	}

	if( m_socket.IsValid() )
		m_socket.Close();

	if( m_info.bIdentServer )
		m_identServer.Stop();

	// notify monitor objects that the connection has been closed
	Notify(NULL);
}

DWORD WINAPI CIrcSession::IRCThreadProc(LPVOID pparam)
{
	CIrcSession* pThis = (CIrcSession*)pparam;
	try { pThis->DoReceive(); } catch( ... ) {}
	pThis->m_info.Reset();
	CloseHandle(pThis->m_hThread);
	pThis->m_hThread = NULL;
	return 0;
}

void CIrcSession::AddMonitor(IIrcSessionMonitor* pMonitor)
{
	ASSERT(pMonitor != NULL);
	EnterCriticalSection(&m_cs);
	m_monitors.insert(pMonitor);
	LeaveCriticalSection(&m_cs);
}

void CIrcSession::RemoveMonitor(IIrcSessionMonitor* pMonitor)
{
	ASSERT(pMonitor != NULL);
	EnterCriticalSection(&m_cs);
	m_monitors.erase(pMonitor);
	LeaveCriticalSection(&m_cs);
}

////////////////////////////////////////////////////////////////////

CIrcSessionInfo::CIrcSessionInfo()
{
	Reset();
}

CIrcSessionInfo::CIrcSessionInfo(const CIrcSessionInfo& si)
	:	sServer(si.sServer),
		sServerName(si.sServerName),
		iPort(si.iPort),
		sNick(si.sNick),
		sUserID(si.sUserID),
		sFullName(si.sFullName),
		sPassword(si.sPassword),
		bIdentServer(si.bIdentServer),
		bCloseIdentAfterQuery(si.bCloseIdentAfterQuery),
		pIdentMonitor(si.pIdentMonitor)
{
}

void CIrcSessionInfo::Reset()
{
	sServer = "";
	sServerName = "";
	iPort = 0;
	sNick = "";
	sUserID = "";
	sFullName = "";
	sPassword = "";
	bIdentServer = false;
	bCloseIdentAfterQuery = false;
	pIdentMonitor = NULL;
}

////////////////////////////////////////////////////////////////////

CIrcIdentServer::CIrcIdentServer()
	: m_uiPort(0), m_hThread(NULL)
{
}

CIrcIdentServer::~CIrcIdentServer()
{
	Stop();
}

bool CIrcIdentServer::Start(
				const char* lpszUserID,
				bool bCloseAfterQuery,
				CIrcIdentServerMonitor* pMonitor,
				unsigned int uiPort,
				const char* lpszResponseType
				)
{
	if( m_socket.IsValid() )
		return false;

	if( !m_socket.Create() )
		return false;

	if( !m_socket.Bind(InetAddr(uiPort)) )
	{
		m_socket.Close();
		return false;
	}

	m_sResponseType = lpszResponseType;
	m_sUserID = lpszUserID;
	m_uiPort = uiPort;
	m_bCloseAfterQuery = bCloseAfterQuery;
	m_pMonitor = pMonitor;

	m_hThread = CreateThread(NULL, 0, ListenProc, this, 0, NULL);
	Sleep(100);

	return true;
}

void CIrcIdentServer::Stop()
{
	if( m_hThread )
	{
		m_socket.Close();
		if( WaitForSingleObject(m_hThread, 5000) != WAIT_OBJECT_0 && m_hThread )
		{
			TerminateThread(m_hThread, 1);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
}

void CIrcIdentServer::DoThread()
{
	m_socket.Listen();

	while( m_socket.IsValid() )
	{
		InetAddr addr;

		Socket s = m_socket.Accept(addr);
		if( !s.IsValid() )
			break;

		char szBuf[1024];
		int cbRead = s.Receive((unsigned char*)szBuf, sizeof(szBuf)-1);
		if( cbRead <= 0 )
			continue;
		szBuf[cbRead] = '\0';

		// strip CRLF from query
		for(char* p = szBuf; *p && *p != '\r' && *p != '\n'; ++p)
			;
		*p = '\0';

		CString sSend;
		sSend.Format("%s : USERID : %s : %s", szBuf, m_sResponseType.c_str(), m_sUserID.c_str());
		s.Send("%s\r\n", (LPCTSTR)sSend);
		if (m_pMonitor != NULL)
			m_pMonitor->OnIdentResponded(sSend);
		Sleep(500);
		s.Close();
		if (m_bCloseAfterQuery)
			break;
	}

	m_socket.Close();
}

DWORD WINAPI CIrcIdentServer::ListenProc(LPVOID pparam)
{
	CIrcIdentServer* pThis = (CIrcIdentServer*)pparam;

	try { pThis->DoThread(); } catch( ... ) {}

	pThis->m_sResponseType = "";
	pThis->m_sUserID = "";
	pThis->m_uiPort = 0;

	CloseHandle(pThis->m_hThread);
	pThis->m_hThread = NULL;

	return 0;
}

////////////////////////////////////////////////////////////////////

CIrcMonitor::HandlersMap CIrcMonitor::m_handlers;
CIrcMonitor::IrcCommandsMapsListEntry CIrcMonitor::m_handlersMapsListEntry
	= { &CIrcMonitor::m_handlers, NULL };


CIrcMonitor::CIrcMonitor(CIrcSession& session)
	: m_session(session)
{
	m_xPost.SetMonitor(this);
}

CIrcMonitor::~CIrcMonitor()
{
}

void CIrcMonitor::OnIrcMessage(const CIrcMessage* pmsg)
{
	CIrcMessage* pMsgCopy = NULL;
	if( pmsg )
		pMsgCopy = new CIrcMessage(*pmsg);
	m_xPost.Post(0, (LPARAM)pMsgCopy);
}

void CIrcMonitor::OnCrossThreadsMessage(WPARAM wParam, LPARAM lParam)
{
	CIrcMessage* pmsg = (CIrcMessage*)lParam;

	OnIrcAll(pmsg);

	if( pmsg )
	{
		if (pmsg->sOriginalMessage.empty())
		{
			OnIrcDisconnected(pmsg);
		} else {
			PfnIrcMessageHandler pfn = FindMethod(pmsg->sCommand.c_str());
			if( pfn )
			{
				// call member function. if it returns 'false',
				// call the default handling
				if( !(this->*pfn)(pmsg) )
					OnIrcDefault(pmsg);
			}
			else // handler not found. call default handler
				OnIrcDefault(pmsg);
		}
		delete pmsg;
	}
}

CIrcMonitor::PfnIrcMessageHandler CIrcMonitor::FindMethod(const char* lpszName)
{
	// call the recursive version with the most derived map
	return FindMethod(GetIrcCommandsMap(), lpszName);
}

CIrcMonitor::PfnIrcMessageHandler CIrcMonitor::FindMethod(IrcCommandsMapsListEntry* pMapsList, const char* lpszName)
{
	HandlersMap::iterator it = pMapsList->pHandlersMap->find(lpszName);
	if( it != pMapsList->pHandlersMap->end() )
		return it->second; // found !
	else if( pMapsList->pBaseHandlersMap )
		return FindMethod(pMapsList->pBaseHandlersMap, lpszName); // try at base class
	return NULL; // not found in any map
}

////////////////////////////////////////////////////////////////////

DECLARE_IRC_MAP(CIrcDefaultMonitor, CIrcMonitor)

CIrcDefaultMonitor::CIrcDefaultMonitor(CIrcSession& session)
	: CIrcMonitor(session)
{
	IRC_MAP_ENTRY(CIrcDefaultMonitor, "NICK", OnIrc_NICK)
	IRC_MAP_ENTRY(CIrcDefaultMonitor, "PING", OnIrc_PING)
	IRC_MAP_ENTRY(CIrcDefaultMonitor, "002", OnIrc_YOURHOST)
	IRC_MAP_ENTRY(CIrcDefaultMonitor, "005", OnIrc_BOUNCE)
}

bool CIrcDefaultMonitor::OnIrc_NICK(const CIrcMessage* pmsg)
{
	if( (m_session.GetInfo().sNick == pmsg->prefix.sNick) && (pmsg->parameters.size() > 0) )
		m_session.m_info.sNick = pmsg->parameters[0];
	return false;
}

bool CIrcDefaultMonitor::OnIrc_PING(const CIrcMessage* pmsg)
{
	char szResponse[100];
	sprintf(szResponse, "PONG %s", pmsg->parameters[0].c_str());
	m_session << szResponse;
	m_session.m_nPingCount++;
	if (m_session.m_nPingCount == 1) {
		OnFirstPing(pmsg->parameters[0].c_str());
	}
	return false;
}

bool CIrcDefaultMonitor::OnIrc_YOURHOST(const CIrcMessage* pmsg)
{
	static const char* lpszFmt = "Your host is %[^ \x5b,], running version %s";
	char szHostName[100], szVersion[100];
	if( sscanf(pmsg->parameters[1].c_str(), lpszFmt, &szHostName, &szVersion) > 0 )
		m_session.m_info.sServerName = szHostName;
	return false;
}

bool CIrcDefaultMonitor::OnIrc_BOUNCE(const CIrcMessage* pmsg)
{
	static const char* lpszFmt = "Try server %[^ ,], port %d";
	char szAltServer[100];
	int iAltPort = 0;
	if( sscanf(pmsg->parameters[1].c_str(), lpszFmt, &szAltServer, &iAltPort) == 2 )
	{
	}
	return false;
}
