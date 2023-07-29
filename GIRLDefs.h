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

#ifndef __GIRLDefs_H__
#define __GIRLDefs_H__

#include <list>
#include <set>
#include <map>
#include <string>

#ifdef _DEBUG
#define CSBOT_TYPE "_DEBUG_ "
#else
#ifdef _TRACES
#define CSBOT_TYPE "_TRACING_ "
#else
#define CSBOT_TYPE ""
#endif
#endif
#define CSBOT_VERSION "0.46b"
#define CSBOT_VERSIONINFO "CSBot " CSBOT_TYPE "Version " CSBOT_VERSION
#define CSBOT_COPYRIGHT "(c) Ilan Tayary. 2001, 2002, 2003, 2004"
#define CSBOT_WEBSITE "http://www.csbot.org"

#define TEAM_COUNT 6
enum Team {TEAM_NONE = 0, TEAM_ONE, TEAM_TWO, TEAM_THREE, TEAM_FOUR, TEAM_SPECTATOR};

typedef unsigned long IRCMSG;
#define IRCMSG_LAST (0xffff)

#define IMAGE_NONE			0
#define IMAGE_CS			1
#define IMAGE_NS			2
#define IMAGE_HL			3
#define IMAGE_COLORS		4
#define IMAGE_SPAWN			5
#define IMAGE_BUILD			6
#define IMAGE_RESEARCH		7
#define IMAGE_GAMEFLOW		8
#define IMAGE_OBJECTIVES	9
#define IMAGE_OTHERS		10
#define IMAGE_DOD			11
#define IMAGE_RECYCLE		12
#define IMAGE_TFC			13

enum MsgType {
	MsgType_Gameflow,
	MsgType_Objectives,
	MsgType_Build,
	MsgType_Research,
	MsgType_Spawn,
	MsgType_Other,
	MsgType_Recycle,
	MsgType_Last
};

#endif
