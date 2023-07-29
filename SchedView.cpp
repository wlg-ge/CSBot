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

// SchedView.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "SchedView.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "RconEvents.h"
#include "EventOnceDlg.h"
#include "EventMinuteDlg.h"
#include "EventHourDlg.h"
#include "EventDayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchedView

#define IDC_EVENTLIST	501

CSchedView::CSchedView(CChildFrame* pFrame) : CCSBotView(pFrame)
{
	m_nSortDir = 1;
	m_nSortColumn = -1;
}

CSchedView::~CSchedView()
{
}


BEGIN_MESSAGE_MAP(CSchedView, CCSBotView)
	//{{AFX_MSG_MAP(CSchedView)
	ON_WM_SIZE()
	ON_COMMAND(ID_SCHED_DELETE, OnSchedDelete)
	ON_COMMAND(ID_SCHED_EDIT, OnSchedEdit)
	ON_COMMAND(ID_SCHED_RUNNOW, OnRunNow)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_RCLICK, IDC_EVENTLIST, OnRclickList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENTLIST, OnColumnclickList)
	ON_COMMAND(ID_INSERT_ONCE, OnInsertOnce)
	ON_COMMAND(ID_INSERT_MINUTE, OnInsertMinute)
	ON_COMMAND(ID_INSERT_HOUR, OnInsertHour)
	ON_COMMAND(ID_INSERT_DAY, OnInsertDay)
	ON_NOTIFY(NM_DBLCLK, IDC_EVENTLIST, OnDblclkList)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSchedView message handlers

BOOL CSchedView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	if (!m_ctlList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS, CRect(0,0,10,10), this, IDC_EVENTLIST))
		return FALSE;

	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	m_ctlList.SetFont(&font, FALSE);

	m_ctlList.SetExtendedStyle(m_ctlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// build the columns in the list control
	m_ctlList.InsertColumn(0, "Comment", LVCFMT_LEFT, 150, 0);
	m_ctlList.InsertColumn(1, "Type", LVCFMT_LEFT, 50, 1);
	m_ctlList.InsertColumn(2, "Next run time", LVCFMT_LEFT, 130, 2);
	m_ctlList.InsertColumn(3, "Last run time", LVCFMT_LEFT, 45, 3);
	m_ctlList.InsertColumn(4, "Critical", LVCFMT_LEFT, 50, 4);

	m_menu.CreatePopupMenu();
	m_menu.AppendMenu(MF_STRING, ID_SCHED_EDIT, "Edit");
	m_menu.AppendMenu(MF_STRING, ID_SCHED_DELETE, "Delete");
	m_menu.AppendMenu(MF_STRING, ID_SCHED_RUNNOW, "Run now");
	m_menu.AppendMenu(MF_SEPARATOR, 0, "");
	m_menu.AppendMenu(MF_STRING, ID_INSERT_ONCE, "Add once");
	m_menu.AppendMenu(MF_STRING, ID_INSERT_MINUTE, "Add every minute");
	m_menu.AppendMenu(MF_STRING, ID_INSERT_HOUR, "Add every hour");
	m_menu.AppendMenu(MF_STRING, ID_INSERT_DAY, "Add every day");

	m_menu.SetDefaultItem(ID_SCHED_EDIT, FALSE);

	SetWindowText("Rcon Sched");

	return TRUE;
}

void CSchedView::OnSize(UINT nType, int cx, int cy) 
{
	CCSBotView::OnSize(nType, cx, cy);
	if (m_ctlList.GetSafeHwnd() != NULL)
		m_ctlList.MoveWindow(0, 0, cx, cy);
}

void CSchedView::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// sort when clicking a column header
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nColumn = pNMListView->iSubItem;

	if (nColumn == m_nSortColumn) {
		m_nSortDir *= -1;
	} else {
		m_nSortColumn = nColumn;
		m_nSortDir = 1;
	}

	m_ctlList.SortItems(&CompareEvents, (DWORD)this);
	
	*pResult = 0;
}

