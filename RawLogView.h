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

// RawLogView.h : interface of the CRawLogView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RawLogView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
#define AFX_RawLogView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRawLogView window

#include "CSBotView.h"
#include "GHOutputWnd.h"

class CRawLogView : public CCSBotView
{
// Construction
public:
	CRawLogView(CChildFrame* pFrame);

// Attributes
public:

// Operations
public:
	virtual void EnableControls();
	void AddLine(CString& sLine);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRawLogView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRawLogView();
	CGHOutputWnd m_wndOutput;

	void SetReconnect(int nSec, int nTimes);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRawLogView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RawLogView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
