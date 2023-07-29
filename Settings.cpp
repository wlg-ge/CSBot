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

#include "stdafx.h"
#include "Settings.h"
#include "GIRLLogParserManager.h"
#include "GIRLLogParser.h"
#ifdef HTTP_WININET
#include <wininet.h>
#endif

#define IRCMSGS_VERSION 2

char* g_pCommandNames[] =
{
	"version",
	"login",
	"logout",
	"help",
	"info",
	"teams",
	"player",
	"rcon",
	"op",
	"deop",
	"voice",
	"devoice",
	"act",
	"say",
	NULL
};

Settings settings;

Settings::Settings() : CommandFlags(0)
{
	m_pLogFile = NULL;
	m_dwAllowedCommands = 0;
	m_bFilterLog = FALSE;
}

Settings::~Settings()
{
	for (PERSONLIST::iterator i = m_lTrusted.begin(); i != m_lTrusted.end(); i++)
		delete (*i);
	for (PLAYERLISTCMDLIST::iterator iCmd = m_lPlayerListCmds.begin(); iCmd != m_lPlayerListCmds.end(); iCmd++)
		delete (*iCmd);
}

void Settings::OpenLogFile()
{
	if (m_pLogFile != NULL)
		fclose(m_pLogFile);
	m_pLogFile = NULL;

	if (m_bLog) {
		CString sFilename;
		CTime date;
		date = CTime::GetCurrentTime();

		sFilename.Format("%s\\%d-%02d-%02d.log", (LPCTSTR)m_sLogFile, date.GetYear(), date.GetMonth(), date.GetDay());
		m_pLogFile = fopen((LPCTSTR)sFilename, "a+");
		if (m_pLogFile == NULL) {
			m_bLog = FALSE;
			CString sMsg;
			sMsg.Format("Could not open the file %s for writing", (LPCTSTR)sFilename);
			AfxMessageBox(sMsg);
			return;
		}
	}
}

