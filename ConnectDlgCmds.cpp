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

// ConnectDlgCmds.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "ConnectDlgCmds.h"
#include "ProfileName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgCmds property page

IMPLEMENT_DYNCREATE(CConnectDlgCmds, CPropertyPage)

CConnectDlgCmds::CConnectDlgCmds(bool bConnect /* = true */, bool bIRC /* = false */) : CPropertyPage(CConnectDlgCmds::IDD)
{
	//{{AFX_DATA_INIT(CConnectDlgCmds)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bConnect = bConnect;
	m_bIRC = bIRC;
	m_psp.dwFlags |= PSP_USETITLE;
	if (m_bConnect)
	{
		m_psp.pszTitle = "Connect";
	} else {
		m_psp.pszTitle = "Disconnect";
	}
}

CConnectDlgCmds::~CConnectDlgCmds()
{
}

void CConnectDlgCmds::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlgCmds)
	DDX_Control(pDX, IDC_CMDS, m_ctlCmds);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		m_lCmds.RemoveAll();
		int i;
		for (i = 0; i < m_ctlCmds.GetItemCount(); i++)
			m_lCmds.AddTail(m_ctlCmds.GetItemText(i, 0));
	} else {
		m_ctlCmds.DeleteAllItems();
		POSITION pos = m_lCmds.GetHeadPosition();
		while (pos != NULL)
		{
			CString sStr = m_lCmds.GetNext(pos);
			m_ctlCmds.InsertItem(m_ctlCmds.GetItemCount(), sStr);
		}
	}
}


BEGIN_MESSAGE_MAP(CConnectDlgCmds, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectDlgCmds)
	ON_BN_CLICKED(IDC_ADDCMD, OnAddcmd)
	ON_BN_CLICKED(IDC_DELETECMD, OnDeletecmd)
	ON_BN_CLICKED(IDC_EDITCMD, OnEditcmd)
	ON_NOTIFY(NM_DBLCLK, IDC_CMDS, OnDblclkCmds)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CMDS, OnItemchangedCmds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgCmds message handlers


BOOL CConnectDlgCmds::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (m_bIRC)
	{
		if (m_bConnect)
		{
			GetDlgItem(IDC_LABEL)->SetWindowText("Execute these IRC commands when connecting");
		} else {
			GetDlgItem(IDC_LABEL)->SetWindowText("Execute these IRC commands when disconnecting");
		}
	} else {
		if (m_bConnect)
		{
			GetDlgItem(IDC_LABEL)->SetWindowText("Execute these RCON commands when connecting");
		} else {
			GetDlgItem(IDC_LABEL)->SetWindowText("Execute these RCON commands when disconnecting");
		}
	}
	
	CRect rect;
	m_ctlCmds.GetClientRect(&rect);
	int nWidth = rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	m_ctlCmds.InsertColumn(0, "", LVCFMT_LEFT, nWidth);
	m_ctlCmds.SetExtendedStyle(m_ctlCmds.GetExtendedStyle() | LVS_EX_FULLROWSELECT);



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlgCmds::OnAddcmd() 
{
	CProfileName dlg;
	if (m_bIRC)
		dlg.m_sTitle = "IRC command";
	else
		dlg.m_sTitle = "Game server command";
	dlg.m_sName = "";
	if (dlg.DoModal() != IDOK)
		return;
	m_ctlCmds.InsertItem(m_ctlCmds.GetItemCount(), dlg.m_sName);
}

void CConnectDlgCmds::OnDeletecmd() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;
	m_ctlCmds.DeleteItem(nItem);
}

void CConnectDlgCmds::OnEditcmd() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;

	CProfileName dlg;
	if (m_bIRC)
		dlg.m_sTitle = "IRC command";
	else
		dlg.m_sTitle = "Game server command";
	dlg.m_sName = m_ctlCmds.GetItemText(nItem, 0);
	if (dlg.DoModal() != IDOK)
		return;
	m_ctlCmds.SetItemText(nItem, 0, dlg.m_sName);
}

void CConnectDlgCmds::OnDblclkCmds(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEditcmd();
	*pResult = 0;
}

void CConnectDlgCmds::OnItemchangedCmds(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	BOOL bEnable = (m_ctlCmds.GetSelectedCount() == 1);
	GetDlgItem(IDC_EDITCMD)->EnableWindow(bEnable);
	GetDlgItem(IDC_DELETECMD)->EnableWindow(bEnable);

	*pResult = 0;
}
