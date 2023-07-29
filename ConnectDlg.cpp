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

// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ConnectDlg.h"
#include "ProfileName.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog

CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDlg::IDD, pParent), 
	m_pageDisconnect(false), 
	m_pageReconnect(false)
{
	//{{AFX_DATA_INIT(CConnectDlg)
	//}}AFX_DATA_INIT
}


void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_TABS, m_ctlTabs);
	DDX_Control(pDX, IDC_PROFILES, m_ctlList);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
	{
		m_pageGeneral.m_dwLocalIP = m_dwLocalIP;
		m_pageGeneral.m_sRcon = m_sRcon;
		m_pageGeneral.m_sServerName = m_sServerName;
		m_pageGeneral.m_uLogPort = m_uLogPort;
		m_pageGeneral.m_uPort = m_uPort;
		m_pageGeneral.m_eType = m_eType;

		m_pageReconnect.m_bReconnect = m_bReconnectToGame;
		m_pageReconnect.m_nReconnectSec = m_nGameReconnectSec;
		m_pageReconnect.m_nReconnectTimes = m_nGameReconnectTimes;
		m_pageReconnect.m_bInactivity = m_bInactivity;
		m_pageReconnect.m_nInactivityTimeout = m_nInactivityTimeout;

		m_pageConnect.m_lCmds.RemoveAll();
		m_pageConnect.m_lCmds.AddTail(&m_lConnectCommands);
		m_pageDisconnect.m_lCmds.RemoveAll();
		m_pageDisconnect.m_lCmds.AddTail(&m_lDisconnectCommands);
	}

	if (m_pageGeneral.GetSafeHwnd() != NULL)
		if (!m_pageGeneral.UpdateData(pDX->m_bSaveAndValidate))
		{
			pDX->Fail();
			return;
		}
	if (m_pageConnect.GetSafeHwnd() != NULL)
		if (!m_pageConnect.UpdateData(pDX->m_bSaveAndValidate))
		{
			pDX->Fail();
			return;
		}
	if (m_pageDisconnect.GetSafeHwnd() != NULL)
		if (!m_pageDisconnect.UpdateData(pDX->m_bSaveAndValidate))
		{
			pDX->Fail();
			return;
		}
	if (m_pageReconnect.GetSafeHwnd() != NULL)
		if (!m_pageReconnect.UpdateData(pDX->m_bSaveAndValidate))
		{
			pDX->Fail();
			return;
		}

	if (pDX->m_bSaveAndValidate)
	{
		m_dwLocalIP = m_pageGeneral.m_dwLocalIP;
		m_sRcon = m_pageGeneral.m_sRcon;
		m_sServerName = m_pageGeneral.m_sServerName;
		m_uLogPort = m_pageGeneral.m_uLogPort;
		m_uPort = m_pageGeneral.m_uPort;
		m_eType = m_pageGeneral.m_eType;

		m_bReconnectToGame = m_pageReconnect.m_bReconnect;
		m_nGameReconnectSec = m_pageReconnect.m_nReconnectSec;
		m_nGameReconnectTimes = m_pageReconnect.m_nReconnectTimes;
		m_bInactivity = m_pageReconnect.m_bInactivity;
		m_nInactivityTimeout = m_pageReconnect.m_nInactivityTimeout;

		m_lConnectCommands.RemoveAll();
		m_lConnectCommands.AddTail(&m_pageConnect.m_lCmds);
		m_lDisconnectCommands.RemoveAll();
		m_lDisconnectCommands.AddTail(&m_pageDisconnect.m_lCmds);
	}
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_BN_CLICKED(IDC_ADDPROFILE, OnAddprofile)
	ON_BN_CLICKED(IDC_REMOVEPROFILE, OnRemoveprofile)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RENAMEPROFILE, OnRenameprofile)
	ON_LBN_SELCHANGE(IDC_PROFILES, OnSelchangeProfiles)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_LBN_DBLCLK(IDC_PROFILES, OnDblclkProfiles)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