void Settings::LoadFromRegistry()
{
	m_bAboutShown = AfxGetApp()->GetProfileInt("Options", "AboutShown", FALSE);
	m_bPMFromIRC = AfxGetApp()->GetProfileInt("Options", "PMFromIRC", FALSE);
	m_bCMFromIRC = AfxGetApp()->GetProfileInt("Options", "CMFromIRC", FALSE);
	m_sCMFromIRCPrefix = AfxGetApp()->GetProfileString("Options", "CMFromIRCPrefix", "!");
	if (m_sCMFromIRCPrefix.GetLength() > 1)
		m_sCMFromIRCPrefix = m_sCMFromIRCPrefix.Left(1);
	m_bDCCChatFromIRC = AfxGetApp()->GetProfileInt("Options", "DCCChatFromIRC", FALSE);
	m_bAutoLogOff = AfxGetApp()->GetProfileInt("Options", "AutoLogOff", FALSE);
	m_nLogOffMinutes = AfxGetApp()->GetProfileInt("Options", "LogOffMinutes", 30);
	m_nLastCmdShow = AfxGetApp()->GetProfileInt("Options", "LastCmdShow", SW_RESTORE);

	m_bLog = AfxGetApp()->GetProfileInt("Options", "Log", FALSE);
	m_sLogFile = AfxGetApp()->GetProfileString("Options", "LogFile", "");
	m_bFilterLog = AfxGetApp()->GetProfileInt("Options", "FilterLog", FALSE);
	m_bRemember = AfxGetApp()->GetProfileInt("Options", "RememberChildWindows", TRUE);
	m_nRunLevel = AfxGetApp()->GetProfileInt("Options", "RunLevel", RUNLEVEL_NONE);
	
	m_bUpdateWonID = AfxGetApp()->GetProfileInt("Options", "UpdateWonIDDB", FALSE);
	m_sWonIDFolder = AfxGetApp()->GetProfileString("Options", "WonIDFolder", "");
	m_nGameConnectionTimeout = AfxGetApp()->GetProfileInt("Options", "GameConnectionTimeout", 4);

	m_sLastCSServer = AfxGetApp()->GetProfileString("Options", "LastCSServer", "");
	m_sLastIRCServer = AfxGetApp()->GetProfileString("Options", "LastIRCServer", "");

	m_bAllowSummon = AfxGetApp()->GetProfileInt("Options", "AllowSummon", FALSE);
	m_bAllowSummonTeam = AfxGetApp()->GetProfileInt("Options", "AllowSummonTeam", FALSE);
	m_bSummonFlash = AfxGetApp()->GetProfileInt("Options", "SummonFlash", FALSE);
	m_bSummonPlay = AfxGetApp()->GetProfileInt("Options", "SummonPlay", FALSE);
	m_sSummonPlayFile = AfxGetApp()->GetProfileString("Options", "SummonPlayFile", "");
	m_sSummonWord = AfxGetApp()->GetProfileString("Options", "SummonWord", "!admin");

	int nAntiFloodLines = AfxGetApp()->GetProfileInt("Options", "AntiFloodLines", 2);
	int nAntiFloodSeconds = AfxGetApp()->GetProfileInt("Options", "AntiFloodSeconds", 6);

	SetAntiflood(nAntiFloodLines, nAntiFloodSeconds);

	int nSec = AfxGetApp()->GetProfileInt("Options", "IRCDelay", 0);
	SetIRCDelay(nSec);

	m_bNotifyUpdates = AfxGetApp()->GetProfileInt("Options", "NotifyUpdates", TRUE);
	m_bWarnLogOff = AfxGetApp()->GetProfileInt("Options", "WarnLogOff", TRUE);
	m_dwAllowedCommands = AfxGetApp()->GetProfileInt("Options", "AllowedCommands", IRCCOMM_HELP | IRCCOMM_LOGIN);

	int nAmount = AfxGetApp()->GetProfileInt("PlayerListCommands", "Count", -1);
	if (nAmount > MAX_PLAYERLIST_COMMANDS)
		nAmount = MAX_PLAYERLIST_COMMANDS;

	if (nAmount != -1)
	{
		for (int i=0; i<nAmount; i++)
		{
			CString sName;
			sName.Format("Name%d", i);
			sName = AfxGetApp()->GetProfileString("PlayerListCommands", sName, "");
			sName.TrimLeft();
			sName.TrimRight();

			CString sCmd;
			sCmd.Format("Command%d", i);
			sCmd = AfxGetApp()->GetProfileString("PlayerListCommands", sCmd, "");
			sCmd.TrimLeft();
			sCmd.TrimRight();

			if (!sName.IsEmpty() && !sCmd.IsEmpty())
				m_lPlayerListCmds.push_back(new PlayerListCommand(sName, sCmd));
		}
	} else {
		m_lPlayerListCmds.push_back(new PlayerListCommand("Kick player", "kick # <id>"));
		m_lPlayerListCmds.push_back(new PlayerListCommand("User info", "user <id>"));
	}

	nAmount = AfxGetApp()->GetProfileInt("AutoSummonCommands", "Count", 0);
	for (int i=0; i<nAmount; i++)
	{
		CString sName;
		sName.Format("String%d", i);
		sName = AfxGetApp()->GetProfileString("AutoSummonCommands", sName, "");
		sName.TrimLeft();
		sName.TrimRight();
		if (!sName.IsEmpty())
			m_lSummonLines.AddTail(sName);
	}

	nAmount = AfxGetApp()->GetProfileInt("Trusted People", "Count", 0);
	TrustedPerson* pPerson;
	for (i=0; i<nAmount; i++)
	{
		CString sSection, sUsername, sPass;
		sSection.Format("Trusted People\\Trusted%d", i);
		sUsername = AfxGetApp()->GetProfileString(sSection, "Username", "");
		if (!sUsername.IsEmpty()) {
			pPerson = new TrustedPerson;
			pPerson->m_sUsername = sUsername;
			pPerson->m_sPass = Decrypt(AfxGetApp()->GetProfileString(sSection, "Password", ""));
			pPerson->m_dwAllowedCommands = AfxGetApp()->GetProfileInt(sSection, "AllowedCommands", 0);
			pPerson->m_bAllowLogin = AfxGetApp()->GetProfileInt(sSection, "AllowLogin", FALSE) ? true : false;
			m_lTrusted.push_back(pPerson);
		}
	}

/*	m_sTColor = AfxGetApp()->GetProfileString("IRC Messages", "CS-TColor", "00,04");
	m_sCTColor = AfxGetApp()->GetProfileString("IRC Messages", "CS-CTColor", "00,12");
	m_sCSSpecColor = AfxGetApp()->GetProfileString("IRC Messages", "CS-SpecColor", "00,14");
	m_sAColor = AfxGetApp()->GetProfileString("IRC Messages", "NS-AColor", "09,01");
	m_sMColor = AfxGetApp()->GetProfileString("IRC Messages", "NS-MColor", "00,02");
	m_sNSSpecColor = AfxGetApp()->GetProfileString("IRC Messages", "NS-SpecColor", "00,14");
*/
	PARSERLIST* pParserList = g_parserManager.GetParserList();
	CString sSection, sName;
	for (PARSERLIST::iterator iParser = pParserList->begin(); iParser != pParserList->end(); iParser++)
	{
		GIRLLogParser* pParser = (*iParser);
		sSection.Format("IRC-%s", *pParser->GameTypeName());

		IRCMSGMAP& mMsgs = pParser->LockMsgMap();
		for (IRCMSGMAP::iterator j = mMsgs.begin(); j != mMsgs.end(); j++)
		{
			IRCCustomMsg& msg = (*j).second;
			sName.Format("Msg%08x", msg.m_uMsg);
			msg.m_sValue = (LPCTSTR)AfxGetApp()->GetProfileString(sSection, sName, msg.m_pDefault);
			sName.Format("Enable%08x", msg.m_uMsg);
			msg.m_bEnabled = AfxGetApp()->GetProfileInt(sSection, sName, msg.m_bEnabled ? TRUE : FALSE) ? true : false;
		}
		pParser->UnlockMsgMap();

		for (int nColor = 0; nColor < TEAM_COUNT; nColor++)
		{
			sName.Format("Color%08x", nColor);
			pParser->SetTeamColor((Team)nColor, (LPCTSTR)AfxGetApp()->GetProfileString(sSection, sName, pParser->GetTeamColor((Team)nColor).c_str()));
		}
	}
}

