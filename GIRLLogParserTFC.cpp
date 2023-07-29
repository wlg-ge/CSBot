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

// GIRLLogParserTFC.cpp: implementation of the GIRLLogParserTFC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserTFC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TEAM_R TEAM_ONE
#define TEAM_B TEAM_TWO

// Log line types are also IRC message IDs in TFC
#define LT_ROUND_START		1
#define LT_ROUND_END		2
#define LT_JOINED			3
#define LT_SPAWN			5
#define LT_SENTRY_BUILD		6
#define LT_SENTRY_UPGRADE2	7
#define LT_SENTRY_UPGRADE3	8
#define LT_SENTRY_REPAIR	9
#define LT_SENTRY_DESTROYED	10
#define LT_DISPENSER_BUILD	11
#define LT_DISPENSER_DESTROYED		16
#define LT_TELE_ENTRANCE	27
#define LT_TELE_EXIT		19
#define LT_TELE_XDESTROYED	20
#define LT_TELE_EDESTROYED	18
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// two different lines
#define LT_TEAM_CT_WIN		12
#define LT_TEAM_T_WIN		13
// produce the same IRC message
#define LT_TEAMWIN			14

#define LT_TEAM_DRAW		15
#define LT_VIP_ESCAPE		17
#define LT_BECAME_VIP		21
#define LT_ALL_RESCUED		22
#define LT_TARGET_BOMBED	23
#define LT_TARGET_SAVED		24
#define LT_BOMB_DEFUSED		25
#define LT_VIP_ASSASINATED	26
#define LT_NONE_ESCAPED		28
#define LT_NOT_RESCUED		29
#define LT_VIP_NOT_ESCAPED	30
#define LT_RESTART_ROUND	31

