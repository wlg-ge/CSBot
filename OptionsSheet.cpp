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

// OptionsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsSheet.h"
#include "OptionsGeneral.h"
#include "OptionsIRCIn.h"
#include "OptionsIRCOut.h"
#include "OptionsSummon.h"
#include "OptionsIRCMsgs.h"
#include "OptionsPlayerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)

COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

COptionsSheet::COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

COptionsSheet::~COptionsSheet()
{
	delete m_pageGeneral;
	delete m_pageIRCIn;
	delete m_pageIRCOut;
	delete m_pageMsgs;
	delete m_pageSummon;
	delete m_pagePlayerList;
}


BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionsSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet message handlers
void COptionsSheet::AddPage(COptionsPage *pPage)
{
	pPage->m_pSheet = this;
	CPropertySheet::AddPage(pPage);
}

void COptionsSheet::AddPages()
{
	m_pageGeneral = new COptionsGeneral;
	AddPage(m_pageGeneral);
	m_pageIRCIn = new COptionsIRCIn;
	AddPage(m_pageIRCIn);
	m_pageIRCOut = new COptionsIRCOut;
	AddPage(m_pageIRCOut);
	m_pageMsgs = new COptionsIRCMsgs;
	AddPage(m_pageMsgs);
	m_pageSummon = new COptionsSummon;
	AddPage(m_pageSummon);
	m_pagePlayerList = new COptionsPlayerList;
	AddPage(m_pagePlayerList);
}

void COptionsSheet::ReadSettings()
{
	m_pageGeneral->ReadSettings();
	m_pageIRCIn->ReadSettings();
	m_pageIRCOut->ReadSettings();
	m_pageSummon->ReadSettings();
	m_pageMsgs->ReadSettings();
	m_pagePlayerList->ReadSettings();
}

void COptionsSheet::WriteSettings()
{
	m_pageGeneral->WriteSettings();
	m_pageIRCIn->WriteSettings();
	m_pageIRCOut->WriteSettings();
	m_pageSummon->WriteSettings();
	m_pageMsgs->WriteSettings();
	m_pagePlayerList->WriteSettings();
}