void Settings::SaveToRegistry()
{
	AfxGetApp()->WriteProfileInt("Options", "AboutShown", m_bAboutShown);
	AfxGetApp()->WriteProfileInt("Options", "PMFromIRC", m_bPMFromIRC);
	AfxGetApp()->WriteProfileInt("Options", "CMFromIRC", m_bCMFromIRC);
	AfxGetApp()->WriteProfileString("Options", "CMFromIRCPrefix", m_sCMFromIRCPrefix);
	AfxGetApp()->WriteProfileInt("Options", "DCCChatFromIRC", m_bDCCChatFromIRC);
	AfxGetApp()->WriteProfileInt("Options", "LastCmdShow", m_nLastCmdShow);

	AfxGetApp()->WriteProfileInt("Options", "Log", m_bLog);
	AfxGetApp()->WriteProfileString("Options", "LogFile", m_sLogFile);
	AfxGetApp()->WriteProfileInt("Options", "FilterLog", m_bFilterLog);

	AfxGetApp()->WriteProfileInt("Options", "RunLevel", m_nRunLevel);
	AfxGetApp()->WriteProfileInt("Options", "RememberChildWindows", m_bRemember);
	AfxGetApp()->WriteProfileInt("Options", "UpdateWonIDDB", m_bUpdateWonID);
	AfxGetApp()->WriteProfileString("Options", "WonIDFolder", m_sWonIDFolder);
	AfxGetApp()->WriteProfileInt("Options", "GameConnectionTimeout", m_nGameConnectionTimeout);

	AfxGetApp()->WriteProfileString("Options", "LastCSServer", m_sLastCSServer);
	AfxGetApp()->WriteProfileString("Options", "LastIRCServer", m_sLastIRCServer);

	AfxGetApp()->WriteProfileInt("Options", "AutoLogOff", m_bAutoLogOff);
	AfxGetApp()->WriteProfileInt("Options", "LogOffMinutes", m_nLogOffMinutes);

	AfxGetApp()->WriteProfileInt("Options", "AntiFloodLines", m_nAntiFloodLines);
	AfxGetApp()->WriteProfileInt("Options", "AntiFloodSeconds", m_nAntiFloodSeconds);
	AfxGetApp()->WriteProfileInt("Options", "IRCDelay", m_nIRCDelay);

	AfxGetApp()->WriteProfileInt("Options", "NotifyUpdates", m_bNotifyUpdates);
	AfxGetApp()->WriteProfileInt("Options", "WarnLogOff", m_bWarnLogOff);
	AfxGetApp()->WriteProfileInt("Options", "AllowedCommands", m_dwAllowedCommands);

	AfxGetApp()->WriteProfileInt("Options", "AllowSummon", m_bAllowSummon);
	AfxGetApp()->WriteProfileInt("Options", "AllowSummonTeam", m_bAllowSummonTeam);
	AfxGetApp()->WriteProfileInt("Options", "SummonFlash", m_bSummonFlash);
	AfxGetApp()->WriteProfileInt("Options", "SummonPlay", m_bSummonPlay);
	AfxGetApp()->WriteProfileString("Options", "SummonPlayFile", m_sSummonPlayFile);
	AfxGetApp()->WriteProfileString("Options", "SummonWord", m_sSummonWord);

	AfxGetApp()->WriteProfileInt("PlayerListCommands", "Count", m_lPlayerListCmds.size());
	int i=0;
	for (PLAYERLISTCMDLIST::iterator iCmd = m_lPlayerListCmds.begin(); iCmd != m_lPlayerListCmds.end(); iCmd++)
	{
		PlayerListCommand* pCmd = (*iCmd);

		CString sName;
		sName.Format("Name%d", i);
		AfxGetApp()->WriteProfileString("PlayerListCommands", sName, pCmd->m_sName);
		sName.Format("Command%d", i);
		AfxGetApp()->WriteProfileString("PlayerListCommands", sName, pCmd->m_sCommand);
		i++;
	}

	AfxGetApp()->WriteProfileInt("AutoSummonCommands", "Count", m_lSummonLines.GetCount());
	POSITION pos = m_lSummonLines.GetHeadPosition();
	i=0;
	while (pos != NULL)
	{
		CString sVal = m_lSummonLines.GetNext(pos);
		CString sName;
		sName.Format("String%d", i);
		AfxGetApp()->WriteProfileString("AutoSummonCommands", sName, sVal);
		i++;
	}

/*	AfxGetApp()->WriteProfileInt("IRC Messages", "Version", IRCMSGS_VERSION);
	AfxGetApp()->WriteProfileString("IRC Messages", "CS-TColor", m_sTColor);
	AfxGetApp()->WriteProfileString("IRC Messages", "CS-CTColor", m_sCTColor);
	AfxGetApp()->WriteProfileString("IRC Messages", "CS-SpecColor", m_sCSSpecColor);
	AfxGetApp()->WriteProfileString("IRC Messages", "NS-AColor", m_sAColor);
	AfxGetApp()->WriteProfileString("IRC Messages", "NS-MColor", m_sMColor);
	AfxGetApp()->WriteProfileString("IRC Messages", "NS-SpecColor", m_sNSSpecColor);
*/
	PARSERLIST* pParserList = g_parserManager.GetParserList();
	CString sSection, sName;
	for (PARSERLIST::iterator iParser = pParserList->begin(); iParser != pParserList->end(); iParser++)
	{
		GIRLLogParser* pParser = (*iParser);
		sSection.Format("IRC-%s", *pParser->GameTypeName());

		IRCMSGMAP& mMsgs = pParser->LockMsgMap();
		for (IRCMSGMAP::iterator j = mMsgs.begin(); j != mMsgs.end(); j++)
		{
			IRCCustomMsg& msg = (*j).second;
			sName.Format("Msg%08x", msg.m_uMsg);
			AfxGetApp()->WriteProfileString(sSection, sName, msg.m_sValue.c_str());
			sName.Format("Enable%08x", msg.m_uMsg);
			AfxGetApp()->WriteProfileInt(sSection, sName, msg.m_bEnabled ? TRUE : FALSE);
		}
		pParser->UnlockMsgMap();

		for (int nColor = 0; nColor < TEAM_COUNT; nColor++)
		{
			sName.Format("Color%08x", nColor);
			AfxGetApp()->WriteProfileString(sSection, sName, pParser->GetTeamColor((Team)nColor).c_str());
		}
	}

	AfxGetApp()->WriteProfileInt("Trusted People", "Count", m_lTrusted.size());
	TrustedPerson* pPerson;
	i=0;
	for (PERSONLIST::iterator it=m_lTrusted.begin(); it!=m_lTrusted.end(); it++,i++)
	{
		pPerson = (*it);
		CString sSection;
		sSection.Format("Trusted People\\Trusted%d", i);
		AfxGetApp()->WriteProfileString(sSection, "Username", pPerson->m_sUsername);
		AfxGetApp()->WriteProfileString(sSection, "Password", Encrypt(pPerson->m_sPass));
		AfxGetApp()->WriteProfileInt(sSection, "AllowedCommands", pPerson->m_dwAllowedCommands);
		AfxGetApp()->WriteProfileInt(sSection, "AllowLogin", pPerson->m_bAllowLogin ? TRUE : FALSE);
	}
}