GIRLLogParserTFC::GIRLLogParserTFC()
{
	m_nBScore = 0;
	m_nRScore = 0;

	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<server>\x0f <host>:<port> <map> <r-score>  <b-score>",
		"This is the format of the IRC channel's topic for a Team-Fortress server",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_JOINED,
		"<name> joined the <team> team",
		"The player joined a team, or switched a team",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_SPAWN,
		"<name> has spawned",
		"The played spawned",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_SENTRY_BUILD, 
		"<name> has built a sentry turret",
		"The player built a sentry turret",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_SENTRY_UPGRADE2, 
		"<name> has upgraded his sentry turret to level 2",
		"The player upgraded his sentry turret to level 2",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_SENTRY_UPGRADE3,
		"<name> has upgraded his sentry turret to level 3",
		"The player upgraded his sentry turret to level 3",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_SENTRY_REPAIR, 
		"<name> has repaired the sentry turret",
		"The player repaired the sentry turret",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_SENTRY_DESTROYED, 
		"<name> has destroyed a sentry turret",
		"The player destroyed a sentry turret",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_DISPENSER_BUILD,
		"<name> has built a dispenser",
		"The player built a dispenser",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_DISPENSER_DESTROYED, 
		"<name> has destroyed a dispenser",
		"The player destroyed a dispenser",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TELE_ENTRANCE,
		"<name> has built a teleporter entrance",
		"The player built a teleporter entrance",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TELE_EXIT, 
		"<name> has built a teleporter exit",
		"The player built a teleporter exit",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TELE_XDESTROYED, 
		"<name> has destroyed a teleporter exit",
		"The player destroyed a teleporter exit",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TELE_EDESTROYED, 
		"<name> has destroyed a teleporter entrance",
		"The player destroyed a teleporter entrance",
		true,
		MsgType_Objectives);
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
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
	RegisterMessage(LT_VIP_ESCAPE, 
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

GIRLLogParserTFC::~GIRLLogParserTFC()
{

}

void GIRLLogParserTFC::Init()
{
	m_nBScore = 0;
	m_nRScore = 0;
}

IRCMSG GIRLLogParserTFC::RecognizeLine(const std::string& sLine)
{
	if (CONTAINS("triggered \"Round_Start\""))
		return LT_ROUND_START;
	if (CONTAINS("triggered \"Round_End\""))
		return LT_ROUND_END;
	if (CONTAINS("\" joined team \""))
		return LT_JOINED;
	if (CONTAINS("\" triggered \"info_player_teamspawn\""))
		return LT_SPAWN;
	if (CONTAINS("\" triggered \"Sentry_Built_Level_1\""))
		return LT_SENTRY_BUILD;
	if (CONTAINS("\" triggered \"Sentry_Upgrade_Level_2\""))
		return LT_SENTRY_UPGRADE2;
	if (CONTAINS("\" triggered \"Sentry_Upgrade_Level_3\""))
		return LT_SENTRY_UPGRADE3;
	if (CONTAINS("\" triggered \"Sentry_Repair\""))
		return LT_SENTRY_REPAIR;
	if (CONTAINS("\" triggered \"Sentry_Destroyed\""))
		return LT_SENTRY_DESTROYED;
	if (CONTAINS("\" triggered \"Built_Dispenser\""))
		return LT_DISPENSER_BUILD;
	if (CONTAINS("\" triggered \"Dispenser_Destroyed\""))
		return LT_DISPENSER_DESTROYED;
	if (CONTAINS("\" triggered \"Teleporter_Entrance_Finished\" ("))
		return LT_TELE_ENTRANCE;
	if (CONTAINS("\" triggered \"Teleporter_Exit_Finished\""))
		return LT_TELE_EXIT;
	if (CONTAINS("\" triggered \"Teleporter_Exit_Destroyed\""))
		return LT_TELE_XDESTROYED;
	if (CONTAINS("\" triggered \"Teleporter_Entrance_Destroyed\""))
		return LT_TELE_EDESTROYED;
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
	if (CONTAINS("\" triggered \"CTs_Win\" ("))
		return LT_TEAM_CT_WIN;
	if (CONTAINS("\" triggered \"Terrorists_Win\" ("))
		return LT_TEAM_T_WIN;
	if (CONTAINS(" triggered \"Round_Draw\" ("))
		return LT_TEAM_DRAW;
	if (CONTAINS("\" triggered \"Escaped_As_VIP\""))
		return LT_VIP_ESCAPE;
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

void GIRLLogParserTFC::ProcessLine(std::string& sLine)
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

	case LT_SPAWN:
	case LT_SENTRY_BUILD:
	case LT_SENTRY_UPGRADE2:
	case LT_SENTRY_UPGRADE3:
	case LT_SENTRY_REPAIR:
	case LT_SENTRY_DESTROYED:
	case LT_DISPENSER_BUILD:
	case LT_DISPENSER_DESTROYED:
	case LT_TELE_ENTRANCE:
	case LT_TELE_EXIT:
	case LT_TELE_XDESTROYED:
	case LT_TELE_EDESTROYED:
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
// **************************************** NOT RELATED //
	case LT_VIP_ESCAPE:
	case LT_BECAME_VIP:
		OutputWithPlayerOnly(sLine, nLineType);
		break;

	case LT_TEAM_DRAW:
	case LT_ALL_RESCUED:
	case LT_TARGET_BOMBED:
	case LT_TARGET_SAVED:
	case LT_BOMB_DEFUSED:
	case LT_VIP_ASSASINATED:
	case LT_NONE_ESCAPED:
	case LT_NOT_RESCUED:
	case LT_VIP_NOT_ESCAPED:
		OutputWithScoresOnly(sLine, nLineType);
		break;

	default:
		GIRLLogParserDefault::ProcessLine(sLine);
	}
}

const char* GIRLLogParserTFC::TeamName(Team t)
{
	switch (t)
	{
	case TEAM_B:
		return "B";
	case TEAM_R:
		return "R";
	case TEAM_SPECTATOR:
		return "Spec";
	}
	return "";
}

const char* GIRLLogParserTFC::TeamDescription(Team t)
{
	switch (t)
	{
	case TEAM_B:
		return "Blue";
	case TEAM_R:
		return "Red";
	case TEAM_SPECTATOR:
		return "Spectators";
	}
	return "";
}

std::string GIRLLogParserTFC::GetTeamColor(Team t)
{
	IGCLock lock(this, "TFC::GetTeamColor");
	switch (t)
	{
	case TEAM_B:
		return m_sBColor;
	case TEAM_R:
		return m_sRColor;
	case TEAM_SPECTATOR:
		return m_sSpecColor;
	}
	return "";
}

const char* GIRLLogParserTFC::GetTeamColorDefault(Team t)
{
	switch (t)
	{
	case TEAM_B:
		return "04";
	case TEAM_R:
		return "12";
	case TEAM_SPECTATOR:
		return "14";
	}
	return "";
}

