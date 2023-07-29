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

// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "NewCSBot.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "RawLogView.h"
#include "PlayerListView.h"
#include "RconView.h"
#include "IRCView.h"
#include "SchedView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UPDATETITLE, OnUpdateTitle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_pMain = NULL;	
	m_pView = NULL;
}

CChildFrame::~CChildFrame()
{
	if (m_pView != NULL)
		delete (m_pView);
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.style &= ~WS_MAXIMIZEBOX;
	cs.style &= ~WS_MINIMIZEBOX;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
void CChildFrame::OnFileClose() 
{
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu.

	SendMessage(WM_CLOSE);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_CONTROLPARENT);

	switch(m_nIDHelp) {
	case IDR_RAWLOG:
		{
			m_nID = I_RAWLOG;
			CRawLogView* pView = new CRawLogView(this);
			pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_pView = pView;
		}
		break;
	case IDR_PLAYERLIST:
		{
			m_nID = I_PLAYERS;
			CPlayerListView* pView = new CPlayerListView(this);
			pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_pView = pView;
		}
		break;
	case IDR_RCON:
		{
			m_nID = I_RCON;
			CRconView* pView = new CRconView(this);
			pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_pView = pView;
		}
		break;
	case IDR_IRC:
		{
			m_nID = I_IRC;
			CIRCView* pView = new CIRCView(this);
			pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_pView = pView;
		}
		break;
	case IDR_SCHED:
		{
			m_nID = I_SCHED;
			CSchedView* pView = new CSchedView(this);
			pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
			m_pView = pView;
		}
		break;
	default:
		ASSERT(FALSE);
	}

	ReadSettings();
	m_pView->ReadSettings();

	return 0;
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	if (m_pView != NULL)
		m_pView->SetFocus();
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
	if (m_pView != NULL)
		if (m_pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	
	// otherwise, do default handling
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CChildFrame::OnDestroy() 
{
	m_pMain->m_pFrames[m_nID] = NULL;

	CMDIChildWnd::OnDestroy();
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	if (m_pView != NULL) {
		if (m_pView->GetSafeHwnd() != NULL) {
			m_pView->MoveWindow(0, 0, cx, cy);
		}
	}
}

LRESULT CChildFrame::OnUpdateTitle(WPARAM wParam, LPARAM lParam)
{
	if (m_pView != NULL) {
		if (m_pView->GetSafeHwnd() != NULL) {
			CString sTitle;
			m_pView->GetWindowText(sTitle);
			SetWindowText(sTitle);
			SetIcon(AfxGetApp()->LoadIcon(m_nIDHelp), TRUE);
			SetIcon(AfxGetApp()->LoadIcon(m_nIDHelp), FALSE);
		}
	}
	return 0;
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	PostMessage(UM_UPDATETITLE);
}

void CChildFrame::OnClose() 
{
	m_pMain->ShowFrame(m_nID);
}

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 200 + (::GetSystemMetrics(SM_CXEDGE) * 2);
	lpMMI->ptMinTrackSize.y = 60 + (::GetSystemMetrics(SM_CYEDGE) * 2) + ::GetSystemMetrics(SM_CYCAPTION);
}

void CChildFrame::WriteSettings()
{
	CRect rect;
	GetWindowRect(&rect);

	CPoint ptTopLeft = rect.TopLeft();
	CPoint ptBotomRight = rect.BottomRight();

	::ScreenToClient(m_pMain->m_hWndMDIClient, &ptTopLeft);
	::ScreenToClient(m_pMain->m_hWndMDIClient, &ptBotomRight);

	rect.SetRect(ptTopLeft, ptBotomRight);

	CString sSection;
	sSection.Format("Views\\View%d", m_nID);

	AfxGetApp()->WriteProfileInt(sSection, "left", rect.left);
	AfxGetApp()->WriteProfileInt(sSection, "right", rect.right);
	AfxGetApp()->WriteProfileInt(sSection, "top", rect.top);
	AfxGetApp()->WriteProfileInt(sSection, "bottom", rect.bottom);
	int nShow = (GetStyle() & WS_VISIBLE) ? TRUE : FALSE;
	AfxGetApp()->WriteProfileInt(sSection, "show", nShow);

	if (m_pView != NULL)
		m_pView->WriteSettings();
}

void CChildFrame::ReadSettings()
{
	CString sSection;
	sSection.Format("Views\\View%d", m_nID);
	int nInt;
	nInt = AfxGetApp()->GetProfileInt(sSection, "show", -1);
	if (nInt != -1) 
		ShowWindow(nInt ? SW_SHOW : SW_HIDE);

	CRect rect;

	nInt = AfxGetApp()->GetProfileInt(sSection, "left", -1);
	if (nInt != -1) {
		rect.left = nInt;
		nInt = AfxGetApp()->GetProfileInt(sSection, "right", -1);
		if (nInt != -1) {
			rect.right = nInt;
			nInt = AfxGetApp()->GetProfileInt(sSection, "top", -1);
			if (nInt != -1) {
				rect.top = nInt;
				nInt = AfxGetApp()->GetProfileInt(sSection, "bottom", -1);
				if (nInt != -1) {
					rect.bottom = nInt;
					MoveWindow(&rect);
				}
			}
		}
	}
}