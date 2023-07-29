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

// ChildView.cpp : implementation of the CIRCView class
//

#include "stdafx.h"
#include "NewCSBot.h"
#include "IRCView.h"
#include "ChildFrm.h"

#include "MainFrm.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_RESPONSE	101
#define IDC_IRCCOMMAND	102
#define IDC_SEND		103

/////////////////////////////////////////////////////////////////////////////
// CIRCView

CIRCView::CIRCView(CChildFrame* pFrame) : CCSBotView(pFrame)
{
	m_hAccel = NULL;
}

CIRCView::~CIRCView()
{
}


BEGIN_MESSAGE_MAP(CIRCView,CWnd )
	//{{AFX_MSG_MAP(CIRCView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_COMMAND(ID_ENTER, OnEnter)
END_MESSAGE_MAP()

#define VIEW_NAME "IRC console"
/////////////////////////////////////////////////////////////////////////////
// CIRCView message handlers
BOOL CIRCView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle | WS_TABSTOP, rect, pParentWnd, nID, pContext))
		return FALSE;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_CONTROLPARENT);

	if (!m_wndOutput.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_RESPONSE))
		return FALSE;
	if (!m_ctlCommand.Create(WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_DROPDOWN , CRect(0,0,0,0), this, IDC_IRCCOMMAND))
		return FALSE;
	if (!m_ctlSend.Create("&Send", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP, CRect(0,0,0,0), this, IDC_SEND))
		return FALSE;

	ACCEL acc;
	acc.fVirt = FVIRTKEY;
	acc.key = VK_RETURN;
	acc.cmd = ID_ENTER;
	m_hAccel = ::CreateAcceleratorTable(&acc, 1);

	CFont fontCtls;
	fontCtls.CreateStockObject(DEFAULT_GUI_FONT);
	m_ctlCommand.SetFont(&fontCtls, FALSE);
	m_ctlSend.SetFont(&fontCtls, FALSE);

	CFont font;
	font.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
	m_wndOutput.SetFont(&font, FALSE);
	m_wndOutput.SetMaxLines(1000);

	m_ctlCommand.SetFocus();

	SetWindowText(VIEW_NAME);
	m_hEditbox = m_ctlCommand.GetWindow(GW_CHILD)->GetSafeHwnd();
	return FALSE;
}

void CIRCView::SetReconnect(int nSec, int nTimes)
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

#define SIZE_EDITBOX_HEIGHT	16
#define SIZE_BUTTON_WIDTH	55
#define SIZE_SPACE_X		5
#define SIZE_SPACE_Y		3

void CIRCView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if (m_wndOutput.GetSafeHwnd() != NULL) {
		int nXEdge = ::GetSystemMetrics(SM_CXEDGE) * 2;
		int nYEdge = ::GetSystemMetrics(SM_CYEDGE) * 2;

		if (cy <= SIZE_EDITBOX_HEIGHT + nYEdge + SIZE_SPACE_Y)
			return;
		int nHeight = cy - SIZE_EDITBOX_HEIGHT - nYEdge - SIZE_SPACE_Y;
		m_wndOutput.MoveWindow(0, 0, cx, nHeight);
		int nWidth = cx - SIZE_BUTTON_WIDTH - nXEdge - SIZE_SPACE_X;
		m_ctlCommand.MoveWindow(0, nHeight+SIZE_SPACE_Y, nWidth, SIZE_EDITBOX_HEIGHT + nYEdge + 200);
		m_ctlSend.MoveWindow(nWidth+SIZE_SPACE_X, nHeight+SIZE_SPACE_Y, SIZE_BUTTON_WIDTH + nXEdge, SIZE_EDITBOX_HEIGHT + nYEdge);
	}
}

void CIRCView::OnSend()
{
	if (!m_pFrame->m_pMain->IsConnectedIRC())
		return;
	CString sCmd, sOrigCmd;
	std::string sRes;
	m_ctlCommand.GetWindowText(sCmd);
	sOrigCmd = sCmd;
	sCmd.TrimLeft();
	sCmd.TrimRight();

	m_pFrame->m_pMain->TranslateIRCCommand(sCmd);

	if (sCmd.IsEmpty()) {
		m_ctlCommand.SetWindowText("");
		return;
	}

	CString sLast;
	m_ctlCommand.InsertString(0, sOrigCmd);
	for (int i=1; i < m_ctlCommand.GetCount();)
	{
		m_ctlCommand.GetLBText(i, sLast);
		if (sLast.Compare(sOrigCmd) == 0) {
			m_ctlCommand.DeleteString(i);
			break;
		}
		i++;
	}

	sCmd += "\n";
	g_ircSession << (LPCTSTR)sCmd;
	m_ctlCommand.SetWindowText("");
}

void CIRCView::AddLine(LPCTSTR pLine)
{
	CString sLine = pLine;
	sLine.TrimLeft();
	sLine.TrimRight();
	if (sLine.IsEmpty())
		return;

	CString sText;
	sText = sLine + "\r\n";
	m_wndOutput.AddText(sText);
}

void CIRCView::DisableIRCControls()
{
	m_ctlSend.EnableWindow(FALSE);
	m_ctlCommand.EnableWindow(FALSE);
	m_ctlCommand.SetWindowText("");
}

void CIRCView::EnableIRCControls()
{
	m_wndOutput.Clear();
	m_ctlSend.EnableWindow();
	m_ctlCommand.EnableWindow();
}

BOOL CIRCView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		// translate using accelerator table
		ASSERT(m_hAccel);
		return ::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg);
	}	
	return CCSBotView::PreTranslateMessage(pMsg);
}

void CIRCView::OnDestroy() 
{
	if (m_hAccel != NULL) {
		::DestroyAcceleratorTable(m_hAccel);
		m_hAccel = NULL;
	}
	CCSBotView::OnDestroy();
}

void CIRCView::OnEnter()
{
	CWnd* pFocus = GetFocus();
	if (pFocus == NULL)
		return;
	if (pFocus->GetSafeHwnd() != m_hEditbox)
		return;

	OnSend();
}
