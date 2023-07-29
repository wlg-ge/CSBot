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

#if !defined(AFX_CONNECTDLGGENERAL_H__94B66B18_DAB0_4433_B845_C93514E88BD7__INCLUDED_)
#define AFX_CONNECTDLGGENERAL_H__94B66B18_DAB0_4433_B845_C93514E88BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// connectdlggeneral.h : header file
//

#include "Settings.h"
/////////////////////////////////////////////////////////////////////////////
// CConnectDlgGeneral dialog

class CConnectDlgGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnectDlgGeneral)

// Construction
public:
	CConnectDlgGeneral();
	~CConnectDlgGeneral();

// Dialog Data
	//{{AFX_DATA(CConnectDlgGeneral)
	enum { IDD = IDD_CONNECT_GAME_GENERAL };
	CComboBox	m_ctlLogQueryType;
	CIPAddressCtrl	m_ctlLocalIP;
	UINT	m_uPort;
	CString	m_sRcon;
	CString	m_sServerName;
	UINT	m_uLogPort;
	//}}AFX_DATA
	DWORD m_dwLocalIP;
	LogQueryType m_eType;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectDlgGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectDlgGeneral)
	afx_msg void OnGuess();
	afx_msg void OnGuessharder();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLogquerytype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDLGGENERAL_H__94B66B18_DAB0_4433_B845_C93514E88BD7__INCLUDED_)
