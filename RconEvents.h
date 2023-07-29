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

#ifndef __RconEvents_H__
#define __RconEvents_H__

#include "Resource.h"
#include "RconEvent.h"

class RconEventMinute : public RconEvent {
public:
	RconEventMinute(CSchedView* pView) : RconEvent(pView) {m_nAmount = 1;};
	int m_nAmount;
	virtual SchedTime CalcNextTime();
	virtual int GetType() {return ID_INSERT_MINUTE;};
	virtual bool ReadFromSection(LPCTSTR pSection);
	virtual bool WriteToSection(LPCTSTR pSection);
};

class RconEventHour : public RconEvent {
public:
	RconEventHour(CSchedView* pView) : RconEvent(pView) {m_nAmount = 1; m_bOnTheHour = 0;};
	int m_nAmount;
	bool m_bOnTheHour;
	virtual SchedTime CalcNextTime();
	virtual int GetType() {return ID_INSERT_HOUR;};
	virtual bool ReadFromSection(LPCTSTR pSection);
	virtual bool WriteToSection(LPCTSTR pSection);
};

class RconEventDay : public RconEvent {
public:
	RconEventDay(CSchedView* pView) : RconEvent(pView) {m_nAmount = 1; m_tTime = 0;};
	int m_nAmount;
	SchedTime m_tTime;
	virtual SchedTime CalcNextTime();
	virtual int GetType() {return ID_INSERT_DAY;};
	virtual bool ReadFromSection(LPCTSTR pSection);
	virtual bool WriteToSection(LPCTSTR pSection);
};

class RconEventOnce : public RconEvent {
public:
	RconEventOnce(CSchedView* pView) : RconEvent(pView) {m_tTime = time(NULL); m_bActivated = false;};
	SchedTime m_tTime;
	bool m_bActivated;
	virtual SchedTime CalcNextTime();
	virtual int GetType() {return ID_INSERT_ONCE;};
	virtual bool ReadFromSection(LPCTSTR pSection);
	virtual bool WriteToSection(LPCTSTR pSection);
	virtual void Activate(CMainFrame* pMain);
};

#endif