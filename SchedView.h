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

#if !defined(AFX_SCHEDVIEW_H__FE0E45E1_05A1_43B5_AC38_1054D140F332__INCLUDED_)
#define AFX_SCHEDVIEW_H__FE0E45E1_05A1_43B5_AC38_1054D140F332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchedView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedView window
#include "CSBotView.h"
#include "RconEvent.h"
#include <list>

typedef std::list<RconEvent*> EVENTLIST;

class CSchedView : public CCSBotView
{
// Construction
public:
	CSchedView(CChildFrame* pFrame);

// Attributes
public:
	CListCtrl		m_ctlList;
	CMenu			m_menu;
	int				m_nSortColumn;
	int				m_nSortDir;

	EVENTLIST		m_lEvents;

// Operations
public:
	void Timer();
	void ClearList();
	virtual void DisableControls();
	virtual void EnableControls();
	static int CALLBACK CompareEvents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	LPCTSTR GetTypeString(int nType);
	void FillList();
	void UpdateList();
	bool SaveEvents(LPCTSTR pFilename);
	bool LoadEvents(LPCTSTR pFilename, bool bMsgs = true);
	void ReadSettings();
	void WriteSettings();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSchedView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSchedView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSchedDelete();
	afx_msg void OnSchedEdit();
	afx_msg void OnRunNow();
	//}}AFX_MSG
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnInsertMinute();
	afx_msg void OnInsertHour();
	afx_msg void OnInsertDay();
	afx_msg void OnInsertWeek();
	afx_msg void OnInsertMonth();
	afx_msg void OnInsertOnce();
	afx_msg void OnWizardMatch();
	afx_msg void OnWizardLock();
	afx_msg void OnWizardUnlock();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDVIEW_H__FE0E45E1_05A1_43B5_AC38_1054D140F332__INCLUDED_)
