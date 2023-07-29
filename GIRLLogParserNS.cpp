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

// GIRLLogParserNS.cpp: implementation of the GIRLLogParserNS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserNS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TEAM_A TEAM_TWO
#define TEAM_M TEAM_ONE

// Log line types are also IRC message IDs in NS
#define LT_CHANGE_ROLE				1
#define LT_NS_SCAN					2
#define LT_NS_VOTEDOWN				3

#define LT_NSBUILD_OBSERVATORY		4
#define LT_NSBUILD_SPAWNPORTAL		5
#define LT_NSBUILD_ARMORY			6
#define LT_NSBUILD_PHASEGATE		7
#define LT_NSBUILD_RESOURCETOWER	8
#define LT_NSBUILD_TURRETFACTORY	9
#define LT_NSBUILD_TURRET			10
#define LT_NSBUILD_ARMSLAB			11
#define LT_NSBUILD_SIEGETURRET		12
#define LT_NSBUILD_COMMANDCONSOLE	13
#define LT_NSBUILD_PROTOTYPELAB		14

#define LT_NSABUILD_RESOURCETOWER	21
#define LT_NSABUILD_HIVE			22
#define LT_NSABUILD_OFFENSECHAMBER	23
#define LT_NSABUILD_SENSORYCHAMBER	24
#define LT_NSABUILD_MOVEMENTCHAMBER	25
#define LT_NSABUILD_DEFENSECHAMBER	26

#define LT_NSRES_ADVARMORY			31
#define LT_NSRES_ADVTURRETFACTORY	32
#define LT_NSRES_MOTIONTRACKING		33
#define LT_NSRES_WEAPONS1			34
#define LT_NSRES_WEAPONS2			35
#define LT_NSRES_WEAPONS3			36
#define LT_NSRES_ARMOR1				37
#define LT_NSRES_ARMOR2				38
#define LT_NSRES_ARMOR3				39
#define LT_NSRES_JETPACKS			40
#define LT_NSRES_HEAVYARMOR			41
#define LT_NSRES_DISTRESSBEACON		42
#define LT_NSRES_ELECTRICAL			43
#define LT_NSRES_PHASETECH			44
#define LT_NSRES_CANCEL				45
#define LT_NSRES_GRENADES			46
#define LT_NSRES_CATALYSTS			47

#define LT_NSSPAWN_HEALTHPACK		51
#define LT_NSSPAWN_WELDER			52
#define LT_NSSPAWN_AMMO				53
#define LT_NSSPAWN_SHOTGUN			54
#define LT_NSSPAWN_HEAVYMACHINEGUN	55
#define LT_NSSPAWN_GRENADEGUN		56
#define LT_NSSPAWN_JETPACK			57
#define LT_NSSPAWN_HEAVYARMOR		58
#define LT_NSSPAWN_MINES			59
#define LT_NSSPAWN_CATALYST			60

#define LT_NSADEST_RESOURCETOWER	61
#define LT_NSADEST_HIVE				62
#define LT_NSADEST_OFFENSECHAMBER	63
#define LT_NSADEST_SENSORYCHAMBER	64
#define LT_NSADEST_MOVEMENTCHAMBER	65
#define LT_NSADEST_DEFENSECHAMBER	66

#define LT_NSDEST_OBSERVATORY		71
#define LT_NSDEST_SPAWNPORTAL		72
#define LT_NSDEST_ARMORY			73
#define LT_NSDEST_PHASEGATE			74
#define LT_NSDEST_RESOURCETOWER		75
#define LT_NSDEST_TURRETFACTORY		76
#define LT_NSDEST_TURRET			77
#define LT_NSDEST_ARMSLAB			78
#define LT_NSDEST_SIEGETURRET		79
#define LT_NSDEST_COMMANDCONSOLE	80
#define LT_NSDEST_PROTOTYPELAB		81
#define LT_NSDEST_ADVARMORY			82
#define LT_NSDEST_ADVTURRETFACTORY	83

