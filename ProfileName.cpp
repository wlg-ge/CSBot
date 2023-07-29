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

// ProfileName.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "ProfileName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileName dialog


CProfileName::CProfileName(CWnd* pParent /*=NULL*/)
	: CDialog(CProfileName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProfileName)
	m_sName = _T("");
	//}}AFX_DATA_INIT
	m_sTitle = "Profile name";
}


void CProfileName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileName)
	DDX_Control(pDX, IDC_NAME, m_ctlEdit);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, 250);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileName, CDialog)
	//{{AFX_MSG_MAP(CProfileName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileName message handlers

BOOL CProfileName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlEdit.SetFocus();
	m_ctlEdit.SetSel(0, -1);

	SetWindowText(m_sTitle);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
