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

// OptionsSummon.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsSummon.h"
#include "ProfileName.h"
#include "Settings.h"
#include "SummonCmdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSummon dialog

IMPLEMENT_DYNCREATE(COptionsSummon, COptionsPage)

COptionsSummon::COptionsSummon() : COptionsPage(COptionsSummon::IDD)
{
	//{{AFX_DATA_INIT(COptionsSummon)
	m_bAllowSummon = FALSE;
	m_bSummonFlash = FALSE;
	m_bSummonPlay = FALSE;
	m_sSummonWord = _T("");
	m_bAllowSummonTeam = FALSE;
	//}}AFX_DATA_INIT
}

COptionsSummon::~COptionsSummon()
{
}


void COptionsSummon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSummon)
	DDX_Control(pDX, IDC_EDITLINE, m_ctlEditLine);
	DDX_Control(pDX, IDC_DELETELINE, m_ctlDeleteLine);
	DDX_Control(pDX, IDC_LINELIST, m_ctlLineList);
	DDX_Control(pDX, IDC_SUMMON_PLAYFILE, m_ctlPlayFile);
	DDX_Check(pDX, IDC_ALLOWSUMMON, m_bAllowSummon);
	DDX_Check(pDX, IDC_SUMMON_FLASH, m_bSummonFlash);
	DDX_Check(pDX, IDC_SUMMON_PLAY, m_bSummonPlay);
	DDX_Text(pDX, IDC_SUMMONWORD, m_sSummonWord);
	DDV_MaxChars(pDX, m_sSummonWord, 100);
	DDX_Check(pDX, IDC_ALLOWSUMMONTEAM, m_bAllowSummonTeam);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		m_lLines.RemoveAll();
		int nMax = m_ctlLineList.GetItemCount();
		for (int i=0; i<nMax; i++)
		{
			m_lLines.AddTail(m_ctlLineList.GetItemText(i, 0));
		}
	} else {
		int nItem;
		m_ctlLineList.DeleteAllItems();
		POSITION pos = m_lLines.GetHeadPosition();
		while (pos != NULL)
		{
			CString sLine = m_lLines.GetNext(pos);
			m_ctlLineList.InsertItem(nItem = m_ctlLineList.GetItemCount(), sLine);
			m_ctlLineList.SetItemText(nItem, 0, sLine);
		}
		if (m_sSummonPlayFile.IsEmpty()) {
			m_ctlPlayFile.SetWindowText("Choose file...");
		} else {
			CString sFile = m_sSummonPlayFile;
			m_ctlPlayFile.SetWindowText(sFile);
		}
	}
}

BEGIN_MESSAGE_MAP(COptionsSummon, CDialog)
	//{{AFX_MSG_MAP(COptionsSummon)
	ON_BN_CLICKED(IDC_ADDLINE, OnAddline)
	ON_BN_CLICKED(IDC_DELETELINE, OnDeleteline)
	ON_BN_CLICKED(IDC_EDITLINE, OnEditline)
	ON_NOTIFY(NM_DBLCLK, IDC_LINELIST, OnDblclkLinelist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LINELIST, OnItemchangedLinelist)
	ON_BN_CLICKED(IDC_SUMMON_PLAYFILE, OnSummonPlayfile)
	ON_EN_CHANGE(IDC_SUMMONWORD, OnModified)
	ON_BN_CLICKED(IDC_ALLOWSUMMON, OnModified)
	ON_BN_CLICKED(IDC_ALLOWSUMMONTEAM, OnModified)
	ON_BN_CLICKED(IDC_SUMMON_FLASH, OnModified)
	ON_BN_CLICKED(IDC_SUMMON_PLAY, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSummon message handlers

void COptionsSummon::OnAddline() 
{
	CSummonCmdDlg dlg;
//	dlg.m_sTitle = "Add IRC command";
	if (dlg.DoModal() == IDOK) {
		CString sText = dlg.m_sName;
		sText.TrimLeft();
		sText.TrimRight();
		if (!sText.IsEmpty()) {
			int nItem = m_ctlLineList.InsertItem(m_ctlLineList.GetItemCount(), sText);
			m_ctlLineList.SetItemText(nItem, 0, sText);
			m_ctlLineList.SetSelectionMark(nItem);
			m_ctlLineList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			OnModified();
		}
	}
}

void COptionsSummon::OnDeleteline() 
{
	POSITION pos = m_ctlLineList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nPos = m_ctlLineList.GetNextSelectedItem(pos);
		m_ctlLineList.DeleteItem(nPos);

		if (nPos == m_ctlLineList.GetItemCount())
			nPos--;
		if (nPos >= 0) {
			m_ctlLineList.SetItemState(nPos, LVIS_SELECTED, LVIS_SELECTED);
		}
		POSITION pos = m_ctlLineList.GetFirstSelectedItemPosition();
		if (pos != NULL) {
			m_ctlDeleteLine.EnableWindow(TRUE);
			m_ctlEditLine.EnableWindow(TRUE);
		} else {
			m_ctlDeleteLine.EnableWindow(FALSE);
			m_ctlEditLine.EnableWindow(FALSE);
		}
		OnModified();
	}
}

