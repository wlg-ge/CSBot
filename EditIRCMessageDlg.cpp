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

// EditIRCMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newcsbot.h"
#include "EditIRCMessageDlg.h"
#include "Settings.h"
#include "MainFrm.h"
#include "GIRLLogParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditIRCMessageDlg dialog


CEditIRCMessageDlg::CEditIRCMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditIRCMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditIRCMessageDlg)
	m_sMessage = _T("");
	m_bEnable = FALSE;
	//}}AFX_DATA_INIT
	m_pParser = NULL;
	m_pMsg = NULL;
}


void CEditIRCMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate)
	{
		m_sMessage = m_pMsg->m_sValue.c_str();
		m_bEnable = m_pMsg->m_bEnabled ? TRUE : FALSE;
	}
	//{{AFX_DATA_MAP(CEditIRCMessageDlg)
	DDX_Control(pDX, IDC_MORETAGS, m_ctlMoreTags);
	DDX_Control(pDX, IDC_SCOPE, m_ctlScope);
	DDX_Control(pDX, IDC_MESSAGE, m_ctlMessage);
	DDX_Control(pDX, IDC_DEFAULT, m_ctlDefault);
	DDX_Control(pDX, IDC_MEANING, m_ctlMeaning);
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
	DDV_MaxChars(pDX, m_sMessage, 150);
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_pMsg->m_sValue = (LPCTSTR)m_sMessage;
		m_pMsg->m_bEnabled = m_bEnable ? true : false;
	}
}


BEGIN_MESSAGE_MAP(CEditIRCMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CEditIRCMessageDlg)
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditIRCMessageDlg message handlers

void CEditIRCMessageDlg::OnRevert() 
{
	m_ctlMessage.SetWindowText(m_pMsg->m_pDefault);
}

BOOL CEditIRCMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlDefault.SetWindowText(m_pMsg->m_pDefault);
	m_ctlMeaning.SetWindowText(m_pMsg->m_pMeaning);
	m_ctlScope.SetWindowText(m_pParser->GameTypeDescription());
	m_ctlMoreTags.SetWindowText(m_pParser->GetAdditionalTags());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
