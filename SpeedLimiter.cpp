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
#include "SpeedLimiter.h"

SpeedLimiter::SpeedLimiter() : CIGCLockable("SpeedLimit") 
{
	SetSpeed(1, 1);
}

SpeedLimiter::~SpeedLimiter()
{
	if (m_pBufferTicks != NULL)
		delete [] m_pBufferTicks;
}

void SpeedLimiter::SetSpeed(int nTicks, int nSecs)
{
	ASSERT(nTicks > 0);
	ASSERT(nSecs > 0);
	IGCLock lock(this, "SetSpeed");

	m_dwNow = GetTickCount();
	if (m_pBufferTicks == NULL)
	{
		m_pBufferTicks = new DWORD[nTicks];
		for (int i = 0; i < nTicks; i++)
			m_pBufferTicks[i] = m_dwNow-(nSecs*1000);
	} else {
		DWORD* pBufferTicks = new DWORD[nTicks];
		memcpy(pBufferTicks, m_pBufferTicks, sizeof(DWORD)*min(m_nTicks, nTicks));
		for (int i = m_nTicks; i < nTicks; i++)
			pBufferTicks[i] = m_pBufferTicks[m_nTicks-1];
		delete [] m_pBufferTicks;
		m_pBufferTicks = pBufferTicks;
	}
	m_nTicks = nTicks;
	m_nSecs = nSecs;
}

void SpeedLimiter::BeforeBuf()
{
	Lock("BeforeBuf");

	m_dwNow = GetTickCount();
	int nTimeDiff = (m_dwNow - m_pBufferTicks[m_nTicks-1]);
	if (nTimeDiff <= 0) nTimeDiff = 1;

#if _TRACES >= 6
	TRACE("SpeedLimiter %p: %d ticks in %d ms. Allowed: %d sec\n", this, m_nTicks, nTimeDiff, m_nSecs);
#endif
	while (nTimeDiff < (m_nSecs*1000)) {
		Sleep(1000);
		m_dwNow = GetTickCount();
		nTimeDiff = (m_dwNow - m_pBufferTicks[m_nTicks-1]);
		if (nTimeDiff <= 0) nTimeDiff = 1;
#if _TRACES >= 6
		TRACE("SpeedLimiter %p: %d ticks in %d ms. Allowed: %d sec\n", this, m_nTicks, nTimeDiff, m_nSecs);
#endif
	}
}

void SpeedLimiter::AfterBuf()
{
	m_dwNow = GetTickCount();
	memmove(m_pBufferTicks+1, m_pBufferTicks, sizeof(DWORD)*(m_nTicks-1));
	m_pBufferTicks[0] = m_dwNow;
	Unlock();
}
