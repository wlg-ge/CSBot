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

#if !defined(AFX_NOTIFYUPDATEDLG_H__62B4315B_6E31_49A8_9A24_81811C6F0F70__INCLUDED_)
#define AFX_NOTIFYUPDATEDLG_H__62B4315B_6E31_49A8_9A24_81811C6F0F70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotifyUpdateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotifyUpdateDlg dialog

class CNotifyUpdateDlg : public CDialog
{
// Construction
public:
	CNotifyUpdateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNotifyUpdateDlg)
	enum { IDD = IDD_UPDATENOTIFY };
	BOOL	m_bNotifyUpdates;
	CString	m_sMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifyUpdateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNotifyUpdateDlg)
	afx_msg void OnWebsite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTIFYUPDATEDLG_H__62B4315B_6E31_49A8_9A24_81811C6F0F70__INCLUDED_)
