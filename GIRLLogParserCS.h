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

// GIRLLogParserCS.h: interface for the GIRLLogParserCS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGPARSERCS_H__90B3232E_A375_4B3D_B4D8_F910E4A07491__INCLUDED_)
#define AFX_GIRLLOGPARSERCS_H__90B3232E_A375_4B3D_B4D8_F910E4A07491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIRLLogParserDefault.h"

class GIRLLogParserCS : public GIRLLogParserDefault
{
public:
	GIRLLogParserCS();
	virtual ~GIRLLogParserCS();

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

	virtual void SetMapName(const char* pName);

private:
	// internal helper methods
	void ReplaceTScore(CString& sMsg);
	void ReplaceCTScore(CString& sMsg);
	virtual void ReplaceRest(CString& sMsg);
	bool GetScores(std::string& sLine);
	void OutputWithScoresOnly(std::string& sLine, IRCMSG uMsg);

	// line handlers
	void LineRoundStart(std::string& sLine);
	void LineRoundEnd(std::string& sLine);
	void LineJoined(std::string& sLine);
	void LineWinCT(std::string& sLine);
	void LineWinT(std::string& sLine);
	void LineRestartRound(std::string& sLine);

protected:
	// internal helper methods
	IRCMSG RecognizeLine(const std::string& sLine);

	// settings members
	std::string m_sTColor;
	std::string m_sCTColor;
	std::string m_sSpecColor;

	// state members
	int m_nCTScore;
	int m_nTScore;
	bool roundRestarting;
};

#endif // !defined(AFX_GIRLLOGPARSERCS_H__90B3232E_A375_4B3D_B4D8_F910E4A07491__INCLUDED_)
