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

// GIRLLogParser.cpp: implementation of the GIRLLogParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParser.h"
#include "GIRLLogParserManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GIRLLogParser::GIRLLogParser() : CIGCLockable("LogParser")
{
	m_pReceiver = NULL;
	m_bActive = false;
	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<hostname>\x0f <map>",
		"This is the format of the IRC channel",
		true,
		MsgType_Gameflow);
}

void GIRLLogParser::InitParser()
{
	m_pReceiver = g_parserManager.GetReceiver();
	SetTeamColorDefaults();
}

std::string GIRLLogParser::DecorateScore(Team t, int nScore)
{
	CString sRet;
	sRet.Format("%s-%d", TeamName(t), nScore);
	std::string s = Decorate((LPCTSTR)sRet, t);
	return s;
}

bool GIRLLogParser::ParseLogLine(std::string sLine)
{
	if (sLine.empty())
		return true;
	if (sLine.size() < 27)
		return true;
	if (sLine.size() > 1000)
		return true;
	if (sLine[0] != 'L')
		return true;

#if _TRACES >= 7
	char c[1024];
	sprintf(c, "LogLine: %s\n", sLine.c_str());
	::OutputDebugString(c);
#endif

	ParseTime(sLine);
	ProcessLine(sLine);
	return true;
}

std::string GIRLLogParser::Decorate(LPCTSTR pText, Team t)
{
	if (m_pReceiver->GetChannelModes().Find('c') != -1)
		return pText;

	std::string sColor = GetTeamColor(t);
	if (sColor.empty())
		return pText;

	return std::string("\x03") + sColor + pText + "\x0f";
}

void GIRLLogParser::SetServerName(const char* pName)
{
	IGCLock lock(this, "SetServerName");
	m_sServerName = pName;
}

std::string GIRLLogParser::GetMapName()
{
	IGCLock lock(this, "GetMapName");
	return m_sMapName;
}

std::string GIRLLogParser::GetServerName()
{
	IGCLock lock(this, "GetServerName");
	return m_sServerName;
}

void GIRLLogParser::ParseTime(std::string& sLine)
{
	// remove time garbage, and leave us with a parsable string
	ASSERT(sLine.size() >= 25);
	m_sTime = sLine.substr(0, 25);
	sLine = sLine.substr(25);
}

void GIRLLogParser::OutputWithRestOnly(std::string& sLine, IRCMSG uMsg)
{
	CString sMsg = GetIRCMessage(uMsg);
	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
}

void GIRLLogParser::ReplacePlayer(CString& sMsg, const CStatsPlayer& player)
{
	ReplaceName(sMsg, Decorate(player.m_sName.c_str(), player.m_team));
	ReplaceTeam(sMsg, TeamName(player.m_team));

	PlayerItemData data;
	if (m_pReceiver->GetPlayerItemData(player.m_nID, &data))
	{
		CString s;
		s.Format("%d", data.nFrags);
		ReplaceFrags(sMsg, (LPCTSTR)s);
		s.Format("%d", data.nDeaths);
		ReplaceDeaths(sMsg, (LPCTSTR)s);
		ReplaceRole(sMsg, (LPCTSTR)data.sRole);
	}
}

void GIRLLogParser::ReplacePlayer2(CString& sMsg, const CStatsPlayer& player)
{
	ReplaceName2(sMsg, Decorate(player.m_sName.c_str(), player.m_team));
	ReplaceTeam2(sMsg, TeamName(player.m_team));

	PlayerItemData data;
	if (m_pReceiver->GetPlayerItemData(player.m_nID, &data))
	{
		CString s;
		s.Format("%d", data.nFrags);
		ReplaceFrags2(sMsg, (LPCTSTR)s);
		s.Format("%d", data.nDeaths);
		ReplaceDeaths2(sMsg, (LPCTSTR)s);
		ReplaceRole2(sMsg, (LPCTSTR)data.sRole);
	}
}

void GIRLLogParser::OutputWithPlayerOnly(std::string& sLine, IRCMSG uMsg)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	CString sMsg = GetIRCMessage(uMsg);

	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
}

void GIRLLogParser::OutputWithTwoPlayers(std::string& sLine, IRCMSG uMsg, std::string sBefore2 /* = "" */)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	if (!sBefore2.empty())
	{
		int nPos = sLine.find(sBefore2);
		if (nPos != std::string::npos)
			sLine.erase(0, nPos);
	}
	CStatsPlayer commander;
	std::string sCommander = GetNextName(sLine, commander);

	CString sMsg = GetIRCMessage(uMsg);
	CString s;

	ReplacePlayer(sMsg, player);
	ReplacePlayer2(sMsg, commander);
	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
}

void GIRLLogParser::OutputIRCMessage(IRCMSG uMsg, const char* pLine)
{
	m_pReceiver->OutputIRCMessage(uMsg, pLine);
}

const char* GIRLLogParser::GetIRCMessage(IRCMSG uMsg)
{
	IGCLock lock(this, "GetIRCMessage");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());

	return (*i).second.m_sValue.c_str();
}

