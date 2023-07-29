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

// OptionsIRCIn.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsIRCIn.h"
#include "EditTrustedPersonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCIn property page

IMPLEMENT_DYNCREATE(COptionsIRCIn, COptionsPage)

COptionsIRCIn::COptionsIRCIn() : COptionsPage(COptionsIRCIn::IDD), CommandFlags(0)
{
	//{{AFX_DATA_INIT(COptionsIRCIn)
	m_bAutoLogOff = FALSE;
	m_nLogOffMinutes = 0;
	m_bPMFromIRC = FALSE;
	m_bDCCChatFromIRC = FALSE;
	m_bCMFromIRC = FALSE;
	m_sCMFromIRCPrefix = _T("");
	//}}AFX_DATA_INIT
	m_bUsersChanged = false;
	m_dwAllowedCommands = 0;
}

COptionsIRCIn::~COptionsIRCIn()
{
	for (PERSONLIST::iterator i = m_lTrusted.begin(); i != m_lTrusted.end(); i++)
		delete (*i);
}

void COptionsIRCIn::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsIRCIn)
	DDX_Control(pDX, IDC_ALLOWCOMMANDS, m_ctlAllowCommands);
	DDX_Control(pDX, IDC_TRUSTLIST, m_ctlTrust);
	DDX_Check(pDX, IDC_AUTOLOGOFF, m_bAutoLogOff);
	DDX_Text(pDX, IDC_LOGOFF_MINUTES, m_nLogOffMinutes);
	DDV_MinMaxInt(pDX, m_nLogOffMinutes, 1, 1440);
	DDX_Check(pDX, IDC_IRCPM, m_bPMFromIRC);
	DDX_Check(pDX, IDC_IRCCHAT, m_bDCCChatFromIRC);
	DDX_Check(pDX, IDC_IRCCM, m_bCMFromIRC);
	DDX_Text(pDX, IDC_IRCCM_PREFIX, m_sCMFromIRCPrefix);
	DDV_MaxChars(pDX, m_sCMFromIRCPrefix, 1);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		m_lTrusted.clear();
		int nCount = m_ctlTrust.GetItemCount();
		for (int nItem = 0; nItem < nCount; nItem++)
		{
			TrustedPerson* pPerson = (TrustedPerson*)m_ctlTrust.GetItemData(nItem);
			m_lTrusted.push_back(pPerson);
		}

		nCount = m_ctlAllowCommands.GetItemCount();
		for (nItem = 0; nItem < nCount; nItem++)
		{
			int nCommand = m_ctlAllowCommands.GetItemData(nItem);
			AllowCommand(nCommand, m_ctlAllowCommands.GetCheck(nItem) ? true : false);
		}
	} else {
		EnableControls();
	}
}


BEGIN_MESSAGE_MAP(COptionsIRCIn, COptionsPage)
	//{{AFX_MSG_MAP(COptionsIRCIn)
	ON_BN_CLICKED(IDC_TRUSTEDIT, OnTrustedit)
	ON_BN_CLICKED(IDC_TRUSTADD, OnTrustadd)
	ON_BN_CLICKED(IDC_TRUSTDELETE, OnTrustdelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRUSTLIST, OnItemchangedTrustlist)
	ON_NOTIFY(NM_DBLCLK, IDC_TRUSTLIST, OnDblclkTrustlist)
	ON_BN_CLICKED(IDC_IRCRCONLOGIN, OnIrcrconlogin)
	ON_BN_CLICKED(IDC_AUTOLOGOFF, OnModified)
	ON_EN_CHANGE(IDC_LOGOFF_MINUTES, OnModified)
	ON_EN_CHANGE(IDC_IRCCM_PREFIX, OnModified)
	ON_BN_CLICKED(IDC_IRCCHAT, OnModified)
	ON_BN_CLICKED(IDC_IRCPM, OnModified)
	ON_BN_CLICKED(IDC_IRCCM, OnIRCCM)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALLOWCOMMANDS, OnItemchangedAllowcommands)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCIn message handlers