void GameServerProfile::LoadFromRegistry(LPCTSTR pProfile)
{
	m_sProfileName = pProfile;
	CString sSection = "Game-Servers\\" + m_sProfileName;

	m_sServerName = AfxGetApp()->GetProfileString(sSection, "ServerName", "localhost");
	m_dwLocalIP = AfxGetApp()->GetProfileInt(sSection, "LocalIP", 0);
	m_uPort = AfxGetApp()->GetProfileInt(sSection, "Port", 27015);
	m_uLogPort = AfxGetApp()->GetProfileInt(sSection, "LogPort", 0);
	m_sRcon = Settings::Decrypt(AfxGetApp()->GetProfileString(sSection, "Rcon", ""));

	m_bReconnectToGame = AfxGetApp()->GetProfileInt(sSection, "ReconnectToGame", TRUE);
	m_nGameReconnectSec = AfxGetApp()->GetProfileInt(sSection, "ReconnectToGameSec", 10);
	m_nGameReconnectTimes = AfxGetApp()->GetProfileInt(sSection, "ReconnectToGameTimes", 30);
	m_bInactivity = AfxGetApp()->GetProfileInt(sSection, "Inactivity", TRUE);
	m_nInactivityTimeout = AfxGetApp()->GetProfileInt(sSection, "InactivityTimeout", 300);

	m_eType = (LogQueryType)AfxGetApp()->GetProfileInt(sSection, "LogQueryType", (int)LQT_AUTODETECT);

	sSection = "Game-Servers\\" + m_sProfileName + "\\ConnectCommands";
	int nAmount = AfxGetApp()->GetProfileInt(sSection, "Count", -1);
	if (nAmount >= 0) {
		for (int i=0; i<nAmount; i++)
		{
			CString sName;
			sName.Format("String%d", i);
			sName = AfxGetApp()->GetProfileString(sSection, sName, "");
			sName.TrimLeft();
			sName.TrimRight();
			if (!sName.IsEmpty())
				m_lConnectCommands.AddTail(sName);
		}
	} else {
		m_lConnectCommands.AddTail("mp_logmessages 1");
	}

	sSection = "Game-Servers\\" + m_sProfileName + "\\DisconnectCommands";
	nAmount = AfxGetApp()->GetProfileInt(sSection, "Count", -1);
	if (nAmount >= 0) {
		for (int i=0; i<nAmount; i++)
		{
			CString sName;
			sName.Format("String%d", i);
			sName = AfxGetApp()->GetProfileString(sSection, sName, "");
			sName.TrimLeft();
			sName.TrimRight();
			if (!sName.IsEmpty())
				m_lDisconnectCommands.AddTail(sName);
		}
	} else {
		m_lDisconnectCommands.AddTail("exec banned.cfg");
		m_lDisconnectCommands.AddTail("writeid");
	}
}

