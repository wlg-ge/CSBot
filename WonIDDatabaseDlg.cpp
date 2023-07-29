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

// WonIDDatabaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "WonIDDatabaseDlg.h"
#include "MainFrm.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWonIDDatabaseDlg dialog


CWonIDDatabaseDlg::CWonIDDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWonIDDatabaseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWonIDDatabaseDlg)
	//}}AFX_DATA_INIT
	m_pMain = NULL;
	m_dwSelID = -2;
}


void CWonIDDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWonIDDatabaseDlg)
	DDX_Control(pDX, IDC_CASE, m_ctlCase);
	DDX_Control(pDX, IDC_FINDNAME, m_ctlFindName);
	DDX_Control(pDX, IDC_NEWNAME, m_ctlNewName);
	DDX_Control(pDX, IDC_NEWEONID, m_ctlNewWonID);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	DDX_Control(pDX, IDC_WONID, m_ctlWonID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWonIDDatabaseDlg, CDialog)
	//{{AFX_MSG_MAP(CWonIDDatabaseDlg)
	ON_CBN_SELCHANGE(IDC_WONID, OnSelchangeWonid)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_ADDWONID, OnAddwonid)
	ON_BN_CLICKED(IDC_ADDNAME, OnAddname)
	ON_BN_CLICKED(IDC_DELETENAME, OnDeletename)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWonIDDatabaseDlg message handlers

BOOL CWonIDDatabaseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillWonIDList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWonIDDatabaseDlg::FillWonIDList()
{
	DWORDARRAY lIDs;
	CString sName;
	m_ctlWonID.ResetContent();
	CString sFolder = settings.m_sWonIDFolder;
	sFolder += "\\*.WonID";
	WIN32_FIND_DATA fd;
	HANDLE hSearch = ::FindFirstFile(sFolder, &fd);
	if (hSearch == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		if (dwError != ERROR_FILE_NOT_FOUND) {
			CString sMsg;
			sMsg.Format("Cannot find first file of '%s'\nWonID list will not be filled.\nGetLastError() = %08x", (LPCTSTR)sFolder, dwError);
			AfxMessageBox(sMsg);
		}
		return;
	}
	DWORD nSize = 0;
	do {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			sName = fd.cFileName;
			sName = sName.Left(sName.GetLength()-6);
			if (WonIDDBManager::IsDigitsOnly(sName)) {
				lIDs.push_back((DWORD)atol(sName));
				nSize += fd.nFileSizeLow;
			}	
		}
	} while (::FindNextFile(hSearch, &fd));
	if (GetLastError() != ERROR_NO_MORE_FILES) {
		CString sMsg;
		sMsg.Format("Error iterating through '%s'\nWonID list is not full.", (LPCTSTR)sFolder);
		AfxMessageBox(sMsg);
	}


	int i, nMax = lIDs.size();
	for (i=0; i<nMax-1; i++)
	{
		for (int j=i+1; j<nMax; j++)
		{
			if (lIDs[i] > lIDs[j])
			{
				DWORD dwTemp = lIDs[i];
				lIDs[i] = lIDs[j];
				lIDs[j] = dwTemp;
			}
		}
	}
	int nSelItem = -1, nIndex;
	for (i=0; i<nMax; i++)
	{
		sName.Format("%u", lIDs[i]);
		nIndex = m_ctlWonID.AddString(sName);
		m_ctlWonID.SetItemData(nIndex, lIDs[i]);
		if (lIDs[i] == m_dwSelID)
			nSelItem = nIndex;
	}
	if (nSelItem >= 0)
	{
		m_ctlWonID.SetCurSel(nSelItem);
	}
	CString sCount;
	sCount.Format("Total of %d WonIDs (%uKb)", m_ctlWonID.GetCount(), (nSize / 1024));
	GetDlgItem(IDC_COUNT)->SetWindowText(sCount);
}

void CWonIDDatabaseDlg::FillNameList(int nItem)
{
	m_ctlList.ResetContent();

	DWORD dwWonID = m_ctlWonID.GetItemData(nItem);

	int nSelItem = -1;
	WonIDDBManager::GetAllNames(dwWonID, this, (LPARAM)&nSelItem);

	if (nSelItem >= 0)
		m_ctlList.SetCurSel(nSelItem);
	else if (m_ctlList.GetCount() > 0)
		m_ctlList.SetCurSel(0);
}

bool CWonIDDatabaseDlg::FoundName(LPCTSTR pName, DWORD dwWonID, LPARAM lParam)
{
	int nIndex = m_ctlList.InsertString(m_ctlList.GetCount(), pName);
	if (m_sName.Compare(pName) == 0) {
		*((int*)lParam) = nIndex;
	}
	return true;
}

void CWonIDDatabaseDlg::OnSelchangeWonid() 
{
	int nItem = m_ctlWonID.GetCurSel();
	if (nItem >= 0) {
		FillNameList(nItem);
	}
}

void CWonIDDatabaseDlg::OnRefresh() 
{
	m_ctlList.ResetContent();
	FillWonIDList();
	int nSel = m_ctlWonID.GetCurSel();
	if (nSel >= 0)
		FillNameList(nSel);
}

