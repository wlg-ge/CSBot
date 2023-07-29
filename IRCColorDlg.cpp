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

// IRCColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "IRCColorDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIRCColorDlg dialog


CIRCColorDlg::CIRCColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIRCColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIRCColorDlg)
	//}}AFX_DATA_INIT
}


void CIRCColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIRCColorDlg)
	DDX_Control(pDX, IDC_FOREGROUND, m_ctlFore);
	DDX_Control(pDX, IDC_BACKGROUND, m_ctlBack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIRCColorDlg, CDialog)
	//{{AFX_MSG_MAP(CIRCColorDlg)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_LBN_SELCHANGE(IDC_BACKGROUND, OnSelchangeBackground)
	ON_LBN_SELCHANGE(IDC_FOREGROUND, OnSelchangeForeground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRCColorDlg message handlers

void CIRCColorDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CString sText;
	switch (nIDCtl)
	{
	case IDC_FOREGROUND:
		m_ctlFore.GetText(lpDrawItemStruct->itemID, sText);
		DrawListItemFore(lpDrawItemStruct->hDC, lpDrawItemStruct->itemData, sText, lpDrawItemStruct->rcItem, GetDlgItem(nIDCtl), lpDrawItemStruct->itemState);
		break;
	case IDC_BACKGROUND:
		m_ctlBack.GetText(lpDrawItemStruct->itemID, sText);
		DrawListItemBack(lpDrawItemStruct->hDC, lpDrawItemStruct->itemData, sText, lpDrawItemStruct->rcItem, GetDlgItem(nIDCtl), lpDrawItemStruct->itemState);
		break;
	case IDC_SAMPLE:
		DrawSample(lpDrawItemStruct->hDC, m_nBack, m_nFore, GetDlgItem(nIDCtl));
		break;
	default:
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void CIRCColorDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	switch (nIDCtl) {
	case IDC_FOREGROUND:
	case IDC_BACKGROUND:
		{
			CRect rect;
			::GetWindowRect(::GetDlgItem(GetSafeHwnd(), nIDCtl), &rect);
			lpMeasureItemStruct->itemWidth = rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
			lpMeasureItemStruct->itemHeight = 14;
		}
		break;
	default:
		CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
	}
}

void CIRCColorDlg::DrawSample(HDC hDC, int nBack, int nFore, CWnd* pWnd)
{
	CFont font;
	CRect rect;
	CDC* pDC = CDC::FromHandle(hDC);
	CBrush brush;
	font.CreateStockObject(/*DEFAULT_GUI_FONT*/OEM_FIXED_FONT);

	if (nBack == 0) {
		pDC->SetBkMode(TRANSPARENT);
		brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	} else {
		pDC->SetBkColor(CMainFrame::m_colorsIRC[nBack]);
		pDC->SetBkMode(OPAQUE);
		brush.CreateSolidBrush(CMainFrame::m_colorsIRC[nBack]);
	}

	pWnd->GetWindowRect(&rect);
	pWnd->ScreenToClient(&rect);
	pDC->FillRect(&rect, &brush);
	pDC->SelectObject(&font);

	pDC->SetTextColor(CMainFrame::m_colorsIRC[nFore]);
	CString sText;
	pWnd->GetWindowText(sText);
	pDC->DrawText(sText, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIRCColorDlg::DrawListItemFore(HDC hDC, int nColor, LPCTSTR pText, CRect rect, CWnd* pWnd, int nState)
{
	CFont font;
	CDC* pDC = CDC::FromHandle(hDC);
	CBrush brush;
	font.CreateStockObject(/*DEFAULT_GUI_FONT*/OEM_FIXED_FONT);
	brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	pDC->FillRect(&rect, &brush);
	pDC->SelectObject(&font);

	pDC->SetBkMode(OPAQUE);
	pDC->SetTextColor(CMainFrame::m_colorsIRC[nColor]);
	DWORD dwCol = CMainFrame::m_colorsIRC[nColor];
	DWORD dwSum = (dwCol & 0xff) + ((dwCol >> 8) & 0xff) + ((dwCol >> 16) & 0xff);
	if (dwSum > 700) {
		pDC->SetBkColor(RGB(0,0,0));
	} else {
		pDC->SetBkColor(RGB(255,255,255));
	}
	pDC->DrawText(pText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	if (nState & ODS_SELECTED) {
		pDC->SetBkMode(TRANSPARENT);
		CBrush brush;
		brush.CreateStockObject(HOLLOW_BRUSH);
		rect.DeflateRect(1, 1);
		CPen pen(PS_DOT, 1, RGB(128,128,128));
		pDC->SelectObject(&brush);
		pDC->SelectObject(&pen);
		pDC->Rectangle(rect);
	}
}

void CIRCColorDlg::DrawListItemBack(HDC hDC, int nColor, LPCTSTR pText, CRect rect, CWnd* pWnd, int nState)
{
	CFont font;
	CDC* pDC = CDC::FromHandle(hDC);
	CBrush brush;
	font.CreateStockObject(/*DEFAULT_GUI_FONT*/OEM_FIXED_FONT);
	brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

	pDC->FillRect(&rect, &brush);
	pDC->SelectObject(&font);

	pDC->SetBkMode(OPAQUE);
	pDC->SetBkColor(CMainFrame::m_colorsIRC[nColor]);
	DWORD dwCol = CMainFrame::m_colorsIRC[nColor];
	DWORD dwSum = (dwCol & 0xff) + (2*((dwCol >> 8) & 0xff)) + ((dwCol >> 16) & 0xff);
	if (dwSum > 450) {
		pDC->SetTextColor(RGB(0,0,0));
	} else {
		pDC->SetTextColor(RGB(255,255,255));
	}
	pDC->DrawText(pText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	if (nState & ODS_SELECTED) {
		pDC->SetBkMode(TRANSPARENT);
		CBrush brush;
		brush.CreateStockObject(HOLLOW_BRUSH);
		rect.DeflateRect(1, 1);
		CPen pen(PS_DOT, 1, RGB(128,128,128));
		pDC->SelectObject(&brush);
		pDC->SelectObject(&pen);
		pDC->Rectangle(rect);
	}
}

const char* pColorNamesBack[16] = {
	"Transparent",
	"Black",
	"Blue",
	"Green",
	"Red",
	"Brown",
	"Purple",
	"Light brown",
	"Yellow",
	"Light green",
	"Cyan",
	"Light cyan",
	"Light blue",
	"Light purple",
	"Gray",
	"Light gray"
};

const char* pColorNamesFore[16] = {
	"White",
	"Black",
	"Blue",
	"Green",
	"Red",
	"Brown",
	"Purple",
	"Light brown",
	"Yellow",
	"Light green",
	"Cyan",
	"Light cyan",
	"Light blue",
	"Light purple",
	"Gray",
	"Light gray"
};


BOOL CIRCColorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int nSelItem = 0, nItem, i;
	for (i=0; i<16; i++) {
		m_ctlFore.SetItemData(nItem = m_ctlFore.InsertString(i, pColorNamesFore[i]), i);
		if (i == m_nFore)
			nSelItem = i;
	}
	m_ctlFore.SetCurSel(nSelItem);


	nSelItem = 0;
	for (i=0; i<16; i++) {
		m_ctlBack.SetItemData(m_ctlBack.InsertString(i, pColorNamesBack[i]), i);
		if (i == m_nBack)
			nSelItem = i;
	}
	m_ctlBack.SetCurSel(nSelItem);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIRCColorDlg::OnSelchangeBackground() 
{
	m_nBack = m_ctlBack.GetItemData(m_ctlBack.GetCurSel());
	GetDlgItem(IDC_SAMPLE)->Invalidate();
}

void CIRCColorDlg::OnSelchangeForeground() 
{
	m_nFore = m_ctlFore.GetItemData(m_ctlFore.GetCurSel());
	GetDlgItem(IDC_SAMPLE)->Invalidate();
}
