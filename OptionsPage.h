#if !defined(AFX_OPTIONSPAGE_H__4FDF8ADB_800A_4BDC_9FC3_E9FDA40826C8__INCLUDED_)
#define AFX_OPTIONSPAGE_H__4FDF8ADB_800A_4BDC_9FC3_E9FDA40826C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsPage dialog

class COptionsSheet;

class COptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsPage)

// Construction
public:
	COptionsPage(UINT nIDTemplate = -1);
	~COptionsPage();

	COptionsSheet* m_pSheet;
	virtual void WriteSettings() {ASSERT(FALSE);};
	virtual void ReadSettings() {ASSERT(FALSE);};

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsPage)
	public:
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL

	afx_msg virtual void OnModified();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPAGE_H__4FDF8ADB_800A_4BDC_9FC3_E9FDA40826C8__INCLUDED_)
