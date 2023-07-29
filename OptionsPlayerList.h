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

#if !defined(AFX_OPTIONSPLAYERLIST_H__6172D8F3_2581_4BB2_9B69_6E7E3833BB6E__INCLUDED_)
#define AFX_OPTIONSPLAYERLIST_H__6172D8F3_2581_4BB2_9B69_6E7E3833BB6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPlayerList.h : header file
//

#include "settings.h"
#include "OptionsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsPlayerList dialog

class COptionsPlayerList : public COptionsPage
{
	DECLARE_DYNCREATE(COptionsPlayerList)

// Construction
public:
	COptionsPlayerList();
	~COptionsPlayerList();

// Dialog Data
	//{{AFX_DATA(COptionsPlayerList)
	enum { IDD = IDD_OPTIONS_PLAYERLIST };
	CListCtrl	m_ctlCmds;
	//}}AFX_DATA

	PLAYERLISTCMDLIST m_lCmds;

	virtual void ReadSettings();
	virtual void WriteSettings();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsPlayerList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsPlayerList)
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnMoveup();
	afx_msg void OnMovedown();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkCommands(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedCommands(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownCommands(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void Clear();
	void EnableDisable();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPLAYERLIST_H__6172D8F3_2581_4BB2_9B69_6E7E3833BB6E__INCLUDED_)