BOOL COptionsIRCIn::OnInitDialog() 
{
	COptionsPage::OnInitDialog();

	CRect rect;
	m_ctlTrust.GetWindowRect(&rect);
	int nWidth = (rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL)) / 2;

	m_ctlTrust.InsertColumn(0, "Username", LVCFMT_LEFT, nWidth, 0);
	m_ctlTrust.InsertColumn(1, "Permissions", LVCFMT_LEFT, nWidth, 1);

	m_ctlTrust.SetExtendedStyle(m_ctlTrust.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	FillList();

	m_ctlAllowCommands.SetExtendedStyle(m_ctlAllowCommands.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_ctlAllowCommands.GetWindowRect(&rect);

	nWidth = rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL) - ::GetSystemMetrics(SM_CXEDGE)*2;
	m_ctlAllowCommands.InsertColumn(0, "Cmd", LVCFMT_LEFT, nWidth, 0);

	for (int nCommand = 0; g_pCommandNames[nCommand] != NULL; nCommand++)
	{
		if ((nCommand == IRCCOMM_VERSION) || (nCommand == IRCCOMM_LOGOUT))
			continue;
		if ((nCommand == IRCCOMM_OP)	|| (nCommand == IRCCOMM_DEOP) ||
			(nCommand == IRCCOMM_VOICE) || (nCommand == IRCCOMM_DEVOICE) ||
			(nCommand == IRCCOMM_SAY)	|| (nCommand == IRCCOMM_ACT))
			continue;
		int nItem = m_ctlAllowCommands.InsertItem(nCommand, g_pCommandNames[nCommand]);
		m_ctlAllowCommands.SetItemData(nItem, nCommand);
		m_ctlAllowCommands.SetCheck(nItem, IsCommandAllowed(nCommand) ? TRUE : FALSE);
	}
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsIRCIn::FillList()
{
	int nItem = 0;
	for (PERSONLIST::iterator i = m_lTrusted.begin(); i != m_lTrusted.end(); i++, nItem++)
	{
		TrustedPerson* pPerson = (*i);
		
		int nIndex = m_ctlTrust.InsertItem(nItem, pPerson->m_sUsername);
		m_ctlTrust.SetItemText(nIndex, 0, pPerson->m_sUsername);
		m_ctlTrust.SetItemText(nIndex, 1, pPerson->FormatPermissions());

		m_ctlTrust.SetItemData(nIndex, (DWORD)pPerson);
	}
}

void COptionsIRCIn::OnTrustedit() 
{
	int nItem = m_ctlTrust.GetSelectionMark();
	if (nItem < 0)
		return;
	TrustedPerson* pPerson = (TrustedPerson*)m_ctlTrust.GetItemData(nItem);
	if (pPerson == NULL)
		return;

	CEditTrustedPersonDlg dlg;
	dlg.m_sUsername = pPerson->m_sUsername;
	dlg.m_sPass = pPerson->m_sPass;
	dlg.m_bAllowLogin = pPerson->m_bAllowLogin ? TRUE : FALSE;
	dlg.m_dwAllowedCommands = pPerson->m_dwAllowedCommands;

	if (dlg.DoModal() == IDOK)
	{
		m_bUsersChanged = true;
		pPerson->m_sUsername = dlg.m_sUsername;
		pPerson->m_sPass = dlg.m_sPass;
		pPerson->m_bAllowLogin = dlg.m_bAllowLogin ? true : false;
		pPerson->m_dwAllowedCommands = dlg.m_dwAllowedCommands;
		m_ctlTrust.SetItemText(nItem, 0, pPerson->m_sUsername);
		m_ctlTrust.SetItemText(nItem, 1, pPerson->FormatPermissions());
		OnModified();
	}
}

void COptionsIRCIn::OnTrustadd() 
{
	CEditTrustedPersonDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_bUsersChanged = true;
		TrustedPerson* pPerson = new TrustedPerson;
		pPerson->m_sUsername = dlg.m_sUsername;
		pPerson->m_sPass = dlg.m_sPass;
		pPerson->m_bAllowLogin = dlg.m_bAllowLogin ? true : false;
		pPerson->m_dwAllowedCommands = dlg.m_dwAllowedCommands;

		int nItem = m_ctlTrust.InsertItem(m_ctlTrust.GetItemCount(), pPerson->m_sUsername);
		m_ctlTrust.SetItemText(nItem, 0, pPerson->m_sUsername);
		m_ctlTrust.SetItemText(nItem, 1, pPerson->FormatPermissions());
		m_ctlTrust.SetItemData(nItem, (DWORD)pPerson);
		OnModified();
	}
}

