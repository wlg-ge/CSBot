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

// OptionsIRCMsgsMsgs.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Settings.h"
#include "OptionsIRCMsgsMsgs.h"
#include "OptionsPage.h"
#include "EditIRCMessageDlg.h"
#include "GIRLLogParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsMsgs property page

IMPLEMENT_DYNCREATE(COptionsIRCMsgsMsgs, CPropertyPage)

COptionsIRCMsgsMsgs::COptionsIRCMsgsMsgs() : CPropertyPage(COptionsIRCMsgsMsgs::IDD)
{
	//{{AFX_DATA_INIT(COptionsIRCMsgsMsgs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bSoft = false;
	m_pParser = NULL;
	m_pPage = NULL;
}

COptionsIRCMsgsMsgs::~COptionsIRCMsgsMsgs()
{
}

void COptionsIRCMsgsMsgs::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsIRCMsgsMsgs)
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsIRCMsgsMsgs, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsIRCMsgsMsgs)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsMsgs message handlers

BOOL COptionsIRCMsgsMsgs::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctlList.SetExtendedStyle(m_ctlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	CRect rect;
	m_ctlList.GetClientRect(&rect);
	m_ctlList.InsertColumn(0, "Message text", LVCFMT_LEFT, rect.Width() - ::GetSystemMetrics(SM_CXEDGE) - ::GetSystemMetrics(SM_CXVSCROLL), 0);
	m_ctlList.InsertColumn(1, "Meaning", LVCFMT_LEFT, rect.Width() - ::GetSystemMetrics(SM_CXEDGE) - ::GetSystemMetrics(SM_CXVSCROLL), 1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsIRCMsgsMsgs::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	int nItem = m_ctlList.GetNextSelectedItem(pos);
	if (nItem < 0)
		return;

	IRCCustomMsg* pMsg = (IRCCustomMsg*)m_ctlList.GetItemData(nItem);
	CEditIRCMessageDlg dlg;
	dlg.m_pMsg = new IRCCustomMsg(*pMsg);
	dlg.m_pParser = m_pParser;
	if (dlg.DoModal())
	{
		pMsg->m_sValue = dlg.m_pMsg->m_sValue;
		pMsg->m_bEnabled = dlg.m_pMsg->m_bEnabled;
		m_ctlList.SetCheck(nItem, pMsg->m_bEnabled ? TRUE : FALSE);
		m_ctlList.SetItemText(nItem, 0, pMsg->m_sValue.c_str());
		m_pPage->OnModified();
	}
	delete dlg.m_pMsg;
	
	*pResult = 0;
}

void COptionsIRCMsgsMsgs::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;
	*pResult = 0;

	if (m_bSoft)
		return;

	if ((pNMListView->uOldState & LVIS_STATEIMAGEMASK) != (pNMListView->uNewState & LVIS_STATEIMAGEMASK))
	{
		IRCCustomMsg* pMsg = (IRCCustomMsg*)m_ctlList.GetItemData(pNMListView->iItem);
		pMsg->m_bEnabled = m_ctlList.GetCheck(pNMListView->iItem) ? true : false;
		m_pPage->OnModified();
	}
}
