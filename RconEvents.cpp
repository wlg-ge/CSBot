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

#include "stdafx.h"
#include "RconEvents.h"

#define FILEREADP(xp, xSize) if (fread((xp), 1, xSize, pFile) != xSize) return false
#define FILEREAD(x) FILEREADP(&x, sizeof(x))
#define FILEWRITEP(xp, xSize) if (fwrite((xp), 1, xSize, pFile) != xSize) return false
#define FILEWRITE(x) FILEWRITEP(&x, sizeof(x))

// ***********************
//
// Once
//
// ***********************

RconEventOnce::SchedTime RconEventOnce::CalcNextTime()
{
	return !m_bActivated ? m_tTime : 0;
}
/*
bool RconEventOnce::HasEnoughTimePassed()
{
	return ((s_tNow >= m_tTime) && (!m_bActivated));
};
*/
bool RconEventOnce::ReadFromSection(LPCTSTR pSection)
{
	if (!RconEvent::ReadFromSection(pSection))
		return false;
	m_tTime = GetProfileTime(pSection, "Time", 0);
	m_bActivated = AfxGetApp()->GetProfileInt(pSection, "Activated", 0) ? true : false;
	return true;
}

bool RconEventOnce::WriteToSection(LPCTSTR pSection)
{
	if (!RconEvent::WriteToSection(pSection))
		return false;
	WriteProfileTime(pSection, "Time", m_tTime);
	AfxGetApp()->WriteProfileInt(pSection, "Activated", m_bActivated ? 1 : 0);
	return true;
}

void RconEventOnce::Activate(CMainFrame* pMain)
{
	RconEvent::Activate(pMain);
	m_bActivated = true;
}


// ***********************
//
// Every Minute
//
// ***********************

RconEventMinute::SchedTime RconEventMinute::CalcNextTime()
{
	return s_tNow + (m_nAmount * 60);
}
/*
bool RconEventMinute::HasEnoughTimePassed()
{
	return (s_tNow-m_tLastActivated >= (60 * m_nAmount));
}
*/
bool RconEventMinute::ReadFromSection(LPCTSTR pSection)
{
	if (!RconEvent::ReadFromSection(pSection))
		return false;
	m_nAmount = AfxGetApp()->GetProfileInt(pSection, "Amount", 0);
	return true;
}

bool RconEventMinute::WriteToSection(LPCTSTR pSection)
{
	if (!RconEvent::WriteToSection(pSection))
		return false;
	AfxGetApp()->WriteProfileInt(pSection, "Amount", m_nAmount);
	return true;
}


// ***********************
//
// Every Hour
//
// ***********************

RconEventHour::SchedTime RconEventHour::CalcNextTime()
{
	if (m_bOnTheHour)
		return (s_tNow - (s_tNow % (60*60)) + (m_nAmount * 60*60));
	else
		return s_tNow + (m_nAmount * 60*60);
}
/*
bool RconEventHour::HasEnoughTimePassed()
{
	if (m_tLastActivated == 0) {
		return true;
	} else {
		return (s_tNow >= GetNextTime());
	}
}
*/
bool RconEventHour::ReadFromSection(LPCTSTR pSection)
{
	if (!RconEvent::ReadFromSection(pSection))
		return false;
	m_nAmount = AfxGetApp()->GetProfileInt(pSection, "Amount", 0);
	m_bOnTheHour = AfxGetApp()->GetProfileInt(pSection, "OnTheHour", 0) ? true : false;
	return true;
}

bool RconEventHour::WriteToSection(LPCTSTR pSection)
{
	if (!RconEvent::WriteToSection(pSection))
		return false;
	AfxGetApp()->WriteProfileInt(pSection, "Amount", m_nAmount);
	AfxGetApp()->WriteProfileInt(pSection, "OnTheHour", m_bOnTheHour ? 1 : 0);
	return true;
}


// ***********************
//
// Every Day
//
// ***********************

RconEventDay::SchedTime RconEventDay::CalcNextTime()
{
	return (s_tNow - (s_tNow % 86400)) + (m_nAmount * 86400) + m_tTime;
}
/*
bool RconEventDay::HasEnoughTimePassed()
{
	if (m_tLastActivated == 0) {
		return true;
	} else {
		return (s_tNow >= GetNextTime());
	}
}
*/
bool RconEventDay::ReadFromSection(LPCTSTR pSection)
{
	if (!RconEvent::ReadFromSection(pSection))
		return false;
	m_nAmount = AfxGetApp()->GetProfileInt(pSection, "Amount", 0);
	m_tTime = GetProfileTime(pSection, "Time", 0);
	return true;
}

bool RconEventDay::WriteToSection(LPCTSTR pSection)
{
	if (!RconEvent::WriteToSection(pSection))
		return false;
	AfxGetApp()->WriteProfileInt(pSection, "Amount", m_nAmount);
	WriteProfileTime(pSection, "Time", m_tTime);
	return true;
}