int CALLBACK CSchedView::CompareEvents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// compare function got sorting the events
	RconEvent* pEvt1 = (RconEvent*)lParam1;
	RconEvent* pEvt2 = (RconEvent*)lParam2;
	CSchedView* pDlg = (CSchedView*)lParamSort;

	int nRet = 0;
	switch (pDlg->m_nSortColumn) {
	case 0:
		nRet = stricmp(pEvt1->m_sComment.c_str(), pEvt2->m_sComment.c_str());
		break;
	case 1:
		nRet = pEvt1->GetType()-pEvt2->GetType();
		break;
	case 2:
		nRet = stricmp(pEvt1->GetNextTimeString().c_str(), pEvt2->GetNextTimeString().c_str());
		break;
	case 3:
		if (pEvt1->m_tLastActivated > pEvt2->m_tLastActivated)
			nRet = 1;
		else if (pEvt1->m_tLastActivated == pEvt2->m_tLastActivated)
			nRet = 0;
		else
			nRet = -1;
		break;
	case 4:
		nRet = pEvt2->m_bCritical - pEvt1->m_bCritical;
		break;
	}
	nRet *= pDlg->m_nSortDir;
	return nRet;
}

void CSchedView::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nSel = m_ctlList.GetSelectedCount();
	
	m_menu.EnableMenuItem(ID_SCHED_EDIT, MF_BYCOMMAND | ((nSel > 0) ? MF_ENABLED : MF_GRAYED));
	m_menu.EnableMenuItem(ID_SCHED_DELETE, MF_BYCOMMAND | ((nSel > 0) ? MF_ENABLED : MF_GRAYED));
	m_menu.EnableMenuItem(ID_SCHED_RUNNOW, MF_BYCOMMAND | ((nSel > 0) ? MF_ENABLED : MF_GRAYED));
	POINT pt;
	GetCursorPos(&pt);
	m_menu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN, pt.x, pt.y, this, NULL);
}

void CSchedView::DisableControls()
{
//	m_ctlList.EnableWindow(FALSE);
}

void CSchedView::EnableControls()
{
//	m_ctlList.EnableWindow();
}

void CSchedView::ClearList()
{
	int nMax = m_ctlList.GetItemCount();
	for (int i=0; i<nMax; i++)
	{
		RconEvent* pEvent = (RconEvent*)m_ctlList.GetItemData(i);
		if (pEvent != NULL)
			delete pEvent;
	}
	m_ctlList.DeleteAllItems();
}

void CSchedView::Timer()
{
	RconEvent::s_tNow = time(NULL);
	// run all events
	for (EVENTLIST::iterator i = m_lEvents.begin(); i != m_lEvents.end(); i++)
		(*i)->Run(settings.m_nRunLevel, m_pFrame->m_pMain);
	// update the statuses etc. in the list
	UpdateList();
}

void CSchedView::UpdateList()
{
	// update the statuses of all events
	int nCount = m_ctlList.GetItemCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		// get the event object
		RconEvent* pEvent = (RconEvent*)m_ctlList.GetItemData(nIndex);
		// set the subitem strings
		m_ctlList.SetItemText(nIndex, 0, pEvent->GetComment());
		m_ctlList.SetItemText(nIndex, 1, GetTypeString(pEvent->GetType()));
		m_ctlList.SetItemText(nIndex, 2, pEvent->GetNextTimeString().c_str());
		m_ctlList.SetItemText(nIndex, 3, pEvent->GetLastTimeString().c_str());
		m_ctlList.SetItemText(nIndex, 4, pEvent->m_bCritical ? "Yes" : "No");
	}
}

LPCTSTR CSchedView::GetTypeString(int nType)
{
	// simply convert type to string
	switch(nType)
	{
	case ID_INSERT_ONCE:
		return "Once";
	case ID_INSERT_MINUTE:
		return "Minute";
	case ID_INSERT_HOUR:
		return "Hour";
	case ID_INSERT_DAY:
		return "Day";
	case ID_INSERT_WEEK:
		return "Week";
	case ID_INSERT_MONTH:
		return "Month";
	}
	// How can this be? must be a MAJOR bug if the program gets here.
	return "** ERROR **";
}


void CSchedView::OnSchedDelete() 
{
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nSel = m_ctlList.GetNextSelectedItem(pos);
	if (nSel < 0)
		return;

	// get the event object
	RconEvent* pEvt = (RconEvent*)m_ctlList.GetItemData(nSel);
	// find it in the list and delete it and the reference to it
	for (EVENTLIST::iterator i = m_lEvents.begin(); i != m_lEvents.end(); i++)
	{
		if ((*i) == pEvt) {
			m_lEvents.erase(i);
			delete pEvt;
			FillList();
			return;
		}
	}
	// hmm... wasn't in the list? what kind of error is this anyway?
	AfxMessageBox("Error deleting event", MB_ICONEXCLAMATION);
}