#define LT_NSRECYCLE_OBSERVATORY	101
#define LT_NSRECYCLE_SPAWNPORTAL	102
#define LT_NSRECYCLE_ARMORY			103
#define LT_NSRECYCLE_PHASEGATE		104
#define LT_NSRECYCLE_RESOURCETOWER	105
#define LT_NSRECYCLE_TURRETFACTORY	106
#define LT_NSRECYCLE_TURRET			107
#define LT_NSRECYCLE_ARMSLAB		108
#define LT_NSRECYCLE_SIEGETURRET	109
#define LT_NSRECYCLE_PROTOTYPELAB	111
#define LT_NSRECYCLE_COMMAND		112
#define LT_NSRECYCLE_ADVARMORY		113
#define LT_NSRECYCLE_ADVTURRETFACTORY	114

// log lines only:
#define LT_TEAMSCORE	91
#define LT_A_WIN		92
#define LT_M_WIN		93
// IRC message only:
#define LT_TEAMWIN		94

GIRLLogParserNS::GIRLLogParserNS()
{
	m_nAScore = 0;
	m_nMScore = 0;

	// there aren't dead indications in NS yet...
	UnregisterMessage(LT_SAY_DEAD);
	UnregisterMessage(LT_SAY_TEAM_DEAD);

	RegisterMessage(LT_TOPIC,
		"\x03" "00,01<server>\x0f <host>:<port> <map>",
		"This is the format of the IRC channel's topic for a Natural-Selection server",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_CHANGE_ROLE,
		"<name> changed role to <role>",
		"Player changes his role or class in the game",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_NS_SCAN,
		"<name> scanned a part of the map",
		"The player used scan to discover a part of the map",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_NS_VOTEDOWN,
		"<name> voted to eject <name2> from the Command Console",
		"The player voted against the commander",
		true,
		MsgType_Gameflow);

	RegisterMessage(LT_TEAMWIN,
		"<team> win the round. <a-score> <m-score>",
		"The team won the round.",
		true,
		MsgType_Gameflow);
	
	RegisterMessage(LT_NSBUILD_OBSERVATORY, 
		"<name> has built an observatory",
		"The player built an observatory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_SPAWNPORTAL, 
		"<name> has built a spawn portal",
		"The player built a spawn portal",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_ARMORY, 
		"<name> has built an armory",
		"The player built an armory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_PHASEGATE, 
		"<name> has built a phasegate",
		"The player built a phasegate",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_RESOURCETOWER, 
		"<name> has built a marine resource tower",
		"The player built a marine resource tower",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_TURRETFACTORY,
		"<name> has built a turret factory",
		"The player built a turret factory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_TURRET, 
		"<name> has built a turret",
		"The player built a turret",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_ARMSLAB, 
		"<name> has built an arms lab",
		"The player built an arms lab",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_SIEGETURRET, 
		"<name> has built a siege turret",
		"The player built a siege turret",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_COMMANDCONSOLE, 
		"<name> has built a command console",
		"The player built a command console",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSBUILD_PROTOTYPELAB,
		"<name> has built a prototype lab",
		"The player built a prototype lab",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_RESOURCETOWER, 
		"<name> has built an alien resource tower",
		"The player built an alien resource tower",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_HIVE, 
		"<name> has built a hive",
		"The player built a hive",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_OFFENSECHAMBER,
		"<name> has built an offence chamber",
		"The player built an offence chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_SENSORYCHAMBER, 
		"<name> has built a sensory chamber",
		"The player built a sensory chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_MOVEMENTCHAMBER, 
		"<name> has built a movement chamber",
		"The player built a movement chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSABUILD_DEFENSECHAMBER,
		"<name> has built a defense chamber",
		"The player built a defense chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSRES_DISTRESSBEACON,
		"<name> has activated the distress beacon",
		"The player activated the distress beacon",
		true,
		MsgType_Gameflow);
	RegisterMessage(LT_NSRES_MOTIONTRACKING, 
		"<name> has started to research motion tracking",
		"The player started to research motion tracking",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_WEAPONS1, 
		"<name> has started to research weapons upgrade #1",
		"The player started to research weapons upgrade #1",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_WEAPONS2, 
		"<name> has started to research weapons upgrade #2",
		"The player started to research weapons upgrade #2",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_WEAPONS3,
		"<name> has started to research weapons upgrade #3",
		"The player started to research weapons upgrade #3",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ARMOR1, 
		"<name> has started to research armor upgrade #1",
		"The player started to research armor upgrade #1",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ARMOR2, 
		"<name> has started to research armor upgrade #2",
		"The player started to research armor upgrade #2",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ARMOR3, 
		"<name> has started to research armor upgrade #3",
		"The player started to research armor upgrade #3",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ADVARMORY, 
		"<name> has started to research advanced armory",
		"The player started to research advanced armory",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ADVTURRETFACTORY, 
		"<name> has started to research advanced turret factory",
		"The player started to research advanced turret factory",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_JETPACKS, 
		"<name> has started to research jetpacks",
		"The player started to research jetpacks",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_GRENADES,
		"<name> has started to research grenades",
		"The player started to research grenades",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_CATALYSTS,
		"<name> has started to research catalysts",
		"The player started to research catalysts",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_HEAVYARMOR,
		"<name> has started to research heavy armor",
		"The player started to research heavy armor",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_ELECTRICAL,
		"<name> has started to research electrical upgrade",
		"The player started to research electrical upgrade",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_PHASETECH,
		"<name> has started to research phasegate technology",
		"The player started to research phasegate technology",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSRES_CANCEL,
		"<name> has cancelled research in progress",
		"The player cancelled research in progress",
		true,
		MsgType_Research);
	RegisterMessage(LT_NSSPAWN_HEALTHPACK, 
		"<name> has dropped a healthpack",
		"The player dropped a healthpack",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_WELDER, 
		"<name> has spawned a welder",
		"The player spawned a welder",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_AMMO,
		"<name> has dropped down ammo",
		"The player dropped down ammunition",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_SHOTGUN, 
		"<name> has spawned a shotgun",
		"The player spawned a shotgun",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_HEAVYMACHINEGUN, 
		"<name> has spawned a heavy machine gun",
		"The player spawned a heave machine gun",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_GRENADEGUN,
		"<name> has spawned a grenade launcher",
		"The player spawned a grenade launcher",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_JETPACK, 
		"<name> has spawned a jetpack",
		"The player spawned a jetpack",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_CATALYST,
		"<name> has spawned a catalyst pack",
		"The player spawned a catalyst pack",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_HEAVYARMOR,
		"<name> has spawned a heavy armor",
		"The player spawned a heavy armor",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSSPAWN_MINES,
		"<name> has spawned a mine pack",
		"The player spawned a mine pack",
		true,
		MsgType_Spawn);
	RegisterMessage(LT_NSDEST_OBSERVATORY, 
		"<name> has destroyed an observatory",
		"The player destroyed an observatory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_SPAWNPORTAL, 
		"<name> has destroyed a spawn portal",
		"The player destroyed a spawn portal",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_ARMORY, 
		"<name> has destroyed an armory",
		"The player destroyed an armory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_ADVARMORY,
		"<name> has destroyed an advanced armory",
		"The player destroyed an advanced armory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_PHASEGATE, 
		"<name> has destroyed a phasegate",
		"The player destroyed a phasegate",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_RESOURCETOWER, 
		"<name> has destroyed a marine resource tower",
		"The player destroyed a marine resource tower",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_TURRETFACTORY,
		"<name> has destroyed a turret factory",
		"The player destroyed a turret factory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_ADVTURRETFACTORY,
		"<name> has destroyed an advanced turret factory",
		"The player destroyed an advanced turret factory",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_TURRET, 
		"<name> has destroyed a turret",
		"The player destroyed a turret",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_ARMSLAB, 
		"<name> has destroyed an arms lab",
		"The player destroyed an arms lab",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_SIEGETURRET, 
		"<name> has destroyed a siege turret",
		"The player destroyed a siege turret",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_COMMANDCONSOLE, 
		"<name> has destroyed a command console",
		"The player destroyed a command console",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSDEST_PROTOTYPELAB,
		"<name> has destroyed a prototype lab",
		"The player destroyed a prototype lab",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSRECYCLE_OBSERVATORY, 
		"<name> has recycled an observatory",
		"The player recycled an observatory",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_SPAWNPORTAL, 
		"<name> has recycled a spawn portal",
		"The player recycled a spawn portal",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_ARMORY, 
		"<name> has recycled an armory",
		"The player recycled an armory",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_COMMAND, 
		"<name> has recycled a command console",
		"The player recycled a command console",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_ADVARMORY,
		"<name> has recycled an advanced armory",
		"The player recycled an advanced armory",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_PHASEGATE, 
		"<name> has recycled a phasegate",
		"The player recycled a phasegate",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_RESOURCETOWER, 
		"<name> has recycled a marine resource tower",
		"The player recycled a marine resource tower",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_TURRETFACTORY,
		"<name> has recycled a turret factory",
		"The player recycled a turret factory",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_ADVTURRETFACTORY,
		"<name> has recycled an advanced turret factory",
		"The player recycled an advanced turret factory",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_TURRET, 
		"<name> has recycled a turret",
		"The player recycled a turret",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_ARMSLAB, 
		"<name> has recycled an arms lab",
		"The player recycled an arms lab",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_SIEGETURRET, 
		"<name> has recycled a siege turret",
		"The player recycled a siege turret",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSRECYCLE_PROTOTYPELAB,
		"<name> has recycled a prototype lab",
		"The player recycled a prototype lab",
		true,
		MsgType_Recycle);
	RegisterMessage(LT_NSADEST_RESOURCETOWER, 
		"<name> has destroyed an alien resource tower",
		"The player destroyed an alien resource tower",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSADEST_HIVE, 
		"<name> has destroyed a hive",
		"The player destroyed a hive",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSADEST_OFFENSECHAMBER,
		"<name> has destroyed an offence chamber",
		"The player destroyed an offence chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSADEST_SENSORYCHAMBER, 
		"<name> has destroyed a sensory chamber",
		"The player destroyed a sensory chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSADEST_MOVEMENTCHAMBER, 
		"<name> has destroyed a movement chamber",
		"The player destroyed a movement chamber",
		true,
		MsgType_Build);
	RegisterMessage(LT_NSADEST_DEFENSECHAMBER,
		"<name> has destroyed a defense chamber",
		"The player destroyed a defense chamber",
		true,
		MsgType_Build);
}

