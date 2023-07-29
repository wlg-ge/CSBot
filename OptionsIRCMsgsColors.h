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

#if !defined(AFX_OPTIONSIRCMSGSCOLORS_H__71004B9A_2571_4931_8697_E5A32BBA1463__INCLUDED_)
#define AFX_OPTIONSIRCMSGSCOLORS_H__71004B9A_2571_4931_8697_E5A32BBA1463__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsIRCMsgsColors.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgsColors dialog

#include "settings.h"

class GIRLLogParser;
class COptionsPage;

struct ColorInfo
{
	ColorInfo() {
		for (int i=0; i<TEAM_COUNT; i++)
			sColor[i] = "01";
	}
	std::string sColor[TEAM_COUNT];
	std::string sColorDefault[TEAM_COUNT];
	std::string sTeam[TEAM_COUNT];
	GIRLLogParser* pParser;
};

class COptionsIRCMsgsColors : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsIRCMsgsColors)

// Construction
public:
	COptionsIRCMsgsColors();
	~COptionsIRCMsgsColors();
	void SetColors(ColorInfo* pColors, GIRLLogParser* pParser);
	COptionsPage* m_pPage;

// Dialog Data
	//{{AFX_DATA(COptionsIRCMsgsColors)
	enum { IDD = IDD_OPTIONS_IRC_MSGS_COLORS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	int m_nFore[TEAM_COUNT];
	int m_nBack[TEAM_COUNT];
	int m_nY;

	ColorInfo* m_pColors;
	GIRLLogParser* m_pParser;

	void Create(CWnd* pParent) {CPropertyPage::Create(IDD, pParent);};
	void SetColor(int nColor, CString sName, LPCTSTR sColor);
	CString GetColor(int nColor);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptionsIRCMsgsColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetColor(const CString& sColor, int& nFore, int& nBack);
	CString GetColor(int& nFore, int& nBack);
	void EditSample(int nColor);

	// Generated message map functions
	//{{AFX_MSG(COptionsIRCMsgsColors)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSample1();
	afx_msg void OnSample2();
	afx_msg void OnSample3();
	virtual BOOL OnInitDialog();
	afx_msg void OnSample0();
	afx_msg void OnSample4();
	afx_msg void OnSample5();
	afx_msg void OnRevertdefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIRCMSGSCOLORS_H__71004B9A_2571_4931_8697_E5A32BBA1463__INCLUDED_)