void CWonIDDatabaseDlg::OnAddwonid() 
{
	CString sID;
	m_ctlNewWonID.GetWindowText(sID);
	if (sID.IsEmpty())
		return;
	m_ctlNewWonID.SetWindowText("");
	if (!WonIDDBManager::IsDigitsOnly(sID)) {
		AfxMessageBox("Please enter only digits");
		return;
	}
	DWORD dwWonID = atoi(sID);
	int nErr = WonIDDBManager::WonIDAdd(dwWonID);
	if (nErr != WONID_OK)
		WonIDDBManager::MsgBox(nErr);
	m_dwSelID = dwWonID;
	OnRefresh();
}

void CWonIDDatabaseDlg::OnAddname() 
{
	int nItem = m_ctlWonID.GetCurSel();
	if (nItem < 0) {
		AfxMessageBox("Please select a WonID from the list");
		return;
	}
	DWORD dwID = m_ctlWonID.GetItemData(nItem);
	CString sName;
	m_ctlNewName.GetWindowText(sName);
	if (sName.IsEmpty())
		return;
	m_ctlNewName.SetWindowText("");
	int nErr = WonIDDBManager::AddWonIDToDatabase(dwID, sName);
	if (nErr != WONID_OK)
		WonIDDBManager::MsgBox(nErr);

	FillNameList(nItem);
}


void CWonIDDatabaseDlg::OnDeletename() 
{
	int nItem = m_ctlWonID.GetCurSel();
	if (nItem < 0) {
		AfxMessageBox("Please select a WonID from the list");
		return;
	}
	DWORD dwWonID = m_ctlWonID.GetItemData(nItem);
	int nNameItem = m_ctlList.GetCurSel();
	if (nNameItem < 0) {
		AfxMessageBox("Please select a name from the list");
		return;
	}
	CString sName;
	m_ctlList.GetText(nNameItem, sName);

	WonIDDBManager::DeleteName(dwWonID, sName);

	FillNameList(nItem);
}

void CWonIDDatabaseDlg::OnFind() 
{
	CWaitCursor wait;
	CString sSub;
	m_ctlFindName.GetWindowText(sSub);
	sSub.TrimLeft();
	sSub.TrimRight();
	if (sSub.IsEmpty()) {
		AfxMessageBox("Please enter a substring to search for");
		return;
	}
	// determine starting point
	int nItem = m_ctlWonID.GetCurSel();
	CString sStartName;
	DWORD dwStartWonID;
	if (nItem >= 0) {
		dwStartWonID = m_ctlWonID.GetItemData(nItem);
		nItem = m_ctlList.GetCurSel();
		if (nItem >= 0)
			m_ctlList.GetText(nItem, sStartName);
	} else {
		dwStartWonID = 0;
		sStartName = "";
	}

	bool bMatchCase = m_ctlCase.GetCheck() ? true : false;
	if (!bMatchCase)
		sSub.MakeLower();

	DWORD dwWonID = dwStartWonID;
	int nRet;
	do {
		nRet = WonIDDBManager::FindNextMatch(sSub, dwWonID, sStartName, bMatchCase);
	} while ((nRet == WONID_NOTFOUND) && (dwWonID != dwStartWonID));

	// and then once more (inside our first WonID)
	if (nRet == WONID_NOTFOUND) {
		nRet = WonIDDBManager::FindNextMatch(sSub, dwWonID, sStartName, bMatchCase);
	}

	switch (nRet)
	{
	case WONID_OK:
		SelectItem(dwWonID, sStartName);
		break;
	case WONID_NOTFOUND:
		AfxMessageBox("Name/WonID not found in database");
		break;
	default:
		WonIDDBManager::MsgBox(nRet);
	}
}

void CWonIDDatabaseDlg::SelectItem(DWORD dwWonID, LPCTSTR pName)
{
	m_dwSelID = dwWonID;
	FillWonIDList();
	if (pName == NULL) 
		m_sName.Empty();
	else
		m_sName = pName;
	OnSelchangeWonid();
	m_sName.Empty();
}

void CWonIDDatabaseDlg::OnExport() 
{
	CFileDialog dlg(FALSE, ".txt", NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, "Text files (*.txt)|*.txt|All files (*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wait;
		CString sFilename = dlg.GetPathName();
		FILE* pFile = fopen(sFilename, "wt");
		if (pFile == NULL) {
			AfxMessageBox("Could not open file for writing");
			return;
		}
		fprintf(pFile, "WonID\tNames...\n");
		int nRet = WonIDDBManager::Export(pFile);
		if (nRet != WONID_OK)
			WonIDDBManager::MsgBox(nRet);
		fclose(pFile);
	}
}

void CWonIDDatabaseDlg::OnImport() 
{
	CFileDialog dlg(TRUE, ".txt", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Text files (*.txt)|*.txt|All files (*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wait;
		CString sFilename = dlg.GetPathName();
		FILE* pFile = fopen(sFilename, "rt");
		if (pFile == NULL) {
			AfxMessageBox("Could not open file for reading");
			return;
		}
		int nRet = WonIDDBManager::Import(pFile);
		if (nRet != WONID_OK)
			WonIDDBManager::MsgBox(nRet);

		fclose(pFile);
		OnRefresh();
	}
}


