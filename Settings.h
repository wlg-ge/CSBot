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

#ifndef __Settings_H__
#define __Settings_H__

#include "GIRLDefs.h"
#include "SpeedLimiter.h"

#define PER_LOGIN			0x80000000

#define RUNLEVEL_NONE		0
#define RUNLEVEL_CRITICAL	1
#define RUNLEVEL_ALL		2

#define MAX_PLAYERLIST_COMMANDS	30
#define MAX_IRCMSG_LEN	2048

enum IRCCommand
{
	IRCCOMM_VERSION,
	IRCCOMM_LOGIN,
	IRCCOMM_LOGOUT,
	IRCCOMM_HELP,
	IRCCOMM_INFO,
	IRCCOMM_TEAMS,
	IRCCOMM_PLAYER,
	IRCCOMM_RCON,
	IRCCOMM_OP,
	IRCCOMM_DEOP,
	IRCCOMM_VOICE,
	IRCCOMM_DEVOICE,
	IRCCOMM_ACT,
	IRCCOMM_SAY
};

enum LogQueryType
{
	LQT_LOGADDRESS,
	LQT_LOGMOD,
	LQT_LOGADDRESS_ADD,
	LQT_AUTODETECT
};

struct CommandFlags {
	CommandFlags(DWORD dw) : m_dwAllowedCommands(dw) {};
	DWORD	m_dwAllowedCommands;

	bool IsCommandAllowed(int nCommand);
	void AllowCommand(int nCommand, bool bAllow);
};

struct TrustedPerson : public CommandFlags {
	TrustedPerson() : CommandFlags(IRCCOMM_HELP | IRCCOMM_LOGOUT) {
		m_bLoggedIn = false;
		m_tLoggedIn = 0;
		m_dwAllowedCommands = 0;
		m_bAllowLogin = false;
	}
	CString m_sUsername;
	CString m_sPass;
	bool	m_bAllowLogin;
	bool	m_bLoggedIn;
	CString m_sUserID;
	CString m_sCurrentNick;
	CTime	m_tLoggedIn;
	void Assign(const TrustedPerson* pPerson) {
		m_sUsername = pPerson->m_sUsername;
		m_sPass = pPerson->m_sPass;
		m_dwAllowedCommands = pPerson->m_dwAllowedCommands;
		m_bAllowLogin = pPerson->m_bAllowLogin;
	}
	TrustedPerson* Copy() {
		TrustedPerson* pPerson = new TrustedPerson;
		pPerson->m_sUsername = m_sUsername;
		pPerson->m_sPass = m_sPass;
		pPerson->m_dwAllowedCommands = m_dwAllowedCommands;
		pPerson->m_bAllowLogin = m_bAllowLogin;
		return pPerson;
	}
	CString FormatPermissions();
};
typedef std::list<TrustedPerson*> PERSONLIST;

struct PlayerListCommand
{
	PlayerListCommand() {};
	PlayerListCommand(const CString& sName, const CString& sCmd) :
		m_sName(sName),
		m_sCommand(sCmd) {};
	PlayerListCommand* Copy()
	{
		return new PlayerListCommand(m_sName, m_sCommand);
	}
	CString m_sName;
	CString m_sCommand;
};
typedef std::list<PlayerListCommand*> PLAYERLISTCMDLIST;

struct GameServerVersion
{
	GameServerVersion() : m_bLinux(false), m_bDedicated(false), m_bX111(false) {};
	bool m_bLinux; // Otherwise Windows
	bool m_bDedicated; // Otherwise Listenserver
	bool m_bX111; // x.1.1.1. Otherwise x.1.1.0
};

struct GameServerProfile {
	GameServerProfile() :
		m_uPort(0),
		m_uLogPort(0),
		m_dwLocalIP(0),
		m_bReconnectToGame(FALSE),
		m_nGameReconnectSec(0),
		m_nGameReconnectTimes(0),
		m_bInactivity(FALSE),
		m_nInactivityTimeout(0),
		m_eType(LQT_AUTODETECT)
		{};

	UINT			m_uPort;
	CString			m_sRcon;
	CString			m_sServerName;
	UINT			m_uLogPort;
	DWORD			m_dwLocalIP;
	CString			m_sProfileName;
	CStringList		m_lConnectCommands;
	CStringList		m_lDisconnectCommands;
	LogQueryType	m_eType;

	BOOL			m_bReconnectToGame;
	int				m_nGameReconnectSec;
	int				m_nGameReconnectTimes;

	BOOL			m_bInactivity;
	int				m_nInactivityTimeout;

	void Assign(const GameServerProfile* pProfile) {
		m_uPort = pProfile->m_uPort;
		m_sRcon = pProfile->m_sRcon;
		m_sServerName = pProfile->m_sServerName;
		m_uLogPort = pProfile->m_uLogPort;
		m_dwLocalIP = pProfile->m_dwLocalIP;
		m_sProfileName = pProfile->m_sProfileName;
		m_bReconnectToGame = pProfile->m_bReconnectToGame;
		m_nGameReconnectSec = pProfile->m_nGameReconnectSec;
		m_nGameReconnectTimes = pProfile->m_nGameReconnectTimes;
		m_bInactivity = pProfile->m_bInactivity;
		m_nInactivityTimeout = pProfile->m_nInactivityTimeout;
		m_eType = pProfile->m_eType;
		m_lConnectCommands.RemoveAll();
		POSITION pos = pProfile->m_lConnectCommands.GetHeadPosition();
		while (pos != NULL)
		{
			CString s = pProfile->m_lConnectCommands.GetNext(pos);
			m_lConnectCommands.AddTail(s);
		}
		m_lDisconnectCommands.RemoveAll();
		pos = pProfile->m_lDisconnectCommands.GetHeadPosition();
		while (pos != NULL)
		{
			CString s = pProfile->m_lDisconnectCommands.GetNext(pos);
			m_lDisconnectCommands.AddTail(s);
		}
	}
	void LoadFromRegistry(LPCTSTR pProfile);
	void SaveToRegistry();
};

