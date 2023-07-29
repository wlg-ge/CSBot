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

// OptionsIRCMsgsColors.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsIRCMsgsColors.h"
#include "OptionsPage.h"
#include "IRCColorDlg.h"
#include "GIRLLogParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsColors property page

IMPLEMENT_DYNCREATE(COptionsIRCMsgsColors, CPropertyPage)

COptionsIRCMsgsColors::COptionsIRCMsgsColors() : CPropertyPage(COptionsIRCMsgsColors::IDD)
{
	//{{AFX_DATA_INIT(COptionsIRCMsgsColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pColors = NULL;
	m_pParser = NULL;
	m_pPage = NULL;
}

COptionsIRCMsgsColors::~COptionsIRCMsgsColors()
{
}

void COptionsIRCMsgsColors::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsIRCMsgsColors)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	ASSERT(m_pColors != NULL);
	if (pDX->m_bSaveAndValidate)
	{
		for (int i=0; i < TEAM_COUNT; i++)
			m_pColors->sColor[i] = (LPCTSTR)GetColor(i);
	} else {
		m_nY = 0;
		if (m_pColors != NULL)
			for (int i=0; i < TEAM_COUNT; i++)
				SetColor(i, m_pColors->sTeam[i].c_str(), m_pColors->sColor[i].c_str());
	}
}


BEGIN_MESSAGE_MAP(COptionsIRCMsgsColors, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsIRCMsgsColors)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SAMPLE1, OnSample1)
	ON_BN_CLICKED(IDC_SAMPLE2, OnSample2)
	ON_BN_CLICKED(IDC_SAMPLE3, OnSample3)
	ON_BN_CLICKED(IDC_SAMPLE0, OnSample0)
	ON_BN_CLICKED(IDC_SAMPLE4, OnSample4)
	ON_BN_CLICKED(IDC_SAMPLE5, OnSample5)
	ON_BN_CLICKED(IDC_REVERTDEFAULT, OnRevertdefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsColors message handlers

void COptionsIRCMsgsColors::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if ((nIDCtl >= IDC_SAMPLE0) && (nIDCtl <= IDC_SAMPLE5))
	{
		int nColor = nIDCtl - IDC_SAMPLE0;
		CIRCColorDlg::DrawSample(lpDrawItemStruct->hDC, m_nBack[nColor], m_nFore[nColor], GetDlgItem(nIDCtl));
	} else
		CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void COptionsIRCMsgsColors::EditSample(int nColor)
{
	CIRCColorDlg dlg;
	dlg.m_nFore = m_nFore[nColor];
	dlg.m_nBack = m_nBack[nColor];
	if (dlg.DoModal() == IDOK)
	{
		m_nFore[nColor] = dlg.m_nFore;
		m_nBack[nColor] = dlg.m_nBack;
		GetDlgItem(IDC_SAMPLE0 + nColor)->Invalidate();
		GetDlgItem(IDC_COLOR0 + nColor)->SetWindowText(GetColor(nColor));
		m_pPage->OnModified();
	}
}

void COptionsIRCMsgsColors::OnSample0() 
{
	EditSample(0);
}

void COptionsIRCMsgsColors::OnSample1() 
{
	EditSample(1);
}

void COptionsIRCMsgsColors::OnSample2() 
{
	EditSample(2);
}

void COptionsIRCMsgsColors::OnSample3() 
{
	EditSample(3);
}

void COptionsIRCMsgsColors::OnSample4() 
{
	EditSample(4);
}

void COptionsIRCMsgsColors::OnSample5() 
{
	EditSample(5);
}

BOOL COptionsIRCMsgsColors::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsIRCMsgsColors::SetColor(const CString& sColor, int& nFore, int& nBack)
{
	nFore = atoi(sColor);
	int nPos = sColor.Find(',');
	if (nPos < 0) {
		nBack = 0;
	} else {
		nBack = atoi(sColor.Mid(nPos+1));
	}
	if (nFore < 0) nFore = 0;
	if (nFore > 15) nFore = 15;
	if (nBack < 0) nBack = 0;
	if (nBack > 15) nBack = 15;
}

void COptionsIRCMsgsColors::SetColor(int nColor, CString sName, LPCTSTR sColor)
{
	SetColor(sColor, m_nFore[nColor], m_nBack[nColor]);
	if (strlen(sName) == 0)
	{
		GetDlgItem(IDC_TEAM0+nColor)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SAMPLE0+nColor)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COLOR0+nColor)->ShowWindow(SW_HIDE);
	} else {
		m_nY += 30;

		CWnd* pWnd;
		CRect rect;
		int nHeight;

		pWnd = GetDlgItem(IDC_TEAM0+nColor);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		nHeight = rect.Height();
		rect.top = m_nY;
		rect.bottom = m_nY+nHeight;
		pWnd->MoveWindow(rect);

		pWnd = GetDlgItem(IDC_SAMPLE0+nColor);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		nHeight = rect.Height();
		rect.top = m_nY;
		rect.bottom = m_nY+nHeight;
		pWnd->MoveWindow(rect);

		pWnd = GetDlgItem(IDC_COLOR0+nColor);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		nHeight = rect.Height();
		rect.top = m_nY;
		rect.bottom = m_nY+nHeight;
		pWnd->MoveWindow(rect);

		sName += ":";
		SetDlgItemText(IDC_TEAM0 + nColor, sName);
		SetDlgItemText(IDC_COLOR0 + nColor, GetColor(nColor));
	}
}

CString COptionsIRCMsgsColors::GetColor(int& nFore, int& nBack)
{
	CString sRet;
	if (nBack == 0)
		sRet.Format("%02d", nFore);
	else
		sRet.Format("%02d,%02d", nFore, nBack);
	return sRet;
}

CString COptionsIRCMsgsColors::GetColor(int nColor)
{
	return GetColor(m_nFore[nColor], m_nBack[nColor]);
}

void COptionsIRCMsgsColors::SetColors(ColorInfo* pColors, GIRLLogParser* pParser)
{
	m_pColors = pColors;
	m_pParser = pParser;
}

void COptionsIRCMsgsColors::OnRevertdefault() 
{
	m_nY = 0;
	for (int nTeam = 0; nTeam < TEAM_COUNT; nTeam++)
	{
		SetColor(nTeam, m_pParser->TeamDescription((Team)nTeam), m_pParser->GetTeamColorDefault((Team)nTeam));
		GetDlgItem(IDC_SAMPLE0 + nTeam)->Invalidate();
	}
	m_pPage->OnModified();
}
