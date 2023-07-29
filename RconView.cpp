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

// ChildView.cpp : implementation of the CRconView class
//

#include "stdafx.h"
#include "NewCSBot.h"
#include "RconView.h"
#include "ChildFrm.h"

#include "MainFrm.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_RESPONSE	101
#define IDC_RCONCOMMAND	102
#define IDC_SEND		103

/////////////////////////////////////////////////////////////////////////////
// CRconView

CRconView::CRconView(CChildFrame* pFrame) : CCSBotView(pFrame)
{
	m_hAccel = NULL;
}

CRconView::~CRconView()
{
}


BEGIN_MESSAGE_MAP(CRconView,CWnd )
	//{{AFX_MSG_MAP(CRconView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_COMMAND(ID_ENTER, OnEnter)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRconView message handlers

BOOL CRconView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle | WS_TABSTOP, rect, pParentWnd, nID, pContext))
		return FALSE;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_CONTROLPARENT);

	if (!m_wndOutput.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_RESPONSE))
		return FALSE;
	if (!m_ctlCommand.Create(WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_DROPDOWN, CRect(0,0,0,0), this, IDC_RCONCOMMAND))
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

	SetWindowText("Rcon console");
	m_hEditbox = m_ctlCommand.GetWindow(GW_CHILD)->GetSafeHwnd();
	return FALSE;
}

#define SIZE_EDITBOX_HEIGHT	16
#define SIZE_BUTTON_WIDTH	55
#define SIZE_SPACE_X		5
#define SIZE_SPACE_Y		3

void CRconView::OnSize(UINT nType, int cx, int cy) 
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

void CRconView::OnSend()
{
	if (!m_pFrame->m_pMain->IsConnected())
		return;
	CString sCmd, sOrigCmd;;
	std::string sRes;
	m_ctlCommand.GetWindowText(sCmd);
	sOrigCmd = sCmd;
	sCmd.TrimLeft();
	sCmd.TrimRight();

	m_ctlCommand.SetWindowText("");
	if (sCmd.IsEmpty())
		return;

	m_pFrame->m_pMain->IssueRcon(sCmd);

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
}

void CRconView::AddResponseLine(LPCTSTR pLine)
{
	CString sLine = pLine;
	sLine.TrimLeft();
	sLine.TrimRight();
	if (sLine.IsEmpty())
		return;

	CString sText = sLine + "\r\n";
	m_wndOutput.AddText(sText);
}

void CRconView::RconLogLines(LPCTSTR pLines)
{
	AddResponseLine(pLines);
}

void CRconView::DisableControls()
{
	m_ctlSend.EnableWindow(FALSE);
	m_ctlCommand.EnableWindow(FALSE);
	m_ctlCommand.SetWindowText("");
}

void CRconView::EnableControls()
{
	m_wndOutput.Clear();
	m_ctlCommand.EnableWindow();
	m_ctlSend.EnableWindow();
}


BOOL CRconView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		// translate using accelerator table
		ASSERT(m_hAccel);
		return ::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg);
	}	
	return CCSBotView::PreTranslateMessage(pMsg);
}

void CRconView::OnDestroy() 
{
	if (m_hAccel != NULL) {
		::DestroyAcceleratorTable(m_hAccel);
		m_hAccel = NULL;
	}
	CCSBotView::OnDestroy();
}

void CRconView::OnEnter()
{
	CWnd* pFocus = GetFocus();
	if (pFocus == NULL)
		return;
	if (pFocus->GetSafeHwnd() != m_hEditbox)
		return;

	OnSend();
}
