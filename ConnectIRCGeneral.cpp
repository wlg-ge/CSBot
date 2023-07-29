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

// ConnectIRCGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "ConnectIRCGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCGeneral property page

IMPLEMENT_DYNCREATE(CConnectIRCGeneral, CPropertyPage)

CConnectIRCGeneral::CConnectIRCGeneral() : CPropertyPage(CConnectIRCGeneral::IDD)
{
	//{{AFX_DATA_INIT(CConnectIRCGeneral)
	m_sChannelName = _T("");
	m_uPort = 0;
	m_sServerName = _T("");
	m_sNickName = _T("");
	m_sFullName = _T("");
	m_sPassword = _T("");
	m_sChannelPassword = _T("");
	//}}AFX_DATA_INIT
}

CConnectIRCGeneral::~CConnectIRCGeneral()
{
}

void CConnectIRCGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectIRCGeneral)
	DDX_Text(pDX, IDC_CHANNELNAME, m_sChannelName);
	DDV_MaxChars(pDX, m_sChannelName, 100);
	DDX_Text(pDX, IDC_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 1024, 65535);
	DDX_Text(pDX, IDC_SERVERNAME, m_sServerName);
	DDV_MaxChars(pDX, m_sServerName, 100);
	DDX_Text(pDX, IDC_NICKNAME, m_sNickName);
	DDV_MaxChars(pDX, m_sNickName, 100);
	DDX_Text(pDX, IDC_FULLNAME, m_sFullName);
	DDV_MaxChars(pDX, m_sFullName, 100);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDV_MaxChars(pDX, m_sPassword, 100);
	DDX_Text(pDX, IDC_CHANNELPASSWORD, m_sChannelPassword);
	DDV_MaxChars(pDX, m_sChannelPassword, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectIRCGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectIRCGeneral)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCGeneral message handlers
