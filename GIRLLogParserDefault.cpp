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

// GIRLLogParserDefault.cpp: implementation of the GIRLLogParserDefault class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserDefault.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GIRLLogParserDefault::GIRLLogParserDefault()
{
	RegisterMessage(LT_START_MAP,
		"Server changed map to <map>",
		"Server has changed map.\nEither the map timed out or a manual changelevel was issued",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_ENTERED, 
		"<name> entered the server",
		"The player connected to the server, and entered the game (and is now choosing a team)",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_DISCONNECTED,
		"<name> disconnected from server",
		"The player disconnected from the server",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_ATTACK,
		"<name> attacked <name2> with <weapon> causing <other> damage",
		"The player shot or otherwise harmed the other player",
		false,
		MsgType_Gameflow);
	RegisterMessage(LT_ATTACK_FRIENDLY,
		"<name> attacked his teammate <name2> with <weapon> causing <other> damage",
		"The player shot or otherwise harmed a teammate",
		false,
		MsgType_Gameflow);
	RegisterMessage(LT_KILL,
		"<name> killed <name2> with <weapon>",
		"The player killed the other player",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_KILL_FRIENDLY,
		"<name> killed his teammate <name2> with <weapon>",
		"The player killed a teammate",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SUICIDE,
		"<name> suicided using <weapon>",
		"The player commited suicide (fell off a roof, crushed, etc)",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_CHANGE_NAME, 
		"<name> changed name to <name2>",
		"The player changed his nickname",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SAY, 
		"<name>: <say>",
		"The player spoke on the regular chat",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SAY_DEAD, 
		"<name>(dead): <say>",
		"The dead player spoke on the regular chat",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SAY_TEAM, 
		"<name>(team): <say>",
		"The player spoke on the team chat",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SAY_TEAM_DEAD,
		"<name>(dead)(team): <say>",
		"The dead player spoke on the team chat",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SAY_SERVER,
		"SERVER: <say>",
		"The server operator spoke (\"say\" on the server console, or \"rcon say\")",
		true,
		MsgType_Other);
	RegisterMessage(LT_SERVER_SHUTDOWN, 
		"Server has shut down",
		"The server has shut down",
		true,
		MsgType_Other);
	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<server>\x0f <host>:<port> <map>",
		"This is the format of the IRC channel's topic",
		true,
		MsgType_Gameflow);
};

int GIRLLogParserDefault::RecognizeLine(const std::string& sLine)
{
	if (CONTAINS("Started map \""))
		return LT_START_MAP;
	if (CONTAINS("Loading map \""))
		return LT_START_MAP;
	if (CONTAINS("\" entered the game"))
		return LT_ENTERED;
	if (CONTAINS("\" changed name to \""))
		return LT_CHANGE_NAME;
	if (CONTAINS("\" killed \"") && CONTAINS("\" with \""))
		return LT_KILL;
	if (CONTAINS("\" attacked \"") && CONTAINS("\" with \""))
		return LT_ATTACK;
	if (CONTAINS("\" committed suicide with \""))
		return LT_SUICIDE;
	if (CONTAINS("\" disconnected"))
		return LT_DISCONNECTED;
	if (EQUALS("Server shutdown"))
		return LT_SERVER_SHUTDOWN;
	if (CONTAINS("Server say \""))
		return LT_SAY_SERVER;
	if (CONTAINS("\" say \""))
	{
		if (IsMessageRegistered(LT_SAY_DEAD))
			if (ENDSWITH("\" (dead)\n"))
				return LT_SAY_DEAD;
		return LT_SAY;
	}
	if (CONTAINS("\" say_team \""))
	{
		if (IsMessageRegistered(LT_SAY_DEAD))
			if (ENDSWITH("\" (dead)\n"))
				return LT_SAY_TEAM_DEAD;
		return LT_SAY_TEAM;
	}
	return LT_UNKNOWN;
}

