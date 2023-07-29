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

#if !defined(AFX_CONNECTDLG_H__F3C612EA_EC18_43F4_9FAB_41580681C5BA__INCLUDED_)
#define AFX_CONNECTDLG_H__F3C612EA_EC18_43F4_9FAB_41580681C5BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog
#include "MainFrm.h"
#include "ConnectDlgGeneral.h"
#include "ConnectDlgCmds.h"
#include "ConnectDlgReconnect.h"

class CConnectDlg : public CDialog, public GameServerProfile
{
// Construction
public:
	CConnectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectDlg)
	enum { IDD = IDD_CONNECT_GAME };
	CListBox	m_ctlList;
	CStatic		m_ctlTabs;
	//}}AFX_DATA

	CPropertySheet m_sheet;
	CConnectDlgGeneral m_pageGeneral;
	CConnectDlgCmds m_pageConnect;
	CConnectDlgCmds m_pageDisconnect;
	CConnectDlgReconnect m_pageReconnect;

	void FillList();
	int InsertProfile(LPCTSTR sProfile, bool bAssign = false);
	int m_nCurrentItem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectDlg)
	afx_msg void OnAddprofile();
	afx_msg void OnRemoveprofile();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRenameprofile();
	afx_msg void OnSelchangeProfiles();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnDblclkProfiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDLG_H__F3C612EA_EC18_43F4_9FAB_41580681C5BA__INCLUDED_)
