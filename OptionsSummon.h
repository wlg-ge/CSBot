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

#if !defined(AFX_OPTIONSSUMMON_H__C3064946_0D5F_4FA9_9FA5_F781221F09AB__INCLUDED_)
#define AFX_OPTIONSSUMMON_H__C3064946_0D5F_4FA9_9FA5_F781221F09AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSummon.h : header file
//

#include "OptionsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsSummon dialog

class COptionsSummon : public COptionsPage
{
	DECLARE_DYNCREATE(COptionsSummon)
// Construction
public:
	COptionsSummon();
	~COptionsSummon();

// Dialog Data
	//{{AFX_DATA(COptionsSummon)
	enum { IDD = IDD_OPTIONS_SUMMON };
	CButton	m_ctlEditLine;
	CButton	m_ctlDeleteLine;
	CListCtrl	m_ctlLineList;
	CButton	m_ctlPlayFile;
	BOOL	m_bAllowSummon;
	BOOL	m_bSummonFlash;
	BOOL	m_bSummonPlay;
	CString	m_sSummonWord;
	BOOL	m_bAllowSummonTeam;
	//}}AFX_DATA

	CStringList m_lLines;
	CString m_sSummonPlayFile;

public:
	virtual void ReadSettings();
	virtual void WriteSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSummon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSummon)
	afx_msg void OnAddline();
	afx_msg void OnDeleteline();
	afx_msg void OnEditline();
	afx_msg void OnDblclkLinelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedLinelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSummonPlayfile();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSUMMON_H__C3064946_0D5F_4FA9_9FA5_F781221F09AB__INCLUDED_)
