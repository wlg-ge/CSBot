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

// ConnectIRCIdent.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "ConnectIRCIdent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCIdent dialog

IMPLEMENT_DYNCREATE(CConnectIRCIdent, CPropertyPage)

CConnectIRCIdent::CConnectIRCIdent() : CPropertyPage(CConnectIRCIdent::IDD)
{
	//{{AFX_DATA_INIT(CConnectIRCIdent)
	m_bCloseIdentAfterQuery = FALSE;
	m_bIdent = FALSE;
	m_sUserID = _T("");
	//}}AFX_DATA_INIT
}

CConnectIRCIdent::~CConnectIRCIdent()
{
}

void CConnectIRCIdent::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectIRCIdent)
	DDX_Check(pDX, IDC_CLOSEIDENTAFTERQUERY, m_bCloseIdentAfterQuery);
	DDX_Check(pDX, IDC_IDENT, m_bIdent);
	DDX_Text(pDX, IDC_USERID, m_sUserID);
	DDV_MaxChars(pDX, m_sUserID, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectIRCIdent, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectIRCIdent)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectIRCIdent message handlers
