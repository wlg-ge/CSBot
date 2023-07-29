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

#if !defined(AFX_OPTIONSIRCMSGSMSGS_H__5B82296D_7607_488B_B444_2A913881686A__INCLUDED_)
#define AFX_OPTIONSIRCMSGSMSGS_H__5B82296D_7607_488B_B444_2A913881686A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIRCMsgsMsgs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsMsgs dialog

class GIRLLogParser;
class COptionsPage;

class COptionsIRCMsgsMsgs : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsIRCMsgsMsgs)

// Construction
public:
	COptionsIRCMsgsMsgs();
	~COptionsIRCMsgsMsgs();

	COptionsPage* m_pPage;

// Dialog Data
	//{{AFX_DATA(COptionsIRCMsgsMsgs)
	enum { IDD = IDD_OPTIONS_IRC_MSGS_MSGS };
	CListCtrl	m_ctlList;
	//}}AFX_DATA

	bool m_bSoft;
	GIRLLogParser* m_pParser;
	void Create(CWnd* pParent) {CPropertyPage::Create(IDD, pParent);};

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsIRCMsgsMsgs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsIRCMsgsMsgs)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIRCMSGSMSGS_H__5B82296D_7607_488B_B444_2A913881686A__INCLUDED_)
