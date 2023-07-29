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

// ChildView.cpp : implementation of the CPlayerListView class
//

#include "stdafx.h"
#include "NewCSBot.h"
#include "PlayerListView.h"
#include "ChildFrm.h"
#include "CustomBanDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayerListView

#define IDC_PLAYERLIST	201
#define USER_CMD_BASE	3000

CPlayerListView::CPlayerListView(CChildFrame* pFrame) : CCSBotView(pFrame)
{
	m_nSortDirection = 1;
	m_nSortColumn = -1;
}

CPlayerListView::~CPlayerListView()
{
}


BEGIN_MESSAGE_MAP(CPlayerListView,CWnd )
	//{{AFX_MSG_MAP(CPlayerListView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_RCLICK, IDC_PLAYERLIST, OnRclickList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PLAYERLIST, OnColumnclickPlayerlist)
	ON_COMMAND(IDC_PLAYER_BAN_5MIN, PlayerBan5Min)
	ON_COMMAND(IDC_PLAYER_BAN_30MIN, PlayerBan30Min)
	ON_COMMAND(IDC_PLAYER_BAN_2HRS, PlayerBan2Hrs)
	ON_COMMAND(IDC_PLAYER_BAN_5HRS, PlayerBan5Hrs)
	ON_COMMAND(IDC_PLAYER_BAN_3DAY, PlayerBan3Day)
	ON_COMMAND(IDC_PLAYER_BAN_FOREVER, PlayerForever)
	ON_COMMAND(IDC_PLAYER_BAN_CUSTOM, PlayerCustom)
	ON_COMMAND(IDC_FORCE_REFRESH, ForceRefresh)
	ON_COMMAND_RANGE(USER_CMD_BASE, USER_CMD_BASE+MAX_PLAYERLIST_COMMANDS, OnUserCommand)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlayerListView message handlers

BOOL CPlayerListView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;
	
	if (!m_ctlList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS, CRect(0,0,10,10), this, IDC_PLAYERLIST))
		return FALSE;

	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	m_ctlList.SetFont(&font, FALSE);

	m_ctlList.SetExtendedStyle(m_ctlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_ctlList.InsertColumn(0, "ID", LVCFMT_RIGHT, 45, 0);
	m_ctlList.InsertColumn(1, "Name", LVCFMT_LEFT, 180, 1);
	m_ctlList.InsertColumn(2, "Team", LVCFMT_LEFT, 50, 2);
	m_ctlList.InsertColumn(3, "WonID", LVCFMT_RIGHT, 75, 3);
	m_ctlList.InsertColumn(4, "Frags", LVCFMT_RIGHT, 50, 4);
	m_ctlList.InsertColumn(5, "Deaths", LVCFMT_RIGHT, 50, 5);
	m_ctlList.InsertColumn(6, "Time", LVCFMT_RIGHT, 45, 6);
	m_ctlList.InsertColumn(7, "Role", LVCFMT_LEFT, 55, 7);

	m_BanMenu.CreatePopupMenu();
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_5MIN, "5 Minutes");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_30MIN, "30 Minutes");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_2HRS, "2 Hours");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_5HRS, "5 Hours");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_3DAY, "3 Days");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_FOREVER, "Forever!");
	m_BanMenu.AppendMenu(MF_STRING, IDC_PLAYER_BAN_CUSTOM, "Custom ban...");

	m_PlayerMenu.CreatePopupMenu();
	m_PlayerMenu.AppendMenu(MF_STRING, IDC_FORCE_REFRESH, "Force refresh");
	m_PlayerMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT)m_BanMenu.GetSafeHmenu(), "&Ban!");
	m_PlayerMenu.AppendMenu(MF_SEPARATOR);
	m_uDefaultPlayerMenuItems = m_PlayerMenu.GetMenuItemCount();

	m_PlayerListMenu.CreatePopupMenu();
	m_PlayerListMenu.AppendMenu(MF_STRING, IDC_FORCE_REFRESH, "Force refresh");

	SetWindowText("Player list");
	return TRUE;
}

void CPlayerListView::OnSize(UINT nType, int cx, int cy) 
{
	CCSBotView::OnSize(nType, cx, cy);
	if (m_ctlList.GetSafeHwnd() != NULL)
		m_ctlList.MoveWindow(0, 0, cx, cy);
}

void CPlayerListView::ClearPlayerList() 
{
	m_ctlList.DeleteAllItems();
}

