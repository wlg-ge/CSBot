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

// ChildView.cpp : implementation of the CRawLogView class
//

#include "stdafx.h"
#include "NewCSBot.h"
#include "RawLogView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRawLogView

CRawLogView::CRawLogView(CChildFrame* pFrame) : CCSBotView(pFrame)
{
}

CRawLogView::~CRawLogView()
{
}


BEGIN_MESSAGE_MAP(CRawLogView,CWnd )
	//{{AFX_MSG_MAP(CRawLogView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define VIEW_NAME "Raw log"
/////////////////////////////////////////////////////////////////////////////
// CRawLogView message handlers

BOOL CRawLogView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;
	if (!m_wndOutput.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, 101))
		return FALSE;

	CFont font;
	font.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
	m_wndOutput.SetFont(&font, FALSE);
	m_wndOutput.SetMaxLines(1000);
	
	SetWindowText(VIEW_NAME);
	return TRUE;
}

void CRawLogView::SetReconnect(int nSec, int nTimes)
{
	if ((nSec < 0) || (nTimes < 0))
	{
		SetWindowText(VIEW_NAME);
	} else {
		CString sTitle;
		sTitle.Format(VIEW_NAME ". Reconnecting in %d... (%d attempts left)", nSec, nTimes);
		SetWindowText(sTitle);
	}
}

void CRawLogView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
	if (m_wndOutput.GetSafeHwnd() != NULL)
		m_wndOutput.MoveWindow(0, 0, cx, cy);
}

void CRawLogView::EnableControls()
{
	m_wndOutput.Clear();
}

void CRawLogView::AddLine(CString& sLine)
{
	sLine.TrimLeft();
	sLine.TrimRight();
	if (sLine.IsEmpty())
		return;

	CString sText = sLine + "\r\n";
	m_wndOutput.AddText(sText);
}

