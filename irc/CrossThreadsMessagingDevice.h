/*
	This file is adopted from Adi Degani's IRC library
	(c) Adi Degani
	For more details, see: http://www.codeguru.com/network/irc.html

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
// CrossThreadsMessagingDevice.h

#ifndef _CrossThreadsMessagingDevice_H_
#define	_CrossThreadsMessagingDevice_H_


class CCrossThreadsMessagingDevice
{
public :
	struct ICrossThreadsMessagingDeviceMonitor
	{
		virtual void OnCrossThreadsMessage(WPARAM wParam, LPARAM lParam) = 0;
	};

	CCrossThreadsMessagingDevice();
	virtual ~CCrossThreadsMessagingDevice();

	void SetMonitor(ICrossThreadsMessagingDeviceMonitor* pMonitor) { m_pMonitor = pMonitor; }
	void Post(WPARAM wParam, LPARAM lParam);

	operator bool() const { return ::IsWindow(m_hWnd)==TRUE; }

private :
	enum { HWM_DATA = WM_USER + 1000 };
	static LPCTSTR m_lpszClassName;
	static int m_iCount;
	HWND m_hWnd;
	ICrossThreadsMessagingDeviceMonitor* m_pMonitor;

	static LRESULT WINAPI HiddenWindowProc(HWND, UINT, WPARAM, LPARAM);
};



#endif // _CrossThreadsMessagingDevice_H_