void COptionsSummon::OnEditline() 
{
	POSITION pos = m_ctlLineList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		CSummonCmdDlg dlg;
		int nPos = m_ctlLineList.GetNextSelectedItem(pos);
		dlg.m_sName = m_ctlLineList.GetItemText(nPos, 0);
		if (dlg.DoModal() == IDOK) {
			dlg.m_sName.TrimLeft();
			dlg.m_sName.TrimRight();
			if (!dlg.m_sName.IsEmpty()) {
				m_ctlLineList.SetItemText(nPos, 0, dlg.m_sName);
				OnModified();
			}
		}
	}	
}

void COptionsSummon::OnDblclkLinelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnEditline();
}

void COptionsSummon::OnItemchangedLinelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION pos = m_ctlLineList.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		m_ctlDeleteLine.EnableWindow(TRUE);
		m_ctlEditLine.EnableWindow(TRUE);
	} else {
		m_ctlDeleteLine.EnableWindow(FALSE);
		m_ctlEditLine.EnableWindow(FALSE);
	}
	
	*pResult = 0;
}

void COptionsSummon::OnSummonPlayfile() 
{
	CFileDialog dlg(TRUE, "wav", m_sSummonPlayFile, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Wave files (*.wav)|*.wav|All files (*.*)|*.*||", this);
	if (dlg.DoModal() == IDOK)
	{
		m_sSummonPlayFile = dlg.GetPathName();
		CString sFile = m_sSummonPlayFile;
		m_ctlPlayFile.SetWindowText(sFile);
		OnModified();
	}
}

BOOL COptionsSummon::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect;
	m_ctlLineList.GetClientRect(rect);
	m_ctlLineList.InsertColumn(0, "", LVCFMT_LEFT, rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL), 0);
	m_ctlLineList.SetExtendedStyle(m_ctlLineList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	POSITION pos = m_ctlLineList.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		m_ctlDeleteLine.EnableWindow(TRUE);
		m_ctlEditLine.EnableWindow(TRUE);
	} else {
		m_ctlDeleteLine.EnableWindow(FALSE);
		m_ctlEditLine.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSummon::ReadSettings()
{
	m_bAllowSummon = settings.m_bAllowSummon;
	m_bAllowSummonTeam = settings.m_bAllowSummonTeam;
	m_bSummonFlash = settings.m_bSummonFlash;
	m_bSummonPlay = settings.m_bSummonPlay;
	m_sSummonPlayFile = settings.m_sSummonPlayFile;
	m_lLines.RemoveAll();
	m_lLines.AddTail(&settings.m_lSummonLines);
	m_sSummonWord = settings.m_sSummonWord;
}

void COptionsSummon::WriteSettings()
{
	settings.m_bAllowSummon = m_bAllowSummon;
	settings.m_bAllowSummonTeam = m_bAllowSummonTeam;
	settings.m_bSummonFlash = m_bSummonFlash;
	settings.m_bSummonPlay = m_bSummonPlay;
	settings.m_sSummonPlayFile = m_sSummonPlayFile;
	settings.m_lSummonLines.RemoveAll();
	settings.m_lSummonLines.AddTail(&m_lLines);
	settings.m_sSummonWord = m_sSummonWord;
}