void GameServerProfile::SaveToRegistry()
{
	CString sSection = "Game-Servers\\" + m_sProfileName;

	AfxGetApp()->WriteProfileString(sSection, "ServerName", m_sServerName);
	AfxGetApp()->WriteProfileInt(sSection, "LocalIP", m_dwLocalIP);
	AfxGetApp()->WriteProfileInt(sSection, "Port", m_uPort);
	AfxGetApp()->WriteProfileInt(sSection, "LogPort", m_uLogPort);
	AfxGetApp()->WriteProfileString(sSection, "Rcon", Settings::Encrypt(m_sRcon));

	AfxGetApp()->WriteProfileInt(sSection, "ReconnectToGame", m_bReconnectToGame);
	AfxGetApp()->WriteProfileInt(sSection, "ReconnectToGameSec", m_nGameReconnectSec);
	AfxGetApp()->WriteProfileInt(sSection, "ReconnectToGameTimes", m_nGameReconnectTimes);
	AfxGetApp()->WriteProfileInt(sSection, "Inactivity", m_bInactivity);
	AfxGetApp()->WriteProfileInt(sSection, "InactivityTimeout", m_nInactivityTimeout);

	AfxGetApp()->WriteProfileInt(sSection, "LogQueryType", (int)m_eType);

	sSection = "Game-Servers\\" + m_sProfileName + "\\ConnectCommands";
	AfxGetApp()->WriteProfileInt(sSection, "Count", m_lConnectCommands.GetCount());
	POSITION pos = m_lConnectCommands.GetHeadPosition();
	int i=0;
	while (pos != NULL)
	{
		CString sVal = m_lConnectCommands.GetNext(pos);
		CString sName;
		sName.Format("String%d", i);
		AfxGetApp()->WriteProfileString(sSection, sName, sVal);
		i++;
	}

	sSection = "Game-Servers\\" + m_sProfileName + "\\DisconnectCommands";
	AfxGetApp()->WriteProfileInt(sSection, "Count", m_lDisconnectCommands.GetCount());
	pos = m_lDisconnectCommands.GetHeadPosition();
	i=0;
	while (pos != NULL)
	{
		CString sVal = m_lDisconnectCommands.GetNext(pos);
		CString sName;
		sName.Format("String%d", i);
		AfxGetApp()->WriteProfileString(sSection, sName, sVal);
		i++;
	}
}

char* cEnCrypt = "lkiganocedmhpjfb";
char* cDeCrypt = "EPHJIODLCNBAKFGM";

CString Settings::Encrypt(CString sString)
{
	unsigned char cLast = 0xb1;
	CString sRet;
	for (int i = 0; i < sString.GetLength(); i++)
	{
		unsigned char c = (unsigned char)sString[i];
		c ^= cLast;
		c = (c >> 3) | ((c & 7) << 5);
		cLast = c;
		sRet += cEnCrypt[c >> 4];
		sRet += cEnCrypt[c & 15];
	}
	return sRet;
}

CString Settings::Decrypt(CString sString)
{
	unsigned char cLast = 0xb1, cTemp;
	CString sRet;

	while (sString.GetLength() > 1) {
		if ((sString[0] < 'a') || (sString[0] > 'p') || (sString[1] < 'a') || (sString[1] > 'p'))
			return "";
		unsigned char c = ((cDeCrypt[sString[0]-'a']-'A') << 4) | (cDeCrypt[sString[1]-'a']-'A');
		sString = sString.Mid(2);
		cTemp = cLast;
		cLast = c;
		c = ((c & 0x1f) << 3) | (c >> 5);
		c ^= cTemp;
		sRet += c;
	}
	return sRet;
}

int Settings::GetIRCDelay()
{
	return m_nIRCDelay;
}

void Settings::SetIRCDelay(int nSec)
{
	if (nSec < 0) nSec = 0;
	if (nSec > 120) nSec = 120;
	m_nIRCDelay = nSec;
}