void GIRLLogParserDefault::ProcessLine(std::string& sLine)
{
	int nLineType = RecognizeLine(sLine);
	switch (nLineType)
	{
	case LT_START_MAP:
		LineStartMap(sLine);
		break;
	case LT_ENTERED:
		LineEntered(sLine);
		break;
	case LT_CHANGE_NAME:
		LineChangeName(sLine);
		break;
	case LT_KILL:
		LineKill(sLine);
		break;
	case LT_SUICIDE:
		LineSuicide(sLine);
		break;
	case LT_ATTACK:
		LineAttack(sLine);
		break;
	case LT_DISCONNECTED:
		LineDisconnect(sLine);
		break;
	case LT_SERVER_SHUTDOWN:
		LineServerShutdown(sLine);
		break;
	case LT_SAY_SERVER:
		LineSayServer(sLine);
		break;
	case LT_SAY:
		LineSay(sLine);
		break;
	case LT_SAY_DEAD:
		LineSayDead(sLine);
		break;
	case LT_SAY_TEAM:
		LineSayTeam(sLine);
		break;
	case LT_SAY_TEAM_DEAD:
		LineSayTeamDead(sLine);
		break;
	}
}

const char* GIRLLogParserDefault::TeamName(Team t)
{
	return "";
}

const char* GIRLLogParserDefault::TeamDescription(Team t)
{
	return TeamName(t);
}

std::string GIRLLogParserDefault::GetTeamColor(Team t)
{
	return "";
}

const char* GIRLLogParserDefault::GetTeamColorDefault(Team t)
{
	return "";
}

void GIRLLogParserDefault::SetTeamColor(Team t, const std::string& sColor)
{
}

Team GIRLLogParserDefault::NameTeam(std::string& sTeam)
{
	return TEAM_NONE;
}

const char** GIRLLogParserDefault::GameTypeName()
{
	static const char* pNames[] = {
		"",
		NULL
	};
	return pNames;
}

const char* GIRLLogParserDefault::GameTypeDescription()
{
	return "Unknown game";
}

int GIRLLogParserDefault::GameTypeImage()
{
	return IMAGE_HL;
}

void GIRLLogParserDefault::LineStartMap(std::string& sLine)
{
	// find position of map name
	int nQuot = sLine.find("\"");
	if (nQuot == std::string::npos)
		return;

	nQuot++;
	int nQuot2 = sLine.find("\"", nQuot);
	if (nQuot2 == std::string::npos)
		return;

	int nLen = nQuot2-nQuot;
	// read it and set current map in server
	std::string sMapName = sLine.substr(nQuot, nLen);
	if (sMapName.compare(GetMapName()) != 0) 
	{
		SetMapName(sMapName.c_str());

		CString sMsg = GetIRCMessage(LT_START_MAP);
		ReplaceRest(sMsg);
		OutputIRCMessage(LT_START_MAP, sMsg);
	}
}

void GIRLLogParserDefault::LineSay(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return;

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return;

	CString sMsg = GetIRCMessage(LT_SAY);
	ReplacePlayer(sMsg, player);
	ReplaceSay(sMsg, sLine.substr(nStart+1, nEnd-nStart-1).c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SAY, sMsg);
	m_pReceiver->Summon(new SummonStruct(sLine.substr(nStart+1, nEnd-nStart-1).c_str(), player, FALSE));
}

void GIRLLogParserDefault::LineSayDead(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return;

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return;

	CString sMsg = GetIRCMessage(LT_SAY_DEAD);
	ReplacePlayer(sMsg, player);
	ReplaceSay(sMsg, sLine.substr(nStart+1, nEnd-nStart-1).c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SAY_DEAD, sMsg);
	m_pReceiver->Summon(new SummonStruct(sLine.substr(nStart+1, nEnd-nStart-1).c_str(), player, FALSE));
}

void GIRLLogParserDefault::LineSayTeam(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return;

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return;

	CString sMsg = GetIRCMessage(LT_SAY_TEAM);
	ReplacePlayer(sMsg, player);
	ReplaceSay(sMsg, sLine.substr(nStart+1, nEnd-nStart-1).c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SAY_TEAM, sMsg);
	m_pReceiver->Summon(new SummonStruct(sLine.substr(nStart+1, nEnd-nStart-1).c_str(), player, TRUE));
}

