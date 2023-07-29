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

#if !defined(AFX_IRCCOLORDLG_H__1A14E7FB_59F1_40E6_8C6B_BA7636116A6C__INCLUDED_)
#define AFX_IRCCOLORDLG_H__1A14E7FB_59F1_40E6_8C6B_BA7636116A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IRCColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIRCColorDlg dialog

class CIRCColorDlg : public CDialog
{
// Construction
public:
	CIRCColorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIRCColorDlg)
	enum { IDD = IDD_IRC_COLOR };
	CListBox	m_ctlFore;
	CListBox	m_ctlBack;
	//}}AFX_DATA

	int m_nFore;
	int m_nBack;

	static void DrawSample(HDC hDC, int nBack, int nFore, CWnd* pWnd);
	static void DrawListItemFore(HDC hDC, int nColor, LPCTSTR pText, CRect rect, CWnd* pWnd, int nState);
	static void DrawListItemBack(HDC hDC, int nColor, LPCTSTR pText, CRect rect, CWnd* pWnd, int nState);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIRCColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIRCColorDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBackground();
	afx_msg void OnSelchangeForeground();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IRCCOLORDLG_H__1A14E7FB_59F1_40E6_8C6B_BA7636116A6C__INCLUDED_)
