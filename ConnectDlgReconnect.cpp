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

// ConnectDlgReconnect.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "ConnectDlgReconnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgReconnect dialog


CConnectDlgReconnect::CConnectDlgReconnect(bool bIRC)
	: CPropertyPage(CConnectDlgReconnect::IDD)
{
	//{{AFX_DATA_INIT(CConnectDlgReconnect)
	m_bReconnect = TRUE;
	m_nReconnectSec = 10;
	m_nReconnectTimes = 30;
	m_bInactivity = TRUE;
	m_nInactivityTimeout = 300;
	//}}AFX_DATA_INIT
	m_bIRC = bIRC;
}

CConnectDlgReconnect::~CConnectDlgReconnect()
{
}

void CConnectDlgReconnect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlgReconnect)
	DDX_Check(pDX, IDC_RECONNECT, m_bReconnect);
	DDX_Text(pDX, IDC_RECONNECTSEC, m_nReconnectSec);
	DDV_MinMaxInt(pDX, m_nReconnectSec, 1, 1000);
	DDX_Text(pDX, IDC_RECONNECTTIMES, m_nReconnectTimes);
	DDV_MinMaxInt(pDX, m_nReconnectTimes, 1, 1000);
	DDX_Check(pDX, IDC_INACTIVITY, m_bInactivity);
	DDX_Text(pDX, IDC_INACTIVITYTIMEOUT, m_nInactivityTimeout);
	DDV_MinMaxInt(pDX, m_nInactivityTimeout, 10, 3600);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
	{
		OnReconnect();
		OnInactivity();
	}
}


BEGIN_MESSAGE_MAP(CConnectDlgReconnect, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectDlgReconnect)
	ON_BN_CLICKED(IDC_RECONNECT, OnReconnect)
	ON_BN_CLICKED(IDC_INACTIVITY, OnInactivity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgReconnect message handlers

void CConnectDlgReconnect::OnReconnect() 
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RECONNECT);
	if (pBtn->GetCheck())
	{
		GetDlgItem(IDC_RECONNECTLABEL1)->EnableWindow();
		GetDlgItem(IDC_RECONNECTLABEL2)->EnableWindow();
		GetDlgItem(IDC_RECONNECTLABEL3)->EnableWindow();
		GetDlgItem(IDC_RECONNECTSEC)->EnableWindow();
		GetDlgItem(IDC_RECONNECTTIMES)->EnableWindow();
	} else {
		GetDlgItem(IDC_RECONNECTLABEL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECONNECTLABEL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECONNECTLABEL3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECONNECTSEC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECONNECTTIMES)->EnableWindow(FALSE);
	}
}

void CConnectDlgReconnect::OnInactivity() 
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_INACTIVITY);
	if (pBtn->GetCheck())
	{
		GetDlgItem(IDC_INACTIVITYLABEL)->EnableWindow();
		GetDlgItem(IDC_INACTIVITYTIMEOUT)->EnableWindow();
	} else {
		GetDlgItem(IDC_INACTIVITYLABEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_INACTIVITYTIMEOUT)->EnableWindow(FALSE);
	}
}

BOOL CConnectDlgReconnect::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (m_bIRC)
	{
		GetDlgItem(IDC_INACTIVITY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INACTIVITYLABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INACTIVITYTIMEOUT)->ShowWindow(SW_HIDE);
	} else {
		GetDlgItem(IDC_INACTIVITY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_INACTIVITYLABEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_INACTIVITYTIMEOUT)->ShowWindow(SW_SHOW);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
