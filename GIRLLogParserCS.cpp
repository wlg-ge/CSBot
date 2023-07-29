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

// GIRLLogParserCS.cpp: implementation of the GIRLLogParserCS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserCS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TEAM_T TEAM_ONE
#define TEAM_CT TEAM_TWO

// Log line types are also IRC message IDs in CS
#define LT_ROUND_START		1
#define LT_ROUND_END		2
#define LT_JOINED			3
#define LT_BOMB_PLANTED		5
#define LT_DEFUSE_NOKIT		6
#define LT_DEFUSE_WITHKIT	7
#define LT_DEFUSE			8
#define LT_KILLED_HOSTAGE	9
#define LT_TOUCHED_HOSTAGE	10
#define LT_RESCUED_HOSTAGE	11
// two different lines
#define LT_TEAM_CT_WIN		12
#define LT_TEAM_T_WIN		13
// produce the same IRC message
#define LT_TEAMWIN			14

#define LT_TEAM_DRAW		15
#define LT_VIP_ASSASIN		16
#define LT_VIP_ESCAPE		17
#define LT_BOMB_SPAWNED		18
#define LT_BOMB_GOT			19
#define LT_BOMB_DROPPED		20
#define LT_BECAME_VIP		21
#define LT_ALL_RESCUED		22
#define LT_TARGET_BOMBED	23
#define LT_TARGET_SAVED		24
#define LT_BOMB_DEFUSED		25
#define LT_VIP_ASSASINATED	26
#define LT_VIP_ESCAPED		27
#define LT_NONE_ESCAPED		28
#define LT_NOT_RESCUED		29
#define LT_VIP_NOT_ESCAPED	30
#define LT_RESTART_ROUND	31

GIRLLogParserCS::GIRLLogParserCS()
{
	m_nTScore = 0;
	m_nCTScore = 0;
	roundRestarting=false;

	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<server>\x0f <host>:<port> <map> <t-score>  <ct-score>",
		"This is the format of the IRC channel's topic for a Counter-Strike server",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_JOINED,
		"<name> joined the <team> team",
		"The player joined a team, or switched a team",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_BOMB_PLANTED,
		"<name> planted the bomb",
		"The played planted the bomb",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_DEFUSE_NOKIT, 
		"<name> is defusing the bomb without defuse-kit",
		"The player started to defuse the bomb without using a defusal-kit",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_DEFUSE_WITHKIT, 
		"<name> is defusing the bomb with defuse-kit",
		"The player started to defuse the bomb using a defusal-kit",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_DEFUSE,
		"<name> has defused the bomb",
		"The player finished defusing the bomb",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_KILLED_HOSTAGE, 
		"<name> killed a hostage",
		"The player killed a hostage",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TOUCHED_HOSTAGE, 
		"<name> is taking a hostage",
		"The player is \"use\"ing a hostage, telling him to follow him",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_RESCUED_HOSTAGE,
		"<name> rescued a hostage",
		"The player brought a hostage to the rescue point",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_VIP_ASSASIN, 
		"<name> assassinated the VIP",
		"The player killed the VIP",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_VIP_ESCAPE,
		"<name> escaped as VIP",
		"The player, who is VIP, escaped through the escape zone",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_BOMB_GOT, 
		"<name> picked up the bomb",
		"The player found the bomb and picked it up",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_BOMB_DROPPED, 
		"<name> dropped the bomb",
		"The player dropped the bomb (either intentionally, or he was killed)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_BOMB_SPAWNED, 
		"<name> has the bomb",
		"The player started this round with the bomb",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_BECAME_VIP,
		"<name> is the VIP",
		"The player is the VIP this round",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TEAMWIN, 
		"<team> win the round. Score is <t-score>  <ct-score>",
		"Round ended because the one team killed all the players of the other team (any map)",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_TEAM_DRAW, 
		"Round draw. Score is <t-score>  <ct-score>",
		"Round ended with a draw (any map)",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_ROUND_START,
		"--- Round started ---",
		"Round starts.",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_ROUND_END,
		"--- Round ended ---",
		"Round ended. and a new round starts",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_ALL_RESCUED, 
		"All hostages rescued. Score is <t-score>  <ct-score>",
		"Round ended because all the hostages were rescued. This is a CT win (cs_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_NOT_RESCUED, 
		"Hostages not rescued. Score is <t-score>  <ct-score>",
		"Round ended because of a time out, and not enough hostages were rescued (cs_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_VIP_NOT_ESCAPED,
		"VIP did not escape. Score is <t-score>  <ct-score>",
		"Round ended because of a time out, and the VIP did not escape (as_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TARGET_BOMBED, 
		"Bomb exploded. Score is <t-score>  <ct-score>",
		"Round ended because the bomb exploded. This is a T win (de_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TARGET_SAVED, 
		"Bomb was not planted. Score is <t-score>  <ct-score>",
		"Round ended because of a time out, and the bomb was not planted (de_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_BOMB_DEFUSED,
		"Bomb has been defused. Score is <t-score>  <ct-score>",
		"Round ended because the bomb was defused. This is a CT win (de_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_VIP_ESCAPED, 
		"VIP has escaped. Score is <t-score>  <ct-score>",
		"Round ended because the VIP escaped. This is a CT win (as_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_VIP_ASSASINATED, 
		"VIP has been assassinated. Score is <t-score>  <ct-score>",
		"Round ended because the VIP was killed. This is a T win (as_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_NONE_ESCAPED,
		"No terrorists escaped. Score is <t-score>  <ct-score>",
		"Round ended because of a time out, and not enough Terrorists escaped (es_ map)",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_RESTART_ROUND,
		"Round will restart in <other> seconds",
		"A restart-round has been launched",
		true,
		MsgType_Gameflow);
}

