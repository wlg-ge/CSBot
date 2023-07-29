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

// RconEvent.cpp: implementation of the RconEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "RconEvent.h"
#include "SchedView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// statics definition
RconEvent::SchedTime	RconEvent::s_tNow;

// Ctor
RconEvent::RconEvent(CSchedView* pView)
{
	m_tLastActivated = 0;
	m_tLastNextTime = 0;
	m_bCritical = false;
	m_sComment = "No Comment";
	m_pView = pView;
}

// Dtor
RconEvent::~RconEvent()
{

}

void RconEvent::WriteProfileTime(LPCTSTR pSection, LPCTSTR pName, SchedTime timeVal)
{
#if _MFC_VER <= 0x600
	AfxGetApp()->WriteProfileInt(pSection, pName, timeVal);
#else
	AfxGetApp()->WriteProfileBinary(pSection, pName, (LPBYTE)&timeVal, sizeof(timeVal));
#endif
}

RconEvent::SchedTime RconEvent::GetProfileTime(LPCTSTR pSection, LPCTSTR pName, SchedTime timeVal)
{
#if _MFC_VER <= 0x600
	return (time_t)AfxGetApp()->GetProfileInt(pSection, pName, timeVal);
#else
	SchedTime ret = timeVal;

	LPBYTE pBuf;
	UINT uSize;
	if (AfxGetApp()->GetProfileBinary(pSection, pName, &pBuf, &uSize) && (pBuf != NULL) && (uSize == sizeof(timeVal)))
		memcpy(&ret, pBuf, sizeof(timeVal));
	if ((uSize > 0) && (pBuf != NULL))
		delete [] pBuf;
	return ret;
#endif
}

char* RconEvent::FormatTime(const SchedTime& timeVal)
{
#if _MFC_VER <= 0x600
	return ctime(&timeVal);
#else
	return _ctime64(&timeVal);
#endif
}

void RconEvent::Run(int nRunLevel, CMainFrame* pMain)
{
	if ((nRunLevel == RUNLEVEL_ALL) || (m_bCritical && (nRunLevel == RUNLEVEL_CRITICAL))) {
		// if this is critical, or we are permitted to run non-critical
		if (HasEnoughTimePassed()) {
			// it's time to run...
			Activate(pMain);
			// recalc the next time to run
			GetNextTime(true);
			// and update the last time activated
			m_tLastActivated = s_tNow;
		}
	} else {
		// this is non-critical, and we're not permitted to run it.
		m_sLastStatus = "** Skipped. Not a critical event **";
	}
}

void RconEvent::Activate(CMainFrame* pMain)
{
	std::string sRes;
	m_sLastStatus = "";
	// go over the command list
	for (STRLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++) {
		// send the RCON commands to the server one by one
		pMain->IssueRcon((*i).c_str());
	}
}

bool RconEvent::ReadFromSection(LPCTSTR pSection)
{
	m_sComment = AfxGetApp()->GetProfileString(pSection, "Comment", "");
	m_tLastActivated = GetProfileTime(pSection, "LastActivated", 0);
	m_bCritical = AfxGetApp()->GetProfileInt(pSection, "Critical", 0) ? true : false;
	int nCount = AfxGetApp()->GetProfileInt(pSection, "CommandCount", 0);
	if (nCount < 0) return false;

	CString sName, sCmd;
	for (int i=0; i<nCount; i++)
	{
		sName.Format("Command%d", i);
		sCmd = AfxGetApp()->GetProfileString(pSection, sName, "");
		if (!sCmd.IsEmpty()) {
			m_lCmds.push_back((LPCTSTR)sCmd);
		}
	}

	m_sLastStatus = AfxGetApp()->GetProfileString(pSection, "LastStatus", "");
	return true;
}

bool RconEvent::WriteToSection(LPCTSTR pSection)
{
	AfxGetApp()->WriteProfileString(pSection, "Comment", m_sComment.c_str());
	WriteProfileTime(pSection, "LastActivated", m_tLastActivated);
	AfxGetApp()->WriteProfileInt(pSection, "Critical", m_bCritical ? 1 : 0);
	int nItem = 0, nCount = m_lCmds.size();
	AfxGetApp()->WriteProfileInt(pSection, "CommandCount", nCount);
	
	CString sName, sCmd;
	for (STRLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++, nItem++)
	{
		sName.Format("Command%d", nItem);
		AfxGetApp()->WriteProfileString(pSection, sName, (*i).c_str());
	}

	AfxGetApp()->WriteProfileString(pSection, "LastStatus", m_sLastStatus.c_str());

	return true;
}

std::string RconEvent::GetLastTimeString()
{
	if (m_tLastActivated == 0) 
		return "Never";

	// convert using ctime() to generic time format
	std::string sStr;
	sStr.assign(FormatTime(m_tLastActivated), 0, 24);
	return sStr;
}

std::string RconEvent::GetNextTimeString()
{
	SchedTime tTime = GetNextTime(false);
	if (tTime == 0) 
		return "Never";

	// convert using ctime() to generic time format
	std::string sStr;
	sStr.assign(FormatTime(tTime), 0, 24);
	return sStr;
}

RconEvent::SchedTime RconEvent::GetNextTime(bool bRecalc)
{
	// get last "next" or recalc.
	if (bRecalc || (m_tLastNextTime == 0))
		m_tLastNextTime = CalcNextTime();
	return m_tLastNextTime;
}

bool RconEvent::HasEnoughTimePassed()
{
	// have we gone beyond the last "next time" ?
	SchedTime tNext = GetNextTime(false);
	return (tNext == 0) ? false : (s_tNow >= tNext);
}
