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

// NewCSBot.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NewCSBot.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "GIRLLogParserDefault.h"
#include "GIRLLogParserCS.h"
#include "GIRLLogParserNS.h"
#include "GIRLLogParserDOD.h"
#include "GIRLLogParserTFC.h"
#include "GIRLLogParserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp

BEGIN_MESSAGE_MAP(CCSBotApp, CWinApp)
	//{{AFX_MSG_MAP(CCSBotApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_HELP_REPORT, OnHelpReport)
	ON_COMMAND(ID_HELP_SITE, OnHelpSite)
	ON_COMMAND(ID_HELP_FORUMS, OnHelpForums)
	ON_COMMAND(ID_HELP_UNITEDADMINS, OnHelpUnitedadmins)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp construction

CCSBotApp::CCSBotApp() : CWinApp("CSBot")
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCSBotApp object

CCSBotApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp initialization

BOOL CCSBotApp::InitInstance()
{
	WSADATA wsa;
	if (::WSAStartup(0x0101, &wsa) != 0)
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER <= 0x600
	// this is only for MFC 6.x
	// deprecated in MFC 7
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	SetRegistryKey("UnitedAdmins");

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	g_parserManager.Add(new GIRLLogParserDefault);
	g_parserManager.Add(new GIRLLogParserCS);
	g_parserManager.Add(new GIRLLogParserNS);
	g_parserManager.Add(new GIRLLogParserDOD);
//	g_parserManager.Add(new GIRLLogParserTFC);

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	switch (settings.m_nLastCmdShow)
	{
	case SW_SHOWMAXIMIZED:
	case SW_SHOWMINIMIZED:
		m_nCmdShow = settings.m_nLastCmdShow;
		break;
	}

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_NEWCSBTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_NEWCSBTYPE));

	if (!ParseCmdLine())
		return FALSE;

	// The main window has been initialized, so show and update it.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp message handlers

int CCSBotApp::ExitInstance() 
{
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	::WSACleanup();

	return CWinApp::ExitInstance();
}

void CCSBotApp::OnFileNew() 
{
	// create a new MDI child window
}



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnLink();
	afx_msg void OnUa();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

void CAboutDlg::OnLink() 
{
	ShellExecute(NULL, "open", "http://www.csbot.org", NULL, "", SW_SHOW);
}

void CAboutDlg::OnUa() 
{
	ShellExecute(NULL, "open", "http://www.unitedadmins.com", NULL, "", SW_SHOW);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_LINK, OnLink)
	ON_BN_CLICKED(IDC_UA, OnUa)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCSBotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCSBotApp message handlers


bool CCSBotApp::ParseCmdLine()
{
	std::string sProfileGame;
	std::string sProfileIRC;

	bool bGuess = false;
	bool bGuessHarder = false;

	int nType = 0;
	for (int i = 1; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		if (nType == 0) {
			if (strcmp(pszParam, "-game") == 0) {
				m_nCmdShow = SW_MINIMIZE;
				nType = 1;
			} else if (strcmp(pszParam, "-irc") == 0){
				m_nCmdShow = SW_MINIMIZE;
				nType = 2;
			} else if (strcmp(pszParam, "-guess") == 0){
				bGuess = true;
			} else if (strcmp(pszParam, "-guessharder") == 0){
				bGuessHarder = true;
			} else if (strcmp(pszParam, "-?") == 0){
				AfxMessageBox(	"CSBot Command line help\n" \
								"\n" \
								"Usage: CSBot.exe [-?] [-game <profilename> [-guess | -guessharder]] [-irc <profilename>]\n" \
								"\n" \
								"   -?  : Show this help message\n" \
								"   -game <profilename> : Automatically connect to the game server <profilename>\n" \
								"   -guess              : Guess local IP before connecting to the game server specified by -game\n" \
								"   -guessharder        : Guess local IP 'harder' before connecting to the game server specified by -game\n" \
								"   -irc <profilename>  : Automatically connect to the IRC server <profilename>\n" \
								"\n" \
								"If either -game or -irc are specified, CSBot will be minimized automatically\n"
				);
			}
		} else {
			switch (nType) {
			case 1:
				sProfileGame = pszParam;
				break;
			case 2:
				sProfileIRC = pszParam;
				break;
			}
			nType = 0;
		}
	}
	if (!sProfileGame.empty())
		((CMainFrame*)m_pMainWnd)->Connect(sProfileGame.c_str(), bGuess, bGuessHarder);
	if (!sProfileIRC.empty())
		((CMainFrame*)m_pMainWnd)->ConnectIRC(sProfileIRC.c_str());
	return true;
}

void CCSBotApp::OnHelpReport() 
{
	ShellExecute(NULL, "open", "mailto:yonatanz@unitedadmins.com?Subject=CSBot", NULL, "", SW_SHOW);
}

void CCSBotApp::OnHelpSite() 
{
	ShellExecute(NULL, "open", "http://www.csbot.org", NULL, "", SW_SHOW);
}

void CCSBotApp::OnHelpForums() 
{
	ShellExecute(NULL, "open", "http://forums.unitedadmins.com/index.php?act=SF&f=194", NULL, "", SW_SHOW);
}

void CCSBotApp::OnHelpUnitedadmins() 
{
	ShellExecute(NULL, "open", "http://www.unitedadmins.com", NULL, "", SW_SHOW);
}

