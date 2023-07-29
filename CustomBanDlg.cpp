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

// CustomBanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CustomBanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomBanDlg dialog


CCustomBanDlg::CCustomBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomBanDlg)
	m_nAmount = 0;
	//}}AFX_DATA_INIT
}


void CCustomBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomBanDlg)
	DDX_Control(pDX, IDC_TYPE, m_ctlType);
	DDX_Text(pDX, IDC_AMOUNT, m_nAmount);
	DDV_MinMaxInt(pDX, m_nAmount, 1, 100);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		int nType = m_ctlType.GetCurSel();
		if (nType < 0) {
			AfxMessageBox("Please choose type of amount");
			pDX->PrepareCtrl(IDC_TYPE);
			pDX->Fail();
			return;
		}
		int nTypeAmount = m_ctlType.GetItemData(nType);
		m_nMinutes = nTypeAmount * m_nAmount;
	}
}


BEGIN_MESSAGE_MAP(CCustomBanDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomBanDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomBanDlg message handlers

BOOL CCustomBanDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int nItem;
	m_ctlType.SetItemData(nItem = m_ctlType.AddString("Minute(s)"), 1);
	m_ctlType.SetItemData(m_ctlType.AddString("Hour(s)"), 60);
	m_ctlType.SetItemData(m_ctlType.AddString("Day(s)"), 1440);
	m_ctlType.SetItemData(m_ctlType.AddString("Week(s)"), 10080);
	m_ctlType.SetItemData(m_ctlType.AddString("Month(s)"), 43200);

	m_ctlType.SetCurSel(nItem);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
