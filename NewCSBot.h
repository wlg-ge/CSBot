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

// NewCSBot.h : main header file for the NEWCSBOT application
//

#if !defined(AFX_NEWCSBOT_H__0E8047F1_8766_41D8_A6C7_F9845897C5DB__INCLUDED_)
#define AFX_NEWCSBOT_H__0E8047F1_8766_41D8_A6C7_F9845897C5DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp:
// See NewCSBot.cpp for the implementation of this class
//

class CCSBotApp : public CWinApp
{
public:
	CCSBotApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSBotApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;
	bool ParseCmdLine();

public:
	//{{AFX_MSG(CCSBotApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnHelpReport();
	afx_msg void OnHelpSite();
	afx_msg void OnHelpForums();
	afx_msg void OnHelpUnitedadmins();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCSBOT_H__0E8047F1_8766_41D8_A6C7_F9845897C5DB__INCLUDED_)
