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

#if !defined(AFX_EVENTDAYDLG_H__54DE9A91_54EE_45B6_901E_4CE39A3E798D__INCLUDED_)
#define AFX_EVENTDAYDLG_H__54DE9A91_54EE_45B6_901E_4CE39A3E798D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventDayDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventDayDlg dialog

class CEventDayDlg : public CDialog
{
// Construction
public:
	CEventDayDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventDayDlg)
	enum { IDD = IDD_INSERT_DAY };
	CSpinButtonCtrl	m_ctlSpin;
	CListBox	m_ctlList;
	CButton	m_btnEdit;
	CButton	m_btnDelete;
	CTime	m_tTime;
	CString	m_sComment;
	int		m_nAmount;
	BOOL	m_bCritical;
	//}}AFX_DATA

	STRLIST m_lCmds;

	void Fill(RconEventDay* pEvent);
	void Empty(RconEventDay* pEvent);
	void EnableButtons();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventDayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventDayDlg)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnSelchangeList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTDAYDLG_H__54DE9A91_54EE_45B6_901E_4CE39A3E798D__INCLUDED_)
