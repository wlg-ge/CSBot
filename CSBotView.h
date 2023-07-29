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

// CSBotView.h : interface of the CCSBotView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSBotView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
#define AFX_CSBotView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCSBotView window

class CChildFrame;

class CCSBotView : public CWnd
{
// Construction
public:
	CCSBotView(CChildFrame* pFrame);

// Attributes
public:

// Operations
public:
	virtual void EnableControls() {};
	virtual void DisableControls() {};
	virtual void EnableIRCControls() {};
	virtual void DisableIRCControls() {};

	virtual void ReadSettings() {};
	virtual void WriteSettings() {};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSBotView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCSBotView();
	CChildFrame* m_pFrame;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCSBotView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSBotView_H__2234C93F_49BC_4803_9DCE_F69A0C598CA0__INCLUDED_)
