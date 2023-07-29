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

#if !defined(AFX_OPTIONSIRCIN_H__FB346A4F_9905_47A2_9A1A_AF8BE5A9D78A__INCLUDED_)
#define AFX_OPTIONSIRCIN_H__FB346A4F_9905_47A2_9A1A_AF8BE5A9D78A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIRCIn.h : header file
//

#include "MainFrm.h"
#include "OptionsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCIn dialog

class COptionsIRCIn : public COptionsPage, public CommandFlags
{
	DECLARE_DYNCREATE(COptionsIRCIn)

// Construction
public:
	COptionsIRCIn();
	~COptionsIRCIn();

// Dialog Data
	//{{AFX_DATA(COptionsIRCIn)
	enum { IDD = IDD_OPTIONS_IRC_IN };
	CListCtrl	m_ctlAllowCommands;
	CListCtrl	m_ctlTrust;
	BOOL	m_bAutoLogOff;
	int		m_nLogOffMinutes;
	BOOL	m_bPMFromIRC;
	BOOL	m_bDCCChatFromIRC;
	BOOL	m_bCMFromIRC;
	CString	m_sCMFromIRCPrefix;
	//}}AFX_DATA

	PERSONLIST m_lTrusted;
	bool m_bUsersChanged;

	void FillList();

public:
	virtual void ReadSettings();
	virtual void WriteSettings();

	void EnableControls();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsIRCIn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsIRCIn)
	virtual BOOL OnInitDialog();
	afx_msg void OnTrustedit();
	afx_msg void OnTrustadd();
	afx_msg void OnTrustdelete();
	afx_msg void OnItemchangedTrustlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTrustlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIrcrconlogin();
	afx_msg void OnModified();
	afx_msg void OnItemchangedAllowcommands(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIRCCM();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIRCIN_H__FB346A4F_9905_47A2_9A1A_AF8BE5A9D78A__INCLUDED_)
