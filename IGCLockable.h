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

#ifndef __IGCLockable_H__
#define __IGCLockable_H__

class CIGCLockable {
public:
	CIGCLockable(LPCTSTR pName);
	virtual ~CIGCLockable();

	void Lock(LPCTSTR pReason = NULL);
	void Unlock();

protected:
#if _TRACES >= 1
	LPCTSTR m_pName;
#endif
	HANDLE	m_hMutex;
	int		m_nLockCount;
};

class IGCLock {
public:
	IGCLock(CIGCLockable* pLock, LPCTSTR pReason = NULL) {m_pLock = pLock; m_pLock->Lock(pReason);};
	virtual ~IGCLock() {m_pLock->Unlock();};
protected:
	CIGCLockable* m_pLock;
};

#endif