GIRLLogParserNS::~GIRLLogParserNS()
{

}

void GIRLLogParserNS::Init()
{
	m_nAScore = 0;
	m_nMScore = 0;
}

int GIRLLogParserNS::RecognizeLine(const std::string& sLine)
{
	if (CONTAINS("\" changed role to \""))
		return LT_CHANGE_ROLE;

	if (CONTAINS("\" triggered \"structure_built\" (type \"scan\")"))
		return LT_NS_SCAN;
	if (CONTAINS("\" triggered \"votedown\" against \""))
		return LT_NS_VOTEDOWN;
	
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_observatory\")"))
		return LT_NSBUILD_OBSERVATORY;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_infportal\")"))
		return LT_NSBUILD_SPAWNPORTAL;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_armory\")"))
		return LT_NSBUILD_ARMORY;
	if (CONTAINS("\" triggered \"structure_built\" (type \"phasegate\")"))
		return LT_NSBUILD_PHASEGATE;
	if (CONTAINS("\" triggered \"structure_built\" (type \"resourcetower\")"))
		return LT_NSBUILD_RESOURCETOWER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_turretfactory\")"))
		return LT_NSBUILD_TURRETFACTORY;
	if (CONTAINS("\" triggered \"structure_built\" (type \"turret\")"))
		return LT_NSBUILD_TURRET;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_armslab\")"))
		return LT_NSBUILD_ARMSLAB;
	if (CONTAINS("\" triggered \"structure_built\" (type \"siegeturret\")"))
		return LT_NSBUILD_SIEGETURRET;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_command\")"))
		return LT_NSBUILD_COMMANDCONSOLE;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_prototypelab\")"))
		return LT_NSBUILD_PROTOTYPELAB;
	
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_observatory\")"))
		return LT_NSDEST_OBSERVATORY;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_infportal\")"))
		return LT_NSDEST_SPAWNPORTAL;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_armory\")"))
		return LT_NSDEST_ARMORY;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_advarmory\")"))
		return LT_NSDEST_ADVARMORY;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"phasegate\")"))
		return LT_NSDEST_PHASEGATE;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"resourcetower\")"))
		return LT_NSDEST_RESOURCETOWER;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_turretfactory\")"))
		return LT_NSDEST_TURRETFACTORY;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_advturretfactory\")"))
		return LT_NSDEST_ADVTURRETFACTORY;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"turret\")"))
		return LT_NSDEST_TURRET;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_armslab\")"))
		return LT_NSDEST_ARMSLAB;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"siegeturret\")"))
		return LT_NSDEST_SIEGETURRET;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_command\")"))
		return LT_NSDEST_COMMANDCONSOLE;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_prototypelab\")"))
		return LT_NSDEST_PROTOTYPELAB;
	
	if (CONTAINS("\" triggered \"recycle\" (type \"team_observatory\")"))
		return LT_NSRECYCLE_OBSERVATORY;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_infportal\")"))
		return LT_NSRECYCLE_SPAWNPORTAL;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_armory\")"))
		return LT_NSRECYCLE_ARMORY;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_command\")"))
		return LT_NSRECYCLE_COMMAND;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_advarmory\")"))
		return LT_NSRECYCLE_ADVARMORY;
	if (CONTAINS("\" triggered \"recycle\" (type \"phasegate\")"))
		return LT_NSRECYCLE_PHASEGATE;
	if (CONTAINS("\" triggered \"recycle\" (type \"resourcetower\")"))
		return LT_NSRECYCLE_RESOURCETOWER;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_turretfactory\")"))
		return LT_NSRECYCLE_TURRETFACTORY;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_advturretfactory\")"))
		return LT_NSRECYCLE_ADVTURRETFACTORY;
	if (CONTAINS("\" triggered \"recycle\" (type \"turret\")"))
		return LT_NSRECYCLE_TURRET;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_armslab\")"))
		return LT_NSRECYCLE_ARMSLAB;
	if (CONTAINS("\" triggered \"recycle\" (type \"siegeturret\")"))
		return LT_NSRECYCLE_SIEGETURRET;
	if (CONTAINS("\" triggered \"recycle\" (type \"team_prototypelab\")"))
		return LT_NSRECYCLE_PROTOTYPELAB;
	
	// v1.05
	if (CONTAINS("\" triggered \"research_start\" (type \"motiontracking\")"))
		return LT_NSRES_MOTIONTRACKING;
	// v2.0
	if (CONTAINS("\" triggered \"research_start\" (type \"research_motiontracking\")"))
		return LT_NSRES_MOTIONTRACKING;

	if (CONTAINS("\" triggered \"research_start\" (type \"research_advarmory\")"))
		return LT_NSRES_ADVARMORY;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_advturretfactory\")"))
		return LT_NSRES_ADVTURRETFACTORY;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_weaponsl1\")"))
		return LT_NSRES_WEAPONS1;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_weaponsl2\")"))
		return LT_NSRES_WEAPONS2;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_weaponsl3\")"))
		return LT_NSRES_WEAPONS3;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_armorl1\")"))
		return LT_NSRES_ARMOR1;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_armorl2\")"))
		return LT_NSRES_ARMOR2;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_armorl3\")"))
		return LT_NSRES_ARMOR3;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_jetpacks\")"))
		return LT_NSRES_JETPACKS;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_heavyarmor\")"))
		return LT_NSRES_HEAVYARMOR;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_phasetech\")"))
		return LT_NSRES_PHASETECH;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_electrical\")"))
		return LT_NSRES_ELECTRICAL;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_distressbeacon\")"))
		return LT_NSRES_DISTRESSBEACON;
	if (CONTAINS("\" triggered \"research_cancel\" (type \"research_cancel\")"))
		return LT_NSRES_CANCEL;

	if (CONTAINS("\" triggered \"structure_built\" (type \"item_health\")"))
		return LT_NSSPAWN_HEALTHPACK;
	if (CONTAINS("\" triggered \"structure_built\" (type \"weapon_welder\")"))
		return LT_NSSPAWN_WELDER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"item_genericammo\")"))
		return LT_NSSPAWN_AMMO;
	if (CONTAINS("\" triggered \"structure_built\" (type \"weapon_shotgun\")"))
		return LT_NSSPAWN_SHOTGUN;
	if (CONTAINS("\" triggered \"structure_built\" (type \"weapon_heavymachinegun\")"))
		return LT_NSSPAWN_HEAVYMACHINEGUN;
	if (CONTAINS("\" triggered \"structure_built\" (type \"weapon_grenadegun\")"))
		return LT_NSSPAWN_GRENADEGUN;
	if (CONTAINS("\" triggered \"structure_built\" (type \"item_jetpack\")"))
		return LT_NSSPAWN_JETPACK;
	if (CONTAINS("\" triggered \"structure_built\" (type \"item_heavyarmor\")"))
		return LT_NSSPAWN_HEAVYARMOR;
	if (CONTAINS("\" triggered \"structure_built\" (type \"weapon_mine\")"))
		return LT_NSSPAWN_MINES;
	
	if (CONTAINS("\" triggered \"structure_built\" (type \"alienresourcetower\")"))
		return LT_NSABUILD_RESOURCETOWER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"team_hive\")"))
		return LT_NSABUILD_HIVE;
	if (CONTAINS("\" triggered \"structure_built\" (type \"offensechamber\")"))
		return LT_NSABUILD_OFFENSECHAMBER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"sensorychamber\")"))
		return LT_NSABUILD_SENSORYCHAMBER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"movementchamber\")"))
		return LT_NSABUILD_MOVEMENTCHAMBER;
	if (CONTAINS("\" triggered \"structure_built\" (type \"defensechamber\")"))
		return LT_NSABUILD_DEFENSECHAMBER;

	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"alienresourcetower\")"))
		return LT_NSADEST_RESOURCETOWER;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"team_hive\")"))
		return LT_NSADEST_HIVE;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"offensechamber\")"))
		return LT_NSADEST_OFFENSECHAMBER;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"sensorychamber\")"))
		return LT_NSADEST_SENSORYCHAMBER;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"movementchamber\")"))
		return LT_NSADEST_MOVEMENTCHAMBER;
	if (CONTAINS("\" triggered \"structure_destroyed\" (type \"defensechamber\")"))
		return LT_NSADEST_DEFENSECHAMBER;

	if (CONTAINS("Team \"") && CONTAINS("\" scored \"") && CONTAINS("\" with \"") && CONTAINS("\" players"))
		return LT_TEAMSCORE;
	if (CONTAINS("\") (victory_team \"marine\") (losing_team \"alien\") (winning_teamsize \""))
		return LT_M_WIN;
	if (CONTAINS("\") (victory_team \"alien\") (losing_team \"marine\") (winning_teamsize \""))
		return LT_A_WIN;

	// v3.0
	if (CONTAINS("\" triggered \"research_start\" (type \"research_grenades\")"))
		return LT_NSRES_GRENADES;
	if (CONTAINS("\" triggered \"research_start\" (type \"research_catalysts\")"))
		return LT_NSRES_CATALYSTS;
	if (CONTAINS("\" triggered \"structure_built\" (type \"item_catalyst\")"))
		return LT_NSSPAWN_CATALYST;

	return LT_UNKNOWN;
}