void CPlayerListView::UpdatePlayerTime()
{
	int nMax = m_ctlList.GetItemCount();
	for (int i=0; i<nMax; i++)
	{
		PlayerItemData* pData = (PlayerItemData*)m_ctlList.GetItemData(i);
		if (pData != NULL)
		{
			CString s;
			m_pFrame->m_pMain->FormatTime(pData->time, s);
			m_ctlList.SetItemText(i, 6, s);
		}
	}
}

void CPlayerListView::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();

	CPoint pt;
	::GetCursorPos(&pt);
	if (pos != NULL) {
		RebuildPlayerMenu();
		m_PlayerMenu.TrackPopupMenu(TPM_CENTERALIGN, pt.x, pt.y, this);
	} else {
		m_PlayerListMenu.TrackPopupMenu(TPM_CENTERALIGN, pt.x, pt.y, this);
	}
}

void CPlayerListView::PlayerBan5Min()
{
	PlayerBan(5);
}

void CPlayerListView::PlayerBan30Min()
{
	PlayerBan(30);
}

void CPlayerListView::PlayerBan2Hrs()
{
	PlayerBan(120);
}

void CPlayerListView::PlayerBan5Hrs()
{
	PlayerBan(300);
}

void CPlayerListView::PlayerBan3Day()
{
	PlayerBan(4320);
}

void CPlayerListView::PlayerForever()
{
	PlayerBan(0);
}

void CPlayerListView::PlayerCustom()
{
	CCustomBanDlg dlg;
	if (dlg.DoModal() == IDOK)
		PlayerBan(dlg.m_nMinutes);
}

void CPlayerListView::ForceRefresh()
{
	m_pFrame->m_pMain->FetchPlayerList();
}

void CPlayerListView::UpdatePlayer(PlayerItemData* pData)
{
	int nItem = FindPlayer(pData);
	if (nItem == -1) {
		nItem = m_ctlList.InsertItem(m_ctlList.GetItemCount(), "");
		m_ctlList.SetItemData(nItem, (DWORD)pData);
	}

	CString s;

	s.Format("%d", pData->nFrags);
	m_ctlList.SetItemText(nItem, 4, s);

	s.Format("%d", pData->nDeaths);
	m_ctlList.SetItemText(nItem, 5, s);

	m_pFrame->m_pMain->FormatTime(pData->time, s);
	m_ctlList.SetItemText(nItem, 6, (LPCTSTR)s);

	s.Format("%u", pData->uID);
	m_ctlList.SetItemText(nItem, 0, s);
	m_ctlList.SetItemText(nItem, 1, pData->sName);
	CString sTeam;
	if (m_pFrame->m_pMain->m_pParser != NULL)
		sTeam = m_pFrame->m_pMain->m_pParser->TeamName(pData->team);
	m_ctlList.SetItemText(nItem, 2, sTeam);
	m_ctlList.SetItemText(nItem, 3, pData->sWonID);
	if (pData->sRole == "N/A")
		m_ctlList.SetItemText(nItem, 7, "");
	else if (pData->sRole != "")
		m_ctlList.SetItemText(nItem, 7, pData->sRole);
}

int CPlayerListView::FindPlayer(int nID)
{
	PlayerItemData* pData;
	int nCount = m_ctlList.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		pData = (PlayerItemData*)m_ctlList.GetItemData(i);
		if (pData != NULL)
		{
			if (pData->uID == (UINT)nID)
			{
				return i;
			}
		}
	}
	return -1;
}

int CPlayerListView::FindPlayer(PlayerItemData* pData)
{
	LVFINDINFO lvfi;
	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (DWORD)pData;
	return m_ctlList.FindItem(&lvfi);
}

void CPlayerListView::UpdateFrags(int nID, int nNewFrags)
{
	int nKiller = FindPlayer(nID);
	if (nKiller >= 0) {
		CString s;
		s.Format("%d", nNewFrags);
		m_ctlList.SetItemText(nKiller, 4, s);
	}
}

void CPlayerListView::UpdateDeaths(int nID, int nNewDeaths)
{
	int nVictim = FindPlayer(nID);
	if (nVictim >= 0) {
		CString s;
		s.Format("%d", nNewDeaths);
		m_ctlList.SetItemText(nVictim, 5, s);
	}
}

