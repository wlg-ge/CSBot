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

// EditTrustedPersonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MainFrm.h"
#include "EditTrustedPersonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTrustedPersonDlg dialog


CEditTrustedPersonDlg::CEditTrustedPersonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditTrustedPersonDlg::IDD, pParent), CommandFlags(0)
{
	//{{AFX_DATA_INIT(CEditTrustedPersonDlg)
	m_sUsername = _T("");
	m_sPass = _T("");
	m_bAllowLogin = TRUE;
	//}}AFX_DATA_INIT
}


void CEditTrustedPersonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTrustedPersonDlg)
	DDX_Control(pDX, IDC_ALLOWEDCOMMANDS, m_ctlAllowCommands);
	DDX_Text(pDX, IDC_NICK, m_sUsername);
	DDV_MaxChars(pDX, m_sUsername, 100);
	DDX_Text(pDX, IDC_PASS, m_sPass);
	DDV_MaxChars(pDX, m_sPass, 100);
	DDX_Check(pDX, IDC_PER_LOGIN, m_bAllowLogin);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		if (m_sUsername.IsEmpty())
		{
			pDX->PrepareEditCtrl(IDC_NICK);
			AfxMessageBox("Nickname cannot be empty");
			pDX->Fail();
		}

		int nCount = m_ctlAllowCommands.GetItemCount();
		for (int nItem = 0; nItem < nCount; nItem++)
		{
			int nCommand = m_ctlAllowCommands.GetItemData(nItem);
			AllowCommand(nCommand, m_ctlAllowCommands.GetCheck(nItem) ? true : false);
		}
	}
}


BEGIN_MESSAGE_MAP(CEditTrustedPersonDlg, CDialog)
	//{{AFX_MSG_MAP(CEditTrustedPersonDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTrustedPersonDlg message handlers

BOOL CEditTrustedPersonDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect;
	m_ctlAllowCommands.SetExtendedStyle(m_ctlAllowCommands.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_ctlAllowCommands.GetWindowRect(&rect);

	int nWidth = rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - ::GetSystemMetrics(SM_CXEDGE)*2;
	m_ctlAllowCommands.InsertColumn(0, "Cmd", LVCFMT_LEFT, nWidth, 0);

	for (int nCommand = 0; g_pCommandNames[nCommand] != NULL; nCommand++)
	{
		if ((nCommand == IRCCOMM_VERSION) || (nCommand == IRCCOMM_LOGOUT))
			continue;
		int nItem = m_ctlAllowCommands.InsertItem(nCommand, g_pCommandNames[nCommand]);
		m_ctlAllowCommands.SetItemData(nItem, nCommand);
		m_ctlAllowCommands.SetCheck(nItem, IsCommandAllowed(nCommand) ? TRUE : FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