void GIRLLogParser::ResetDefaultIRCMessages()
{
	IGCLock lock(this, "ResetDefaultIRCMessages");
	for (IRCMSGMAP::iterator iMsg = m_mMsgs.begin(); iMsg != m_mMsgs.end(); iMsg++)
		(*iMsg).second.m_sValue = (*iMsg).second.m_pDefault;
}

void GIRLLogParser::SetIRCMessage(IRCMSG uMsg, const char* pValue)
{
	IGCLock lock(this, "GetIRCMessageStruct");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());
	
	(*i).second.m_sValue = pValue;
}

const char* GIRLLogParser::GetIRCMessageDefault(IRCMSG uMsg)
{
	IGCLock lock(this, "GetIRCMessageDefault");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());

	return (*i).second.m_pDefault;
}

const char* GIRLLogParser::GetIRCMessageMeaning(IRCMSG uMsg)
{
	IGCLock lock(this, "GetIRCMessageMeaning");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());

	return (*i).second.m_pMeaning;
}

bool GIRLLogParser::IsMessageRegistered(IRCMSG uMsg)
{
	return m_mMsgs.find(uMsg) != m_mMsgs.end();
}

bool GIRLLogParser::IsIRCMessageEnabled(IRCMSG uMsg)
{
	IGCLock lock(this, "IsIRCMessageEnabled");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	if (i == m_mMsgs.end())
		return false;

	return (*i).second.m_bEnabled;
}

void GIRLLogParser::EnableIRCMessage(IRCMSG uMsg, bool bEnable /* = true */)
{
	IGCLock lock(this, "EnableIRCMessage");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());

	(*i).second.m_bEnabled = bEnable;
}

const IRCCustomMsg* GIRLLogParser::GetIRCMessageStruct(IRCMSG uMsg)
{
	IGCLock lock(this, "GetIRCMessageStruct");
	IRCMSGMAP::iterator i = m_mMsgs.find(uMsg);
	ASSERT(i != m_mMsgs.end());
	return &(*i).second;
}

