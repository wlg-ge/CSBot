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

#if !defined(AFX_OPTIONSIRCOUT_H__2EDE78DA_E139_4093_8E49_7F58576CC905__INCLUDED_)
#define AFX_OPTIONSIRCOUT_H__2EDE78DA_E139_4093_8E49_7F58576CC905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIRCOut.h : header file
//

#include "MainFrm.h"
#include "OptionsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCOut dialog

class COptionsIRCOut : public COptionsPage
{
	DECLARE_DYNCREATE(COptionsIRCOut)

// Construction
public:
	COptionsIRCOut();
	~COptionsIRCOut();

// Dialog Data
	//{{AFX_DATA(COptionsIRCOut)
	enum { IDD = IDD_OPTIONS_IRC_OUT };
	int		m_nLines;
	int		m_nSeconds;
	int		m_nIRCDelay;
	//}}AFX_DATA

public:
	virtual void ReadSettings();
	virtual void WriteSettings();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsIRCOut)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionsIRCOut)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIRCOUT_H__2EDE78DA_E139_4093_8E49_7F58576CC905__INCLUDED_)
