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

// OptionsPlayerList.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsPlayerList.h"
#include "EditPlayerListCommandDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPlayerList property page

IMPLEMENT_DYNCREATE(COptionsPlayerList, COptionsPage)

COptionsPlayerList::COptionsPlayerList() : COptionsPage(COptionsPlayerList::IDD)
{
	//{{AFX_DATA_INIT(COptionsPlayerList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COptionsPlayerList::~COptionsPlayerList()
{
	Clear();
}

void COptionsPlayerList::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPlayerList)
	DDX_Control(pDX, IDC_COMMANDS, m_ctlCmds);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		Clear();
		CString sName, sCmd;
		for (int nItem = 0; nItem < m_ctlCmds.GetItemCount(); nItem++)
		{
			sName = m_ctlCmds.GetItemText(nItem, 0);
			sCmd = m_ctlCmds.GetItemText(nItem, 1);
			m_lCmds.push_back(new PlayerListCommand(sName, sCmd));
		}
	} else {
		m_ctlCmds.DeleteAllItems();
		int nItem;
		for (PLAYERLISTCMDLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++)
		{
			nItem = m_ctlCmds.InsertItem(m_ctlCmds.GetItemCount(), "");
			m_ctlCmds.SetItemText(nItem, 0, (*i)->m_sName); 
			m_ctlCmds.SetItemText(nItem, 1, (*i)->m_sCommand); 
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsPlayerList, COptionsPage)
	//{{AFX_MSG_MAP(COptionsPlayerList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_NOTIFY(NM_DBLCLK, IDC_COMMANDS, OnDblclkCommands)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_COMMANDS, OnItemchangedCommands)
	ON_NOTIFY(LVN_KEYDOWN, IDC_COMMANDS, OnKeydownCommands)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPlayerList message handlers

BOOL COptionsPlayerList::OnInitDialog() 
{
	COptionsPage::OnInitDialog();

	CRect rect;
	m_ctlCmds.GetClientRect(&rect);
	int nWidth = (rect.Width() - ::GetSystemMetrics(SM_CXEDGE)*2 - ::GetSystemMetrics(SM_CXVSCROLL))/2;

	m_ctlCmds.InsertColumn(0, "Name", LVCFMT_LEFT, nWidth, 0);
	m_ctlCmds.InsertColumn(1, "Command", LVCFMT_LEFT, nWidth, 1);

	m_ctlCmds.SetExtendedStyle(m_ctlCmds.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPlayerList::OnAdd() 
{
	CEditPlayerListCommandDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		int nItem = m_ctlCmds.InsertItem(m_ctlCmds.GetItemCount(), "");
		m_ctlCmds.SetItemText(nItem, 0, dlg.m_sName);
		m_ctlCmds.SetItemText(nItem, 1, dlg.m_sCommand);
		m_ctlCmds.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED); 
		OnModified();
	}
	EnableDisable();
}

void COptionsPlayerList::OnEdit() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;

	CEditPlayerListCommandDlg dlg;
	dlg.m_sName = m_ctlCmds.GetItemText(nItem, 0);
	dlg.m_sCommand = m_ctlCmds.GetItemText(nItem, 1);
	if (dlg.DoModal() == IDOK)
	{
		m_ctlCmds.SetItemText(nItem, 0, dlg.m_sName);
		m_ctlCmds.SetItemText(nItem, 1, dlg.m_sCommand);
		m_ctlCmds.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED); 
		OnModified();
	}
	EnableDisable();
}

void COptionsPlayerList::OnDelete() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;

	if (AfxMessageBox("Are you sure you want to delete this player-list command?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		m_ctlCmds.DeleteItem(nItem);
		if (nItem < m_ctlCmds.GetItemCount())
			m_ctlCmds.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED); 
		else if (m_ctlCmds.GetItemCount() > 0)
			m_ctlCmds.SetItemState(m_ctlCmds.GetItemCount()-1, LVIS_SELECTED, LVIS_SELECTED); 
		OnModified();
	}
}

void COptionsPlayerList::OnMoveup() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem <= 0)
		return;

	CString sName = m_ctlCmds.GetItemText(nItem, 0);
	CString sCommand = m_ctlCmds.GetItemText(nItem, 1);

	m_ctlCmds.DeleteItem(nItem);
	nItem = m_ctlCmds.InsertItem(nItem-1, "");

	m_ctlCmds.SetItemText(nItem, 0, sName);
	m_ctlCmds.SetItemText(nItem, 1, sCommand);

	m_ctlCmds.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED); 
	OnModified();
}

void COptionsPlayerList::OnMovedown() 
{
	POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nItem = m_ctlCmds.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;
	if (nItem >= m_ctlCmds.GetItemCount()-1)
		return;
	
	CString sName = m_ctlCmds.GetItemText(nItem, 0);
	CString sCommand = m_ctlCmds.GetItemText(nItem, 1);

	m_ctlCmds.DeleteItem(nItem);
	nItem = m_ctlCmds.InsertItem(nItem+1, "");

	m_ctlCmds.SetItemText(nItem, 0, sName);
	m_ctlCmds.SetItemText(nItem, 1, sCommand);
	m_ctlCmds.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED); 
	OnModified();
}

void COptionsPlayerList::ReadSettings()
{
	// clear us
	Clear();
	// and copy from settings
	for (PLAYERLISTCMDLIST::iterator i = settings.m_lPlayerListCmds.begin(); i != settings.m_lPlayerListCmds.end(); i++)
		m_lCmds.push_back((*i)->Copy());
}


void COptionsPlayerList::WriteSettings()
{
	// clear in settings
	for (PLAYERLISTCMDLIST::iterator i = settings.m_lPlayerListCmds.begin(); i != settings.m_lPlayerListCmds.end(); i++)
		delete (*i);
	settings.m_lPlayerListCmds.clear();

	// and copy
	for (i = m_lCmds.begin(); i != m_lCmds.end(); i++)
		settings.m_lPlayerListCmds.push_back((*i)->Copy());
}

void COptionsPlayerList::Clear()
{
	for (PLAYERLISTCMDLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++)
		delete (*i);
	m_lCmds.clear();
}

void COptionsPlayerList::OnDblclkCommands(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();	
	*pResult = 0;
}

void COptionsPlayerList::OnItemchangedCommands(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	EnableDisable();

	*pResult = 0;
}

void COptionsPlayerList::OnKeydownCommands(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	*pResult = 0;
	
	switch (pLVKeyDow->wVKey)
	{
	case VK_F2:
		OnEdit();
		break;
	case VK_DELETE:
		OnDelete();
		break;
	case VK_INSERT:
		OnAdd();
		break;
	case VK_UP:
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			OnMoveup();
		break;
	case VK_DOWN:
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			OnMovedown();
		break;
	}
}

void COptionsPlayerList::EnableDisable()
{
	int nSel = m_ctlCmds.GetSelectedCount();
	if (nSel > 0)
	{
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
		POSITION pos = m_ctlCmds.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int nItem = m_ctlCmds.GetNextSelectedItem(pos);
		if (nItem < 0)
			return;

		GetDlgItem(IDC_MOVEUP)->EnableWindow((nItem > 0) ? TRUE : FALSE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow((nItem < m_ctlCmds.GetItemCount()-1) ? TRUE : FALSE);
	} else {
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);
	}
	if (m_ctlCmds.GetItemCount() < MAX_PLAYERLIST_COMMANDS)
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
}

