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

// OptionsIRCOut.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsIRCOut.h"
#include "ProfileName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCOut property page

IMPLEMENT_DYNCREATE(COptionsIRCOut, COptionsPage)

COptionsIRCOut::COptionsIRCOut() : COptionsPage(COptionsIRCOut::IDD)
{
	//{{AFX_DATA_INIT(COptionsIRCOut)
	m_nLines = 100;
	m_nSeconds = 1;
	m_nIRCDelay = 0;
	//}}AFX_DATA_INIT
}

COptionsIRCOut::~COptionsIRCOut()
{
}

void COptionsIRCOut::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsIRCOut)
	DDX_Text(pDX, IDC_LINES, m_nLines);
	DDV_MinMaxInt(pDX, m_nLines, 1, 100);
	DDX_Text(pDX, IDC_SECONDS, m_nSeconds);
	DDV_MinMaxInt(pDX, m_nSeconds, 1, 10);
	DDX_Text(pDX, IDC_IRCDELAY, m_nIRCDelay);
	DDV_MinMaxInt(pDX, m_nIRCDelay, 0, 120);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsIRCOut, COptionsPage)
	//{{AFX_MSG_MAP(COptionsIRCOut)
	ON_EN_CHANGE(IDC_LINES, OnModified)
	ON_EN_CHANGE(IDC_SECONDS, OnModified)
	ON_EN_CHANGE(IDC_IRCDELAY, OnModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCOut message handlers

BOOL COptionsIRCOut::OnInitDialog() 
{
	COptionsPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsIRCOut::ReadSettings()
{
	m_nLines = settings.GetAntifloodLines();
	m_nSeconds = settings.GetAntifloodSeconds();
	m_nIRCDelay = settings.GetIRCDelay();
}

void COptionsIRCOut::WriteSettings()
{
	settings.SetAntiflood(m_nLines, m_nSeconds);
	settings.SetIRCDelay(m_nIRCDelay);
}