void CSchedView::OnRunNow()
{
	// fetch the selected item index
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nSel = m_ctlList.GetNextSelectedItem(pos);
	if (nSel < 0)
		return;

	RconEvent::s_tNow = time(NULL);
	// get the event object
	RconEvent* pEvt = (RconEvent*)m_ctlList.GetItemData(nSel);
	// run it now
	pEvt->Activate(m_pFrame->m_pMain);
	pEvt->GetNextTime(true);
}

void CSchedView::OnSchedEdit() 
{
	// fetch the selected item index
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int nSel = m_ctlList.GetNextSelectedItem(pos);
	if (nSel < 0)
		return;

	// get the event object
	RconEvent* pEvt = (RconEvent*)m_ctlList.GetItemData(nSel);
	// edit it according to type

	// Empty() and Fill() are used to xfer info back and from the event to the dialog
	switch (pEvt->GetType()) {
	case ID_INSERT_ONCE:
		{
			RconEventOnce* pEvent = (RconEventOnce*)pEvt;
			CEventOnceDlg dlg;
			dlg.Empty(pEvent);

			if (dlg.DoModal() == IDOK) {
				dlg.Fill(pEvent);
				pEvent->m_bActivated = false;
				FillList();
			}
		}
		break;
	case ID_INSERT_MINUTE:
		{
			RconEventMinute* pEvent = (RconEventMinute*)pEvt;
			CEventMinuteDlg dlg;
			dlg.Empty(pEvent);

			if (dlg.DoModal() == IDOK) {
				dlg.Fill(pEvent);
				FillList();
			}
		}
		break;
	case ID_INSERT_HOUR:
		{
			RconEventHour* pEvent = (RconEventHour*)pEvt;
			CEventHourDlg dlg;
			dlg.Empty(pEvent);
			if (dlg.DoModal() == IDOK) {
				dlg.Fill(pEvent);
				FillList();
			}
		}
		break;
	case ID_INSERT_DAY:
		{
			RconEventDay* pEvent = (RconEventDay*)pEvt;
			CEventDayDlg dlg;
			dlg.Empty(pEvent);
			if (dlg.DoModal() == IDOK) {
				dlg.Fill(pEvent);
				FillList();
			}
		}
		break;
	default:
		break;
	}
}

void CSchedView::OnInsertOnce()
{
	RconEvent::s_tNow = time(NULL);
	RconEventOnce* pEvent = new RconEventOnce(this);
	pEvent->m_tTime = time(NULL);
	pEvent->m_bActivated = false;
	CEventOnceDlg dlg;
	dlg.Empty(pEvent);
	if (dlg.DoModal() == IDOK) {
		dlg.Fill(pEvent);
		pEvent->GetNextTime(true);
		m_lEvents.push_back(pEvent);
		FillList();
		m_ctlList.SetItemState(m_ctlList.GetItemCount()-1, LVIS_SELECTED, LVIS_SELECTED);
	} else
		delete pEvent;
}

void CSchedView::OnInsertMinute()
{
	// set the current time
	RconEvent::s_tNow = time(NULL);
	// create new event
	RconEventMinute* pEvent = new RconEventMinute(this);
	// init members
	pEvent->m_nAmount = 1;
	CEventMinuteDlg dlg;
	// run dialog
	dlg.Empty(pEvent);
	if (dlg.DoModal() == IDOK) {
		// fetch params
		dlg.Fill(pEvent);
		// add to list and to control
		pEvent->GetNextTime(true);
		m_lEvents.push_back(pEvent);
		FillList();
		m_ctlList.SetItemState(m_ctlList.GetItemCount()-1, LVIS_SELECTED, LVIS_SELECTED);
	} else
		// otherwise, dispose of the object
		delete pEvent;
}

