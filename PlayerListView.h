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

// PlayerListView.h : interface of the CPlayerListView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlayerListView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
#define AFX_PlayerListView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPlayerListView window

#include "CSBotView.h"
#include "GIRLLogParser.h"
#include "IGCLockable.h"

class CPlayerListView : public CCSBotView
{
// Construction
public:
	CPlayerListView(CChildFrame* pFrame);

// Attributes
public:
	CMenu			m_PlayerMenu;
	CMenu			m_BanMenu;
	CMenu			m_PlayerListMenu;
	int				m_nSortColumn;
	int				m_nSortDirection;
	UINT			m_uDefaultPlayerMenuItems;

// Operations
public:
	void	ClearPlayerList();
	void	UpdatePlayerTime();
	void	UpdatePlayer(PlayerItemData* pData);
	int		FindPlayer(PlayerItemData* pData);
	int		FindPlayer(int nID);
	void	UpdateFrags(int nID, int nNewFrags);
	void	UpdateDeaths(int nID, int nNewDeaths);
	void	PlayerDisconnected(int nID);
	void	RebuildPlayerMenu();

	virtual void DisableControls();
	virtual void EnableControls();
	static int CALLBACK ComparePlayers(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void PlayerBan(int nMin);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayerListView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlayerListView();
	CListCtrl m_ctlList;

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlayerListView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void PlayerBan5Min();
	afx_msg void PlayerBan30Min();
	afx_msg void PlayerBan3Day();
	afx_msg void PlayerBan2Hrs();
	afx_msg void PlayerBan5Hrs();
	afx_msg void PlayerForever();
	afx_msg void PlayerCustom();
	afx_msg void ForceRefresh();
	afx_msg void OnColumnclickPlayerlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUserCommand(UINT nID);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PlayerListView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
