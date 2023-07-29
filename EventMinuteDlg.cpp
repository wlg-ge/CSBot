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

// EventMinuteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RconEvents.h"
#include "ProfileName.h"
#include "EventMinuteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventMinuteDlg dialog


CEventMinuteDlg::CEventMinuteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventMinuteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventMinuteDlg)
	m_sComment = _T("");
	m_nAmount = 0;
	m_bCritical = FALSE;
	//}}AFX_DATA_INIT
}


void CEventMinuteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventMinuteDlg)
	DDX_Control(pDX, IDC_AMOUNTSPIN, m_ctlSpin);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	DDX_Control(pDX, IDC_EDIT, m_btnEdit);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDV_MaxChars(pDX, m_sComment, 100);
	DDX_Text(pDX, IDC_AMOUNT, m_nAmount);
	DDV_MinMaxInt(pDX, m_nAmount, 1, 1439);
	DDX_Check(pDX, IDC_CRITICAL, m_bCritical);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		CString s;
		m_lCmds.clear();
		for (int i = 0; i<m_ctlList.GetCount(); i++)
		{
			m_ctlList.GetText(i, s);
			m_lCmds.push_back((LPCTSTR)s);
		}
	} else {
		m_ctlList.ResetContent();
		for (STRLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++)
			m_ctlList.InsertString(-1, (*i).c_str());
	}
}


BEGIN_MESSAGE_MAP(CEventMinuteDlg, CDialog)
	//{{AFX_MSG_MAP(CEventMinuteDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventMinuteDlg message handlers

BOOL CEventMinuteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ctlSpin.SetRange(1, 1439);
	EnableButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventMinuteDlg::OnAdd() 
{
	CProfileName dlg;
	dlg.m_sTitle = "Add command";
	if (dlg.DoModal() == IDOK)
	{
		m_ctlList.SetCurSel(m_ctlList.InsertString(m_ctlList.GetCount(), dlg.m_sName));
	}
	EnableButtons();
}

void CEventMinuteDlg::OnEdit() 
{
	int nSel = m_ctlList.GetCurSel();
	if (nSel < 0)
		return;
	CProfileName dlg;
	dlg.m_sTitle = "Edit command";
	m_ctlList.GetText(nSel, dlg.m_sName);
	if (dlg.DoModal() == IDOK)
	{
		m_ctlList.DeleteString(nSel);
		m_ctlList.SetCurSel(m_ctlList.InsertString(nSel, dlg.m_sName));
	}
	EnableButtons();
}

void CEventMinuteDlg::OnDelete() 
{
	if (AfxMessageBox("Are you sure?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		m_ctlList.DeleteString(m_ctlList.GetCurSel());
	}
	EnableButtons();
}

void CEventMinuteDlg::OnSelchangeList() 
{
	EnableButtons();
}

void CEventMinuteDlg::EnableButtons()
{
	BOOL bSel = (m_ctlList.GetCurSel() >= 0);
	m_btnEdit.EnableWindow(bSel);
	m_btnDelete.EnableWindow(bSel);
}

void CEventMinuteDlg::Fill(RconEventMinute* pEvent)
{
	CString s;
	pEvent->m_nAmount = m_nAmount;
	pEvent->m_sComment = m_sComment;
	pEvent->m_bCritical = m_bCritical ? true : false;
	pEvent->GetCmds().clear();
	for (STRLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++)
		pEvent->GetCmds().push_back(*i);
}

void CEventMinuteDlg::Empty(RconEventMinute* pEvent)
{
	m_nAmount = pEvent->m_nAmount;
	m_sComment = pEvent->m_sComment.c_str();
	m_bCritical = pEvent->m_bCritical ? TRUE : FALSE;
	m_lCmds.clear();

	for (STRLIST::iterator i = pEvent->GetCmds().begin(); i != pEvent->GetCmds().end(); i++)
		m_lCmds.push_back(*i);
}