void GIRLLogParserNS::ProcessLine(std::string& sLine)
{
	int nLineType = RecognizeLine(sLine);
	switch (nLineType)
	{
	case LT_NS_SCAN:

	case LT_NSBUILD_OBSERVATORY:
	case LT_NSBUILD_SPAWNPORTAL:
	case LT_NSBUILD_ARMORY:
	case LT_NSBUILD_PHASEGATE:
	case LT_NSBUILD_RESOURCETOWER:
	case LT_NSBUILD_TURRETFACTORY:
	case LT_NSBUILD_TURRET:
	case LT_NSBUILD_ARMSLAB:
	case LT_NSBUILD_SIEGETURRET:
	case LT_NSBUILD_COMMANDCONSOLE:
	case LT_NSBUILD_PROTOTYPELAB:

	case LT_NSRES_ADVARMORY:
	case LT_NSRES_ADVTURRETFACTORY:
	case LT_NSRES_MOTIONTRACKING:
	case LT_NSRES_WEAPONS1:
	case LT_NSRES_WEAPONS2:
	case LT_NSRES_WEAPONS3:
	case LT_NSRES_ARMOR1:
	case LT_NSRES_ARMOR2:
	case LT_NSRES_ARMOR3:
	case LT_NSRES_JETPACKS:
	case LT_NSRES_HEAVYARMOR:
	case LT_NSRES_PHASETECH:
	case LT_NSRES_ELECTRICAL:
	case LT_NSRES_DISTRESSBEACON:
	case LT_NSRES_CATALYSTS:
	case LT_NSRES_GRENADES:
	case LT_NSRES_CANCEL:

	case LT_NSSPAWN_HEALTHPACK:
	case LT_NSSPAWN_WELDER:
	case LT_NSSPAWN_AMMO:
	case LT_NSSPAWN_SHOTGUN:
	case LT_NSSPAWN_HEAVYMACHINEGUN:
	case LT_NSSPAWN_GRENADEGUN:
	case LT_NSSPAWN_JETPACK:
	case LT_NSSPAWN_HEAVYARMOR:
	case LT_NSSPAWN_MINES:
	case LT_NSSPAWN_CATALYST:

	case LT_NSABUILD_RESOURCETOWER:
	case LT_NSABUILD_HIVE:
	case LT_NSABUILD_OFFENSECHAMBER:
	case LT_NSABUILD_SENSORYCHAMBER:
	case LT_NSABUILD_MOVEMENTCHAMBER:
	case LT_NSABUILD_DEFENSECHAMBER:

	case LT_NSDEST_OBSERVATORY:
	case LT_NSDEST_SPAWNPORTAL:
	case LT_NSDEST_ARMORY:
	case LT_NSDEST_ADVARMORY:
	case LT_NSDEST_PHASEGATE:
	case LT_NSDEST_RESOURCETOWER:
	case LT_NSDEST_TURRETFACTORY:
	case LT_NSDEST_ADVTURRETFACTORY:
	case LT_NSDEST_TURRET:
	case LT_NSDEST_ARMSLAB:
	case LT_NSDEST_SIEGETURRET:
	case LT_NSDEST_COMMANDCONSOLE:
	case LT_NSDEST_PROTOTYPELAB:

	case LT_NSRECYCLE_OBSERVATORY:
	case LT_NSRECYCLE_SPAWNPORTAL:
	case LT_NSRECYCLE_ARMORY:
	case LT_NSRECYCLE_ADVARMORY:
	case LT_NSRECYCLE_PHASEGATE:
	case LT_NSRECYCLE_RESOURCETOWER:
	case LT_NSRECYCLE_TURRETFACTORY:
	case LT_NSRECYCLE_ADVTURRETFACTORY:
	case LT_NSRECYCLE_TURRET:
	case LT_NSRECYCLE_ARMSLAB:
	case LT_NSRECYCLE_SIEGETURRET:
	case LT_NSRECYCLE_COMMAND:
	case LT_NSRECYCLE_PROTOTYPELAB:

	case LT_NSADEST_RESOURCETOWER:
	case LT_NSADEST_HIVE:
	case LT_NSADEST_OFFENSECHAMBER:
	case LT_NSADEST_SENSORYCHAMBER:
	case LT_NSADEST_MOVEMENTCHAMBER:
	case LT_NSADEST_DEFENSECHAMBER:
		OutputWithPlayerOnly(sLine, nLineType);
		break;

	case LT_CHANGE_ROLE:
		LineChangeRole(sLine);
		break;
	case LT_NS_VOTEDOWN:
		OutputWithTwoPlayers(sLine, nLineType, "against");
		break;
	case LT_TEAMSCORE:
		LineTeamScore(sLine);
		break;
	case LT_A_WIN:
		LineTeamWin(TEAM_A, sLine);
		break;
	case LT_M_WIN:
		LineTeamWin(TEAM_M, sLine);
		break;
	default:
		GIRLLogParserDefault::ProcessLine(sLine);
	}
}

