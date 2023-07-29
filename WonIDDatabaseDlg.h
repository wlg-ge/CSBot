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

#if !defined(AFX_WONIDDATABASEDLG_H__81B5F404_8054_4FD1_A0C3_B3CF2E8B5F56__INCLUDED_)
#define AFX_WONIDDATABASEDLG_H__81B5F404_8054_4FD1_A0C3_B3CF2E8B5F56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WonIDDatabaseDlg.h : header file
//

#include "WonIDDBManager.h"

/////////////////////////////////////////////////////////////////////////////
// CWonIDDatabaseDlg dialog

class CMainFrame;

class CWonIDDatabaseDlg : public CDialog, public WonIDNameCallback
{
// Construction
public:
	CWonIDDatabaseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWonIDDatabaseDlg)
	enum { IDD = IDD_WONIDDLG };
	CButton	m_ctlCase;
	CEdit	m_ctlFindName;
	CEdit	m_ctlNewName;
	CEdit	m_ctlNewWonID;
	CListBox	m_ctlList;
	CComboBox	m_ctlWonID;
	//}}AFX_DATA

	CMainFrame* m_pMain;
	virtual bool FoundName(LPCTSTR pName, DWORD dwWonID, LPARAM lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWonIDDatabaseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillWonIDList();
	void FillNameList(int nItem);
	void SelectItem(DWORD dwWonID, LPCTSTR pName);

	DWORD m_dwSelID;
	CString m_sName;

	// Generated message map functions
	//{{AFX_MSG(CWonIDDatabaseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWonid();
	afx_msg void OnRefresh();
	afx_msg void OnAddwonid();
	afx_msg void OnAddname();
	afx_msg void OnDeletename();
	afx_msg void OnFind();
	afx_msg void OnExport();
	afx_msg void OnImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WONIDDATABASEDLG_H__81B5F404_8054_4FD1_A0C3_B3CF2E8B5F56__INCLUDED_)
