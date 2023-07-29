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

// RconView.h : interface of the CRconView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RconView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
#define AFX_RconView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRconView window

#include "CSBotView.h"
#include "GHOutputWnd.h"

class CRconView : public CCSBotView
{
// Construction
public:
	CRconView(CChildFrame* pFrame);

// Attributes
public:

// Operations
public:
	void AddResponseLine(LPCTSTR pLine);
	void RconLogLines(LPCTSTR pLines);

	virtual void DisableControls();
	virtual void EnableControls();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRconView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRconView();
	CGHOutputWnd m_wndOutput;
	CComboBox m_ctlCommand;
	CButton m_ctlSend;

	HACCEL m_hAccel;
	HWND m_hEditbox;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRconView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnSend();
	afx_msg void OnEnter();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RconView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