void CConnectDlg::OnAddprofile() 
{
	if (m_nCurrentItem >= 0) {
		if (!UpdateData())
			return;
		
		GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
		pProfile->Assign(this);
	}

	CProfileName dlg;
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_sName.IsEmpty()) {
			AfxMessageBox("Cannot create a profile with an empty name");
			return;
		}
		int nMax = m_ctlList.GetCount();
		for (int i = 0; i < nMax; i++)
		{
			GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(i);
			if (pProfile->m_sProfileName.CompareNoCase(dlg.m_sName) == 0)
			{
				AfxMessageBox("A profile with this name already exists");
				return;
			}
		}
		int nItem = InsertProfile(dlg.m_sName, true);
		m_ctlList.SetCurSel(nItem);
		m_nCurrentItem = nItem;
		UpdateData(FALSE);
	}
}

void CConnectDlg::OnRemoveprofile() 
{
	int nItem = m_ctlList.GetCurSel();
	if (nItem < 0)
		return;

	if (!UpdateData())
		return;
		
	if (m_ctlList.GetCount() < 2) {
		AfxMessageBox("Cannot delete last profile. You must have at least one profile at all times.");
		return;
	}

	GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(nItem);

	CString s;
	s.Format("Do you wish to delete the profile '%s'?", (LPCTSTR)pProfile->m_sProfileName);
	if (AfxMessageBox(s, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	delete pProfile;
	m_nCurrentItem = -1;
	m_ctlList.DeleteString(nItem);
	if (nItem >= m_ctlList.GetCount()) 
		nItem = m_ctlList.GetCount()-1;
	m_ctlList.SetCurSel(nItem);
	Assign((GameServerProfile*)m_ctlList.GetItemData(nItem));
	UpdateData(FALSE);
}

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_sheet.AddPage(&m_pageGeneral);
	m_sheet.AddPage(&m_pageConnect);
	m_sheet.AddPage(&m_pageDisconnect);
	m_sheet.AddPage(&m_pageReconnect);

	m_sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_sheet.ModifyStyle(0, WS_TABSTOP);

	CWnd* pWndPrev = GetDlgItem(IDC_TABS)->GetWindow(GW_HWNDPREV);

	CRect rcSheet;
	GetDlgItem(IDC_TABS)->GetWindowRect(&rcSheet);
	ScreenToClient( &rcSheet );
	m_sheet.SetWindowPos(pWndPrev, rcSheet.left-7, rcSheet.top-7, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

	FillList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlg::FillList()
{
	HKEY hKey = AfxGetApp()->GetSectionKey("Game-Servers");
	if (hKey == NULL)
		return;
	CString sLastProfile = m_sProfileName;
	CString sProfileName;
	DWORD dwIndex = 0;
	FILETIME fileTime;
	DWORD dwSize = MAX_PATH;
	CStringList lProfiles;
	LONG lRet = ::RegEnumKeyEx(hKey, dwIndex++, sProfileName.GetBuffer(MAX_PATH+1), &dwSize, NULL, NULL, NULL, &fileTime);
	while ((lRet == ERROR_SUCCESS) || (lRet == ERROR_MORE_DATA)) {
		sProfileName.ReleaseBuffer();
		lProfiles.AddTail(sProfileName);

		dwSize = MAX_PATH;
		lRet = ::RegEnumKeyEx(hKey, dwIndex++, sProfileName.GetBuffer(MAX_PATH+1), &dwSize, NULL, NULL, NULL, &fileTime);
	}
	sProfileName.ReleaseBuffer(0);
	::RegCloseKey(hKey);

	int nSelItem = -1, nItem;
	POSITION pos = lProfiles.GetHeadPosition();
	while (pos != NULL) {
		sProfileName = lProfiles.GetNext(pos);
		nItem = InsertProfile(sProfileName);
		if (sLastProfile.Compare(sProfileName) == 0)
			nSelItem = nItem;
	}

	if (m_ctlList.GetCount() < 1) {
		nItem = InsertProfile("Default");
	}
	if (nSelItem < 0)
		nSelItem = nItem;
	m_ctlList.SetCurSel(nSelItem);
	Assign((GameServerProfile*)m_ctlList.GetItemData(nSelItem));
	UpdateData(FALSE);
	m_nCurrentItem = nSelItem;
}

int CConnectDlg::InsertProfile(LPCTSTR sProfile, bool bAssign)
{
	GameServerProfile* pProfile = new GameServerProfile;
	pProfile->LoadFromRegistry(sProfile);
	int nItem = m_ctlList.AddString(sProfile);
	m_ctlList.SetItemData(nItem, (DWORD)pProfile);
	if (bAssign)
		Assign(pProfile);
	return nItem;
}

void CConnectDlg::OnDestroy() 
{
	HKEY hKey = AfxGetApp()->GetSectionKey("Game-Servers");
	if (hKey == NULL)
		return;
	CString sProfileName;
	DWORD dwIndex = 0;
	FILETIME fileTime;
	DWORD dwSize = MAX_PATH;
	CStringList lProfiles;
	LONG lRet = ::RegEnumKeyEx(hKey, dwIndex++, sProfileName.GetBuffer(MAX_PATH+1), &dwSize, NULL, NULL, NULL, &fileTime);
	while ((lRet == ERROR_SUCCESS) || (lRet == ERROR_MORE_DATA)) {
		sProfileName.ReleaseBuffer();
		lProfiles.AddTail(sProfileName);

		dwSize = MAX_PATH;
		lRet = ::RegEnumKeyEx(hKey, dwIndex++, sProfileName.GetBuffer(MAX_PATH+1), &dwSize, NULL, NULL, NULL, &fileTime);
	}
	sProfileName.ReleaseBuffer(0);

	POSITION pos = lProfiles.GetHeadPosition();
	while (pos != NULL) {
		sProfileName = lProfiles.GetNext(pos);
		::SHDeleteKey(hKey, sProfileName);
	}

	::RegCloseKey(hKey);

	int nMax = m_ctlList.GetCount();
	for (int i = 0; i<nMax; i++)
	{
		GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(i);
		pProfile->SaveToRegistry();
		delete pProfile;
	}
	CDialog::OnDestroy();	
}

void CConnectDlg::OnRenameprofile() 
{
	if (m_nCurrentItem >= 0) {
		if (!UpdateData())
			return;
		
		GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
		pProfile->Assign(this);
	}

	CProfileName dlg;
	dlg.m_sName = m_sProfileName;
	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.m_sName.IsEmpty()) {
		AfxMessageBox("Cannot rename to an empty name");
		return;
	}
	int nMax = m_ctlList.GetCount();
	for (int i = 0; i < nMax; i++)
	{
		GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(i);
		if (pProfile->m_sProfileName.CompareNoCase(dlg.m_sName) == 0)
		{
			AfxMessageBox("A profile with this name already exists");
			return;
		}
	}
	int nItem = m_nCurrentItem;

	GameServerProfile* pProfile = (GameServerProfile*)m_ctlList.GetItemData(nItem);
	m_ctlList.DeleteString(nItem);
	int nNewItem = m_ctlList.InsertString(nItem, dlg.m_sName);
	pProfile->m_sProfileName = dlg.m_sName;
	m_ctlList.SetItemData(nNewItem, (DWORD)pProfile);
	m_ctlList.SetCurSel(nNewItem);
	Assign((GameServerProfile*)m_ctlList.GetItemData(nNewItem));
	m_nCurrentItem = nNewItem;
}

void CConnectDlg::OnSelchangeProfiles() 
{
	GameServerProfile* pProfile;
	int nItem = m_ctlList.GetCurSel();
	if (m_nCurrentItem >= 0) {
		if (!UpdateData()) {
			m_ctlList.SetCurSel(m_nCurrentItem);
			return;
		}
		
		pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
		pProfile->Assign(this);
	}

	m_nCurrentItem = m_ctlList.GetCurSel();
	pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
	Assign(pProfile);
	UpdateData(FALSE);
}

void CConnectDlg::OnOk() 
{
	GameServerProfile* pProfile;
	if (m_nCurrentItem >= 0) {
		if (!UpdateData()) {
			m_ctlList.SetCurSel(m_nCurrentItem);
			return;
		}
		
		pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
		pProfile->Assign(this);
	}
	EndDialog(IDOK);
}

void CConnectDlg::OnCancel() 
{
	GameServerProfile* pProfile;
	if (m_nCurrentItem >= 0) {
		if (!UpdateData()) {
			m_ctlList.SetCurSel(m_nCurrentItem);
			return;
		}
		
		pProfile = (GameServerProfile*)m_ctlList.GetItemData(m_nCurrentItem);
		pProfile->Assign(this);
	}
	EndDialog(IDCANCEL);
}


void CConnectDlg::OnDblclkProfiles() 
{
	OnSelchangeProfiles();
	OnOk();
}
