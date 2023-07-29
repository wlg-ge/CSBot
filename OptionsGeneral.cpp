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

// OptionsGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsGeneral.h"
#include "SchedView.h"
#include "Settings.h"
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsGeneral property page

IMPLEMENT_DYNCREATE(COptionsGeneral, COptionsPage)

COptionsGeneral::COptionsGeneral() : COptionsPage(COptionsGeneral::IDD)
{
	//{{AFX_DATA_INIT(COptionsGeneral)
	m_sLogFile = _T("");
	m_bLog = FALSE;
	m_bRemember = FALSE;
	m_bUpdateWonID = FALSE;
	m_sWonIDFolder = _T("");
	m_bNotifyUpdates = FALSE;
	m_bWarnLogOff = FALSE;
	m_nGameConnectionTimeout = 0;
	m_bFilterLog = FALSE;
	//}}AFX_DATA_INIT
	m_nRunLevel = -1;
}

COptionsGeneral::~COptionsGeneral()
{
}

void COptionsGeneral::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsGeneral)
	DDX_Control(pDX, IDC_RUNLEVEL, m_ctlRunLevel);
	DDX_Text(pDX, IDC_FILENAME, m_sLogFile);
	DDV_MaxChars(pDX, m_sLogFile, 255);
	DDX_Check(pDX, IDC_LOGTOFILE, m_bLog);
	DDX_Check(pDX, IDC_REMEMBER, m_bRemember);
	DDX_Check(pDX, IDC_WONIDDB, m_bUpdateWonID);
	DDX_Text(pDX, IDC_WONIDFOLDER, m_sWonIDFolder);
	DDV_MaxChars(pDX, m_sWonIDFolder, 255);
	DDX_Check(pDX, IDC_NOTIFYUPDATES, m_bNotifyUpdates);
	DDX_Check(pDX, IDC_WARNLOGOFF, m_bWarnLogOff);
	DDX_Text(pDX, IDC_GAMECONNECTTIMEOUT, m_nGameConnectionTimeout);
	DDV_MinMaxInt(pDX, m_nGameConnectionTimeout, 1, 1000);
	DDX_Check(pDX, IDC_FILTER_LOG, m_bFilterLog);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		if (m_bUpdateWonID)
		{
			struct _stat stt;
			// make sure that the WonID database folder exists, and that it is indeed a folder
			if (_stat((LPCTSTR)m_sWonIDFolder, &stt) != 0)
			{
				AfxMessageBox("The specified path for the WonID Database does not exist.\nPlease select an existing folder.", MB_ICONERROR | MB_OK);
				pDX->PrepareEditCtrl(IDC_WONIDFOLDER);
				pDX->Fail();
			}
			if ((stt.st_mode & _S_IFDIR) != _S_IFDIR)
			{
				AfxMessageBox("The specified path for the WonID Database is not a folder.\nPlease select an existing folder.", MB_ICONERROR | MB_OK);
				pDX->PrepareEditCtrl(IDC_WONIDFOLDER);
				pDX->Fail();
			}
		}

		int nSel = m_ctlRunLevel.GetCurSel();
		if (nSel >= 0)
			m_nRunLevel = m_ctlRunLevel.GetItemData(nSel);
	} else {
		int nMax = m_ctlRunLevel.GetCount();
		for (int i = 0; i< nMax; i++)
		{
			if ((int)m_ctlRunLevel.GetItemData(i) == m_nRunLevel) {
				m_ctlRunLevel.SetCurSel(i);
				break;
			}
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsGeneral, COptionsPage)
	//{{AFX_MSG_MAP(COptionsGeneral)
	ON_BN_CLICKED(IDC_LOGBROWSE, OnLogbrowse)
	ON_BN_CLICKED(IDC_WONIDBROWSE, OnWonidbrowse)
	ON_BN_CLICKED(IDC_LOGTOFILE, OnModified)
	ON_BN_CLICKED(IDC_LOGIRCTOFILE, OnModified)
	ON_BN_CLICKED(IDC_WONIDDB, OnModified)
	ON_BN_CLICKED(IDC_REMEMBER, OnModified)
	ON_BN_CLICKED(IDC_NOTIFYUPDATES, OnModified)
	ON_BN_CLICKED(IDC_WARNLOGOFF, OnModified)
	ON_BN_CLICKED(IDC_FILTER_LOG, OnModified)
	ON_EN_CHANGE(IDC_FILENAME, OnModified)
	ON_EN_CHANGE(IDC_IRCFOLDER, OnModified)
	ON_EN_CHANGE(IDC_WONIDFOLDER, OnModified)
	ON_EN_CHANGE(IDC_RCONTIMEOUT, OnModified)
	ON_CBN_SELCHANGE(IDC_RUNLEVEL, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsGeneral message handlers

int COptionsGeneral::BrowseCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		COptionsGeneral* pDlg = (COptionsGeneral*)lpData;
		if (pDlg == NULL)
			return 0;
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)pDlg->m_sStartFolder);
	}
	return 0;
}

