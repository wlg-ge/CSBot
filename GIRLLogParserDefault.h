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

// GIRLLogParserDefault.h: interface for the GIRLLogParserDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGPARSERDEFAULT_H__B73E0DAB_1716_49BB_8B70_F7783A00455B__INCLUDED_)
#define AFX_GIRLLOGPARSERDEFAULT_H__B73E0DAB_1716_49BB_8B70_F7783A00455B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIRLLogParser.h"

#define LT_START_MAP		0x40000001
#define LT_ENTERED			0x40000002
#define LT_CHANGE_NAME		0x40000003
#define LT_DISCONNECTED		0x40000005
#define LT_SERVER_SHUTDOWN	0x40000006
#define LT_SAY_SERVER		0x40000007
#define LT_SAY				0x40000008
#define LT_SAY_DEAD			0x40000009
#define LT_SAY_TEAM			0x4000000A
#define LT_SAY_TEAM_DEAD	0x4000000B
#define LT_ATTACK			0x4000000D
#define LT_ATTACK_FRIENDLY	0x4000000E
#define LT_KILL				0x4000000F
#define LT_KILL_FRIENDLY	0x40000010
#define LT_SUICIDE			0x40000011

class GIRLLogParserDefault : public GIRLLogParser
{
public:
	GIRLLogParserDefault();
	virtual ~GIRLLogParserDefault() {};

protected:
	int RecognizeLine(const std::string& sLine);

private:
	void LineStartMap(std::string& sLine);
	void LineEntered(std::string& sLine);
	void LineChangeName(std::string& sLine);
	void LineKill(std::string& sLine);
	void LineKillFriendly(std::string& sLine);
	void LineSuicide(std::string& sLine);
	void LineDisconnect(std::string& sLine);
	void LineServerShutdown(std::string& sLine);
	void LineSayServer(std::string& sLine);
	void LineSay(std::string& sLine);
	void LineSayDead(std::string& sLine);
	void LineSayTeam(std::string& sLine);
	void LineSayTeamDead(std::string& sLine);
	void LineAttack(std::string& sLine);

public:
	virtual void ProcessLine(std::string& sLine);

	virtual const char* TeamName(Team t);
	virtual const char* TeamDescription(Team t);
	virtual int GameTypeImage();
	virtual Team NameTeam(std::string& sTeam);
	virtual std::string GetTeamColor(Team t);
	virtual const char* GetTeamColorDefault(Team t);
	virtual void SetTeamColor(Team t, const std::string& sColor);

	virtual void Init() {};
	virtual const char** GameTypeName();
	virtual const char* GameTypeDescription();
};

#endif // !defined(AFX_GIRLLOGPARSERDEFAULT_H__B73E0DAB_1716_49BB_8B70_F7783A00455B__INCLUDED_)
