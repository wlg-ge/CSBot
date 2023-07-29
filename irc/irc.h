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
// irc.h

#ifndef _IRC_H_
#define	_IRC_H_

/*
	IRC (RFC #1459) Client Implementation
*/

#pragma warning (disable: 4786)

#include "socket.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include "CrossThreadsMessagingDevice.h"

////////////////////////////////////////////////////////////////////
namespace irc {
////////////////////////////////////////////////////////////////////

typedef std::string String;

static const char* endl = "\r\n";

////////////////////////////////////////////////////////////////////
class CIrcSession;
class CIrcIdentServerMonitor;

class CIrcMessage
{
public :
	struct Prefix
	{
		String sNick, sUser, sHost;
	} prefix;
	String sOriginalMessage;
	String sCommand;
	std::vector<String> parameters;
	bool m_bIncoming;
	CIrcSession* m_pOrigin;

	CIrcMessage() : m_bIncoming(false), m_pOrigin(NULL) {} // default constructor
	CIrcMessage(const char* lpszCmdLine, bool bIncoming=false); // parser constructor
	CIrcMessage(const CIrcMessage& m); // copy constructor

	void Reset();

	CIrcMessage& operator = (const CIrcMessage& m);
	CIrcMessage& operator = (const char* lpszCmdLine);

	String AsString() const;

private :
	void ParseIrcCommand(const char* lpszCmdLine);
};

////////////////////////////////////////////////////////////////////

struct IIrcSessionMonitor
{
	virtual void OnIrcMessage(const CIrcMessage* pmsg) = 0;
};

////////////////////////////////////////////////////////////////////

struct CIrcSessionInfo
{
	String sServer;
	String sServerName;
	unsigned int iPort;
	String sNick;
	String sUserID;
	String sFullName;
	String sPassword;

	bool bIdentServer;
	bool bCloseIdentAfterQuery;
	CIrcIdentServerMonitor* pIdentMonitor;

	CIrcSessionInfo();
	CIrcSessionInfo(const CIrcSessionInfo& si);

	void Reset();
};

////////////////////////////////////////////////////////////////////

class CIrcDefaultMonitor; // foreward

class CIrcSession
{
public :
	friend class CIrcDefaultMonitor;

	CIrcSession(IIrcSessionMonitor* pMonitor = NULL);
	virtual ~CIrcSession();

	void AddMonitor(IIrcSessionMonitor* pMonitor);
	void RemoveMonitor(IIrcSessionMonitor* pMonitor);
		
	bool Connect(const CIrcSessionInfo& info);
	void Disconnect(const char* lpszMessage = "Bye!");

	const CIrcSessionInfo& GetInfo() const
				{ return (const CIrcSessionInfo&)m_info; }

	operator bool() const { return m_socket.IsValid(); }

	// send-to-stream operators
	friend CIrcSession& operator << (CIrcSession& os, const char* pMsg);

	int m_nPingCount;
	bool m_bDCC;
	std::string m_sNick;

protected :
	Socket m_socket;
	CIrcSessionInfo m_info;

	void DoReceive();

private :
	std::set<IIrcSessionMonitor*> m_monitors;
	HANDLE m_hThread;
	CRITICAL_SECTION m_cs; // protect m_monitors

	void Notify(const CIrcMessage* pmsg);
	static DWORD WINAPI IRCThreadProc(LPVOID pparam);
};


__inline CIrcSession& operator << (CIrcSession& os, const char* pMsg)
{
	if( os )
	{
		std::string s = pMsg;
		if (os.m_bDCC) {
			// CAN ONLY SEND PRIVMSGs in DCC
			ASSERT(strncmp(pMsg, "PRIVMSG ", 8) == 0);
			s.erase(0, 8);
			if (s[0] == ' ')
				s.erase(0, 1);
			if (s[0] == ':')
				s.erase(0, 1);
		}
		if (s[s.size()-1] != '\n')
			s += "\n";
		CIrcMessage msg(s.c_str(), false);
		os.m_socket.Send(msg.AsString().c_str());
		os.Notify(&msg);
	}
	return os;
}

////////////////////////////////////////////////////////////////////

// RFC's Identity Server (RFC #1413)
class CIrcIdentServerMonitor
{
public:
	virtual void OnIdentResponded(LPCTSTR pResponse) = 0;
};

class CIrcIdentServer
{
public :
	CIrcIdentServer();
	virtual ~CIrcIdentServer();