GIRLLogParserCS::~GIRLLogParserCS()
{

}

void GIRLLogParserCS::Init()
{
	m_nCTScore = 0;
	m_nTScore = 0;
	roundRestarting=false;
}

IRCMSG GIRLLogParserCS::RecognizeLine(const std::string& sLine)
{
	if (CONTAINS("triggered \"Round_Start\""))
		return LT_ROUND_START;
	if (CONTAINS("triggered \"Round_End\""))
		return LT_ROUND_END;
	if (CONTAINS("\" joined team \""))
		return LT_JOINED;
	if (CONTAINS("\" triggered \"Planted_The_Bomb\""))
		return LT_BOMB_PLANTED;
	if (CONTAINS("\" triggered \"Begin_Bomb_Defuse_Without_Kit\""))
		return LT_DEFUSE_NOKIT;
	if (CONTAINS("\" triggered \"Begin_Bomb_Defuse_With_Kit\""))
		return LT_DEFUSE_WITHKIT;
	if (CONTAINS("\" triggered \"Defused_The_Bomb\""))
		return LT_DEFUSE;
	if (CONTAINS("\" triggered \"Killed_A_Hostage\""))
		return LT_KILLED_HOSTAGE;
	if (CONTAINS("\" triggered \"Touched_A_Hostage\""))
		return LT_TOUCHED_HOSTAGE;
	if (CONTAINS("\" triggered \"Rescued_A_Hostage\""))
		return LT_RESCUED_HOSTAGE;
	if (CONTAINS("\" triggered \"CTs_Win\" ("))
		return LT_TEAM_CT_WIN;
	if (CONTAINS("\" triggered \"Terrorists_Win\" ("))
		return LT_TEAM_T_WIN;
	if (CONTAINS(" triggered \"Round_Draw\" ("))
		return LT_TEAM_DRAW;
	if (CONTAINS("\" triggered \"Assassinated_The_VIP\""))
		return LT_VIP_ASSASIN;
	if (CONTAINS("\" triggered \"Escaped_As_VIP\""))
		return LT_VIP_ESCAPE;
	if (CONTAINS("\" triggered \"Spawned_With_The_Bomb\""))
		return LT_BOMB_SPAWNED;
	if (CONTAINS("\" triggered \"Got_The_Bomb\""))
		return LT_BOMB_GOT;
	if (CONTAINS("\" triggered \"Dropped_The_Bomb\""))
		return LT_BOMB_DROPPED;
	if (CONTAINS("\" triggered \"Became_VIP\""))
		return LT_BECAME_VIP;
	if (CONTAINS("\" triggered \"All_Hostages_Rescued\""))
		return LT_ALL_RESCUED;
	if (CONTAINS("\" triggered \"Target_Bombed\" ("))
		return LT_TARGET_BOMBED;
	if (CONTAINS("\" triggered \"Target_Saved\" ("))
		return LT_TARGET_SAVED;
	if (CONTAINS("\" triggered \"Bomb_Defused\" ("))
		return LT_BOMB_DEFUSED;
	if (CONTAINS("\" triggered \"VIP_Assassinated\" ("))
		return LT_VIP_ASSASINATED;
	if (CONTAINS("\" triggered \"VIP_Escaped\" ("))
		return LT_VIP_ESCAPED;
	if (CONTAINS("\" triggered \"CTs_PreventEscape\" ("))
		return LT_NONE_ESCAPED;
	if (CONTAINS("\" triggered \"Hostages_Not_Rescued\" ("))
		return LT_NOT_RESCUED;
	if (CONTAINS("\" triggered \"VIP_Not_Escaped\" ("))
		return LT_VIP_NOT_ESCAPED;
	if (STARTSWITH("World triggered \"Restart_Round_("))
		return LT_RESTART_ROUND;
	return LT_UNKNOWN;
}

