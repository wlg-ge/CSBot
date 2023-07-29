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

// connectdlggeneral.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "connectdlggeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgGeneral property page

IMPLEMENT_DYNCREATE(CConnectDlgGeneral, CPropertyPage)

CConnectDlgGeneral::CConnectDlgGeneral() : CPropertyPage(CConnectDlgGeneral::IDD)
{
	//{{AFX_DATA_INIT(CConnectDlgGeneral)
	m_uPort = 0;
	m_sRcon = _T("");
	m_sServerName = _T("");
	m_uLogPort = 0;
	//}}AFX_DATA_INIT
	m_dwLocalIP = 0x7f000001;
}

CConnectDlgGeneral::~CConnectDlgGeneral()
{
}

void CConnectDlgGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlgGeneral)
	DDX_Control(pDX, IDC_LOGQUERYTYPE, m_ctlLogQueryType);
	DDX_Control(pDX, IDC_LOCALIP, m_ctlLocalIP);
	DDX_Text(pDX, IDC_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 1024, 65535);
	DDX_Text(pDX, IDC_RCON, m_sRcon);
	DDV_MaxChars(pDX, m_sRcon, 100);
	DDX_Text(pDX, IDC_SERVERNAME, m_sServerName);
	DDV_MaxChars(pDX, m_sServerName, 100);
	DDX_Text(pDX, IDC_LOCALLOGPORT, m_uLogPort);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		m_sServerName.TrimLeft();
		m_sServerName.TrimRight();
		if (m_sServerName.IsEmpty())
		{
			pDX->PrepareEditCtrl(IDC_SERVERNAME);
			AfxMessageBox("Please enter a server name or IP address", MB_OK | MB_ICONWARNING);
			pDX->Fail();
			return;
		}
		if ((m_uLogPort != 0) && ((m_uLogPort < 1024) || (m_uLogPort > 65535)))
		{
			pDX->PrepareEditCtrl(IDC_LOCALLOGPORT);
			AfxMessageBox("Please enter a port number between 1024 and 65535.\nEnter 0 for a random port", MB_OK | MB_ICONWARNING);
			pDX->Fail();
			return;
		}
		m_ctlLocalIP.GetAddress(m_dwLocalIP);
		m_dwLocalIP = htonl(m_dwLocalIP);
		int nSel = m_ctlLogQueryType.GetCurSel();
		if (nSel < 0)
			m_eType = LQT_AUTODETECT;
		else
			m_eType = (LogQueryType)m_ctlLogQueryType.GetItemData(nSel);
	} else {
		m_ctlLocalIP.SetAddress(ntohl(m_dwLocalIP));
		for (int nItem = 0; nItem < m_ctlLogQueryType.GetCount(); nItem++)
		{
			if ((LogQueryType)m_ctlLogQueryType.GetItemData(nItem) == m_eType)
			{
				m_ctlLogQueryType.SetCurSel(nItem);
				break;
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CConnectDlgGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectDlgGeneral)
	ON_BN_CLICKED(IDC_GUESS, OnGuess)
	ON_BN_CLICKED(IDC_GUESSHARDER, OnGuessharder)
	ON_CBN_SELCHANGE(IDC_LOGQUERYTYPE, OnSelchangeLogquerytype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlgGeneral message handlers

void CConnectDlgGeneral::OnGuess() 
{
	DWORD dwIP = 0;
	if (!settings.GuessIP(dwIP))
	{
		AfxMessageBox("Failed to guess local IP");
		return;
	}
	m_ctlLocalIP.SetAddress(dwIP);
	return;
}

void CConnectDlgGeneral::OnGuessharder() 
{
	CWaitCursor wait;

	DWORD dwIP = 0;
	if (!settings.GuessIPHarder(dwIP))
	{
		AfxMessageBox("Failed to guess local IP harder");
		return;
	}
	m_ctlLocalIP.SetAddress(dwIP);
}

BOOL CConnectDlgGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctlLogQueryType.SetItemData(m_ctlLogQueryType.AddString("Autodetect (or I don't know)"), (DWORD)LQT_AUTODETECT);
	m_ctlLogQueryType.SetItemData(m_ctlLogQueryType.AddString("Classic (logaddress)"), (DWORD)LQT_LOGADDRESS);
	m_ctlLogQueryType.SetItemData(m_ctlLogQueryType.AddString("HLDS x.1.1.1 (logaddress_add)"), (DWORD)LQT_LOGADDRESS_ADD);
	m_ctlLogQueryType.SetItemData(m_ctlLogQueryType.AddString("LogMod"), (DWORD)LQT_LOGMOD);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlgGeneral::OnSelchangeLogquerytype() 
{
	LogQueryType eType = LQT_AUTODETECT;
	int nSel = m_ctlLogQueryType.GetCurSel();
	if (nSel >= 0)
		eType = (LogQueryType)m_ctlLogQueryType.GetItemData(nSel);

	switch (eType)
	{
	case LQT_LOGADDRESS:
	case LQT_LOGADDRESS_ADD:
	case LQT_AUTODETECT:
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LOCALIP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GUESS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GUESSHARDER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LOCALLOGPORT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC5)->ShowWindow(SW_SHOW);
		break;
	case LQT_LOGMOD:
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LOCALIP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GUESS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GUESSHARDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LOCALLOGPORT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC5)->ShowWindow(SW_HIDE);
		break;
	}
}
