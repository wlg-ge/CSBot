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

#if !defined(AFX_OPTIONSIRCMSGS_H__E5C4FA26_5CDF_4536_94D9_43CFEFC3A825__INCLUDED_)
#define AFX_OPTIONSIRCMSGS_H__E5C4FA26_5CDF_4536_94D9_43CFEFC3A825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIRCMsgs.h : header file
//

#include "OptionsIRCMsgsColors.h"
#include "OptionsIRCMsgsMsgs.h"
#include "GIRLLogParser.h"
#include "Settings.h"
#include "OptionsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgs dialog

struct TreeItemData;
class GIRLLogParser;
typedef std::list<IRCCustomMsg> IRCMSGLIST;
typedef std::map<GIRLLogParser*, IRCMSGLIST> PARSER2MSGS;
typedef std::map<GIRLLogParser*, ColorInfo> PARSER2COLORS;
typedef std::list<TreeItemData*> TIDLIST;

class COptionsIRCMsgs : public COptionsPage
{
// Construction
public:
	COptionsIRCMsgs();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsIRCMsgs)
	enum { IDD = IDD_OPTIONS_IRC_MSGS };
	CTreeCtrl	m_ctlTree;
	//}}AFX_DATA

	COptionsIRCMsgsMsgs m_pageMsgs;
	COptionsIRCMsgsColors m_pageColors;
	CDialog* m_pCurrentDialog;

	PARSER2MSGS m_mMsgs;
	PARSER2COLORS m_mColors;

	CImageList m_imageList;
	TIDLIST m_lTIDs;

public:
	virtual void ReadSettings();
	virtual void WriteSettings();

	void ShowList(GIRLLogParser* pParser, MsgType msgType = (MsgType)-1);
	void ShowColors(GIRLLogParser* pParser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsIRCMsgs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsIRCMsgs)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIRCMSGS_H__E5C4FA26_5CDF_4536_94D9_43CFEFC3A825__INCLUDED_)