const char* GIRLLogParserNS::TeamName(Team t)
{
	switch (t)
	{
	case TEAM_A:
		return "A";
	case TEAM_M:
		return "M";
	case TEAM_SPECTATOR:
		return "Spec";
	}
	return "";
}

const char* GIRLLogParserNS::TeamDescription(Team t)
{
	switch (t)
	{
	case TEAM_A:
		return "Aliens";
	case TEAM_M:
		return "Marines";
	case TEAM_SPECTATOR:
		return "Spectators";
	}
	return "";
}

std::string GIRLLogParserNS::GetTeamColor(Team t)
{
	IGCLock lock(this, "NS::GetTeamColor");
	switch (t)
	{
	case TEAM_A:
		return m_sAColor;
	case TEAM_M:
		return m_sMColor;
	case TEAM_SPECTATOR:
		return m_sSpecColor;
	}
	return "";
}

const char* GIRLLogParserNS::GetTeamColorDefault(Team t)
{
	switch (t)
	{
	case TEAM_A:
		return "09,01";
	case TEAM_M:
		return "00,02";
	case TEAM_SPECTATOR:
		return "00,14";
	}
	return "";
}

void GIRLLogParserNS::SetTeamColor(Team t, const std::string& sColor)
{
	IGCLock lock(this, "NS::SetTeamColor");
	switch (t)
	{
	case TEAM_A:
		m_sAColor = sColor;
		break;
	case TEAM_M:
		m_sMColor = sColor;
		break;
	case TEAM_SPECTATOR:
		m_sSpecColor = sColor;
		break;
	}
}

