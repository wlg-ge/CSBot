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

#if !defined(AFX_CONNECTIRCGENERAL_H__91A31F6B_618F_4C90_9E37_819F10BA24D2__INCLUDED_)
#define AFX_CONNECTIRCGENERAL_H__91A31F6B_618F_4C90_9E37_819F10BA24D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectIRCGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCGeneral dialog

class CConnectIRCGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnectIRCGeneral)

// Construction
public:
	CConnectIRCGeneral();
	~CConnectIRCGeneral();

// Dialog Data
	//{{AFX_DATA(CConnectIRCGeneral)
	enum { IDD = IDD_CONNECT_IRC_GENERAL };
	CString	m_sChannelName;
	UINT	m_uPort;
	CString	m_sServerName;
	CString	m_sNickName;
	CString	m_sFullName;
	CString	m_sPassword;
	CString	m_sChannelPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectIRCGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectIRCGeneral)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIRCGENERAL_H__91A31F6B_618F_4C90_9E37_819F10BA24D2__INCLUDED_)
