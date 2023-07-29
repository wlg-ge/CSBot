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

// NotifyUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "NotifyUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotifyUpdateDlg dialog


CNotifyUpdateDlg::CNotifyUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNotifyUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNotifyUpdateDlg)
	m_bNotifyUpdates = FALSE;
	m_sMsg = _T("");
	//}}AFX_DATA_INIT
}


void CNotifyUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotifyUpdateDlg)
	DDX_Check(pDX, IDC_NOTIFYUPDATES, m_bNotifyUpdates);
	DDX_Text(pDX, IDC_MSG, m_sMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotifyUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CNotifyUpdateDlg)
	ON_BN_CLICKED(IDC_WEBSITE, OnWebsite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotifyUpdateDlg message handlers


void CNotifyUpdateDlg::OnWebsite() 
{
	::ShellExecute(NULL, NULL, "http://www.csbot.org", NULL, NULL, SW_SHOW);	
}
