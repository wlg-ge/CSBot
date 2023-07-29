/*
	CSBot - An IRC bot for publishing CS and NS games
	Copyright (C) 2001, 2002, 2003, 2004 Ilan Tayary ([NoCt]Yonatanz)
	Email: yonatanz@unitedadmins.com
	Website: http://www.csbot.org

	This file is copyright Scooz, 2003, 2004

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

// GIRLLogParserDOD.cpp: implementation of the GIRLLogParserDOD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserDOD.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TEAM_AXIS TEAM_ONE
#define TEAM_ALLIES TEAM_TWO

// Log line types are also IRC message IDs in DOD
#define LT_ROUND_START		11
#define LT_ROUND_END		12
#define LT_JOINED			13

#define LT_TEAMSCORE		17

#define LT_TEAM_WIN			23
#define LT_TEAM_TIE			24

#define LT_TEAM_CAP			33
#define LT_PLAYER_CAP		34

#define LT_RESTARTMAP		41
#define LT_CHANGE_ROLE		42

GIRLLogParserDOD::GIRLLogParserDOD()
{
	Init();

	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<server>\x0f <host>:<port> <map> <a-score>  <x-score>",
		"This is the format of the IRC channel's topic for a Day of Defeat server",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_JOINED,
		"<name> joined the <team> team",
		"The player joined a team, or switched a team",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_CHANGE_ROLE,
		"<name> has changed class to <role>",
		"A player changed his class",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_TEAM_WIN,
		"<team> have won the round",
		"The team has achieved their objective and won the round",
		true,
		MsgType_Objectives);
	RegisterMessage(LT_TEAM_CAP,
		"The <team> have captured the command point <point>",
		"The team has captured a command point",
		false,
		MsgType_Objectives);
	RegisterMessage(LT_PLAYER_CAP,
		"<name> has captured the command point <point>",
		"The player has captured a command point",
		true,
		MsgType_Objectives);

}

GIRLLogParserDOD::~GIRLLogParserDOD()
{

}

void GIRLLogParserDOD::Init()
{
	m_nAScore = 0;
	m_nXScore = 0;
	m_nAPrevScore = 0;
	m_nXPrevScore = 0;
}

IRCMSG GIRLLogParserDOD::RecognizeLine(const std::string& sLine)
{
	if (CONTAINS("\" joined team \""))
		return LT_JOINED;
	if (CONTAINS("\" changed role to \""))
		return LT_CHANGE_ROLE;
	if (STARTSWITH("Team \"") && CONTAINS("\" triggered a \"dod_capture_area\""))
		return LT_TEAM_CAP;
	if (STARTSWITH("\"") && CONTAINS(">\" triggered a \"dod_capture_area\""))
		return LT_PLAYER_CAP;
	if (CONTAINS("\" scored \"") && CONTAINS("\" with \"") && CONTAINS("\" players"))
		return LT_TEAMSCORE;
	return LT_UNKNOWN;
}

void GIRLLogParserDOD::ProcessLine(std::string& sLine)
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
	case LT_CHANGE_ROLE:
		LineChangeRole(sLine);
		break;
	case LT_RESTARTMAP:
		LineRestartMap(sLine);
		break;
	case LT_TEAM_CAP:
		LineCap(sLine);
		break;
	case LT_PLAYER_CAP:
		LinePlayerCap(sLine);
		break;
	case LT_TEAMSCORE:
		LineTeamScore(sLine);
		break;
	default:
		GIRLLogParserDefault::ProcessLine(sLine);
	}
}


std::string GIRLLogParserDOD::GetTeamColor(Team t)
{
	IGCLock lock(this, "DOD::GetTeamColor");
	switch (t)
	{
	case TEAM_ALLIES:
		return m_sAColor;
	case TEAM_AXIS:
		return m_sXColor;
	case TEAM_SPECTATOR:
		return m_sSpecColor;
	}
	return "";
}

int GIRLLogParserDOD::GameTypeImage()
{
	return IMAGE_DOD;
}

CString GIRLLogParserDOD::GetAdditionalTags() {
	return GIRLLogParserDefault::GetAdditionalTags() +
		"<a-score> - Allies score\n"
		"<x-score> - Axis score\n"
		"<point> - Name of point or area\n";
}

const char** GIRLLogParserDOD::GameTypeName()
{
	static const char* pNames[] = {
		"dod",
		NULL
	};
	return pNames;
}

const char* GIRLLogParserDOD::GameTypeDescription()
{
	return "Day of Defeat";
}

const char* GIRLLogParserDOD::GetTeamColorDefault(Team t)
{
	switch (t)
	{
	case TEAM_ALLIES:
		return "03";
	case TEAM_AXIS:
		return "04";
	case TEAM_SPECTATOR:
		return "14";
	}
	return "";	
}

Team GIRLLogParserDOD::NameTeam(std::string& sTeam)
{
	if (strcmp(sTeam.c_str(), "Allies") == 0)
		return TEAM_ALLIES;
	if (strcmp(sTeam.c_str(), "Axis") == 0)
		return TEAM_AXIS;
	if (strcmp(sTeam.c_str(), "Spec") == 0)
		return TEAM_SPECTATOR;

	return TEAM_NONE;
}

void GIRLLogParserDOD::SetTeamColor(Team t, const std::string& sColor)
{
	IGCLock lock(this, "DOD::SetTeamColor");
	switch (t)
	{
	case TEAM_ALLIES:
		m_sAColor = sColor;
		break;
	case TEAM_AXIS:
		m_sXColor = sColor;
		break;
	case TEAM_SPECTATOR:
		m_sSpecColor = sColor;
		break;
	}
}

const char* GIRLLogParserDOD::TeamDescription(Team t)
{
	switch (t)
	{
	case TEAM_ALLIES:
		return "Allies";
	case TEAM_AXIS:
		return "Axis";
	case TEAM_SPECTATOR:
		return "Spectators";
	}
	return "";
}

const char* GIRLLogParserDOD::TeamName(Team t)
{
	switch (t)
	{
	case TEAM_ALLIES:
		return "Allies";
	case TEAM_AXIS:
		return "Axis";
	case TEAM_SPECTATOR:
		return "Spec";
	}
	return "";
}

void GIRLLogParserDOD::LineRoundStart(std::string& sLine)
{

}

void GIRLLogParserDOD::LineRoundEnd(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_ROUND_END);

	ReplaceRest(sMsg);
	OutputIRCMessage(LT_ROUND_END, sMsg);
}

void GIRLLogParserDOD::LineRestartMap(std::string& sLine)
{
	m_nAScore = 0;
	m_nAPrevScore = 0;
	m_nXScore = 0;
	m_nXPrevScore = 0;
}

void GIRLLogParserDOD::LineJoined(std::string& sLine)
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

void GIRLLogParserDOD::LineChangeRole(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	player.m_sRole = RoleName(GetRole(sLine));

	// update
	m_pReceiver->UpdatePlayer(new UpdatePlayerStruct(player));

	CString sMsg = GetIRCMessage(LT_CHANGE_ROLE);
	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_CHANGE_ROLE, sMsg);
}

void GIRLLogParserDOD::LineWin(std::string& sLine, Team tWon)
{
	CString sMsg = GetIRCMessage(LT_TEAM_WIN);
	ReplaceTeam(sMsg, TeamDescription(tWon));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAM_WIN, sMsg);
}

void GIRLLogParserDOD::LineTeamScore(std::string& sLine)
{
	Team team = NameTeam(GetNextVar(sLine));
	int nScore = atoi(GetNextVar(sLine).c_str());
	int nPlayers = atoi(GetNextVar(sLine).c_str());


	// The axis score line always comes after the allies score line
	// So we check who won on the second line, and publish that.
	switch (team)
	{
	case TEAM_ALLIES:
		m_nAPrevScore = m_nAScore;
		m_nAScore = nScore;
		break;
	case TEAM_AXIS:
		m_nXPrevScore = m_nXScore;
		m_nXScore = nScore;
		// Check which team gained the most score
		// to determine who won
		int nADiff = m_nAScore - m_nAPrevScore;
		int nXDiff = m_nXScore - m_nXPrevScore;

		if (nADiff == nXDiff)
			// Tie!
			OutputWithRestOnly(sLine, LT_TEAM_TIE);
		else if (nADiff > nXDiff)
			// Allies Won
			LineWin(sLine, TEAM_ALLIES);
		else
			// Axis Won
			LineWin(sLine, TEAM_AXIS);

		// Update the scores in the topic
		SetIRCTopic();
		return;
	}	
}

void GIRLLogParserDOD::LineCap(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_TEAM_CAP);

	// fetch the team's name
	std::string sTeam = GetNextVar(sLine);
	ReplaceTeam(sMsg, TeamDescription(NameTeam(sTeam)));

	// fetch the event (we don't care what it is, though)
	std::string sEvent = GetNextVar(sLine);

	// fetch the point name (only in STEAM servers)
	std::string sPoint = GetNextVar(sLine);
	ReplacePoint(sMsg, sPoint);

	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAM_CAP, sMsg);
}

void GIRLLogParserDOD::LinePlayerCap(std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_PLAYER_CAP);

	// fetch the player
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);
	ReplacePlayer(sMsg, player);

	// fetch the event (we don't care what it is, though)
	std::string sEvent = GetNextVar(sLine);

	// fetch the point name (only in STEAM servers)
	std::string sPoint = GetNextVar(sLine);
	ReplacePoint(sMsg, sPoint);

	ReplaceRest(sMsg);
	OutputIRCMessage(LT_PLAYER_CAP, sMsg);
}

std::string GIRLLogParserDOD::RoleName(const std::string& sRole)
{
	// Axis
	if (sRole == "#class_axis_kar98")
		return "Grenadier [K98]";
	if (sRole == "#class_axis_k43")
		return "Stosstruppe [K43]";
	if (sRole == "#class_axis_mp40")
		return "Unteroffizier [MP40]";
	if (sRole == "#class_axis_sniper")
		return "Scharfschutze [Scoped K98]";
	if (sRole == "#class_axis_mp44")
		return "Sturmtruppe [STG44]";
	if (sRole == "#class_axis_mg34")
		return "MG-Schutze [MG34]";
	if (sRole == "#class_axis_mg42")
		return "MG-Schutze [MG42]";

	// Axis Para
	if (sRole == "#class_axispara_kar98")
		return "Grenadier [K98]";
	if (sRole == "#class_axispara_k43")
		return "Stosstruppe [K43]";
	if (sRole == "#class_axispara_mp40")
		return "Unteroffizier [MP40]";
	if (sRole == "#class_axispara_mp44")
		return "Sturmtruppe [STG44]";
	if (sRole == "#class_axispara_fg42bipod")
		return "Fallschirmjager [FG42]";
	if (sRole == "#class_axispara_fg42scope")
		return "Fallschirmjager [FG42 Sniper]";
	if (sRole == "#class_axispara_mg34")
		return "MG-Schutze [MG34]";
	if (sRole == "#class_axispara_mg42")
		return "MG-Schutze [MG42]";

	// Allies
	if (sRole == "#class_allied_garand")
		return "Rifleman [Garand]";
	if (sRole == "#class_allied_carbine")
		return "Staff Sergeant [Carbine]";
	if (sRole == "#class_allied_grease")
		return "Sergeant [Grease Gun]";
	if (sRole == "#class_allied_thompson")
		return "Master Sergeant [Thompson]";
	if (sRole == "#class_allied_sniper")
		return "Sniper [Springfield]";
	if (sRole == "#class_allied_heavy")
		return "Support Infantry [BAR]";
	if (sRole == "#class_alliedpara_bar")
		return "Support Infantry [BAR]";
	if (sRole == "#class_allied_mg")
		return "Machine Gunner [.30 CAL]";

	// Allies Para
	if (sRole == "#class_alliedpara_garand")
		return "Rifleman [Garand]";
	if (sRole == "#class_alliedpara_carbine")
		return "Staff Sergeant [Carbine]";
	if (sRole == "#class_alliedpara_grease")
		return "Sergeant [Grease Gun]";
	if (sRole == "#class_alliedpara_thompson")
		return "Master Sergeant [Thompson]";
	if (sRole == "#class_alliedpara_sniper")
		return "Sniper [Springfield]";
	if (sRole == "#class_alliedpara_bar")
		return "Support Infantry [BAR]";
	if (sRole == "#class_alliedpara_30cal")
		return "Machine Gunner [.30 CAL]";

	// British
	if (sRole == "#class_brit_light")
		return "Rifleman [Enfield]";
	if (sRole == "#class_brit_medium")
		return "Sergeant Major [Sten]";
	if (sRole == "#class_brit_sniper")
		return "Marksman [Scoped Enfield]";
	if (sRole == "#class_brit_heavy")
		return "Gunner [Bren]";
	return sRole;
}

std::string GIRLLogParserDOD::WeaponName(const std::string& sWeapon)
{
	// Axis Weapons
	if (sWeapon == "spade")
		return "Spade";
	if (sWeapon == "luger")
		return "Luger";
	if (sWeapon == "kar")
		return "K98";
	if (sWeapon == "bayonet")
		return "K98 Bayonet";
	if (sWeapon == "k43")
		return "K43";
	if (sWeapon == "mp40")
		return "Mp40";
	if (sWeapon == "mp44")
		return "Mp44";
	if (sWeapon == "scopedkar")
		return "Scoped K98";
	// Fg42 Bipod?
	// Fg42 Scoped?
	if (sWeapon == "mg34")
		return "Mg34";
	if (sWeapon == "mg42")
		return "Mg42";
	if (sWeapon == "grenade2")
		return "Axis Grenade";
	// Axis Para Weapons?

	// Allied Weapons
	if (sWeapon == "amerknife")
		return "American Knife";
	if (sWeapon == "colt")
		return "Colt";
	if (sWeapon == "garand")
		return "M1 Garand";
	if (sWeapon == "garandbutt")
		return "M1 Garand Buttstock";
	if (sWeapon == "m1carbine")
		return "M1 Carbine";
	if (sWeapon == "greasegun")
		return "M3 Greasegun";
	if (sWeapon == "thompson")
		return "Thompson";
	if (sWeapon == "spring")
		return "Springfield";
	if (sWeapon == "bar")
		return "BAR";
	// Allied MG?
	if (sWeapon == "grenade")
		return "American Grenade";
	// Allied Para Weapons?

	// British Weapons
	if (sWeapon == "brit_knife")
		return "British Knife";
	if (sWeapon == "webley")
		return "Webley";
	if (sWeapon == "enfield")
		return "Enfield";
	// Enfield Bayonet?
	if (sWeapon == "sten")
		return "Sten";
	if (sWeapon == "scoped_enfield")
		return "Scoped Enfield";
	if (sWeapon == "bren")
		return "Bren";
	// British Grenade?
	return sWeapon;
}

void GIRLLogParserDOD::ReplaceRest(CString& sMsg)
{
	ReplaceAScore(sMsg);
	ReplaceXScore(sMsg);
	ReplacePoint(sMsg, "");
	GIRLLogParserDefault::ReplaceRest(sMsg);
}

void GIRLLogParserDOD::ReplacePoint(CString& sMsg, const std::string& sReplacement)
{
	sMsg.Replace("<point>", sReplacement.c_str());
}

void GIRLLogParserDOD::ReplaceAScore(CString& sMsg)
{
	sMsg.Replace("<a-score>", DecorateScore(TEAM_ALLIES, m_nAScore).c_str());
}

void GIRLLogParserDOD::ReplaceXScore(CString& sMsg)
{
	sMsg.Replace("<x-score>", DecorateScore(TEAM_AXIS, m_nXScore).c_str());
}


