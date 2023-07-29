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

// GIRLLogParserManager.h: interface for the GIRLLogParserManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGPARSERMANAGER_H__372B125A_8C85_47FE_B183_1C474091F442__INCLUDED_)
#define AFX_GIRLLOGPARSERMANAGER_H__372B125A_8C85_47FE_B183_1C474091F442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIRLDefs.h"

class GIRLLogParser;
class GIRLLogReceiver;
struct IRCCustomMsg;

typedef std::map<std::string, GIRLLogParser*> STR2PARSER;
typedef std::list<GIRLLogParser*> PARSERLIST;

class GIRLLogParserManager  
{
public:
	GIRLLogParserManager();
	virtual ~GIRLLogParserManager();

protected:
	// mod name -> parser object
	STR2PARSER m_mParsers;
	// list of all registered parsers
	PARSERLIST m_lParsers;
	// current receiver
	GIRLLogReceiver* m_pReceiver;

public:
	STR2PARSER* GetParserMap() {return &m_mParsers;};
	PARSERLIST* GetParserList() {return &m_lParsers;};

	GIRLLogReceiver* GetReceiver();
	void SetReceiver(GIRLLogReceiver* pReceiver);
	void Add(GIRLLogParser* pParser);
	void ClearParsers();
	void ResetDefaultIRCMessages();

	GIRLLogParser* GetParserForGame(LPCTSTR pGame);
};

extern GIRLLogParserManager g_parserManager;

#endif // !defined(AFX_GIRLLOGPARSERMANAGER_H__372B125A_8C85_47FE_B183_1C474091F442__INCLUDED_)
