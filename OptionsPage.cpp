// OptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsPage.h"
#include "OptionsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPage property page

IMPLEMENT_DYNCREATE(COptionsPage, CPropertyPage)

COptionsPage::COptionsPage(UINT nIDTemplate) : CPropertyPage(nIDTemplate)
{
	//{{AFX_DATA_INIT(COptionsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COptionsPage::~COptionsPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// COptionsPage message handlers

BOOL COptionsPage::OnApply() 
{
	WriteSettings();
	return CPropertyPage::OnApply();
}

void COptionsPage::OnModified()
{
	SetModified(TRUE);
}
