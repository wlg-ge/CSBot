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
// CrossThreadsMessagingDevice.cpp

#include "stdafx.h"
#include "CrossThreadsMessagingDevice.h"


LPCTSTR CCrossThreadsMessagingDevice::m_lpszClassName = _T("CCrossThreadsMessagingDevice_HiddenWindow");
int CCrossThreadsMessagingDevice::m_iCount = 0;


CCrossThreadsMessagingDevice::CCrossThreadsMessagingDevice()
	: m_hWnd(NULL), m_pMonitor(NULL)
{
	if( m_iCount++ == 0 )
	{
		const WNDCLASS wc = 
			{
				0,
				HiddenWindowProc,
				sizeof(DWORD) * 2,
				sizeof(DWORD) * 2,
				GetModuleHandle(NULL),
				(HICON)NULL,
				(HCURSOR)NULL,
				(HBRUSH)(COLOR_WINDOW + 1),
				(LPCTSTR)NULL,
				m_lpszClassName
			};

		if( !RegisterClass(&wc) )
			return;
	}

	m_hWnd = CreateWindow(
			m_lpszClassName,
			_T(""),
			WS_OVERLAPPED,
			0, 0, 0, 0,
			(HWND)NULL,
			(HMENU)NULL,
			GetModuleHandle(NULL),
			this
			);
}

CCrossThreadsMessagingDevice::~CCrossThreadsMessagingDevice()
{
	if( ::IsWindow(m_hWnd) )
		DestroyWindow(m_hWnd);

	if( --m_iCount == 0 )
	{
		UnregisterClass(m_lpszClassName, GetModuleHandle(NULL));
	}
}

void CCrossThreadsMessagingDevice::Post(WPARAM wParam, LPARAM lParam)
{
	ASSERT(::IsWindow(m_hWnd));
	PostMessage(m_hWnd, HWM_DATA, wParam, lParam);
}

LRESULT WINAPI CCrossThreadsMessagingDevice::HiddenWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT rc = 0;

	CCrossThreadsMessagingDevice* pThis = 
		(CCrossThreadsMessagingDevice*)GetWindowLong(hWnd, GWL_USERDATA);

	switch( uMsg )
	{
		case WM_NCCREATE :
		{
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
			ASSERT(lpcs->lpCreateParams != NULL);
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpcs->lpCreateParams);
			rc = TRUE;
			break;
		}

		case HWM_DATA :
		{
			ASSERT(pThis != NULL);
			if( pThis->m_pMonitor )
				pThis->m_pMonitor->OnCrossThreadsMessage(wParam, lParam);
			break;
		}

		default :
		{
			rc = DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}
	}

	return rc;
}

