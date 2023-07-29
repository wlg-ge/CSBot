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

// GIRLLogParserManager.cpp: implementation of the GIRLLogParserManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogParserManager.h"
#include "GIRLLogParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GIRLLogParserManager g_parserManager;

GIRLLogParserManager::GIRLLogParserManager()
{
	m_pReceiver = NULL;
}

GIRLLogParserManager::~GIRLLogParserManager()
{
	ClearParsers();
}

void GIRLLogParserManager::ClearParsers()
{
	for (PARSERLIST::iterator i = m_lParsers.begin(); i != m_lParsers.end(); i++)
		delete (*i);
	m_lParsers.clear();
	m_mParsers.clear();
}

void GIRLLogParserManager::SetReceiver(GIRLLogReceiver* pReceiver)
{
	m_pReceiver = pReceiver;

	for (PARSERLIST::iterator i = m_lParsers.begin(); i != m_lParsers.end(); i++)
	{
		GIRLLogParser* pParser = (*i);
		pParser->InitParser();
	}
}

GIRLLogReceiver* GIRLLogParserManager::GetReceiver()
{
	return m_pReceiver;
}

void GIRLLogParserManager::Add(GIRLLogParser* pParser)
{
	const char** pNames = pParser->GameTypeName();
	while (*pNames != NULL)
	{
		// In debug, make sure we don't have more than one parser trying to fight over the same mod
		ASSERT(m_mParsers.find(*pNames) == m_mParsers.end());
		m_mParsers[*pNames] = pParser;
		pNames++;
	}
	m_lParsers.push_back(pParser);
}

GIRLLogParser* GIRLLogParserManager::GetParserForGame(LPCTSTR pGame)
{
	STR2PARSER::iterator i = m_mParsers.find(pGame);
	if (i == m_mParsers.end())
	{
		i = m_mParsers.find("");
		if (i == m_mParsers.end())
			return NULL;
	}
	return (*i).second;
}

void GIRLLogParserManager::ResetDefaultIRCMessages()
{
	for (PARSERLIST::iterator iParser = m_lParsers.begin(); iParser != m_lParsers.end(); iParser++)
		(*iParser)->ResetDefaultIRCMessages();
}