int Settings::GetAntifloodLines()
{
	return m_nAntiFloodLines;
}

int Settings::GetAntifloodSeconds()
{
	return m_nAntiFloodSeconds;
}

void Settings::SetAntiflood(int nAntiFloodLines, int nAntiFloodSeconds)
{
	if (nAntiFloodLines < 1) nAntiFloodLines = 1;
	if (nAntiFloodLines > 100) nAntiFloodLines = 100;
	if (nAntiFloodSeconds < 1) nAntiFloodSeconds = 1;
	if (nAntiFloodSeconds > 10) nAntiFloodSeconds = 10;
	
	if ((m_nAntiFloodLines != nAntiFloodLines) ||
		(m_nAntiFloodSeconds != nAntiFloodSeconds))
	{
		m_nAntiFloodLines = nAntiFloodLines;
		m_nAntiFloodSeconds = nAntiFloodSeconds;

		m_limiter.SetSpeed(m_nAntiFloodLines, m_nAntiFloodSeconds);
	}
}

BOOL Settings::IsLogging()
{
	return m_bLog;
}

LPCTSTR Settings::GetLogPath()
{
	return m_sLogFile;
}

void Settings::SetLogging(BOOL bLog, LPCTSTR pLogPath, BOOL bFilterLog)
{
	m_bLog = bLog;
	m_sLogFile = pLogPath;
	m_bFilterLog = bFilterLog;
	OpenLogFile();
}

void IRCServerProfile::LoadFromRegistry(LPCTSTR pProfile)
{
	m_sProfileName = pProfile;
	CString sSection = "IRC-Servers\\" + m_sProfileName;

	m_sChannelName = AfxGetApp()->GetProfileString(sSection, "ChannelName", "");
	m_sChannelPassword = AfxGetApp()->GetProfileString(sSection, "ChannelPassword", "");
	m_sServerName = AfxGetApp()->GetProfileString(sSection, "ServerName", "");
	m_uPort = AfxGetApp()->GetProfileInt(sSection, "Port", 6667);
	m_sNickName = AfxGetApp()->GetProfileString(sSection, "NickName", "CSBot");
	m_sUserID = AfxGetApp()->GetProfileString(sSection, "UserID", "CSBot");
	m_sFullName = AfxGetApp()->GetProfileString(sSection, "FullName", "CSBot");
	m_sPassword = Settings::Decrypt(AfxGetApp()->GetProfileString(sSection, "Password", ""));
	m_bIdentServer = AfxGetApp()->GetProfileInt(sSection, "IdentServer", TRUE);
	m_bCloseIdentAfterQuery = AfxGetApp()->GetProfileInt(sSection, "CloseIdentAfterQuery", TRUE);
	m_bReconnectToIRC = AfxGetApp()->GetProfileInt(sSection, "ReconnectToIRC", TRUE);
	m_nIRCReconnectSec = AfxGetApp()->GetProfileInt(sSection, "IRCReconnectSec", 120);
	m_nIRCReconnectTimes = AfxGetApp()->GetProfileInt(sSection, "IRCReconnectTimes", 30);
	sSection = "IRC-Servers\\" + m_sProfileName + "\\ConnectCommands";
	int nAmount = AfxGetApp()->GetProfileInt(sSection, "Count", 0);
	if (nAmount > 0) 
	{
		for (int i=0; i<nAmount; i++)
		{
			CString sName;
			sName.Format("String%d", i);
			sName = AfxGetApp()->GetProfileString(sSection, sName, "");
			sName.TrimLeft();
			sName.TrimRight();
			if (!sName.IsEmpty())
				m_lConnectCommands.AddTail(sName);
		}
	}
}

void IRCServerProfile::SaveToRegistry()
{
	CString sSection = "IRC-Servers\\" + m_sProfileName;

	AfxGetApp()->WriteProfileString(sSection, "ChannelName", m_sChannelName);
	AfxGetApp()->WriteProfileString(sSection, "ChannelPassword", m_sChannelPassword);
	AfxGetApp()->WriteProfileString(sSection, "ServerName", m_sServerName);
	AfxGetApp()->WriteProfileInt(sSection, "Port", m_uPort);
	AfxGetApp()->WriteProfileString(sSection, "NickName", m_sNickName);
	AfxGetApp()->WriteProfileString(sSection, "UserID", m_sUserID);
	AfxGetApp()->WriteProfileString(sSection, "FullName", m_sFullName);
	AfxGetApp()->WriteProfileString(sSection, "Password", Settings::Encrypt(m_sPassword));
	AfxGetApp()->WriteProfileInt(sSection, "IdentServer", m_bIdentServer);
	AfxGetApp()->WriteProfileInt(sSection, "CloseIdentAfterQuery", m_bCloseIdentAfterQuery);
	AfxGetApp()->WriteProfileInt(sSection, "ReconnectToIRC", m_bReconnectToIRC);
	AfxGetApp()->WriteProfileInt(sSection, "IRCReconnectSec", m_nIRCReconnectSec);
	AfxGetApp()->WriteProfileInt(sSection, "IRCReconnectTimes", m_nIRCReconnectTimes);

	sSection = "IRC-Servers\\" + m_sProfileName + "\\ConnectCommands";
	AfxGetApp()->WriteProfileInt(sSection, "Count", m_lConnectCommands.GetCount());
	POSITION pos = m_lConnectCommands.GetHeadPosition();
	int i=0;
	while (pos != NULL)
	{
		CString sVal = m_lConnectCommands.GetNext(pos);
		CString sName;
		sName.Format("String%d", i);
		AfxGetApp()->WriteProfileString(sSection, sName, sVal);
		i++;
	}
}