void CPlayerListView::PlayerDisconnected(int nID)
{
	int nPlayer = FindPlayer(nID);
	if (nPlayer >= 0)
		m_ctlList.DeleteItem(nPlayer);
}

void CPlayerListView::DisableControls()
{
	ClearPlayerList();
	m_ctlList.EnableWindow(FALSE);
}

void CPlayerListView::EnableControls()
{
	m_ctlList.EnableWindow();
}

void CPlayerListView::OnColumnclickPlayerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nColumn = pNMListView->iSubItem;
	if (m_nSortColumn == nColumn)
		m_nSortDirection = -m_nSortDirection;
	else
		m_nSortDirection = -1;

	m_nSortColumn = nColumn;
	m_ctlList.SortItems(&ComparePlayers, (DWORD)this);
	
	*pResult = 0;
}

int CALLBACK CPlayerListView::ComparePlayers(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CPlayerListView* pThis = (CPlayerListView*)lParamSort;

	int nItem1 = pThis->FindPlayer(((PlayerItemData*)lParam1)->uID);
	int nItem2 = pThis->FindPlayer(((PlayerItemData*)lParam2)->uID);
	switch (pThis->m_nSortColumn)
	{
	case 0:
	case 3:
	case 4:
	case 5:
		{
			int nVal1 = atol(pThis->m_ctlList.GetItemText(nItem1, pThis->m_nSortColumn));
			int nVal2 = atol(pThis->m_ctlList.GetItemText(nItem2, pThis->m_nSortColumn));
			return (nVal2-nVal1)*pThis->m_nSortDirection;
		}
	case 1:
	case 2:
	case 6:
	case 7:
		{
			CString sVal1 = pThis->m_ctlList.GetItemText(nItem1, pThis->m_nSortColumn);
			CString sVal2 = pThis->m_ctlList.GetItemText(nItem2, pThis->m_nSortColumn);
			return (sVal2.CompareNoCase(sVal1))*pThis->m_nSortDirection;
		}
	}
	return 0;
}

void CPlayerListView::OnDestroy() 
{
	m_PlayerMenu.DestroyMenu();
	m_BanMenu.DestroyMenu();
	m_PlayerListMenu.DestroyMenu();

	CWnd ::OnDestroy();
}

void CPlayerListView::PlayerBan(int nMin)
{
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_ctlList.GetNextSelectedItem(pos);

		PlayerItemData* pData = (PlayerItemData*)m_ctlList.GetItemData(nItem);
		if (pData != NULL) {
			CString sCommand;
			sCommand.Format("banid %d %s kick", nMin, (LPCTSTR)pData->sWonID);
			m_pFrame->m_pMain->IssueRcon(sCommand);
		}
	}
}

void CPlayerListView::RebuildPlayerMenu()
{
	// get rid of old stuff
	while (m_PlayerMenu.GetMenuItemCount() > m_uDefaultPlayerMenuItems)
		m_PlayerMenu.RemoveMenu(m_uDefaultPlayerMenuItems, MF_BYPOSITION);
		
	// add new stuff
	int nCmd = USER_CMD_BASE;
	for (PLAYERLISTCMDLIST::iterator i = settings.m_lPlayerListCmds.begin(); i != settings.m_lPlayerListCmds.end(); i++)
	{
		m_PlayerMenu.AppendMenu(MF_STRING, nCmd, (*i)->m_sName);
		nCmd++;
	}
}

void CPlayerListView::OnUserCommand(UINT nID)
{
	POSITION pos = m_ctlList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_ctlList.GetNextSelectedItem(pos);

		PlayerItemData* pData = (PlayerItemData*)m_ctlList.GetItemData(nItem);
		if (pData == NULL)
			continue;

		// find the command that the user wants
		int nCmd = nID - USER_CMD_BASE;
		PLAYERLISTCMDLIST::iterator i = settings.m_lPlayerListCmds.begin();
		while ((i != settings.m_lPlayerListCmds.end()) && (nCmd > 0))
		{
			nCmd--;
			i++;
		}

		CString sCommand = (*i)->m_sCommand;
		CString s;

		s.Format("%u", pData->uID);
		sCommand.Replace("<id>", s);

		s.Format("%u", pData->sWonID);
		sCommand.Replace("<wonid>", s);

		sCommand.Replace("<name>", pData->sName);

		m_pFrame->m_pMain->IssueRcon(sCommand);
	}
}