void COptionsIRCIn::OnTrustdelete() 
{
	int nItem = m_ctlTrust.GetSelectionMark();
	if (nItem < 0)
		return;

	TrustedPerson* pPerson = (TrustedPerson*)m_ctlTrust.GetItemData(nItem);
	if (pPerson == NULL)
		return;

	m_bUsersChanged = true;
	delete pPerson;
	m_ctlTrust.DeleteItem(nItem);

	while (nItem >= m_ctlTrust.GetItemCount())
		nItem--;
	if (nItem >= 0)
		m_ctlTrust.SetSelectionMark(nItem);
	OnModified();
}

void COptionsIRCIn::OnItemchangedTrustlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nItem = m_ctlTrust.GetSelectionMark();
	BOOL bEn = (nItem >= 0);

	GetDlgItem(IDC_TRUSTEDIT)->EnableWindow(bEn);
	GetDlgItem(IDC_TRUSTDELETE)->EnableWindow(bEn);
	
	*pResult = 0;
}

void COptionsIRCIn::OnDblclkTrustlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	::GetCursorPos(&pt);
	m_ctlTrust.ScreenToClient(&pt);

	if (m_ctlTrust.HitTest(pt) >= 0)
		OnTrustedit();

	*pResult = 0;
}

void COptionsIRCIn::ReadSettings()
{
	for (PERSONLIST::iterator it = settings.m_lTrusted.begin(); it != settings.m_lTrusted.end(); it++)
		m_lTrusted.push_back((*it)->Copy());
	m_bAutoLogOff = settings.m_bAutoLogOff;
	m_nLogOffMinutes = settings.m_nLogOffMinutes;
	m_bPMFromIRC = settings.m_bPMFromIRC;
	m_bCMFromIRC = settings.m_bCMFromIRC;
	m_sCMFromIRCPrefix = settings.m_sCMFromIRCPrefix;
	m_bDCCChatFromIRC = settings.m_bDCCChatFromIRC;
	m_dwAllowedCommands = settings.m_dwAllowedCommands;
}

void COptionsIRCIn::WriteSettings()
{
	settings.m_bAutoLogOff = m_bAutoLogOff;
	settings.m_nLogOffMinutes = m_nLogOffMinutes;
	settings.m_bPMFromIRC = m_bPMFromIRC;
	settings.m_bCMFromIRC = m_bCMFromIRC;
	settings.m_sCMFromIRCPrefix = m_sCMFromIRCPrefix;
	settings.m_bDCCChatFromIRC = m_bDCCChatFromIRC;
	settings.m_dwAllowedCommands = m_dwAllowedCommands;
	// clear old ones
	CString sMsg;
	if (m_bUsersChanged) 
	{
		for (PERSONLIST::iterator it = settings.m_lTrusted.begin(); it != settings.m_lTrusted.end(); it++) 
		{
			TrustedPerson* pPerson = (*it);
			if (pPerson->m_bLoggedIn) {
				// let the old logged in person know he was kicked
				sMsg.Format("PRIVMSG %s :Users were changed in CSBot. You are now logged out.\n", (LPCTSTR)pPerson->m_sCurrentNick);
				g_ircSession << (LPCTSTR)sMsg;
			}
			delete (*it);
		}

		settings.m_lTrusted.clear();
		// copy new ones
		for (it = m_lTrusted.begin(); it != m_lTrusted.end(); it++)
			settings.m_lTrusted.push_back((*it)->Copy());
		m_bUsersChanged = false;
	}
}

void COptionsIRCIn::OnIrcrconlogin() 
{
	m_bUsersChanged = true;
}

void COptionsIRCIn::OnIRCCM()
{
	EnableControls();
	OnModified();
}

void COptionsIRCIn::OnModified() 
{
	SetModified(TRUE);	
}

void COptionsIRCIn::OnItemchangedAllowcommands(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if ((pNMListView->uOldState & LVIS_STATEIMAGEMASK) != (pNMListView->uNewState & LVIS_STATEIMAGEMASK))
		OnModified();
	
	*pResult = 0;
}

void COptionsIRCIn::EnableControls()
{
	bool bCMChecked = ((CButton*)GetDlgItem(IDC_IRCCM))->GetCheck() ? true : false;
	GetDlgItem(IDC_IRCCM_PREFIX)->EnableWindow(bCMChecked ? TRUE : FALSE);
}