void GIRLLogParserCS::ProcessLine(std::string& sLine)
{
	IRCMSG nLineType = RecognizeLine(sLine);
	switch (nLineType)
	{
	case LT_ROUND_START:
		LineRoundStart(sLine);
		break;
	case LT_ROUND_END:
		LineRoundEnd(sLine);
		break;
	case LT_JOINED:
		LineJoined(sLine);
		break;
	case LT_TEAM_CT_WIN:
		LineWinCT(sLine);
		break;
	case LT_TEAM_T_WIN:
		LineWinT(sLine);
		break;
	case LT_RESTART_ROUND:
		LineRestartRound(sLine);
		break;

	case LT_BOMB_PLANTED:
	case LT_DEFUSE_NOKIT:
	case LT_DEFUSE_WITHKIT:
	case LT_DEFUSE:
	case LT_KILLED_HOSTAGE:
	case LT_TOUCHED_HOSTAGE:
	case LT_RESCUED_HOSTAGE:
	case LT_VIP_ASSASIN:
	case LT_VIP_ESCAPE:
	case LT_BOMB_SPAWNED:
	case LT_BOMB_GOT:
	case LT_BOMB_DROPPED:
	case LT_BECAME_VIP:
		OutputWithPlayerOnly(sLine, nLineType);
		break;

	case LT_TEAM_DRAW:
	case LT_ALL_RESCUED:
	case LT_TARGET_BOMBED:
	case LT_TARGET_SAVED:
	case LT_BOMB_DEFUSED:
	case LT_VIP_ASSASINATED:
	case LT_VIP_ESCAPED:
	case LT_NONE_ESCAPED:
	case LT_NOT_RESCUED:
	case LT_VIP_NOT_ESCAPED:
		OutputWithScoresOnly(sLine, nLineType);
		break;

	default:
		GIRLLogParserDefault::ProcessLine(sLine);
	}
}

const char* GIRLLogParserCS::TeamName(Team t)
{
	switch (t)
	{
	case TEAM_T:
		return "T";
	case TEAM_CT:
		return "CT";
	case TEAM_SPECTATOR:
		return "Spec";
	}
	return "";
}

const char* GIRLLogParserCS::TeamDescription(Team t)
{
	switch (t)
	{
	case TEAM_T:
		return "Terrorists";
	case TEAM_CT:
		return "Counter-Terrorists";
	case TEAM_SPECTATOR:
		return "Spectators";
	}
	return "";
}

std::string GIRLLogParserCS::GetTeamColor(Team t)
{
	IGCLock lock(this, "CS::GetTeamColor");
	switch (t)
	{
	case TEAM_T:
		return m_sTColor;
	case TEAM_CT:
		return m_sCTColor;
	case TEAM_SPECTATOR:
		return m_sSpecColor;
	}
	return "";
}

const char* GIRLLogParserCS::GetTeamColorDefault(Team t)
{
	switch (t)
	{
	case TEAM_T:
		return "04";
	case TEAM_CT:
		return "12";
	case TEAM_SPECTATOR:
		return "14";
	}
	return "";
}

void GIRLLogParserCS::SetTeamColor(Team t, const std::string& sColor)
{
	IGCLock lock(this, "CS::SetTeamColor");
	switch (t)
	{
	case TEAM_T:
		m_sTColor = sColor;
		break;
	case TEAM_CT:
		m_sCTColor = sColor;
		break;
	case TEAM_SPECTATOR:
		m_sSpecColor = sColor;
		break;
	}
}

