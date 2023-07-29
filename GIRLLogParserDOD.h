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

// GIRLLogParserDOD.h: interface for the GIRLLogParserDOD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGPARSERDOD_H__009B129C_5279_492C_B5FA_DAE90BEB9263__INCLUDED_)
#define AFX_GIRLLOGPARSERDOD_H__009B129C_5279_492C_B5FA_DAE90BEB9263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIRLLogParserDefault.h"

class GIRLLogParserDOD : public GIRLLogParserDefault  
{
public:
	GIRLLogParserDOD();
	virtual ~GIRLLogParserDOD();

	// overrides
	virtual void ProcessLine(std::string& sLine);

	virtual const char* TeamName(Team t);
	virtual const char* TeamDescription(Team t);
	virtual Team NameTeam(std::string& sTeam);
	virtual std::string GetTeamColor(Team t);
	virtual const char* GetTeamColorDefault(Team t);
	virtual void SetTeamColor(Team t, const std::string& sColor);

	virtual const char** GameTypeName();
	virtual int GameTypeImage();
	virtual const char* GameTypeDescription();
	virtual CString GetAdditionalTags();
	virtual void Init();

private:
	// internal helper methods
	void ReplaceAScore(CString& sMsg);
	void ReplaceXScore(CString& sMsg);
	void ReplacePoint(CString& sMsg, const std::string& sReplacement);
	virtual void ReplaceRest(CString& sMsg);

	std::string RoleName(const std::string& sRole);
	std::string WeaponName(const std::string& sWeapon);

	// line handlers
	void LineRoundStart(std::string& sLine);
	void LineRoundEnd(std::string& sLine);
	void LineJoined(std::string& sLine);
	void LineChangeRole(std::string& sLine);

	void LineWin(std::string& sLine, Team tWon);
	void LineCap(std::string& sLine);
	void LinePlayerCap(std::string& sLine);

	void LineRestartMap(std::string& sLine);
	void LineTeamScore(std::string& sLine);

protected:
	// internal helper methods
	IRCMSG RecognizeLine(const std::string& sLine);

	// settings members
	std::string m_sXColor;		// Axis Color
	std::string m_sAColor;		// Allies Color
	std::string m_sSpecColor;	// Spectator Color

	// state members
	int m_nXScore;			// Axis Score
	int m_nXPrevScore;		// Previous Axis Score
	int m_nAScore;			// Allies Score
	int m_nAPrevScore;		// Previous Allies Score

};

#endif // !defined(AFX_GIRLLOGPARSERDOD_H__009B129C_5279_492C_B5FA_DAE90BEB9263__INCLUDED_)
