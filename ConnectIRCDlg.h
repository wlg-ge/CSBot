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

#if !defined(AFX_CONNECTIRCDLG_H__B8578802_89C9_42AC_800A_67F86FE8C89C__INCLUDED_)
#define AFX_CONNECTIRCDLG_H__B8578802_89C9_42AC_800A_67F86FE8C89C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectIRCDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCDlg dialog
#include "MainFrm.h"
#include "ConnectIRCGeneral.h"
#include "ConnectIRCIdent.h"
#include "ConnectDlgCmds.h"
#include "ConnectDlgReconnect.h"

class CConnectIRCDlg : public CDialog, public IRCServerProfile
{
// Construction
public:
	CConnectIRCDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectIRCDlg)
	enum { IDD = IDD_CONNECTIRC };
	CStatic		m_ctlTabs;
	CListCtrl	m_ctlCmds;
	CListBox	m_ctlList;
	//}}AFX_DATA

	CPropertySheet m_sheet;
	CConnectIRCGeneral m_pageGeneral;
	CConnectIRCIdent m_pageIdent;
	CConnectDlgCmds m_pageConnect;
	CConnectDlgReconnect m_pageReconnect;

	void FillList();
	int InsertProfile(LPCTSTR sProfile, bool bAssign = false);
	int m_nCurrentItem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectIRCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectIRCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddircprofile();
	afx_msg void OnRemoveircprofile();
	afx_msg void OnRenameircprofile();
	afx_msg void OnSelchangeProfiles();
	afx_msg void OnDestroy();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnDblclkProfiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIRCDLG_H__B8578802_89C9_42AC_800A_67F86FE8C89C__INCLUDED_)