Team GIRLLogParserCS::NameTeam(std::string& sTeam)
{
	if (strcmp(sTeam.c_str(), "TERRORIST") == 0)
		return TEAM_T;
	if (strcmp(sTeam.c_str(), "CT") == 0)
		return TEAM_CT;
	if (strcmp(sTeam.c_str(), "SPECTATOR") == 0)
		return TEAM_SPECTATOR;

	return TEAM_NONE;
}

const char** GIRLLogParserCS::GameTypeName()
{
	static const char* pNames[] = {
		"cstrike",
		NULL
	};
	return pNames;
}

const char* GIRLLogParserCS::GameTypeDescription()
{
	return "Counter Strike";
}

CString GIRLLogParserCS::GetAdditionalTags()
{
	return GIRLLogParserDefault::GetAdditionalTags() +
		"<t-score> - Terrorists score\n"
		"<ct-score> - Counter-Terrorists score\n";
}

int GIRLLogParserCS::GameTypeImage()
{
	return IMAGE_CS;
}

void GIRLLogParserCS::ReplaceTScore(CString& s)
{
	s.Replace("<t-score>", DecorateScore(TEAM_T, m_nTScore).c_str());
}

void GIRLLogParserCS::ReplaceCTScore(CString& s)
{
	s.Replace("<ct-score>", DecorateScore(TEAM_CT, m_nCTScore).c_str());
}

void GIRLLogParserCS::ReplaceRest(CString& sMsg)
{
	ReplaceTScore(sMsg);
	ReplaceCTScore(sMsg);
	GIRLLogParserDefault::ReplaceRest(sMsg);
}

bool GIRLLogParserCS::GetScores(std::string& sLine)
{
	int nStart, nEnd;
	nStart = sLine.find("(CT \"");
	if (nStart == std::string::npos)
		return false;
	nEnd = sLine.find('\"', nStart+6);
	if (nEnd == std::string::npos)
		return false;
	m_nCTScore = atoi(sLine.substr(nStart+5).c_str());

	sLine = sLine.substr(nEnd);

	nStart = sLine.find("(T \"");
	if (nStart == std::string::npos)
		return false;
	nEnd = sLine.find('\"', nStart+5);
	if (nEnd == std::string::npos)
		return false;
	m_nTScore = atoi(sLine.substr(nStart+4).c_str());

	sLine = sLine.substr(nEnd);

	return true;
}

void GIRLLogParserCS::LineRoundStart(std::string& sLine)
{
	if ( roundRestarting == true ) {
		roundRestarting=false;
	}
	CString sMsg = GetIRCMessage(LT_ROUND_START);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_ROUND_START, sMsg);
}

void GIRLLogParserCS::LineRoundEnd(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_ROUND_END);

	ReplaceRest(sMsg);
	OutputIRCMessage(LT_ROUND_END, sMsg);
}

void GIRLLogParserCS::LineJoined(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);
	// set team
	player.m_team = GetTeam(sLine);
	// and update
	m_pReceiver->UpdatePlayer(new UpdatePlayerStruct(player));

	CString sMsg = GetIRCMessage(LT_JOINED);
	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_JOINED, sMsg);
}

void GIRLLogParserCS::LineWinT(std::string& sLine)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(LT_TEAMWIN);
	ReplaceTeam(sMsg, TeamDescription(TEAM_T));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAMWIN, sMsg);
	SetIRCTopic();
}

void GIRLLogParserCS::LineWinCT(std::string& sLine)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(LT_TEAMWIN);
	ReplaceTeam(sMsg, TeamDescription(TEAM_CT));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAMWIN, sMsg);
	SetIRCTopic();
}

void GIRLLogParserCS::LineRestartRound(std::string& sLine)
{
	roundRestarting=true;

	int nPos = sLine.find('(');
	if (nPos == std::string::npos)
		return;

	int nSec = atoi(sLine.c_str()+nPos+1);
	char c[32];
	sprintf(c, "%d", nSec);

	CString sMsg = GetIRCMessage(LT_RESTART_ROUND);
	ReplaceOther(sMsg, c);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_RESTART_ROUND, sMsg);
}

void GIRLLogParserCS::OutputWithScoresOnly(std::string& sLine, IRCMSG uMsg)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(uMsg);

	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
	SetIRCTopic();
}

void GIRLLogParserCS::SetMapName(const char* pName)
{
	m_nCTScore = 0;
	m_nTScore = 0;
	GIRLLogParserDefault::SetMapName(pName);
}
