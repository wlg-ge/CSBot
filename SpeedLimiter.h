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

#ifndef __SPEEDLIMITER_H__
#define __SPEEDLIMITER_H__

#include "IGCLockable.h"

class SpeedLimiter : protected CIGCLockable
{
public:
	SpeedLimiter();
	virtual ~SpeedLimiter();

	void BeforeBuf();
	void AfterBuf();
	void SetSpeed(int nTicks, int nSecs);

private:
	int m_nTicks;
	int m_nSecs;
	DWORD* m_pBufferTicks;
	DWORD m_dwNow;
};

#endif