void GIRLLogParserDefault::LineSayTeamDead(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return;

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return;

	CString sMsg = GetIRCMessage(LT_SAY_TEAM_DEAD);
	ReplacePlayer(sMsg, player);
	ReplaceSay(sMsg, sLine.substr(nStart+1, nEnd-nStart-1).c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SAY_TEAM_DEAD, sMsg);
	m_pReceiver->Summon(new SummonStruct(sLine.substr(nStart+1, nEnd-nStart-1).c_str(), player, TRUE));
}

void GIRLLogParserDefault::LineSayServer(std::string& sLine)
{
	int nStart = sLine.find('\"');
	if (nStart == std::string::npos)
		return;

	int nEnd = sLine.find('\"', nStart+1);
	if (nEnd == std::string::npos)
		return;

	CString sMsg = GetIRCMessage(LT_SAY_SERVER);
	ReplaceSay(sMsg, sLine.substr(nStart+1, nEnd-nStart-1).c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SAY_SERVER, sMsg);
}

void GIRLLogParserDefault::LineServerShutdown(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_SERVER_SHUTDOWN);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SERVER_SHUTDOWN, sMsg);
}

void GIRLLogParserDefault::LineDisconnect(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	CString sMsg = GetIRCMessage(LT_DISCONNECTED);
	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);

	m_pReceiver->PlayerDisconnected(player.m_nID);

	OutputIRCMessage(LT_DISCONNECTED, sMsg);
}

void GIRLLogParserDefault::LineKill(std::string& sLine)
{
	// fetch event parameters
	CStatsPlayer victim, killer;
	std::string sKiller = GetNextName(sLine, killer);
	std::string sVictim = GetNextName(sLine, victim);
	std::string sWeapon = GetWeapon(sLine);

	m_pReceiver->UpdateFrag(killer.m_nID);
	m_pReceiver->UpdateDeath(victim.m_nID);

	IRCMSG uMsg = (killer.m_team == victim.m_team) ? LT_KILL_FRIENDLY : LT_KILL;
	CString sMsg = GetIRCMessage(uMsg);
	
	ReplacePlayer(sMsg, killer);
	ReplacePlayer2(sMsg, victim);
	ReplaceWeapon(sMsg, sWeapon);
	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
}

void GIRLLogParserDefault::LineSuicide(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);
	std::string sWeapon = GetWeapon(sLine);

	CString sMsg = GetIRCMessage(LT_SUICIDE);
	ReplacePlayer(sMsg, player);
	ReplaceWeapon(sMsg, sWeapon.c_str());
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_SUICIDE, sMsg);
}

void GIRLLogParserDefault::LineAttack(std::string& sLine)
{
	CStatsPlayer victim, killer;
	std::string sKiller = GetNextName(sLine, killer);
	std::string sVictim = GetNextName(sLine, victim);
	std::string sWeapon = GetWeapon(sLine);
	std::string sDamage = GetDetail(sLine, "damage");

	IRCMSG uMsg = (killer.m_team == victim.m_team) ? LT_ATTACK_FRIENDLY : LT_ATTACK;
	CString sMsg = GetIRCMessage(uMsg);

	ReplacePlayer(sMsg, killer);
	ReplacePlayer2(sMsg, victim);
	ReplaceWeapon(sMsg, sWeapon);
	ReplaceOther(sMsg, sDamage);
	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
}

void GIRLLogParserDefault::LineChangeName(std::string& sLine)
{
	CStatsPlayer player;
	std::string sFrom = GetNextName(sLine, player);
	int nPos = sLine.find('"');
	if (nPos == std::string::npos)
		return;
	int nPos2 = sLine.find('"', nPos+1);
	std::string sTo = sLine.substr(nPos+1, nPos2-nPos-1);

	// first send IRC output with both of the names
	CString sMsg = GetIRCMessage(LT_CHANGE_NAME);
	ReplacePlayer(sMsg, player);
	ReplaceName2(sMsg, Decorate(sTo.c_str(), player.m_team));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_CHANGE_NAME, sMsg);

	// then update the UI with the new name
	player.m_sName = sTo.c_str();
	m_pReceiver->UpdatePlayer(new UpdatePlayerStruct(player));
}

void GIRLLogParserDefault::LineEntered(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	CString sMsg = GetIRCMessage(LT_ENTERED);

	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_ENTERED, sMsg);
}