// Using the WinInet HTTP client.
// Lengthier, harder, takes more memory, more CPU, 
// loads several DLLs, but very comprehensive and compatible
CString Settings::ReadHttp(LPCTSTR pServer, LPCTSTR pObject, int nTimeout /* = 60 */)
{
	char c[1025];
	HINTERNET hInet = ::InternetOpen("CSBot/" CSBOT_VERSION, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInet == NULL)
	{
#if _TRACES >= 3
		sprintf(c, "Can't open internet: %d\n", GetLastError());
		::OutputDebugString(c);
#endif
		return "";
	}

	HINTERNET hConnect = ::InternetConnect(hInet, pServer, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hConnect == NULL)
	{
#if _TRACES >= 3
		sprintf(c, "Can't connect internet: %d\n", GetLastError());
		::OutputDebugString(c);
#endif
		::InternetCloseHandle(hInet);
		return "";
	}

	HINTERNET hRequest = ::HttpOpenRequest(hConnect, "GET", pObject, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, 0);
	if (hRequest == NULL)
	{
#if _TRACES >= 3
		sprintf(c, "Can't open http request: %d\n", GetLastError());
		::OutputDebugString(c);
#endif
		::InternetCloseHandle(hConnect);
		::InternetCloseHandle(hInet);
		return "";
	}

	unsigned long lTimeout = nTimeout * 1000;
	BOOL bOk;
	bOk = ::InternetSetOption(hRequest, INTERNET_OPTION_CONNECT_TIMEOUT, &lTimeout, sizeof(lTimeout));
	bOk = ::InternetSetOption(hRequest, INTERNET_OPTION_RECEIVE_TIMEOUT, &lTimeout, sizeof(lTimeout));
	bOk = ::InternetSetOption(hRequest, INTERNET_OPTION_SEND_TIMEOUT, &lTimeout, sizeof(lTimeout));

	if (!::HttpSendRequest(hRequest, NULL, 0, NULL, 0))
	{
#if _TRACES >= 3
		sprintf(c, "Can't send http request: %d\n", GetLastError());
		::OutputDebugString(c);
#endif
		::InternetCloseHandle(hRequest);
		::InternetCloseHandle(hConnect);
		::InternetCloseHandle(hInet);
		return "";
	}

	DWORD dwCode = 0;
	DWORD dwLength = sizeof(dwCode);
	DWORD dwIndex = 0;
	if (!::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwLength, &dwIndex))
	{
#if _TRACES >= 3
		sprintf(c, "Can't query status code: %d\n", GetLastError());
		::OutputDebugString(c);
#endif
		::InternetCloseHandle(hRequest);
		::InternetCloseHandle(hConnect);
		::InternetCloseHandle(hInet);
		return "";
	}

	if (dwCode != HTTP_STATUS_OK)
	{
#if _TRACES >= 3
		sprintf(c, "Http error: %d\n", dwCode);
		::OutputDebugString(c);
#endif
		::InternetCloseHandle(hRequest);
		::InternetCloseHandle(hConnect);
		::InternetCloseHandle(hInet);
		return "";
	}

	CString sRet;
	DWORD dwRead = 0;

	do {
		if (!::InternetReadFile(hRequest, c, 1024, &dwRead))
		{
			::InternetCloseHandle(hRequest);
			::InternetCloseHandle(hConnect);
			::InternetCloseHandle(hInet);
			return "";
		}
		if (dwRead < 0)
		{
#if _TRACES >= 3
			sprintf(c, "Can't read internet file: %d\n", GetLastError());
			::OutputDebugString(c);
#endif
			::InternetCloseHandle(hRequest);
			::InternetCloseHandle(hConnect);
			::InternetCloseHandle(hInet);
			return "";
		}
		if (dwRead > 0)
		{
			c[dwRead] = '\0';
			sRet += c;
		}
	} while (dwRead > 0);

	DWORD dwError = GetLastError();
	
	::InternetCloseHandle(hRequest);
	::InternetCloseHandle(hConnect);
	::InternetCloseHandle(hInet);

	return sRet;
}