struct IRCServerProfile {
	IRCServerProfile() :
		m_uPort(0),
		m_bIdentServer(0),
		m_bCloseIdentAfterQuery(FALSE),
		m_bReconnectToIRC(FALSE),
		m_nIRCReconnectSec(0),
		m_nIRCReconnectTimes(0)
		{};
	CString	m_sChannelName;
	CString	m_sChannelPassword;
	UINT	m_uPort;
	CString	m_sServerName;
	CString	m_sNickName;
	CString	m_sFullName;
	CString	m_sPassword;
	CString	m_sUserID;
	CString	m_sProfileName;
	CStringList	m_lConnectCommands;

	BOOL	m_bIdentServer;
	BOOL	m_bCloseIdentAfterQuery;

	BOOL	m_bReconnectToIRC;
	int		m_nIRCReconnectSec;
	int		m_nIRCReconnectTimes;

	void Assign(const IRCServerProfile* pProfile) {
		m_sChannelName = pProfile->m_sChannelName;
		m_sChannelPassword = pProfile->m_sChannelPassword;
		m_uPort = pProfile->m_uPort;
		m_sServerName = pProfile->m_sServerName;
		m_sNickName = pProfile->m_sNickName;
		m_sFullName = pProfile->m_sFullName;
		m_sPassword = pProfile->m_sPassword;
		m_sUserID = pProfile->m_sUserID;
		m_sProfileName = pProfile->m_sProfileName;
		m_bIdentServer = pProfile->m_bIdentServer;
		m_bCloseIdentAfterQuery = pProfile->m_bCloseIdentAfterQuery;
		m_bReconnectToIRC = pProfile->m_bReconnectToIRC;
		m_nIRCReconnectSec = pProfile->m_nIRCReconnectSec;
		m_nIRCReconnectTimes = pProfile->m_nIRCReconnectTimes;
		m_lConnectCommands.RemoveAll();
		POSITION pos = pProfile->m_lConnectCommands.GetHeadPosition();
		while (pos != NULL)
		{
			CString s = pProfile->m_lConnectCommands.GetNext(pos);
			m_lConnectCommands.AddTail(s);
		}
	}
	void LoadFromRegistry(LPCTSTR pProfile);
	void SaveToRegistry();
};

struct Settings : public CommandFlags
{
	Settings();
	~Settings();
	BOOL			m_bUpdateWonID;
	CString			m_sWonIDFolder;
	BOOL			m_bAboutShown;

	BOOL			m_bCMFromIRC;
	CString			m_sCMFromIRCPrefix;
	BOOL			m_bPMFromIRC;
	BOOL			m_bDCCChatFromIRC;

	BOOL			m_bRemember;
	int				m_nRunLevel;
	BOOL			m_bAllowSummon;
	BOOL			m_bAllowSummonTeam;
	BOOL			m_bSummonFlash;
	BOOL			m_bSummonPlay;
	CStringList		m_lSummonLines;
	CString			m_sSummonPlayFile;
	CString			m_sSummonWord;
	CStringArray	m_arIRCMessages;
	int				m_nGameConnectionTimeout;

	PERSONLIST		m_lTrusted;
	BOOL			m_bAutoLogOff;
	int				m_nLogOffMinutes;

	BOOL			m_bNotifyUpdates;
	BOOL			m_bWarnLogOff;

	CString			m_sLastCSServer;
	CString			m_sLastIRCServer;
	int				m_nLastCmdShow;

protected:
	int				m_nAntiFloodLines;
	int				m_nAntiFloodSeconds;
	int				m_nIRCDelay;
	CString			m_sLogFile;
	BOOL			m_bLog;
	BOOL			m_bFilterLog;

public:
	int		GetAntifloodSeconds();
	int		GetAntifloodLines();
	void	SetAntiflood(int nAntiFloodLines, int nAntiFloodSeconds);
	int		GetIRCDelay();
	void	SetIRCDelay(int nSec);

	BOOL	IsLogging();
	BOOL	IsFilterLog() const {return m_bFilterLog;};
	LPCTSTR	GetLogPath();
	void	SetLogging(BOOL bLog, LPCTSTR pLogPath, BOOL bFilterLog);

public:
	SpeedLimiter	m_limiter;
	FILE*			m_pLogFile;

	PLAYERLISTCMDLIST m_lPlayerListCmds;

	void OpenLogFile();
	CString GetString(LPCTSTR pName, LPCTSTR pDefault = NULL);
	void SetString(LPCTSTR pName, LPCTSTR pValue);

	static CString Encrypt(CString sString);
	static CString Decrypt(CString sString);

	CString ReadHttp(LPCTSTR pServer, LPCTSTR pObject, int nTimeout = 60);

	void ReplaceLocalIP(DWORD& dwLocalIP, DWORD lIP);
	bool GuessIP(DWORD& dwLocalIP);
	bool GuessIPHarder(DWORD& dwLocalIP);

	void LoadFromRegistry();
	void SaveToRegistry();
};

extern Settings settings;
extern char* g_pCommandNames[];

#endif