void GIRLLogParserTFC::SetTeamColor(Team t, const std::string& sColor)
{
	IGCLock lock(this, "TFC::SetTeamColor");
	switch (t)
	{
	case TEAM_B:
		m_sBColor = sColor;
		break;
	case TEAM_R:
		m_sRColor = sColor;
		break;
	case TEAM_SPECTATOR:
		m_sSpecColor = sColor;
		break;
	}
}

Team GIRLLogParserTFC::NameTeam(std::string& sTeam)
{
	if (strcmp(sTeam.c_str(), "Blue") == 0)
		return TEAM_B;
	if (strcmp(sTeam.c_str(), "Red") == 0)
		return TEAM_R;
	if (strcmp(sTeam.c_str(), "SPECTATOR") == 0)
		return TEAM_SPECTATOR;

	return TEAM_NONE;
}

const char** GIRLLogParserTFC::GameTypeName()
{
	static const char* pNames[] = {
		"tfc",
		NULL
	};
	return pNames;
}

const char* GIRLLogParserTFC::GameTypeDescription()
{
	return "Team Fortress";
}

CString GIRLLogParserTFC::GetAdditionalTags()
{
	return GIRLLogParserDefault::GetAdditionalTags() +
		"<b-score> - Blue score\n"
		"<r-score> - Red score\n";
}

int GIRLLogParserTFC::GameTypeImage()
{
	return IMAGE_TFC;
}

void GIRLLogParserTFC::ReplaceTScore(CString& s)
{
	s.Replace("<b-score>", DecorateScore(TEAM_B, m_nBScore).c_str());
}

void GIRLLogParserTFC::ReplaceCTScore(CString& s)
{
	s.Replace("<r-score>", DecorateScore(TEAM_R, m_nRScore).c_str());
}

void GIRLLogParserTFC::ReplaceRest(CString& sMsg)
{
	ReplaceTScore(sMsg);
	ReplaceCTScore(sMsg);
	GIRLLogParserDefault::ReplaceRest(sMsg);
}

bool GIRLLogParserTFC::GetScores(std::string& sLine)
 {
	int nStart, nEnd;
	nStart = sLine.find("(B \"");
	if (nStart == std::string::npos)
		return false;
	nEnd = sLine.find('\"', nStart+6);
	if (nEnd == std::string::npos)
		return false;
	m_nBScore = atoi(sLine.substr(nStart+5).c_str());

	sLine = sLine.substr(nEnd);

	nStart = sLine.find("(R \"");
	if (nStart == std::string::npos)
		return false;
	nEnd = sLine.find('\"', nStart+5);
	if (nEnd == std::string::npos)
		return false;
	m_nRScore = atoi(sLine.substr(nStart+4).c_str());

	sLine = sLine.substr(nEnd);

	return true;
}

void GIRLLogParserTFC::LineRoundStart(std::string& sLine)
{
}

void GIRLLogParserTFC::LineRoundEnd(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_ROUND_END);

	ReplaceRest(sMsg);
	OutputIRCMessage(LT_ROUND_END, sMsg);
}

void GIRLLogParserTFC::LineJoined(std::string& sLine)
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

void GIRLLogParserTFC::LineWinT(std::string& sLine)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(LT_TEAMWIN);
	ReplaceTeam(sMsg, TeamDescription(TEAM_B));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAMWIN, sMsg);
	SetIRCTopic();
}

void GIRLLogParserTFC::LineWinCT(std::string& sLine)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(LT_TEAMWIN);
	ReplaceTeam(sMsg, TeamDescription(TEAM_R));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAMWIN, sMsg);
	SetIRCTopic();
}

void GIRLLogParserTFC::LineRestartRound(std::string& sLine)
{
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

void GIRLLogParserTFC::OutputWithScoresOnly(std::string& sLine, IRCMSG uMsg)
{
	if (!GetScores(sLine))
		return;

	CString sMsg = GetIRCMessage(uMsg);

	ReplaceRest(sMsg);
	OutputIRCMessage(uMsg, sMsg);
	SetIRCTopic();
}

void GIRLLogParserTFC::SetMapName(const char* pName)
{
	m_nBScore = 0;
	m_nRScore = 0;
	GIRLLogParserDefault::SetMapName(pName);
}
