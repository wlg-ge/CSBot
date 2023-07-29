/*
	CSBot - An IRC bot for publishing CS and NS games
	Copyright (C) 2004 Harry J Walsh
	Email: puzl@jarhedz.com
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
// LogFilter.cpp: implementation of the LogFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFilter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogFilter::LogFilter()
{
	oneSteam.setRegex("\\(.*\\)\\(<[0-9]*><STEAM_[0-9]:[0-9]:[0-9]*>\\)\\(.*\\)");
	twoSteam.setRegex("\\(.*\\)\\(<[0-9]*><STEAM_[0-9]:[0-9]:[0-9]*>\\)\\(.*\\)\\(<[0-9]*><STEAM_[0-9]:[0-9]:[0-9]*>\\)\\(.*\\)");
	world.setRegex("^L .*-.*: World triggered.*");
	team.setRegex("^L .*-.*: Team \"[A-Z]*\" triggered.*");
	connected.setRegex("^L .*-.*: .*<[0-9]*><STEAM_ID_PENDING><>.*");
	validated.setRegex("^L .*-.*: .*<[0-9]*><STEAM_[0-9]:[0-9]:[0-9]*><>.*STEAM USERID validated");
}

LogFilter::~LogFilter()
{

}

void LogFilter::filter(CString& sLine) {
	if ( connected.matchLine((const char *)sLine) || validated	.matchLine( (const char *)sLine) ) {
			sLine.Empty();
	}
	else {
		if ( twoSteam.matchLine((const char *)sLine)){
			bool selections[5];
			selections[0]=true;
			selections[1]=false;
			selections[2]=true;
			selections[3]=false;
			selections[4]=true;
			sLine=twoSteam.clean(selections, 5);
		}
		else if ( oneSteam.matchLine((const char *)sLine )){
			bool selections[3];
			selections[0]=true;
			selections[1]=false;
			selections[2]=true;

			sLine=oneSteam.clean(selections, 3);

		}
		else {
			if (world.matchLine( (const char *)sLine) == false && 
				team.matchLine((const char *)sLine) == false ) {
				sLine.Empty();
			}
		}
	}
}