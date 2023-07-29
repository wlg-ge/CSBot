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

#if !defined(AFX_OPTIONSSHEET_H__40CC62F5_1D63_4C03_9332_3C9BBE9297A8__INCLUDED_)
#define AFX_OPTIONSSHEET_H__40CC62F5_1D63_4C03_9332_3C9BBE9297A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet

class COptionsSheet;
class COptionsPage;
class COptionsGeneral;
class COptionsIRCIn;
class COptionsIRCOut;
class COptionsSummon;
class COptionsIRCMsgs;
class COptionsPlayerList;

class COptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsSheet)

// Construction
public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	COptionsGeneral*	m_pageGeneral;
	COptionsIRCIn*		m_pageIRCIn;
	COptionsIRCOut*		m_pageIRCOut;
	COptionsSummon*		m_pageSummon;
	COptionsIRCMsgs*	m_pageMsgs;
	COptionsPlayerList*	m_pagePlayerList;

// Operations
public:
	void AddPage(COptionsPage *pPage);
	void AddPages();
	void ReadSettings();
	void WriteSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSHEET_H__40CC62F5_1D63_4C03_9332_3C9BBE9297A8__INCLUDED_)
