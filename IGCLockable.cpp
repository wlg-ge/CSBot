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
#include "IGCLockable.h"

CIGCLockable::CIGCLockable(LPCTSTR pName)
{
#if _TRACES >= 9
	m_pName = pName;
#endif
	m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	m_nLockCount = 0;
}

CIGCLockable::~CIGCLockable()
{
	CloseHandle(m_hMutex);
}

void CIGCLockable::Lock(LPCTSTR pReason /* = NULL */)
{
	DWORD dwRet;
#if _TRACES >= 9
	int nCount = 0;
#endif
	do {
		dwRet = ::WaitForSingleObject(m_hMutex, 500);
#if _TRACES >= 9
		nCount++;
		if (nCount > 10) {
			char c[1024];
			sprintf(c, "Lock halted object %p (%s). so far %d times. lock count %d. Trying lock by thread %u for %s\n", this, m_pName, nCount, m_nLockCount, ::GetCurrentThreadId(), pReason);
			::OutputDebugString(c);
		}
#endif
	} while (dwRet != WAIT_OBJECT_0);
	m_nLockCount++;
#if _TRACES >= 9
	char c[1024];
	sprintf(c, "Object %p (%s) locked #%d by thread %u for %s\n", this, m_pName, m_nLockCount, ::GetCurrentThreadId(), pReason);
	::OutputDebugString(c);
#endif
}

void CIGCLockable::Unlock()
{
	if (m_nLockCount > 0) {
		m_nLockCount--;
#if _TRACES >= 9
		char c[1024];
		sprintf(c, "Object %p (%s) unlocked #%d by thread %u\n", this, m_pName, m_nLockCount, ::GetCurrentThreadId());
		::OutputDebugString(c);
#endif
		::ReleaseMutex(m_hMutex);
#if _TRACES >= 9
	} else {
		char c[1024];
		sprintf(c, "Lock underrun on object %p (%s). lock count is %d\n", this, m_pName, m_nLockCount);
		::OutputDebugString(c);
		ASSERT(FALSE);
#endif
	}
}
