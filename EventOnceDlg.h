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

#if !defined(AFX_EVENTOnceDLG_H__E0C95ACF_347E_4454_8894_4E8ADD10DAC4__INCLUDED_)
#define AFX_EVENTOnceDLG_H__E0C95ACF_347E_4454_8894_4E8ADD10DAC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventOnceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventOnceDlg dialog

class RconEventOnce;

class CEventOnceDlg : public CDialog
{
// Construction
public:
	CEventOnceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventOnceDlg)
	enum { IDD = IDD_INSERT_ONCE };
	CListBox	m_ctlList;
	CButton	m_btnDelete;
	CButton	m_btnEdit;
	CTime	m_tTime;
	CTime	m_tDate;
	CString	m_sComment;
	BOOL	m_bCritical;
	//}}AFX_DATA

	STRLIST m_lCmds;

	void Fill(RconEventOnce* pEvent);
	void Empty(RconEventOnce* pEvent);
	void EnableButtons();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventOnceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventOnceDlg)
	afx_msg void OnSelchangeList();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTOnceDLG_H__E0C95ACF_347E_4454_8894_4E8ADD10DAC4__INCLUDED_)
