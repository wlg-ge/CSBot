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

#if !defined(AFX_CONNECTDLGRECONNECT_H__0107B0D9_9A78_4E2F_89C2_8DACFC951E88__INCLUDED_)
#define AFX_CONNECTDLGRECONNECT_H__0107B0D9_9A78_4E2F_89C2_8DACFC951E88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectDlgReconnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgReconnect dialog

class CConnectDlgReconnect : public CPropertyPage
{
// Construction
public:
	CConnectDlgReconnect(bool bIRC);
	~CConnectDlgReconnect();

// Dialog Data
	//{{AFX_DATA(CConnectDlgReconnect)
	enum { IDD = IDD_CONNECT_RECONNECT };
	BOOL	m_bReconnect;
	int		m_nReconnectSec;
	int		m_nReconnectTimes;
	BOOL	m_bInactivity;
	int		m_nInactivityTimeout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectDlgReconnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bIRC;

	// Generated message map functions
	//{{AFX_MSG(CConnectDlgReconnect)
	afx_msg void OnReconnect();
	afx_msg void OnInactivity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDLGRECONNECT_H__0107B0D9_9A78_4E2F_89C2_8DACFC951E88__INCLUDED_)
