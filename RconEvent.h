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

// RconEvent.h: interface for the RconEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RCONEVENT_H__E53534E1_4421_409A_AA15_7B12C32A1800__INCLUDED_)
#define AFX_RCONEVENT_H__E53534E1_4421_409A_AA15_7B12C32A1800__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
class CSchedView;

typedef std::list<std::string> STRLIST;

// This is the base class for all types of events.
// Is is pure virtual.
class CMainFrame;

class RconEvent  
{
public:
	// Default Constructor / Destructor
	RconEvent(CSchedView* pView);
	virtual ~RconEvent();

// MFC 7 uses time64 for CTime operations.
// while MFC 6 uses time_t
#if _MFC_VER <= 0x600
	typedef time_t SchedTime;
#else
	typedef __time64_t SchedTime;
#endif
	void WriteProfileTime(LPCTSTR pSection, LPCTSTR pName, SchedTime timeVal);
	SchedTime GetProfileTime(LPCTSTR pSection, LPCTSTR pName, SchedTime timeVal);
	char* FormatTime(const SchedTime& timeVal);

	// object members
	static SchedTime s_tNow;

	// comment (the name of the event)
	std::string m_sComment;
	// last run-status string
	std::string m_sLastStatus;
	// last time activated
	SchedTime		m_tLastActivated;
	// last "next time" calculated
	SchedTime		m_tLastNextTime;
	// Is this event critical?
	bool		m_bCritical;
	CSchedView* m_pView;

protected:
	// list of commands to be sent as RCON
	STRLIST m_lCmds;

public:
	// return the list of commands for manipulation
	virtual STRLIST& GetCmds() {return m_lCmds;};

	// did we go past the last "next time" calculated?
	virtual bool HasEnoughTimePassed();

	// run this event (bNonCritical - run even if not critical)
	virtual void Run(int nRunLevel, CMainFrame* pMain);

	// send the commands. called by Run() if EnoughTimePassed.
	virtual void Activate(CMainFrame* pMain);

	// Retrieve the comment
	virtual const char* GetComment() {return m_sComment.c_str();};
	// Convert the last time activated into a string
	virtual std::string GetLastTimeString();
	// Convert the last "next time" calculated into a string
	virtual std::string GetNextTimeString();
	// retrieve / recalc the next time
	virtual SchedTime GetNextTime(bool bRecalc);
	// calculate next time (as if last "next time" occured, if passed)
	virtual SchedTime CalcNextTime() = 0;
	// returns the type. used to identify the event type (instead of dynamic_casting)
	virtual int  GetType() = 0;
	// read the event parameters (type specific) from file
//	virtual bool ReadFromFile(FILE* pFile);
	// write the event parameters (type specific) to file
//	virtual bool WriteToFile(FILE* pFile);
	virtual bool ReadFromSection(LPCTSTR pSection);
	virtual bool WriteToSection(LPCTSTR pSection);
};

#endif // !defined(AFX_RCONEVENT_H__E53534E1_4421_409A_AA15_7B12C32A1800__INCLUDED_)