Team GIRLLogParserNS::NameTeam(std::string& sTeam)
{
	if (strcmp(sTeam.c_str(), "alien1team") == 0)
		return TEAM_A;
	if (strcmp(sTeam.c_str(), "marine1team") == 0)
		return TEAM_M;
	if (strcmp(sTeam.c_str(), "none") == 0)
		return TEAM_SPECTATOR;

	return TEAM_NONE;
}

const char** GIRLLogParserNS::GameTypeName()
{
	static const char* pNames[] = {
		"ns",
		"nsp",
		NULL
	};
	return pNames;
}

const char* GIRLLogParserNS::GameTypeDescription()
{
	return "Natural Selection";
}

CString GIRLLogParserNS::GetAdditionalTags()
{
	return GIRLLogParserDefault::GetAdditionalTags() +
		"<a-score> - Aliens score\n"
		"<m-score> - Marines score\n";
}

int GIRLLogParserNS::GameTypeImage()
{
	return IMAGE_NS;
}

void GIRLLogParserNS::ReplaceAScore(CString& s)
{
	s.Replace("<a-score>", DecorateScore(TEAM_A, m_nAScore).c_str());
}

void GIRLLogParserNS::ReplaceMScore(CString& s)
{
	s.Replace("<m-score>", DecorateScore(TEAM_M, m_nMScore).c_str());
}