	bool Start(
			const char* lpszUserID,
			bool bCloseAfterQuery,
			CIrcIdentServerMonitor* pMonitor,
			unsigned int uiPort = 113,
			const char* lpszResponseType = "UNIX"
			);
	void Stop();

protected :
	String m_sResponseType;
	unsigned int m_uiPort;
	String m_sUserID;
	bool m_bCloseAfterQuery;
	CIrcIdentServerMonitor* m_pMonitor;

	void DoThread();

private :
	Socket m_socket;
	HANDLE m_hThread;

	static DWORD WINAPI ListenProc(LPVOID pparam);
};

////////////////////////////////////////////////////////////////////

class CIrcMonitor :
	public IIrcSessionMonitor,
	private CCrossThreadsMessagingDevice::ICrossThreadsMessagingDeviceMonitor
{
public :
	typedef bool (CIrcMonitor::*PfnIrcMessageHandler)(const CIrcMessage* pmsg);
	struct LessString
	{
		bool operator()(const char* s1, const char* s2) const
			{ return stricmp(s1, s2) < 0; }
	};
	typedef std::map<const char*, PfnIrcMessageHandler, LessString> HandlersMap;
	struct IrcCommandsMapsListEntry
	{
		HandlersMap* pHandlersMap;
		IrcCommandsMapsListEntry* pBaseHandlersMap;
	};

	CIrcMonitor(CIrcSession& session);
	virtual ~CIrcMonitor();

	virtual void OnIrcMessage(const CIrcMessage* pmsg);

protected :
	CIrcSession& m_session;

	virtual IrcCommandsMapsListEntry* GetIrcCommandsMap() 
				{ return &m_handlersMapsListEntry; }

	virtual void OnIrcAll(const CIrcMessage* pmsg) {}
	virtual void OnIrcDefault(const CIrcMessage* pmsg) {}
	virtual void OnIrcDisconnected(const CIrcMessage* pmsg) {}

protected:
	static IrcCommandsMapsListEntry m_handlersMapsListEntry;
private:
	CCrossThreadsMessagingDevice m_xPost;
	static HandlersMap m_handlers;

	PfnIrcMessageHandler FindMethod(const char* lpszName);
	PfnIrcMessageHandler FindMethod(IrcCommandsMapsListEntry* pMapsList, const char* lpszName);

	virtual void OnCrossThreadsMessage(WPARAM wParam, LPARAM lParam);
};

// define an IRC command-to-member map.
// put that macro inside the class definition (.H file)
#define	DEFINE_IRC_MAP()	\
protected :	\
	virtual IrcCommandsMapsListEntry* GetIrcCommandsMap()	\
				{ return &m_handlersMapsListEntry; }	\
protected:	\
	static CIrcMonitor::IrcCommandsMapsListEntry m_handlersMapsListEntry;	\
private: \
	static CIrcMonitor::HandlersMap m_handlers;	\
protected:

// IRC command-to-member map's declaration. 
// add this macro to the class's .CPP file
#define	DECLARE_IRC_MAP(this_class, base_class)	\
	CIrcMonitor::HandlersMap this_class##::m_handlers;	\
	CIrcMonitor::IrcCommandsMapsListEntry this_class##::m_handlersMapsListEntry	\
		= { &this_class##::m_handlers, &base_class##::m_handlersMapsListEntry };

// map actual member functions to their associated IRC command.
// put any number of this macro in the class's constructor.
#define	IRC_MAP_ENTRY(class_name, name, member)	\
	m_handlers[(name)] = (PfnIrcMessageHandler)&class_name##::member;

////////////////////////////////////////////////////////////////////

class CIrcDefaultMonitor : public CIrcMonitor
{
public :
	CIrcDefaultMonitor(CIrcSession& session);

	DEFINE_IRC_MAP()

	CString sPassMsg;
	CString sNickMsg;
	CString sUserMsg;

protected :
	virtual bool OnIrc_NICK(const CIrcMessage* pmsg);
	virtual bool OnIrc_PING(const CIrcMessage* pmsg);
	virtual bool OnIrc_YOURHOST(const CIrcMessage* pmsg);
	virtual bool OnIrc_BOUNCE(const CIrcMessage* pmsg);
	virtual void OnFirstPing(LPCTSTR pID) {};
};

////////////////////////////////////////////////////////////////////
}; // end of namespace irc
////////////////////////////////////////////////////////////////////

#endif // _IRC_H_