void COptionsGeneral::OnLogbrowse() 
{
	char c[MAX_PATH];
	BROWSEINFO info;
	info.hwndOwner = GetSafeHwnd();
	info.pidlRoot = NULL;
	info.pszDisplayName = c;
	info.lpszTitle = "Select CS logging folder";
	info.ulFlags = 0;
	info.lpfn = &BrowseCallBack;
	info.lParam = (LPARAM)this;

	GetDlgItemText(IDC_FILENAME, m_sStartFolder);

	LPITEMIDLIST idList = SHBrowseForFolder(&info);
	if (idList != NULL) {
		CString s;
		SHGetPathFromIDList(idList, s.GetBuffer(MAX_PATH));
		s.ReleaseBuffer();
		SetDlgItemText(IDC_FILENAME, (LPCTSTR)s);
		OnModified();
	}
}

void COptionsGeneral::OnWonidbrowse() 
{
	char c[MAX_PATH];
	BROWSEINFO info;
	info.hwndOwner = GetSafeHwnd();
	info.pidlRoot = NULL;
	info.pszDisplayName = c;
	info.lpszTitle = "Select WonID DB folder";
	info.ulFlags = 0;
	info.lpfn = &BrowseCallBack;
	info.lParam = (LPARAM)this;

	GetDlgItemText(IDC_WONIDFOLDER, m_sStartFolder);;

	LPITEMIDLIST idList = SHBrowseForFolder(&info);
	if (idList != NULL) {
		CString s;
		SHGetPathFromIDList(idList, s.GetBuffer(MAX_PATH));
		s.ReleaseBuffer();
		SetDlgItemText(IDC_WONIDFOLDER, (LPCTSTR)s);;
		OnModified();
	}
}

BOOL COptionsGeneral::OnInitDialog() 
{
	COptionsPage::OnInitDialog();

	m_ctlRunLevel.SetItemData(m_ctlRunLevel.AddString("None - No events will be executed"), RUNLEVEL_NONE);
	m_ctlRunLevel.SetItemData(m_ctlRunLevel.AddString("Critical - Only critical events"), RUNLEVEL_CRITICAL);
	m_ctlRunLevel.SetItemData(m_ctlRunLevel.AddString("All - All types of events"), RUNLEVEL_ALL);

	UpdateData(FALSE);

	EnableControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsGeneral::ReadSettings()
{
	m_bLog = settings.IsLogging();
	m_sLogFile = settings.GetLogPath();
	m_bFilterLog = settings.IsFilterLog();
	m_bUpdateWonID = settings.m_bUpdateWonID;
	m_sWonIDFolder = settings.m_sWonIDFolder;
	m_bRemember = settings.m_bRemember;
	m_nRunLevel = settings.m_nRunLevel;
	m_bNotifyUpdates = settings.m_bNotifyUpdates;
	m_bWarnLogOff = settings.m_bWarnLogOff;
	m_nGameConnectionTimeout = settings.m_nGameConnectionTimeout;
	EnableControls();
}

void COptionsGeneral::WriteSettings()
{
	settings.SetLogging(m_bLog, m_sLogFile, m_bFilterLog);
	settings.m_bUpdateWonID = m_bUpdateWonID;
	settings.m_sWonIDFolder = m_sWonIDFolder;
	settings.m_bRemember = m_bRemember;
	settings.m_nRunLevel = m_nRunLevel;
	settings.m_bNotifyUpdates = m_bNotifyUpdates;
	settings.m_bWarnLogOff = m_bWarnLogOff;
	settings.m_nGameConnectionTimeout = m_nGameConnectionTimeout;
}

void COptionsGeneral::EnableControls()
{
	if (GetSafeHwnd() == NULL)
		return;
	BOOL bEnable = ((CButton*)GetDlgItem(IDC_LOGTOFILE))->GetCheck() == 1;
	GetDlgItem(IDC_FILTER_LOG)->EnableWindow(bEnable ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_LOGBROWSE)->EnableWindow(bEnable ? SW_SHOW : SW_HIDE);
	bEnable = ((CButton*)GetDlgItem(IDC_WONIDDB))->GetCheck() == 1;
	GetDlgItem(IDC_WONIDBROWSE)->EnableWindow(bEnable ? SW_SHOW : SW_HIDE);
}

void COptionsGeneral::OnModified()
{
	EnableControls();
	COptionsPage::OnModified();
}