void GIRLLogParserNS::ReplaceRest(CString& sMsg)
{
	ReplaceAScore(sMsg);
	ReplaceMScore(sMsg);
	GIRLLogParserDefault::ReplaceRest(sMsg);
}

void GIRLLogParserNS::LineTeamScore(std::string& sLine)
{
	Team team = (Team)atoi(GetNextVar(sLine).c_str());
	int nScore = atoi(GetNextVar(sLine).c_str());
	int nPlayers = atoi(GetNextVar(sLine).c_str());

	switch (team)
	{
	case TEAM_A:
		m_nAScore = nScore;
		break;
	case TEAM_M:
		m_nMScore = nScore;
		break;
	}
}

void GIRLLogParserNS::LineTeamWin(Team team, std::string& sLine)
{
	CString sMsg = GetIRCMessage(LT_TEAMWIN);
	ReplaceTeam(sMsg, TeamDescription(team));
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_TEAMWIN, sMsg);
}

void GIRLLogParserNS::LineChangeRole(std::string& sLine)
{
	CStatsPlayer player;
	std::string sPlayer = GetNextName(sLine, player);

	std::string sRole = GetRole(sLine);
	player.m_sRole = sRole;

	// update
	m_pReceiver->UpdatePlayer(new UpdatePlayerStruct(player));

	CString sMsg = GetIRCMessage(LT_CHANGE_ROLE);
	ReplacePlayer(sMsg, player);
	ReplaceRest(sMsg);
	OutputIRCMessage(LT_CHANGE_ROLE, sMsg);
}

void GIRLLogParserNS::SetMapName(const char* pName)
{
	m_nAScore = 0;
	m_nMScore = 0;
	GIRLLogParserDefault::SetMapName(pName);
}