void GIRLLogParser::ReplaceRole(CString& s, const std::string& sReplacement)
{
	s.Replace("<role>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceRole2(CString& s, const std::string& sReplacement)
{
	s.Replace("<role2>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceName(CString& s, const std::string& sReplacement)
{
	s.Replace("<name>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceName2(CString& s, const std::string& sReplacement)
{
	s.Replace("<name2>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceTeam(CString& s, const std::string& sReplacement)
{
	s.Replace("<team>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceTeam2(CString& s, const std::string& sReplacement)
{
	s.Replace("<team2>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceFrags(CString& s, const std::string& sReplacement)
{
	s.Replace("<frags>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceFrags2(CString& s, const std::string& sReplacement)
{
	s.Replace("<frags2>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceDeaths(CString& s, const std::string& sReplacement)
{
	s.Replace("<deaths>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceDeaths2(CString& s, const std::string& sReplacement)
{
	s.Replace("<deaths2>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceWeapon(CString& s, const std::string& sReplacement)
{
	s.Replace("<weapon>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceServer(CString& s, const std::string& sReplacement)
{
	s.Replace("<server>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceSay(CString& s, const std::string& sReplacement)
{
	s.Replace("<say>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceMap(CString& s, const std::string& sReplacement)
{
	s.Replace("<map>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceOther(CString& s, const std::string& sReplacement)
{
	s.Replace("<other>", sReplacement.c_str());
}

void GIRLLogParser::ReplaceRest(CString& s)
{
	s.Replace("<name>", "");
	s.Replace("<name2>", "");
	s.Replace("<team>", "");
	s.Replace("<team2>", "");
	s.Replace("<weapon>", "");
	s.Replace("<say>", "");
	s.Replace("<role>", "");
	s.Replace("<role2>", "");
	s.Replace("<frags>", "");
	s.Replace("<frags2>", "");
	s.Replace("<deaths>", "");
	s.Replace("<deaths2>", "");
	s.Replace("<other>", "");
	s.Replace("<server>", m_sServerName.c_str());
	s.Replace("<host>", (LPCTSTR)m_pReceiver->GetServerName());
	s.Replace("<ip>", (LPCTSTR)m_pReceiver->GetLastIP());
	s.Replace("<map>", m_sMapName.c_str());
	CString sPort;
	sPort.Format("%u", m_pReceiver->GetServerPort());
	s.Replace("<port>", (LPCTSTR)sPort);
}

std::string GIRLLogParser::GetNextVar(std::string& sLine)
{
	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return "";

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return "";

	std::string sName = sLine.substr(nStart+1, nEnd-nStart-1);
	sLine = sLine.substr(nEnd+1);
	return sName;
}

std::string GIRLLogParser::GetNextName(std::string& sLine, CStatsPlayer& player, bool bAutoUpdate /* = true */)
{
	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return "";

	int nSep4 = sLine.find(">\"", nStart+1);
	if (nSep4 == std::string::npos)
		return "";

	int nSep3 = sLine.rfind("><", nSep4-1);
	if (nSep3 == std::string::npos)
		return "";

	int nSep2 = sLine.rfind("><", nSep3-1);
	if (nSep2 == std::string::npos)
		return "";

	int nSep1 = sLine.rfind("<", nSep2-1);
	if (nSep1 == std::string::npos)
		return "";


	std::string sName = sLine.substr(nStart+1, nSep1-nStart-1);
	std::string sID = sLine.substr(nSep1+1, nSep2-nSep1-1);
	std::string sWonID = sLine.substr(nSep2+2, nSep3-nSep2-2);
	std::string sTeam = sLine.substr(nSep3+2, nSep4-nSep3-2);

	player.m_sName = FixPlayerName(sName);
	player.m_nID = atol(sID.c_str());
	player.m_sWonID = sWonID;
	player.m_team = NameTeam(sTeam);
	player.m_sRole = "";

	if (bAutoUpdate)
		m_pReceiver->UpdatePlayer(new UpdatePlayerStruct(player));

	sLine = sLine.substr(nSep4+2);
	return sName;
}

std::string GIRLLogParser::FixPlayerName(std::string sName)
{
	for (int i=0; i<(int)sName.size(); i++)
	{
		char c = sName[i];
		if ((c < 32) || (c > 127)) {
			sName.erase(i, 1);
		}
	}
	return sName;
}

std::string GIRLLogParser::GetWeapon(std::string& sLine)
{
	int nStart = sLine.find("with \"");
	if (nStart == std::string::npos)
		return "";

	int nEnd = sLine.find('\"', nStart+6);
	if (nEnd == std::string::npos)
		return "";

	std::string sWeapon = sLine.substr(nStart+6, nEnd-nStart-6);
	sLine = sLine.substr(nEnd+1);
	return sWeapon;
}

std::string GIRLLogParser::GetDetail(std::string& sLine, LPCTSTR pDetail)
{
	std::string sFind = "(";
	sFind += pDetail;
	sFind += " \"";

	int nStart = sLine.find(sFind);
	if (nStart == std::string::npos)
		return "";

	int nEnd = sLine.find('\"', nStart+sFind.size());
	if (nEnd == std::string::npos)
		return "";

	std::string sDetail = sLine.substr(nStart+sFind.size(), nEnd-nStart-sFind.size());
	sLine = sLine.substr(nEnd+2);
	return sDetail;
}

std::string GIRLLogParser::GetRole(std::string& sLine)
{
	int nStart = sLine.find("role to \"");
	if (nStart == std::string::npos)
		return "";

	int nEnd = sLine.find('\"', nStart+9);
	if (nEnd == std::string::npos)
		return "";

	std::string sRole = sLine.substr(nStart+9, nEnd-nStart-9);

	return sRole;
}

Team GIRLLogParser::GetTeam(std::string& sLine)
{
	int nPos = sLine.find("team \"");
	if (nPos == std::string::npos)
		return TEAM_NONE;

	int nPos2 = sLine.find('\"', nPos+6);
	if (nPos2 == std::string::npos)
		return TEAM_NONE;

	std::string sTeam = sLine.substr(nPos+6, nPos2-nPos-6);
	return NameTeam(sTeam);
}

void GIRLLogParser::RegisterMessage(IRCMSG uMsg, const char* pDefault, const char* pMeaning, bool bEnabled, MsgType eType) 
{
	ASSERT(m_pReceiver == NULL);
	m_mMsgs[uMsg] = IRCCustomMsg(uMsg, pDefault, pMeaning, "", bEnabled, eType);
}

void GIRLLogParser::UnregisterMessage(IRCMSG uMsg) 
{
	ASSERT(m_pReceiver == NULL);
	m_mMsgs.erase(uMsg);
}

void GIRLLogParser::SetIRCTopic()
{
	IGCLock lock(this, "SetIRCTopic");
	if (IsIRCMessageEnabled(LT_TOPIC))
	{
		CString sMsg = GetIRCMessage(LT_TOPIC);
		ReplaceRest(sMsg);
		m_pReceiver->SetIRCTopic(sMsg);
	}
}

void GIRLLogParser::SetMapName(const char* pName)
{
	if (m_sMapName.compare(pName) != 0)
	{
		m_sMapName = pName;
		m_pReceiver->SetMapName(new SetMapNameStruct(pName));
		SetIRCTopic();
	}
}

void GIRLLogParser::SetTeamColorDefaults()
{
	IGCLock lock(this, "SetTeamColorDefaults");
	for (int nTeam = 0; nTeam < TEAM_COUNT; nTeam++)
	{
		SetTeamColor((Team)nTeam, GetTeamColorDefault((Team)nTeam));
	}
}

IRCMSGMAP& GIRLLogParser::LockMsgMap()
{
	Lock("LockMsgMap");
	return m_mMsgs;
}

void GIRLLogParser::UnlockMsgMap()
{
	Unlock();
}

void GIRLLogParser::SetActive(bool bActive /* = true */)
{
	m_bActive = bActive;
}

bool GIRLLogParser::IsActive()
{
	return m_bActive;
}
