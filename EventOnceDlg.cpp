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

// EventOnceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RconEvents.h"
#include "ProfileName.h"
#include "EventOnceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventOnceDlg dialog


CEventOnceDlg::CEventOnceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventOnceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventOnceDlg)
	m_tTime = 0;
	m_tDate = 0;
	m_sComment = _T("");
	m_bCritical = FALSE;
	//}}AFX_DATA_INIT
}


void CEventOnceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventOnceDlg)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_EDIT, m_btnEdit);
	DDX_DateTimeCtrl(pDX, IDC_TIME, m_tTime);
	DDX_DateTimeCtrl(pDX, IDC_DATE, m_tDate);
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	DDV_MaxChars(pDX, m_sComment, 255);
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


BEGIN_MESSAGE_MAP(CEventOnceDlg, CDialog)
	//{{AFX_MSG_MAP(CEventOnceDlg)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventOnceDlg message handlers

BOOL CEventOnceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	EnableButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventOnceDlg::OnSelchangeList() 
{
	EnableButtons();
}

void CEventOnceDlg::EnableButtons()
{
	BOOL bSel = (m_ctlList.GetCurSel() >= 0);
	m_btnEdit.EnableWindow(bSel);
	m_btnDelete.EnableWindow(bSel);
}

void CEventOnceDlg::OnAdd() 
{
	CProfileName dlg;
	dlg.m_sTitle = "Add command";
	if (dlg.DoModal() == IDOK)
	{
		m_ctlList.SetCurSel(m_ctlList.InsertString(m_ctlList.GetCount(), dlg.m_sName));
	}
	EnableButtons();
}

void CEventOnceDlg::OnEdit() 
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

void CEventOnceDlg::OnDelete() 
{
	if (AfxMessageBox("Are you sure?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		m_ctlList.DeleteString(m_ctlList.GetCurSel());
	}
	EnableButtons();
}

void CEventOnceDlg::Fill(RconEventOnce* pEvent)
{
	CString s;
	pEvent->m_tTime = (m_tTime.GetTime() % 86400);
	pEvent->m_tTime += m_tDate.GetTime() - (m_tDate.GetTime() % 86400);
	pEvent->m_sComment = m_sComment;
	pEvent->m_bCritical = m_bCritical ? true : false;
	pEvent->GetCmds().clear();
	for (STRLIST::iterator i = m_lCmds.begin(); i != m_lCmds.end(); i++)
		pEvent->GetCmds().push_back(*i);
}

void CEventOnceDlg::Empty(RconEventOnce* pEvent)
{
	m_tTime = pEvent->m_tTime % 86400;
	m_tDate = pEvent->m_tTime - m_tTime.GetTime();
	m_sComment = pEvent->m_sComment.c_str();
	m_bCritical = pEvent->m_bCritical ? TRUE : FALSE;
	m_lCmds.clear();

	for (STRLIST::iterator i = pEvent->GetCmds().begin(); i != pEvent->GetCmds().end(); i++)
		m_lCmds.push_back(*i);
}