CString Settings::GetString(LPCTSTR pName, LPCTSTR pDefault /* = NULL */)
{
	return AfxGetApp()->GetProfileString("Settings", pName, pDefault);
}

void Settings::SetString(LPCTSTR pName, LPCTSTR pValue)
{
	AfxGetApp()->WriteProfileString("Settings", pName, pValue);
}

bool CommandFlags::IsCommandAllowed(int nCommand)
{
	return (m_dwAllowedCommands & (1 << nCommand)) ? true : false;
}

void CommandFlags::AllowCommand(int nCommand, bool bAllow)
{
	if (bAllow)
		m_dwAllowedCommands |= (1 << nCommand);
	else
		m_dwAllowedCommands &= ~(1 << nCommand);
}

CString TrustedPerson::FormatPermissions()
{
	CString sRet;
	if (m_bAllowLogin)
		sRet = "Enabled";
	else
		sRet = "Disabled";
	for (int nCommand = 0; g_pCommandNames[nCommand] != NULL; nCommand++)
	{
		if (IsCommandAllowed(nCommand))
		{
			sRet += ", ";
			sRet += g_pCommandNames[nCommand];
		}
	}
	return sRet;
}

bool Settings::GuessIPHarder(DWORD& dwLocalIP)
{
	CString sResponse = settings.ReadHttp("xml.showmyip.com", "/", 3);
	if (sResponse.IsEmpty())
	{
#if _TRACES >= 3
		char c[1024];
		sprintf(c, "FetchIP2: empty HTTP response from xml.showmyip.com\n");
		::OutputDebugString(c);
#endif
		sResponse = settings.ReadHttp("www.csbot.org", "/ip.php", 3);
		if (sResponse.IsEmpty())
		{
#if _TRACES >= 3
			char c[1024];
			sprintf(c, "FetchIP2: empty HTTP response from www.csbot.org\n");
			::OutputDebugString(c);
#endif
			return false;
		}
	} else {
		int nPos = sResponse.Find("<ip>");
		if (nPos < 0) 
		{
#if _TRACES >= 3
			char c[1024];
			sprintf(c, "FetchIP2: can't find <ip>\n");
			::OutputDebugString(c);
#endif
			return false;
		}
		nPos += 4;
		sResponse.Delete(0, nPos);
		nPos = sResponse.Find("</ip>");
		if (nPos < 0) 
		{
#if _TRACES >= 3
			char c[1024];
			sprintf(c, "FetchIP2: can't find </ip>\n");
			::OutputDebugString(c);
#endif
			return false;
		}
		sResponse = sResponse.Left(nPos);
	}

	dwLocalIP = ntohl(inet_addr((LPCTSTR)sResponse));
	return true;
}

bool Settings::GuessIP(DWORD& dwLocalIP)
{
	dwLocalIP = 0x7f000001;
	char c[256];
	int nErr = ::gethostname(c, 255);
	if (nErr == SOCKET_ERROR) {
		AfxMessageBox("Failed to guess local IP");
		return false;
	}
	hostent* pHe = ::gethostbyname(c);
	if (pHe == NULL) {
		AfxMessageBox("Failed to guess local IP");
		return false;
	}
	if (pHe->h_addr_list == NULL) {
		AfxMessageBox("Failed to guess local IP");
		return false;
	}

	int i = 0;
	while ((pHe->h_addr_list[i] != NULL))
	{
		unsigned long lIP = ntohl(*((long*)pHe->h_addr_list[i]));
		ReplaceLocalIP(dwLocalIP, lIP);
		i++;
	}
	return true;
}

void Settings::ReplaceLocalIP(DWORD& dwLocalIP, DWORD lIP)
{
	// try not to take 127.0.0.1
	if (dwLocalIP == 0x7F000001) {
		dwLocalIP = lIP;
		return;
	}
	if (lIP == 0x7F000001)
		return;

	// try not to take things like 10.x.x.x/8
	if ((dwLocalIP & 0xFF000000) == 0x0A000000) {
		dwLocalIP = lIP;
		return;
	}
	if ((lIP & 0xFF000000) == 0x0A000000) 
		return;

	// try not to take things like 172.16.x.x/12
	if ((dwLocalIP & 0xFFF00000) == 0xAC100000) {
		dwLocalIP = lIP;
		return;
	}
	if ((lIP & 0xFFFF0000) == 0xAC100000) 
		return;

	// try not to take things like 192.168.x.x/16
	if ((dwLocalIP & 0xFFFF0000) == 0xC0A80000) {
		dwLocalIP = lIP;
		return;
	}
	if ((lIP & 0xFFFF0000) == 0xC0A80000) 
		return;

	// try not to take things like 169.254.x.x/16
	if ((dwLocalIP & 0xFFFF0000) == 0xA9FE0000) {
		dwLocalIP = lIP;
		return;
	}
	if ((lIP & 0xFFFF0000) == 0xA9FE0000) 
		return;

	dwLocalIP = lIP;
}