void CSchedView::OnInsertHour()
{
	RconEvent::s_tNow = time(NULL);
	RconEventHour* pEvent = new RconEventHour(this);
	pEvent->m_nAmount = 1;
	pEvent->m_bOnTheHour = false;
	CEventHourDlg dlg;
	dlg.Empty(pEvent);
	if (dlg.DoModal() == IDOK) {
		dlg.Fill(pEvent);
		pEvent->GetNextTime(true);
		m_lEvents.push_back(pEvent);
		FillList();
		m_ctlList.SetItemState(m_ctlList.GetItemCount()-1, LVIS_SELECTED, LVIS_SELECTED);
	} else
		delete pEvent;
}

void CSchedView::OnInsertDay()
{
	RconEvent::s_tNow = time(NULL);
	RconEventDay* pEvent = new RconEventDay(this);
	pEvent->m_nAmount = 1;
	pEvent->m_tTime = time(NULL) % 86400;
	CEventDayDlg dlg;
	dlg.Empty(pEvent);
	if (dlg.DoModal() == IDOK) {
		dlg.Fill(pEvent);
		pEvent->GetNextTime(true);
		m_lEvents.push_back(pEvent);
		FillList();
		m_ctlList.SetItemState(m_ctlList.GetItemCount()-1, LVIS_SELECTED, LVIS_SELECTED);
	} else
		delete pEvent;
}

void CSchedView::OnInsertWeek()
{
}

void CSchedView::OnInsertMonth()
{
}

void CSchedView::OnWizardMatch()
{
}

void CSchedView::OnWizardLock()
{
}

void CSchedView::OnWizardUnlock()
{
}

void CSchedView::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//double-click = edit
	OnSchedEdit();
	*pResult = 0;
}

void CSchedView::FillList()
{
	int nIndex = -1;
	m_ctlList.DeleteAllItems();
	// add all events from the beginning
	for (EVENTLIST::iterator i = m_lEvents.begin(); i != m_lEvents.end(); i++)
	{
		nIndex = m_ctlList.InsertItem(m_ctlList.GetItemCount(), (*i)->GetComment());
		m_ctlList.SetItemData(nIndex, (DWORD)(*i));
	}
	// update statuses
	UpdateList();
}

void CSchedView::ReadSettings()
{
	CString sSection;
	int nMax = AfxGetApp()->GetProfileInt("RconSched", "Count", 0);
	for (int i = 0; i < nMax; i++)
	{
		sSection.Format("RconSched\\Event%d", i);
		int nType = AfxGetApp()->GetProfileInt(sSection , "Type", -1);
		switch (nType) {
		case ID_INSERT_ONCE:
			{
				RconEvent::s_tNow = time(NULL);
				RconEventOnce* pEvent = new RconEventOnce(this);
				if (pEvent->ReadFromSection(sSection)) {
					pEvent->GetNextTime(true);
					m_lEvents.push_back(pEvent);
				} else
					delete pEvent;
			}
			break;
		case ID_INSERT_MINUTE:
			{
				RconEvent::s_tNow = time(NULL);
				RconEventMinute* pEvent = new RconEventMinute(this);
				if (pEvent->ReadFromSection(sSection)) {
					pEvent->GetNextTime(true);
					m_lEvents.push_back(pEvent);
				} else
					delete pEvent;
			}
			break;
		case ID_INSERT_HOUR:
			{
				RconEvent::s_tNow = time(NULL);
				RconEventHour* pEvent = new RconEventHour(this);
				if (pEvent->ReadFromSection(sSection)) {
					pEvent->GetNextTime(true);
					m_lEvents.push_back(pEvent);
				} else
					delete pEvent;
			}
			break;
		case ID_INSERT_DAY:
			{
				RconEvent::s_tNow = time(NULL);
				RconEventDay* pEvent = new RconEventDay(this);
				if (pEvent->ReadFromSection(sSection)) {
					pEvent->GetNextTime(true);
					m_lEvents.push_back(pEvent);
				} else
					delete pEvent;
			}
			break;
		}
	}
	FillList();
}

void CSchedView::WriteSettings()
{
	CString sSection;
	AfxGetApp()->WriteProfileInt("RconSched", "Count", m_lEvents.size());
	int nItem = 0;
	RconEvent* pEvent = NULL;
	for (EVENTLIST::iterator i = m_lEvents.begin(); i != m_lEvents.end(); i++, nItem++)
	{
		sSection.Format("RconSched\\Event%d", nItem);
		pEvent = (*i);
		AfxGetApp()->WriteProfileInt(sSection , "Type", pEvent->GetType());
		pEvent->WriteToSection(sSection);
	}
}
