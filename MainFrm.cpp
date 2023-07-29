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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NewCSBot.h"
#include "ChildFrm.h"
#include "CSBotView.h"
#include "PlayerListView.h"
#include "RconView.h"
#include "IRCView.h"
#include "RawLogView.h"
#include "SchedView.h"
#include "ConnectDlg.h"
#include "ConnectIRCDlg.h"
#include "WonIDDatabaseDlg.h"
#include "WonIDDBManager.h"
#include "IRCOutputThread.h"
#include "LogFilter.h"
#include "NotifyUpdateDlg.h"

#include "MainFrm.h"
#include "OptionsSheet.h"
#include "Mmsystem.h"
#include "GIRLLogParserManager.h"
#include "GIRLWindowing.h"
#include "GIRLUdpServer.h"
#include "RegexHelper.h"
#include "LogFilter.h"
extern "C" {
	#include "regex.h"
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Our tray-icon message
#define UM_TRAYMSG (WM_USER+61)

// Other messages used for cross-thread notifications
#define UM_UDP					(WM_USER+749)
#define UM_UPDATEPLAYER			(WM_USER+750)
#define UM_SETMAPNAME			(WM_USER+752)
#define UM_UPDATEFRAG			(WM_USER+753)
#define UM_PLAYERDISCONNECTED	(WM_USER+754)
#define UM_SUMMON				(WM_USER+755)
#define UM_UPDATEDEATH			(WM_USER+756)
#define UM_IDENTRESPONDED		(WM_USER+758)
#define UM_NOTIFY_UPDATE		(WM_USER+759)

// Timer IDs
#define IDT_COMMITWONIDS	122
#define IDT_STATUS			102
#define IDT_FLASH			133
#define IDT_SECOND			141

// Shell message that is send when taskbar is re-created
const UINT wm_tbc = RegisterWindowMessage("TaskbarCreated");

// mIRC's default colors (user can change them in mIRC, but can't in here
COLORREF CMainFrame::m_colorsIRC[16] = {
	RGB(255,255,255),	//0
	RGB(0, 0, 0),		//1
	RGB(0, 0, 127),		//2
	RGB(0, 147, 0),		//3
	RGB(255, 0, 0),		//4
	RGB(127, 0, 0),		//5
	RGB(156, 0, 156),	//6
	RGB(252,127,0),		//7
	RGB(255,255,0),		//8
	RGB(0,255,0),		//9
	RGB(0,147,147),		//10
	RGB(0,255,255),		//11
	RGB(0,0,252),		//12
	RGB(255,0,255),		//13
	RGB(127,127,127),	//14
	RGB(210,210,210)	//15
};

typedef std::map<int, std::string> INT2STR;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
DECLARE_IRC_MAP(CMainFrame, CIrcDefaultMonitor)

// one global IRC session is used for the main IRC connection with the IRCD
CIrcSession g_ircSession;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_SERVER_CONNECT_GAME, OnUpdateServerConnectCs)
	ON_COMMAND(ID_SERVER_CONNECT_GAME, OnServerConnectCs)
	ON_UPDATE_COMMAND_UI(ID_SERVER_CONNECT_IRC, OnUpdateServerConnectIrc)
	ON_COMMAND(ID_SERVER_CONNECT_IRC, OnServerConnectIrc)
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_RAWLOG, OnViewRawlog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_PLAYERS, OnViewPlayers)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_VIEW_RCON, OnViewRcon)
	ON_COMMAND(ID_VIEW_IRC, OnViewIrc)
	ON_COMMAND(ID_SERVER_WONIDS, OnServerWonids)
	ON_UPDATE_COMMAND_UI(ID_SERVER_WONIDS, OnUpdateServerWonids)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_IRC, OnUpdateViewIrc)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYERS, OnUpdateViewPlayers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RAWLOG, OnUpdateViewRawlog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RCON, OnUpdateViewRcon)
	ON_COMMAND(ID_VIEW_SCHED, OnViewSched)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCHED, OnUpdateViewSched)
	ON_WM_ACTIVATE()
	ON_WM_QUERYENDSESSION()
	ON_COMMAND(ID_MUTE, OnMute)
	ON_UPDATE_COMMAND_UI(ID_MUTE, OnUpdateMute)
	ON_COMMAND(ID_HELP_UPDATE, OnHelpUpdate)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UDP, OnUdp)
	ON_MESSAGE(UM_TRAYMSG, OnTrayMsg)
	ON_MESSAGE(UM_UPDATEPLAYER, OnUpdatePlayer)
	ON_MESSAGE(UM_SETMAPNAME, OnSetMapName)
	ON_MESSAGE(UM_UPDATEFRAG, OnUpdateFrag)
	ON_MESSAGE(UM_UPDATEDEATH, OnUpdateDeath)
	ON_MESSAGE(UM_PLAYERDISCONNECTED, OnPlayerDisconnected)
	ON_MESSAGE(UM_SUMMON, OnSummon)
	ON_MESSAGE(UM_IDENTRESPONDED, OnIdentResponded)
	ON_MESSAGE(UM_NOTIFY_UPDATE, OnUpdateNotify)
	ON_REGISTERED_MESSAGE(wm_tbc, OnTaskbarCreated)
	ON_COMMAND(ID_TRAY_RESTORE, OnTrayRestore)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_VIEW_RCON,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : CIrcDefaultMonitor(g_ircSession), CIGCLockable("MainFrame")
{
	// Set up the IRC events and handlers
	IRC_MAP_ENTRY(CMainFrame, "JOIN", OnIrc_JOIN)
	IRC_MAP_ENTRY(CMainFrame, "PART", OnIrc_PART)
	IRC_MAP_ENTRY(CMainFrame, "351", OnIrc_VERSION)
	IRC_MAP_ENTRY(CMainFrame, "001", OnIrc_WELCOME)
	IRC_MAP_ENTRY(CMainFrame, "PRIVMSG", OnIrc_PRIVMSG)
	IRC_MAP_ENTRY(CMainFrame, "ERROR", OnIrc_ERROR)
	IRC_MAP_ENTRY(CMainFrame, "366", OnIrc_ENDOFNAMES)
	IRC_MAP_ENTRY(CMainFrame, "323", OnIrc_ENDOFLIST)
	IRC_MAP_ENTRY(CMainFrame, "QUIT", OnIrc_QUIT)
	IRC_MAP_ENTRY(CMainFrame, "NICK", OnIrc_NICK)
	IRC_MAP_ENTRY(CMainFrame, "DCCMSG", OnIrc_DCCMSG)
	IRC_MAP_ENTRY(CMainFrame, "513", OnIrc_513)
	IRC_MAP_ENTRY(CMainFrame, "322", OnIrc_CHANMODES)
	IRC_MAP_ENTRY(CMainFrame, "324", OnIrc_CHANMODE)
	IRC_MAP_ENTRY(CMainFrame, "332", OnIrc_CHANTOPIC)
	IRC_MAP_ENTRY(CMainFrame, "MODE", OnIrc_MODE)
	IRC_MAP_ENTRY(CMainFrame, "TOPIC", OnIrc_TOPIC)

	// Initialize members
	m_hIconTray = NULL;
	m_bHasFlashTimer = false;
	m_bSentAutoCommands = false;
	m_bSentRconCommands = false;
	m_bSentRconTest = false;
	m_bSentRules = false;
	m_bNotifyNoUpdate = false;

	m_ResourceIDs[I_RAWLOG] = IDR_RAWLOG;
	m_ResourceIDs[I_PLAYERS] = IDR_PLAYERLIST;
	m_ResourceIDs[I_RCON] = IDR_RCON;
	m_ResourceIDs[I_IRC] = IDR_IRC;
	m_ResourceIDs[I_SCHED] = IDR_SCHED;
	for (int i=0; i<I_MAX; i++)
		m_pFrames[i] = NULL;

	g_parserManager.SetReceiver(this);
	// Load all settings
	settings.LoadFromRegistry();

	// And initialize more members
	settings.OpenLogFile();

	m_bConnected = false;
	m_bConnectedIRC = false;
	m_bJoinedIRC = false;
	m_nMaxPlayers = 0;
	m_bHasTimers = false;
	m_nReconnectGameSecLeft = -1;
	m_nReconnectGameAttemptsLeft = -1;
	m_bDuringConnection = false;
	m_nReconnectIRCSecLeft = -1;
	m_nReconnectIRCAttemptsLeft = -1;
	m_bIRCDisconnectRequest = false;
	m_bMute = false;
	m_nUnmuteImage = 0;
	m_nMuteImage = 0;
	m_pParser = NULL;
	m_pIrcOutput = NULL;
	m_tLastLogLine = CTime::GetCurrentTime();
	m_pUdpServer = NULL;
	m_eTypeUsed = LQT_AUTODETECT;
}

CMainFrame::~CMainFrame()
{
	// get rid of member memory allocations
	if (m_pUdpServer != NULL)
		delete m_pUdpServer;
	m_pUdpServer = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Timer for updating the players' status
	SetTimer(IDT_STATUS, 5000, NULL);
	// Timer for IRC reconnection
	SetTimer(IDT_SECOND, 1000, NULL);
	// Timer for commiting any changes to the WonID database
	SetTimer(IDT_COMMITWONIDS, 60*1000, NULL);
	m_bHasTimers = true;

	// create the toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	// load buttons and bitmap from resource
	if (!GIRLLoadToolBar(m_wndToolBar, MAKEINTRESOURCE(IDR_MAINFRAME)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText("Standard toolbar");

	// prepare the additional image for the mute button
	UINT uID, uStyle;
	int nButton = m_wndToolBar.CommandToIndex(ID_MUTE);
	m_wndToolBar.GetButtonInfo(nButton, uID, uStyle, m_nUnmuteImage);
	m_nMuteImage = m_wndToolBar.GetToolBarCtrl().GetImageList()->GetImageCount() - 1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// register our tray icon
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	::GetVersionEx(&ovi);

	// ifdentify WinXP and WinME. we use 256 colors tray icons in those. otherwise, 16 colors
	if (((ovi.dwMajorVersion == 4) && (ovi.dwMinorVersion == 90) && (ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)) ||
		((ovi.dwMajorVersion == 5) && (ovi.dwMinorVersion == 1) && (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT)))
	{
		m_hIconTray = AfxGetApp()->LoadIcon(IDR_TRAY256);
	} else {
		m_hIconTray = AfxGetApp()->LoadIcon(IDR_TRAY16);
	}


	if (!CreateSystemTrayIcon())
	{
		TRACE0("Failed to create tray icon\n");
		return -1;      // fail to create
	}

	// prepare tray-icon popup menu
	m_TrayMenu.CreatePopupMenu();
	m_TrayMenu.AppendMenu(MF_STRING, ID_TRAY_RESTORE, "Restore CSBot");
	m_TrayMenu.AppendMenu(MF_SEPARATOR, 0, "");
	m_TrayMenu.AppendMenu(MF_STRING, ID_SERVER_CONNECT_GAME, "game");
	m_TrayMenu.AppendMenu(MF_STRING, ID_SERVER_CONNECT_IRC, "irc");
	m_TrayMenu.AppendMenu(MF_STRING, ID_VIEW_OPTIONS, "Options...");
	m_TrayMenu.AppendMenu(MF_STRING, ID_SERVER_WONIDS, "WonID Database...");
	m_TrayMenu.AppendMenu(MF_SEPARATOR, 0, "");
	m_TrayMenu.AppendMenu(MF_STRING, ID_APP_ABOUT, "About CSBot...");
	m_TrayMenu.AppendMenu(MF_STRING, ID_APP_EXIT, "Close CSBot");
	m_TrayMenu.SetDefaultItem(ID_TRAY_RESTORE);

	m_wndStatusBar.SetPaneInfo(0, 1001, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(0, "");
	m_wndStatusBar.SetPaneInfo(1, 1002, SBPS_NORMAL, 80);
	m_wndStatusBar.SetPaneText(1, "");
	m_wndStatusBar.SetPaneInfo(2, 1003, SBPS_NORMAL, 50);
	m_wndStatusBar.SetPaneText(2, "");
	m_wndStatusBar.SetPaneInfo(3, 1004, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(3, "");
	m_wndStatusBar.SetPaneInfo(4, 1005, SBPS_NORMAL, 80);
	m_wndStatusBar.SetPaneText(4, "");
	m_wndStatusBar.SetPaneInfo(5, 1006, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(5, "");

	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndToolBar);

	// prepare the IRC session
	g_ircSession.AddMonitor(this);
	g_ircSession.m_bDCC = false;

	// Create and show all child frames
	ShowFrame(I_RAWLOG);
	ShowFrame(I_PLAYERS);
	ShowFrame(I_RCON);
	ShowFrame(I_IRC);
	ShowFrame(I_SCHED);

	// Disable all server-related UI controls. game and IRC
	DisableControls();
	DisableIRCControls();

#if _TRACES >= 1
	SetWindowText("CSBot - Tracing prerelease version");
#endif

//	AttachMenuImagesFromToolbar(GetMenu(), &m_wndToolBar.GetToolBarCtrl());
	m_pIrcOutput = new IRCOutputThread(this);
	m_pLogFilter = new LogFilter();
	// Show GPL disclaimer at least once
	if (!settings.m_bAboutShown)
	{
		((CCSBotApp*)AfxGetApp())->OnAppAbout();
		settings.m_bAboutShown = TRUE;
	}

	NotifyCSBotUpdate();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnIrcDefault(const CIrcMessage* pmsg) 
{
}

void CMainFrame::OnIrcDisconnected(const CIrcMessage* pmsg) 
{
	// need not do anything upon DCC disconnection.
	if (pmsg->m_pOrigin->m_bDCC)
		return;

	if (!m_bIRCDisconnectRequest)
	{
		// Disconnected, not by user request
		if (m_serverIRC.m_bReconnectToIRC)
		{
			if (m_nReconnectIRCAttemptsLeft >= 0)
				m_nReconnectIRCSecLeft = m_serverIRC.m_nIRCReconnectSec;
		} else {
			m_nReconnectIRCSecLeft = -1;
			m_nReconnectIRCAttemptsLeft = -1;
		}
		if (m_pFrames[I_IRC] != NULL)
		{
			CIRCView* pView = (CIRCView*)m_pFrames[I_IRC];
			pView->SetReconnect(m_nReconnectIRCSecLeft, m_nReconnectIRCAttemptsLeft);
		}
	}
	m_bIRCDisconnectRequest = false;
	DisconnectIRC();
	// Disconnected? make sure the UI reflects this
	DisableIRCControls();
}

bool CMainFrame::OnIrc_PART(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->parameters.size() < 1)
		return false;
	if (stricmp(pmsg->parameters[0].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0) {
		if (stricmp(pmsg->prefix.sNick.c_str(), (LPCTSTR)m_serverIRC.m_sNickName) == 0)
		{
			// we parted our channel. remember this.
			m_bJoinedIRC = false;
		}
	}

	return true;
}

bool CMainFrame::OnIrc_ERROR(const CIrcMessage* pmsg)
{
	// If there is an IRC error - Disconnect.
	return true;
}

bool CMainFrame::OnIrc_JOIN(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->parameters.size() < 1)
		return false;
	if (stricmp(pmsg->parameters[0].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0) {
		if (stricmp(pmsg->prefix.sNick.c_str(), (LPCTSTR)m_serverIRC.m_sNickName) == 0)
		{
			// if we joined our channel, then get the channel status, our mode, and set topic if needed
			CString cMsg;
			cMsg.Format("LIST %s\n", (LPCTSTR)m_serverIRC.m_sChannelName);
			m_session << (LPCTSTR)cMsg;
			cMsg.Format("MODE %s\n", (LPCTSTR)m_serverIRC.m_sChannelName);
			m_session << (LPCTSTR)cMsg;
			m_bJoinedIRC = true;
		}
	}

	return true;
}

bool CMainFrame::OnIrc_VERSION(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->parameters.size() < 3)
		return false;
	// update status bar with the IRCD info
	m_wndStatusBar.SetPaneText(3, pmsg->parameters[2].c_str());
	m_wndStatusBar.SetPaneText(4, m_serverIRC.m_sChannelName);

	return true;
}

void CMainFrame::OnIrcAll(const CIrcMessage* pmsg) 
{
	if (pmsg == NULL)
		return;
	if (m_pFrames[I_IRC] == NULL)
		return;
	if (m_pFrames[I_IRC]->m_pView == NULL)
		return;

	// Log all IRC traffic to the IRC console
	CString sParams;

	for (std::vector<String>::const_iterator i = pmsg->parameters.begin(); i!= pmsg->parameters.end(); i++)
	{
		if (!sParams.IsEmpty())
			sParams += " ";
		if (!(*i).empty())
			sParams += (*i).c_str();
	}
	sParams += "";
	CString sLine;
	if (!pmsg->m_bIncoming) {
		sLine = "-> ";
		sLine += pmsg->sOriginalMessage.c_str();
	} else {
		if (!pmsg->prefix.sHost.empty() || !pmsg->prefix.sNick.empty() || !pmsg->prefix.sUser.empty())
		{
			sLine += "\"";
			if (!pmsg->prefix.sHost.empty())
				sLine += pmsg->prefix.sHost.c_str();
			sLine += ",";
			if (!pmsg->prefix.sNick.empty())
				sLine += pmsg->prefix.sNick.c_str();
			sLine += ",";
			if (!pmsg->prefix.sUser.empty())
				sLine += pmsg->prefix.sUser.c_str();
			sLine += "\": ";
		}
		sLine += pmsg->sCommand.c_str();
		sLine += "  ";
		sLine += sParams;
	}
	((CIRCView*)m_pFrames[I_IRC]->m_pView)->AddLine((LPCTSTR)sLine);
}

void CMainFrame::OutputIRCMessageDelayed(LPCTSTR pMessage, LPCTSTR pTo /* = NULL */, CIrcSession* pSession /* = NULL */)
{
	if (m_bConnectedIRC && m_bJoinedIRC) {
		// output a message, using the session and nickname given
		// Use the antiflood protection limits
		CString sMsg;
		sMsg.Format("PRIVMSG %s :%s\n", (pTo == NULL) ? (LPCTSTR)m_serverIRC.m_sChannelName : pTo, pMessage);
		if ((pSession == NULL) || (pSession == &g_ircSession))
			m_pIrcOutput->QueueOutputMsg(sMsg);
		else 
			*pSession << (LPCTSTR)sMsg;
	}
}

void CMainFrame::OutputIRCMessageImmediate(LPCTSTR pMessage, LPCTSTR pTo /* = NULL */, CIrcSession* pSession /* = NULL */)
{
	if (m_bConnectedIRC && m_bJoinedIRC) {
		// output a message, using the session and nickname given
		// Use the antiflood protection limits
		CString sMsg;
		sMsg.Format("PRIVMSG %s :%s\n", (pTo == NULL) ? (LPCTSTR)m_serverIRC.m_sChannelName : pTo, pMessage);
		if ((pSession == NULL) || (pSession == &g_ircSession))
			m_pIrcOutput->QueueOutputMsgImmediate(sMsg);
		else 
			*pSession << (LPCTSTR)sMsg;
	}
}

void CMainFrame::OutputIRCMessage(unsigned long uMsg, LPCTSTR pMessage, LPCTSTR pTo /* = NULL */)
{
	// nothing to lock, but note that this method is called from a different thread (the logger's thread)
	// Conditionally outputs a message, if it is allowed
	if (m_bMute)
		return;
	if (m_pParser != NULL)
		if (m_pParser->IsIRCMessageEnabled(uMsg))
			OutputIRCMessageDelayed(pMessage, pTo);
}

void CMainFrame::OnFirstPing(LPCTSTR pID)
{
}

void CMainFrame::SendPassNickUser()
{
	CString sMsg;
	
	if( !m_serverIRC.m_sPassword.IsEmpty() ) {
		sMsg.Format("PASS %s\n", (LPCTSTR)m_serverIRC.m_sPassword);
		g_ircSession << (LPCTSTR)sMsg;
	}

	sMsg.Format("NICK %s\n", (LPCTSTR)m_serverIRC.m_sNickName);
	g_ircSession << (LPCTSTR)sMsg;

	TCHAR szHostName[MAX_PATH];
	DWORD cbHostName = sizeof(szHostName);
	::GetComputerName(szHostName, &cbHostName);

	sMsg.Format("USER %s 0 %s :%s\n", (LPCTSTR)m_serverIRC.m_sUserID, szHostName, (LPCTSTR)m_serverIRC.m_sFullName);
	g_ircSession << (LPCTSTR)sMsg;
}

bool CMainFrame::OnIrc_WELCOME(const CIrcMessage* pmsg)
{
	CWaitCursor wait;
	if (!pmsg->m_bIncoming)
		return true;

	// send autocommands on WELCOME (right after USER and NICK)
	g_ircSession << "version\n";

	if (!m_bSentAutoCommands)
	{
		POSITION pos = m_serverIRC.m_lConnectCommands.GetHeadPosition();
		CString sMsg;
		while (pos != NULL)
		{
			sMsg = m_serverIRC.m_lConnectCommands.GetNext(pos);

			TranslateIRCCommand(sMsg);

			if (!sMsg.IsEmpty()) {
				sMsg += "\n";
				g_ircSession << (LPCTSTR)sMsg;
			}
		}
		m_bSentAutoCommands = true;
	}

	Sleep(5000);

	CString sMsg;
	if (m_serverIRC.m_sChannelPassword.IsEmpty())
		sMsg.Format("JOIN %s\n", (LPCTSTR)m_serverIRC.m_sChannelName);
	else
		sMsg.Format("JOIN %s :%s\n", (LPCTSTR)m_serverIRC.m_sChannelName, (LPCTSTR)m_serverIRC.m_sChannelPassword);
	g_ircSession << (LPCTSTR)sMsg;

	return true;
}

bool CMainFrame::OnIrc_QUIT(const CIrcMessage* pmsg)
{
	return true;
}

bool CMainFrame::OnIrc_NICK(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	return true;
}

void CMainFrame::OnUpdateServerConnectCs(CCmdUI* pCmdUI) 
{
	if (m_bConnected) {
		pCmdUI->SetCheck(TRUE);
		pCmdUI->SetText("Disconnect from &Game server");
	} else {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->SetText("Connect to &Game server...");
	}
}

void CMainFrame::OnServerConnectCs() 
{
	// user is interventing. stop all reconnection attempts
	m_nReconnectGameSecLeft = -1;
	m_nReconnectGameAttemptsLeft = -1;
	if (m_pFrames[I_RAWLOG] != NULL)
	{
		CRawLogView* pView = (CRawLogView*)m_pFrames[I_RAWLOG];
		pView->SetReconnect(m_nReconnectGameSecLeft, m_nReconnectGameAttemptsLeft);
	}

	if (m_bConnected) {
		Disconnect();
	} else {
		Connect();
	}
}

void CMainFrame::OnUpdateServerConnectIrc(CCmdUI* pCmdUI) 
{
	if (m_bConnectedIRC) {
		pCmdUI->SetCheck(TRUE);
		pCmdUI->SetText("Disconnect from &IRC server");
	} else {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->SetText("Connect to &IRC server...");
	}
}

void CMainFrame::OnServerConnectIrc() 
{
	// user is interventing. stop all reconnection attempts
	m_nReconnectIRCSecLeft = -1;
	m_nReconnectIRCAttemptsLeft = -1;
	if (m_pFrames[I_IRC] != NULL)
	{
		CIRCView* pView = (CIRCView*)m_pFrames[I_IRC];
		pView->SetReconnect(m_nReconnectIRCSecLeft, m_nReconnectIRCAttemptsLeft);
	}

	if (m_bConnectedIRC) {
		m_bIRCDisconnectRequest = true;
		DisconnectIRC();
		m_nReconnectIRCSecLeft = -1;
		m_nReconnectIRCAttemptsLeft = -1;
	} else {
		ConnectIRC();
	}
}

void CMainFrame::Connect() 
{
	CConnectDlg	dlg;
	dlg.m_sProfileName = settings.m_sLastCSServer;
	if (dlg.DoModal() == IDOK)
	{
		m_serverGame.Assign(&dlg);

		DoConnect(false);
	}
}

void CMainFrame::Disconnect() 
{
	std::string sRes;

	if (m_pUdpServer != NULL)
	{
		if (m_bSentRconCommands)
		{
			POSITION pos = m_serverGame.m_lDisconnectCommands.GetHeadPosition();
			while (pos != NULL) {
				CString sCmd = m_serverGame.m_lDisconnectCommands.GetNext(pos);
				IssueRcon(sCmd);
			}

			CString sMsg;
			in_addr LocalAddr;
			switch (m_eTypeUsed)
			{
			case LQT_LOGADDRESS:
				m_pUdpServer->SendRcon("logaddress 127.0.0.1 27015", m_serverGame.m_sRcon);
				break;
			case LQT_LOGADDRESS_ADD:
				LocalAddr.S_un.S_addr = m_serverGame.m_dwLocalIP;
				sMsg.Format("logaddress_del %s %u", inet_ntoa(LocalAddr), m_pUdpServer->GetLocalPort());
				m_pUdpServer->SendRcon(sMsg, m_serverGame.m_sRcon);
				break;
			}
		}
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}

	if (m_pParser != NULL)
	{
		m_pParser->SetActive(false);
		m_pParser = NULL;
	}

	DisableControls();
	m_bConnected = false;
	m_nReconnectGameSecLeft = -1;
	m_nReconnectGameAttemptsLeft = -1;
}

in_addr CMainFrame::ResolveServerName(bool bAutomatic)
{
	m_serverGame.m_sServerName.TrimLeft();
	m_serverGame.m_sServerName.TrimRight();
	in_addr addr;
	addr.S_un.S_addr = inet_addr((LPCTSTR)m_serverGame.m_sServerName);
	if (addr.S_un.S_addr == 0xffffffff) {
		// isn't an IP address... damn we need to resolve.
		hostent* pHe = ::gethostbyname((LPCTSTR)m_serverGame.m_sServerName);
		if (pHe == NULL) {
			if (!bAutomatic)
			{
				CString sMsg;
				sMsg.Format("Could not resolve host name %s", (LPCTSTR)m_serverGame.m_sServerName);
				AfxMessageBox(sMsg);
			}
			return addr;
		}
		if (pHe->h_addr_list[0] == NULL) {
			if (!bAutomatic)
			{
				CString sMsg;
				sMsg.Format("Could not resolve host name %s", (LPCTSTR)m_serverGame.m_sServerName);
				AfxMessageBox(sMsg);
			}
			return addr;
		}
		addr.S_un.S_addr = *((long*)pHe->h_addr_list[0]);
	}
	return addr;
}

LRESULT CMainFrame::OnUdp(WPARAM wParam, LPARAM lParam)
{
	GIRLUdpMessage* pMsg = (GIRLUdpMessage*)lParam;

	ASSERT((CMainFrame*)pMsg->m_dwParam == this);

	switch (pMsg->GetType())
	{
	case Msg_Rcon:
		OnUdpRcon((GIRLUdpMessageRcon*)pMsg);
		break;
	case Msg_Log:
		OnUdpLog((GIRLUdpMessageLog*)pMsg);
		break;
#if _TRACES >= 5
	case Msg_Debug:
		{
			GIRLUdpMessageDebug* pMsgDebug = (GIRLUdpMessageDebug*)pMsg;
			::OutputDebugString(pMsgDebug->m_sString.c_str());
		}
		break;
#endif
	case Msg_Rules:
		OnUdpRules((GIRLUdpMessageRules*)pMsg);
		break;
	case Msg_Players:
		{
			GIRLUdpMessagePlayers* pMsgPlayers = (GIRLUdpMessagePlayers*)pMsg;
			// donno what to do with this yet :o
		}
		break;
	case Msg_Info:
		OnUdpInfo((GIRLUdpMessageInfo*)pMsg);
		break;
	case Msg_Error:
		OnUdpError((GIRLUdpMessageError*)pMsg);
		break;
	default:
		ASSERT(FALSE);
	}
	delete pMsg;
	return 0;
}

void CMainFrame::OnIdentResponded(LPCTSTR pResponse)
{
	// ident server thread
	char* pChar = strdup(pResponse);
	PostMessage(UM_IDENTRESPONDED, 0, (LPARAM)pChar);
}

LRESULT CMainFrame::OnIdentResponded(WPARAM wParam, LPARAM lParam)
{
	// UI thread
	char* pLine = (char*)lParam;
	CString sLine = "IDENT: ";
	sLine += pLine;
	free(pLine);

	((CIRCView*)m_pFrames[I_IRC]->m_pView)->AddLine((LPCTSTR)sLine);
	return 0;
}

void CMainFrame::EnableControls()
{
	for (int i=0; i<I_MAX; i++)
		if (m_pFrames[i] != NULL)
			if (m_pFrames[i]->m_pView != NULL) 
				m_pFrames[i]->m_pView->EnableControls();
}

void CMainFrame::DisableControls()
{
	m_wndStatusBar.SetPaneText(0, "");
	m_wndStatusBar.SetPaneText(1, "");
	m_wndStatusBar.SetPaneText(2, "");

	for (int i=0; i<I_MAX; i++)
		if (m_pFrames[i] != NULL)
			if (m_pFrames[i]->m_pView != NULL) 
				m_pFrames[i]->m_pView->DisableControls();
}

void CMainFrame::EnableIRCControls()
{
	for (int i=0; i<I_MAX; i++)
		if (m_pFrames[i] != NULL)
			if (m_pFrames[i]->m_pView != NULL) 
				m_pFrames[i]->m_pView->EnableIRCControls();
}

void CMainFrame::DisableIRCControls()
{
	m_wndStatusBar.SetPaneText(3, "");
	m_wndStatusBar.SetPaneText(4, "");
	m_wndStatusBar.SetPaneText(5, "");

	for (int i=0; i<I_MAX; i++)
		if (m_pFrames[i] != NULL)
			if (m_pFrames[i]->m_pView != NULL) 
				m_pFrames[i]->m_pView->DisableIRCControls();
}

void CMainFrame::FetchPlayerList()
{
	if (m_pUdpServer != NULL)
		m_pUdpServer->SendRcon("status", (LPCTSTR)m_serverGame.m_sRcon);
}

void CMainFrame::ConnectIRC()
{
	CConnectIRCDlg dlg;
	dlg.m_sProfileName = settings.m_sLastIRCServer;
	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;
		m_serverIRC.Assign(&dlg);

		ConnectToIRCServer(false);
	}
}

void CMainFrame::DisconnectIRC()
{
	if (m_bConnectedIRC) {
		g_ircSession.Disconnect("CSBot shutting down");
		for (SESSIONLIST::iterator i = m_lSessions.begin(); i != m_lSessions.end(); i++)
		{
			(*i)->Disconnect("CSBot shutting down");
			delete (*i);
		}
		m_lSessions.clear();
		DisableIRCControls();
		IGCLock lock(this, "DisconnectIRC");
		m_bConnectedIRC = false;
		m_sLastTopic = "";
		m_bJoinedIRC = false;
		m_sChannelModes = "";
	}
}

void CMainFrame::OnDestroy() 
{
	Shell_NotifyIcon(NIM_DELETE, &m_notify);
	m_TrayMenu.DestroyMenu();
	KillTimers();

	WINDOWPLACEMENT placement;
	GetWindowPlacement(&placement);
	switch (placement.showCmd)
	{
	case SW_SHOWMAXIMIZED:
	case SW_SHOWMINIMIZED:
		settings.m_nLastCmdShow = placement.showCmd;
		break;
	default:
		settings.m_nLastCmdShow = SW_RESTORE;
	}

	CloseEverything();
	ClearPlayerList();

	delete m_pIrcOutput;
	m_pIrcOutput = NULL;
	
	settings.SaveToRegistry();

	settings.SetLogging(FALSE, "", FALSE);
	settings.OpenLogFile();

	for (int i=0; i<I_MAX; i++)
	{
		if (m_pFrames[i] != NULL)
		{
			delete m_pFrames[i];
			m_pFrames[i] = NULL;
		}
	}

	MSG msg;
	while (::PeekMessage(&msg, GetSafeHwnd(), UM_UPDATEPLAYER, UM_SUMMON, PM_REMOVE) != 0)
	{
		switch(msg.message)
		{
		case UM_UPDATEPLAYER:
			{
				UpdatePlayerStruct* pUpdate = (UpdatePlayerStruct*)msg.wParam;
				delete pUpdate;
			}
			break;
		case UM_SETMAPNAME:
			{
				SetMapNameStruct* pMapName = (SetMapNameStruct*)msg.wParam;
				delete pMapName;
			}
			break;
		case UM_UPDATEFRAG:
		case UM_UPDATEDEATH:
		case UM_PLAYERDISCONNECTED:
			break;
		case UM_SUMMON:
			{
				SetMapNameStruct* pSummon = (SetMapNameStruct*)msg.wParam;
				delete pSummon;
			}
			break;
		}
	}

	delete m_pLogFilter;

	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::ShowFrame(int nID)
{
	if (m_pFrames[nID] == NULL) {
		m_pFrames[nID] = (CChildFrame*)CreateNewChild(RUNTIME_CLASS(CChildFrame), m_ResourceIDs[nID]);
		m_pFrames[nID]->m_pMain = this;
		m_pFrames[nID]->BringWindowToTop();
		if (settings.m_bRemember)
			m_pFrames[nID]->ReadSettings();
	} else {
		if (m_pFrames[nID]->IsWindowVisible()) {
			m_pFrames[nID]->ShowWindow(SW_HIDE);
		} else {
			m_pFrames[nID]->ShowWindow(SW_SHOW);
			m_pFrames[nID]->BringWindowToTop();
		}
	}
}

/*
LRESULT CMainFrame::OnLogWarning(WPARAM wParam, LPARAM lParam)
{
#if _TRACES >= 6
	::OutputDebugString("Handling LogWarning\n");
#endif
	char* pLine = (char*)lParam;
	CString sLine = pLine;
	free(pLine);

	if (settings.m_bWarnLogOff)
		AfxMessageBox(sLine);
	return 0;
}

LRESULT CMainFrame::OnLogLine(WPARAM wParam, LPARAM lParam)
{
#if _TRACES >= 6
	::OutputDebugString("Handling LogLine\n");
#endif
	char* pLine = (char*)lParam;
	CString sLine = pLine;
	free(pLine);
	sLine.TrimLeft();
	sLine.TrimRight();

	if (m_pFrames[I_RAWLOG] != NULL) {
		((CRawLogView*)m_pFrames[I_RAWLOG]->m_pView)->AddLine(sLine);
	}

	if (settings.m_pLogFile != NULL)
	{
		fprintf(settings.m_pLogFile, "%s\n", (LPCTSTR)sLine);
		fflush(settings.m_pLogFile);
	}
	m_tLastLogLine = CTime::GetCurrentTime();

	return 0;
}
*/

void CMainFrame::OnViewRawlog() 
{
	ShowFrame(I_RAWLOG);
}

void CMainFrame::OnViewPlayers() 
{
	ShowFrame(I_PLAYERS);
}

void CMainFrame::OnViewRcon() 
{
	ShowFrame(I_RCON);
}

void CMainFrame::OnViewIrc() 
{
	ShowFrame(I_IRC);
}

void CMainFrame::OnViewSched() 
{
	ShowFrame(I_SCHED);
}

void CMainFrame::OnClose() 
{
	CMDIFrameWnd::OnClose();
}

void CMainFrame::CloseEverything()
{
	if (settings.m_bRemember)
		for (int i=0; i<I_MAX; i++)
			if (m_pFrames[i] != NULL)
				m_pFrames[i]->WriteSettings();
	ClearSchedList();
	ClearPlayerList();
	DisconnectIRC();
	g_ircSession.RemoveMonitor(this);
	KillTimers();
	CommitWonIDs();

	Disconnect();
	m_nReconnectIRCSecLeft = -1;
	m_nReconnectIRCAttemptsLeft = -1;
}

bool CMainFrame::CheckInactivityTimeout()
{
	if (m_serverGame.m_bInactivity)
	{
		CTimeSpan span = CTime::GetCurrentTime() - m_tLastLogLine;
		if (span.GetTotalSeconds() > m_serverGame.m_nInactivityTimeout)
		{
			return true;
		}
	}
	return false;
}

void CMainFrame::OnUdpLog(GIRLUdpMessageLog* pMsg)
{
	if (m_eTypeUsed == LQT_LOGMOD)
	{
		if (pMsg->m_sLine.compare("L 00/00/0000 - 00:00:00: [LogMod] Heartbeat?") == 0)
		{
			m_pUdpServer->SendLogMod("\xff\xff\xff\xff" "LogMod: Heartbeat");
			// Hide this line from the user
			return;
		}
	}

	if (m_pParser != NULL)
		m_pParser->ParseLogLine(pMsg->m_sLine);

	CString sLine = pMsg->m_sLine.c_str();
	sLine.TrimLeft();
	sLine.TrimRight();

	if (m_pFrames[I_RAWLOG] != NULL) {
		((CRawLogView*)m_pFrames[I_RAWLOG]->m_pView)->AddLine(sLine);
	}

	// I'm going to copy sLine to make sure I don't impact other threads
	CString sLogLine(sLine);
	if (settings.IsFilterLog())
		m_pLogFilter->filter(sLogLine);
	if (!sLogLine.IsEmpty() && (settings.m_pLogFile != NULL)) 
	{
		fprintf(settings.m_pLogFile, "%s\n", (LPCTSTR)sLogLine);
		fflush(settings.m_pLogFile);
	}
	m_tLastLogLine = CTime::GetCurrentTime();
}

void CMainFrame::OnUdpRules(GIRLUdpMessageRules* pMsg)
{
	// at this point we know for sure if the server has logmod or not (according to rules)
	if (!m_bSentRconTest)
	{
		m_pUdpServer->SendRcon("version", m_serverGame.m_sRcon);
//		m_pUdpServer->SendRcon("echo CSBot: Test", m_serverGame.m_sRcon);
		m_bSentRconTest = true;
	}
}

void CMainFrame::OnUdpError(GIRLUdpMessageError* pMsg)
{
	switch (pMsg->m_eType)
	{
	case E_RconChallange:
		if (!m_bSentRules)
		{
			m_pUdpServer->Send("\xff\xff\xff\xff" "rules");
			m_pUdpServer->Send("\xff\xff\xff\xff" "rules");
			m_bSentRules = true;
		}
		return;
	}

	// Disconnected, not by user request
	CString sError;
	sError.Format("Disconnected from game server.\nCSBot error no. %d\nSocket error no. %d", pMsg->m_nError, pMsg->m_nWSAError);
	OnSuddenDisconnect(true, sError);
}

void CMainFrame::OnSuddenDisconnect(bool bAllowReconnect /* = true */, LPCTSTR pErrorMessage /* = NULL */)
{
	if (m_pParser != NULL)
	{
		m_pParser->SetActive(false);
		m_pParser = NULL;
	}
	if (m_pUdpServer != NULL)
		delete m_pUdpServer;
	m_pUdpServer = NULL;
	DisableControls();
	m_bConnected = false;

	// Disconnected, not by user request
	if (m_bDuringConnection)
		m_bDuringConnection = false;
	if (bAllowReconnect)
	{
		if (m_serverGame.m_bReconnectToGame)
		{
			if (m_nReconnectGameAttemptsLeft >= 0)
				m_nReconnectGameSecLeft = m_serverGame.m_nGameReconnectSec;
		} else {
			m_nReconnectGameSecLeft = -1;
			m_nReconnectGameAttemptsLeft = -1;
			if (pErrorMessage != NULL)
				AfxMessageBox(pErrorMessage, MB_OK | MB_ICONWARNING);
		}
		if (m_pFrames[I_RAWLOG] != NULL)
		{
			CRawLogView* pView = (CRawLogView*)m_pFrames[I_RAWLOG];
			pView->SetReconnect(m_nReconnectGameSecLeft, m_nReconnectGameAttemptsLeft);
		}
	} else {
		m_nReconnectGameSecLeft = -1;
		m_nReconnectGameAttemptsLeft = -1;
		if (pErrorMessage != NULL)
			AfxMessageBox(pErrorMessage, MB_OK | MB_ICONWARNING);
	}
}

bool CMainFrame::ParseServerVersion(const char* pLines)
{
	const char* pExeVersion = strstr(pLines, "Exe version ");
	if (pExeVersion == NULL)
		return false;

	pExeVersion += 12;

	const char* pExeVersionEnd = strchr(pExeVersion, '\n');
	if (pExeVersionEnd == NULL)
		return false;

	int nLen = pExeVersionEnd - pExeVersion;
	if (nLen < 7)
		return false;

	std::string sExeVersion(pExeVersion, 7);

	int nVersion1 = atoi(sExeVersion.c_str());
	int nVersion2 = atoi(sExeVersion.c_str()+2);
	int nVersion3 = atoi(sExeVersion.c_str()+4);
	int nVersion4 = atoi(sExeVersion.c_str()+6);

	m_versionGame.m_bLinux = (nVersion1 == 3);
	m_versionGame.m_bDedicated = (nVersion1 == 3) || (nVersion1 == 4);
	m_versionGame.m_bX111 = (nVersion2 == 1) && (((nVersion3 == 1) && (nVersion4 >= 1)) || (nVersion3 > 1));
	return true;
}

void CMainFrame::OnUdpRcon(GIRLUdpMessageRcon* pRcon)
{
	CString sLines = pRcon->m_sString.c_str();
	sLines.TrimLeft();
	sLines.TrimRight();

	if (sLines.IsEmpty())
		return;

	if (sLines.Compare("Bad rcon_password.") == 0) 
	{
		OnSuddenDisconnect(false, "Disconnected from game server.\nBad RCON password");
		return;
	}

	if (m_bDuringConnection)
	{
		m_bDuringConnection = false;
		// Tried to connect, and succeeded
		if (m_serverGame.m_bReconnectToGame)
		{
			m_nReconnectGameAttemptsLeft = m_serverGame.m_nGameReconnectTimes;
			m_nReconnectGameSecLeft = -1;
		} else {
			m_nReconnectGameAttemptsLeft = -1;
			m_nReconnectGameSecLeft = -1;
		}
	}

	if (m_eTypeUsed == LQT_LOGMOD)
	{
		if (sLines.Left(20).Compare("[LogMod] EnableLog: ") == 0)
		{
			// Take care of logmod's "Key" mechanism (challenge-response)
			DWORD dwKey = atol(sLines.Mid(20));
			CString sLogModKey;
			sLogModKey.Format("\xff\xff\xff\xff" "LogMod: Key %u", dwKey);
			m_pUdpServer->SendLogMod(sLogModKey);
			// Hide this line from the user
			return;
		}
	}

	if (sLines.Left(10).Compare("hostname: ") == 0) {
		ParsePlayerList((LPCTSTR)sLines);
	} else if (sLines.Left(2).Compare("#1") == 0) {
		ParsePlayerList((LPCTSTR)sLines);
	} else if (sLines.Left(2).Compare("#2") == 0) {
		ParsePlayerList((LPCTSTR)sLines);
	} else if (sLines.Left(2).Compare("#3") == 0) {
		ParsePlayerList((LPCTSTR)sLines);
	} else if (!m_bSentRconCommands) {
		if ((sLines.Find("Protocol version ") >= 0) && (sLines.Find("Exe version ") >= 0))
		{
			// get the version information
			if (ParseServerVersion(sLines))
			{
				// and send all rcon commands. redirect log, and auto-connect commands
				in_addr LocalAddr;
				LocalAddr.S_un.S_addr = m_serverGame.m_dwLocalIP;

				switch (m_serverGame.m_eType)
				{
				case LQT_AUTODETECT:
					// user chose automatic mode.
					// decide upon a logging system based on all the info we got from the server so far

					if (m_pUdpServer->IsLogMod())
						// server supports logmod - this is the best
						m_eTypeUsed = LQT_LOGMOD;
					else if (m_versionGame.m_bX111)
						// server supports logaddress_add. this is ok
						m_eTypeUsed = LQT_LOGADDRESS_ADD;
					else
						// this is the worst case. though it still works great.
						m_eTypeUsed = LQT_LOGADDRESS;
					break;

					// make sure that the requested logging method is supported by server.
					// if not - pop up an error message
				case LQT_LOGMOD:
					if (!m_pUdpServer->IsLogMod()) 
					{
						// LogMod requested by user, but it isn't available
						OnSuddenDisconnect(false, "Disconnected from game server.\nLogMod support is not available on this game server.\nPlease select a different logging system in the game server profile");
						return;
					}
					m_eTypeUsed = m_serverGame.m_eType;
					break;
				case LQT_LOGADDRESS_ADD:
					if (!m_versionGame.m_bX111) 
					{
						// logaddress_add requested by user, but it isn't available
						OnSuddenDisconnect(false, "Disconnected from game server.\nlogaddress_add is not supported by this game server (only x.1.1.1 servers support it).\nPlease select a different logging system in the game server profile");
						return;
					}
					m_eTypeUsed = m_serverGame.m_eType;
					break;
				case LQT_LOGADDRESS:
					// nothing to check here
					m_eTypeUsed = m_serverGame.m_eType;
				}

				CString sMsg;
				// now we know what logging system to use. use it.
				switch (m_eTypeUsed)
				{
				case LQT_LOGMOD:
					// LogMod log forwarding
					m_pUdpServer->SendRcon("logmod addtmp 1 logfwd", m_serverGame.m_sRcon);
					break;
				case LQT_LOGADDRESS:
					// Regular logaddress log forwarding
					sMsg.Format("logaddress %s %u", inet_ntoa(LocalAddr), m_pUdpServer->GetLocalPort());
					m_pUdpServer->SendRcon(sMsg, m_serverGame.m_sRcon);
					break;
				case LQT_LOGADDRESS_ADD:
					// HLDS x.1.1.1 logaddress_add log forwarding
					sMsg.Format("logaddress_add %s %u", inet_ntoa(LocalAddr), m_pUdpServer->GetLocalPort());
					m_pUdpServer->SendRcon(sMsg, m_serverGame.m_sRcon);
					break;
				default:
					ASSERT(FALSE);
				}
				EnableControls();
				m_bConnected = true;

				SendFirstRconCommands();
				m_bSentRconCommands = true;
			}
		}
	}
	if ((m_pFrames[I_RCON] != NULL) && (m_pFrames[I_RCON]->m_pView != NULL))
		((CRconView*)m_pFrames[I_RCON]->m_pView)->RconLogLines(sLines);
}

void CMainFrame::OnUdpInfo(GIRLUdpMessageInfo* pMsg)
{
	if (m_pParser != NULL)
		m_pParser->SetActive(false);
	m_pParser = g_parserManager.GetParserForGame(pMsg->sGame.c_str());
	m_pParser->SetActive(true);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent) {
	case IDT_SECOND:
		// check game-server connection timeout
		if (m_bDuringConnection)
		{
			DWORD dwSec = (::GetTickCount() - m_dwConnectionStart) / 1000;
			if (dwSec >= settings.m_nGameConnectionTimeout)
			{
				// Timeout connecting to game server
				OnSuddenDisconnect(true, "Timeout waiting for Game server to respond to rcon requests.");
			}
		}
		// take care of the rare occasion in which we are suppoed to be
		// connected, but we're not running anymore.
		if (m_bConnected)
		{
			if (CheckInactivityTimeout())
			{
				OnSuddenDisconnect(true, "Disconnected from game server.\nLog inactivity timeout");
			} else 
			{
				CString sPlayers;
				if (m_pFrames[I_PLAYERS] != NULL) {
					if (m_pFrames[I_PLAYERS]->m_pView != NULL) {
						sPlayers.Format("%d / %d", ((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->m_ctlList.GetItemCount(), m_nMaxPlayers);
						m_wndStatusBar.SetPaneText(2, sPlayers);
					}
				}
			}
		}
		// if we aren't connected, but we're supposed to reconnect
		if (!m_bConnected && m_serverGame.m_bReconnectToGame)
		{
			// first make sure we didn't disconnect manually or smtg
			if (m_nReconnectGameSecLeft >= 0)
			{
				// decrease seconds left
				m_nReconnectGameSecLeft--;
				if (m_nReconnectGameSecLeft < 0)
				{
					if (m_nReconnectGameAttemptsLeft >= 0)
					{
						m_nReconnectGameAttemptsLeft--;
						DoConnect(true);
					}
				}
				if (m_pFrames[I_RAWLOG] != NULL)
				{
					CRawLogView* pView = (CRawLogView*)m_pFrames[I_RAWLOG];
					pView->SetReconnect(m_nReconnectGameSecLeft, m_nReconnectGameAttemptsLeft);
				}
			}
		}
		if (!m_bConnectedIRC && m_serverIRC.m_bReconnectToIRC)
		{
			// first make sure we didn't disconnect manually or smtg
			if (m_nReconnectIRCSecLeft >= 0)
			{
				// decrease seconds left
				m_nReconnectIRCSecLeft--;
				if (m_nReconnectIRCSecLeft < 0)
				{
					if (m_nReconnectIRCAttemptsLeft >= 0)
					{
						m_nReconnectIRCAttemptsLeft--;
						ConnectToIRCServer(true);
					}
				}
				if (m_pFrames[I_IRC] != NULL)
				{
					CRawLogView* pView = (CRawLogView*)m_pFrames[I_IRC];
					pView->SetReconnect(m_nReconnectIRCSecLeft, m_nReconnectIRCAttemptsLeft);
				}
			}
		}
		break;
	case IDT_STATUS:
		if (m_bConnected)
		{
			if (m_pFrames[I_PLAYERS] != NULL)
				if (m_pFrames[I_PLAYERS]->m_pView != NULL)
					((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->UpdatePlayerTime();
			if (m_bConnected)
				if (m_pFrames[I_SCHED] != NULL)
					if (m_pFrames[I_SCHED]->m_pView != NULL)
						((CSchedView*)m_pFrames[I_SCHED]->m_pView)->Timer();
		}
		if (m_bConnectedIRC)
		{
			CString sDelay;
			sDelay.Format("Delay: %s, %d msgs", (LPCTSTR)m_pIrcOutput->GetTotalDelay().Format("%M:%S"), m_pIrcOutput->GetTotalQueuedMsgs());
			m_wndStatusBar.SetPaneText(5, sDelay);
		} else
			m_wndStatusBar.SetPaneText(5, "");
		break;
	case IDT_COMMITWONIDS:
		// update the challange every now and then :)
		if (m_pUdpServer != NULL)
			m_pUdpServer->Send("\xff\xff\xff\xff" "challenge rcon");

		// commit changes to wonid database
		CommitWonIDs();
		break;
	case IDT_FLASH:
		FlashWindow(TRUE);
		break;
	}
	CMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::FormatTime(CTime& time, CString& s)
{
	s.Empty();
	CTime now;
	now = CTime::GetCurrentTime();

	CTimeSpan ts = now - time;

	if (ts.GetDays() > 0)
		s.Format("%02d:%02d:%02d:%02d", ts.GetDays(), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
	else if (ts.GetHours() > 0)
		s.Format("%d:%02d:%02d", ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
	else 
		s.Format("%02d:%02d", ts.GetMinutes(), ts.GetSeconds());
}

bool CMainFrame::ParsePlayerList(std::string sRes)
{
	// UI thread
	// parse the player list...
	CString sText = sRes.c_str();
	sText.TrimLeft();
	sText.TrimRight();
	int nPos;

	nPos = sText.Find(": ");
	int nQuote = sText.Find("\"");
	if ((nQuote != -1) && (nQuote < nPos))
		nPos = -1;

	if (nPos != -1) {
		ClearPlayerList();
		m_nPlayers = 0;
	}
		
	while ((nPos != -1) && (nPos <= 8)) {
		CString sKey = sText.Left(nPos);
		sText = sText.Mid(nPos+2);
		nPos = sText.FindOneOf("\r\n");
		CString sValue = sText.Left(nPos);
		sText = sText.Mid(nPos);
		sText.TrimLeft();
		nPos = sText.Find(": ");
		int nQuote = sText.Find("\"");
		if ((nQuote != -1) && (nQuote < nPos))
			nPos = -1;

		sKey.TrimRight();
		sValue.TrimLeft();

		if (sKey.Compare("hostname") == 0) {
			m_wndStatusBar.SetPaneText(0, sValue);
			if (m_pParser != NULL)
				m_pParser->SetServerName((LPCTSTR)sValue);
#if _TRACES >= 4
			char c[1024];
			sprintf(c, "Hostname: %s\n", (LPCTSTR)sValue);
			::OutputDebugString(c);
#endif
		} else if (sKey.Compare("map") == 0) {
			int nEnd = sValue.FindOneOf(" \r\n");
			m_wndStatusBar.SetPaneText(1, sValue.Left(nEnd));
			if (m_pParser != NULL)
			{
				CString sMap = sValue.Left(nEnd);
				m_wndStatusBar.SetPaneText(1, (LPCTSTR)sMap);
				m_pParser->SetMapName((LPCTSTR)sMap);
			}
#if _TRACES >= 4
			char c[1024];
			sprintf(c, "Map: %s\n", (LPCTSTR)sValue);
			::OutputDebugString(c);
#endif
		} else if (sKey.Compare("players") == 0) {
			int nPos = sValue.Find("(");
			sValue = sValue.Mid(nPos+1);
			m_nMaxPlayers = atoi((LPCTSTR)sValue);
#if _TRACES >= 4
			char c[1024];
			sprintf(c, "Players: %s\n", (LPCTSTR)sValue);
			::OutputDebugString(c);
#endif
		} else {
#if _TRACES >= 4
			char c[1024];
			sprintf(c, "Other: %s = %s\n", (LPCTSTR)sKey, (LPCTSTR)sValue);
			::OutputDebugString(c);
#endif
		}
	}

	CTime now = CTime::GetCurrentTime();

	//if (m_pParser != NULL)
	//	m_pParser->SetIRCTopic();

	nPos = 0;
	while (nPos != -1)
	{
		CString sPlayer;
		nPos = sText.FindOneOf("\r\n");
		if (nPos >= 0)
			sPlayer = sText.Left(nPos);
		else
			sPlayer = sText;
		sText = sText.Mid(nPos);
		sText.TrimLeft();
		sPlayer.TrimRight();

#if _TRACES >= 4
		char c[1024];
		sprintf(c, "Player: %s\n", (LPCTSTR)sPlayer);
		::OutputDebugString(c);
#endif

		if (!sPlayer.IsEmpty() && (sPlayer.Left(1).Compare("#") == 0) && (sPlayer.Left(3).Compare("#  ") != 0)) {
			m_nPlayers++;
			// add the player;
			CString s;
			// dispose of # sign
			sPlayer = sPlayer.Mid(1);
			// num
			s = GetNextString(sPlayer);
			// name
			s = GetNextString(sPlayer);
			CString sName = s;
			// id
			s = GetNextString(sPlayer);
			int nID = atoi((LPCTSTR)s);
			// wonid
			CString sWonID = GetNextString(sPlayer);
			// frags
			s = GetNextString(sPlayer);
			int nFrags = atoi((LPCTSTR)s);
			// time
			s = GetNextString(sPlayer);
			int nDays, nHours, nMins, nSecs;
			ParseTime(s, nDays, nHours, nMins, nSecs);

			// ping
			s = GetNextString(sPlayer);
			// loss
			s = GetNextString(sPlayer);
			// addr
			s = GetNextString(sPlayer);

			CStatsPlayer player;
			player.m_sWonID = sWonID;
			player.m_nID = nID;
			player.m_sName = sName;
			player.m_team = TEAM_NONE;

			CTimeSpan diff(nDays, nHours, nMins, nSecs);
			CTime ts = now - diff;
			UpdatePlayer(new UpdatePlayerStruct(player, 0, &ts));
		}
	}
#if _TRACES >= 4
	::OutputDebugString("Done PlayerList\n");
#endif

	CString sPlayers;
	sPlayers.Format("%d / %d", m_nPlayers, m_nMaxPlayers);
	m_wndStatusBar.SetPaneText(2, sPlayers);
	return true;
}

void CMainFrame::ClearPlayerList()
{
	ClearPlayerMap();

	if ((m_pFrames[I_PLAYERS] != NULL) && (m_pFrames[I_PLAYERS]->m_pView != NULL))
		((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->ClearPlayerList();
}

void CMainFrame::ClearSchedList()
{
	if (m_pFrames[I_SCHED] == NULL)
		return;
	if (m_pFrames[I_SCHED]->m_pView == NULL)
		return;

	((CSchedView*)m_pFrames[I_SCHED]->m_pView)->ClearList();
}

CString CMainFrame::GetNextString(CString& str)
{
	str.TrimLeft();
	if (str.IsEmpty())
		return "";
	if (str.GetAt(0) == '\"')
	{
		int nEnd = str.Find('\"', 1);
		if (nEnd < 1)
			return "";
		CString sRet = str.Mid(1, nEnd-1);
		str = str.Mid(nEnd+1);
		return sRet;
	} else {
		int nPos = str.FindOneOf("\r\n\t ");
		CString sRet = str.Left(nPos);
		str = str.Mid(nPos);
		return sRet;
	}
}

void CMainFrame::ParseTime(CString s, int& nDays, int& nHours, int& nMins, int& nSecs)
{
	nDays = 0;
	nHours = 0;
	nMins = 0;
	nSecs = 0;

	int nColon = s.ReverseFind(':');
	if (nColon < 0) {
		nSecs = atoi((LPCTSTR)s);
		return;
	}
	nSecs = atoi(s.Mid(nColon+1));
	s = s.Mid(0, nColon);

	nColon = s.ReverseFind(':');
	if (nColon < 0) {
		nMins = atoi((LPCTSTR)s);
		return;
	}
	nMins = atoi(s.Mid(nColon+1));
	s = s.Mid(0, nColon);

	nColon = s.ReverseFind(':');
	if (nColon < 0) {
		nHours = atoi((LPCTSTR)s);
		return;
	}
	nHours = atoi(s.Mid(nColon+1));
	s = s.Mid(0, nColon);

	nColon = s.ReverseFind(':');
	if (nColon < 0) {
		nDays = atoi((LPCTSTR)s);
		return;
	}
	nDays = atoi(s.Mid(nColon+1));
	s = s.Mid(0, nColon);
}

void CMainFrame::KillTimers()
{
	if (m_bHasTimers) {
		KillTimer(IDT_STATUS);
		KillTimer(IDT_SECOND);
		KillTimer(IDT_COMMITWONIDS);
		m_bHasTimers = false;
	}
}

LRESULT CMainFrame::OnUpdateFrag(WPARAM wParam, LPARAM lParam)
{
	int nID = (int)wParam;
	int nFrags = (int)lParam;
	if ((m_pFrames[I_PLAYERS] != NULL) && (m_pFrames[I_PLAYERS]->m_pView != NULL))
		((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->UpdateFrags(nID, nFrags);

	return 0;
}

LRESULT CMainFrame::OnUpdateDeath(WPARAM wParam, LPARAM lParam)
{
	// UI thread
	int nID = (int)wParam;
	int nDeaths = (int)lParam;
	if ((m_pFrames[I_PLAYERS] != NULL) && (m_pFrames[I_PLAYERS]->m_pView != NULL))
		((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->UpdateDeaths(nID, nDeaths);

	return 0;
}

bool CMainFrame::OnIrc_ENDOFNAMES(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() < 2)
		return true;

	if (pmsg->parameters[1].compare(m_serverIRC.m_sChannelName) != 0)
		return true;

	return true;
}

bool CMainFrame::OnIrc_ENDOFLIST(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() != 2)
		return true;

	if (m_pParser != NULL)
		m_pParser->SetIRCTopic();

	return true;
}

void CMainFrame::OnServerWonids() 
{
	CWonIDDatabaseDlg dlg(this);
	dlg.m_pMain = this;
	dlg.DoModal();
}

void CMainFrame::OnUpdateServerWonids(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(settings.m_bUpdateWonID);
}

void CMainFrame::CommitWonIDs()
{
	for (STR2STRLIST::iterator i = m_mWonIDs.begin(); i != m_mWonIDs.end(); i++)
	{
		for (STRLIST::iterator j = (*i).second.begin(); j != (*i).second.end(); j++)
		{
			WonIDDBManager::AddWonIDToDatabase(atol((*i).first.c_str()), (*j).c_str());
		}
	}
	m_mWonIDs.clear();
}

void CMainFrame::Deiconize()
{
	// show - to restore the task-bar button
	ShowWindow(SW_SHOW);
	// resture - to actually show the window
	ShowWindow(SW_RESTORE);
	// bring to top in case it's already shown.
	BringWindowToTop();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	switch (nType) {
	case SIZE_MINIMIZED:
		ShowWindow(SW_HIDE);
		break;
	}
}

bool CMainFrame::CreateSystemTrayIcon()
{
	// fill in the struct
	m_notify.cbSize = sizeof(m_notify);
	m_notify.hWnd = GetSafeHwnd();
	m_notify.uID = 1;
	m_notify.uCallbackMessage = UM_TRAYMSG;
	strcpy(m_notify.szTip, "CSBot");
	m_notify.hIcon = m_hIconTray;
	m_notify.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
	// and presto!
	if (!Shell_NotifyIcon(NIM_ADD, &m_notify))
		return false;
	return true;
}

LRESULT CMainFrame::OnTrayMsg(WPARAM wParam, LPARAM lParam)
{
#if _TRACES >= 6
	::OutputDebugString("Handling tray msg\n");
#endif
	// act according to a tray message
	switch (lParam) {
	case WM_LBUTTONDBLCLK:
		// primary button double-click = restore
		Deiconize();
		break;
	case WM_RBUTTONUP:
//	case WM_CONTEXTMENU:
		// secondary button click = pop up the "context menu"
		POINT pt;
		::GetCursorPos(&pt);
		m_TrayMenu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN, pt.x, pt.y, this, NULL);
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		lParam = lParam;
		break;
	}
	return 0;
}

LRESULT CMainFrame::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	// Explorer re-created the taskbar
	// we want to put the tray icon there too.
	CreateSystemTrayIcon();
	return 0;
}

void CMainFrame::OnTrayRestore()
{
	Deiconize();
}

void CMainFrame::OnUpdateViewIrc(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrames[I_IRC]->IsWindowVisible());
}

void CMainFrame::OnUpdateViewPlayers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrames[I_PLAYERS]->IsWindowVisible());
}

void CMainFrame::OnUpdateViewRawlog(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrames[I_RAWLOG]->IsWindowVisible());
}

void CMainFrame::OnUpdateViewRcon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrames[I_RCON]->IsWindowVisible());
}

void CMainFrame::OnUpdateViewSched(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrames[I_SCHED]->IsWindowVisible());
}

void CMainFrame::SendFirstRconCommands()
{
	std::string sRes;

	if (m_pUdpServer != NULL)
		m_pUdpServer->SendRcon("mp_logmessages 1", (LPCTSTR)m_serverGame.m_sRcon);

	POSITION pos = m_serverGame.m_lConnectCommands.GetHeadPosition();
	while (pos != NULL) {
		CString sCmd = m_serverGame.m_lConnectCommands.GetNext(pos);
		IssueRcon(sCmd);
	}
	FetchPlayerList();
}

bool CMainFrame::DoConnectSteps(bool bAutomatic)
{
	m_eTypeUsed = LQT_AUTODETECT;
	settings.m_sLastCSServer = m_serverGame.m_sProfileName;

	in_addr addr = ResolveServerName(bAutomatic);
	if (addr.S_un.S_addr == 0xffffffff)
		return false;

	// update our commander with the ip and port
	if (m_pUdpServer != NULL)
	{
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}

	m_bSentRconCommands = false;
	m_bSentRconTest = false;
	m_bSentRules = false;

	m_pUdpServer = new GIRLUdpServer(GetSafeHwnd(), UM_UDP, m_serverGame.m_uLogPort, ntohl(addr.S_un.S_addr), m_serverGame.m_uPort, (DWORD)this);
	m_sLastIP = inet_ntoa(addr);

	// get the server info!
	ServerInfoStruct sis;

	// sending twice, in order to try to overcome UDP's unreliability problem,
	// the server responses to these commands are crucial
	if (!m_pUdpServer->Send("\xff\xff\xff\xff" "info"))
	{
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}
	if (!m_pUdpServer->Send("\xff\xff\xff\xff" "info"))
	{
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}
	if (!m_pUdpServer->Send("\xff\xff\xff\xff" "challenge rcon"))
	{
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}
	if (!m_pUdpServer->Send("\xff\xff\xff\xff" "challenge rcon"))
	{
		delete m_pUdpServer;
		m_pUdpServer = NULL;
	}

	// connect to a default parser until the info is returned
	if (m_pParser != NULL)
		m_pParser->SetActive(false);
	m_pParser = g_parserManager.GetParserForGame("");
	m_pParser->SetActive(true);

	// allow the udp server to receive data now
	m_pUdpServer->Continue();

	return true;
}

void CMainFrame::DoConnect(bool bAutomatic)
{
	m_bDuringConnection = true;
	m_dwConnectionStart = ::GetTickCount();

	if (m_nReconnectGameAttemptsLeft == -1)
		m_nReconnectGameAttemptsLeft = m_serverGame.m_nGameReconnectTimes;

	if (!DoConnectSteps(bAutomatic))
	{
		// Tried to connect, but failed
		m_bDuringConnection = false;
		if (m_serverGame.m_bReconnectToGame)
		{
			m_nReconnectGameSecLeft = m_serverGame.m_nGameReconnectTimes;
		} else {
			m_nReconnectGameAttemptsLeft = -1;
			m_nReconnectGameSecLeft = -1;
		}
	}
	if (m_pFrames[I_RAWLOG] != NULL)
	{
		CRawLogView* pView = (CRawLogView*)m_pFrames[I_RAWLOG];
		pView->SetReconnect(m_nReconnectGameSecLeft, m_nReconnectGameAttemptsLeft);
	}
	m_tLastLogLine = CTime::GetCurrentTime();
}

void CMainFrame::Connect(LPCTSTR pProfileName, bool bGuess, bool bGuessHarder)
{
	CWaitCursor wait;
	m_serverGame.LoadFromRegistry(pProfileName);

	DWORD dwIP = 0;
	if (bGuess)
		if (settings.GuessIP(dwIP))
			m_serverGame.m_dwLocalIP = htonl(dwIP);

	if (bGuessHarder)
		if (settings.GuessIPHarder(dwIP))
			m_serverGame.m_dwLocalIP = htonl(dwIP);
	
	DoConnect(false);
}

void CMainFrame::ConnectIRC(LPCTSTR pProfileName)
{
	m_serverIRC.LoadFromRegistry(pProfileName);
	ConnectToIRCServer(false);
}

void CMainFrame::ConnectToIRCServer(bool bAutomatic)
{
	m_bIRCDisconnectRequest = false;

	settings.m_sLastIRCServer = m_serverIRC.m_sProfileName;

	m_serverIRC.m_sServerName.TrimLeft();
	m_serverIRC.m_sServerName.TrimRight();
	m_IrcInfo.sServer = m_serverIRC.m_sServerName;
	m_IrcInfo.iPort = m_serverIRC.m_uPort;
	m_IrcInfo.sNick = m_serverIRC.m_sNickName;
	m_IrcInfo.sUserID = m_serverIRC.m_sUserID;
	m_IrcInfo.sFullName = m_serverIRC.m_sFullName;
	m_IrcInfo.sPassword = m_serverIRC.m_sPassword;
	m_IrcInfo.bIdentServer = m_serverIRC.m_bIdentServer ? true : false;
	m_IrcInfo.bCloseIdentAfterQuery = m_serverIRC.m_bCloseIdentAfterQuery ? true : false;
	m_IrcInfo.pIdentMonitor = this;

	m_bSentAutoCommands = false;
	if (!g_ircSession.Connect(m_IrcInfo))
	{
		// Tried to connect, but failed
		if (m_serverIRC.m_bReconnectToIRC)
		{
			m_nReconnectIRCSecLeft = m_serverIRC.m_nIRCReconnectTimes;
		} else {
			m_nReconnectIRCAttemptsLeft = -1;
			m_nReconnectIRCSecLeft = -1;
		}

		if (!bAutomatic)
			AfxMessageBox("Could not connect to IRC server");
		return;
	}

	// Tried to connect, and succeeded
	if (m_serverIRC.m_bReconnectToIRC)
	{
		m_nReconnectIRCAttemptsLeft = m_serverIRC.m_nIRCReconnectTimes;
		m_nReconnectIRCSecLeft = -1;
	} else {
		m_nReconnectIRCAttemptsLeft = -1;
		m_nReconnectIRCSecLeft = -1;
	}
	SendPassNickUser();
	m_sLastTopic = "";

 	m_bConnectedIRC = true;
	EnableIRCControls();
}

bool CMainFrame::OnIrc_DCCMSG(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->m_pOrigin == NULL)
		return true;
	if (!pmsg->m_pOrigin->m_bDCC)
		return true;

	// no DCC allowed -> we do nothing
	if (!settings.m_bDCCChatFromIRC)
		return true;

	std::string sCmd;

	for (std::vector<String>::const_iterator i = pmsg->parameters.begin(); i!= pmsg->parameters.end(); i++)
	{
		if (!sCmd.empty())
			sCmd += " ";
		if (!(*i).empty())
			sCmd += (*i).c_str();
	}

	int nPos;
	while ((nPos = sCmd.find_first_of("\x02\x0f\x16\x03\x1f")) != std::string::npos)
		sCmd.erase(nPos, 1);
	
	ProcessIRCCommand(pmsg, sCmd, pmsg->m_pOrigin);

	return true;
}

bool CMainFrame::OnIrc_PRIVMSG(const CIrcMessage* pmsg)
{
	// we don't care about outgoing msgs.
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->prefix.sNick.compare((LPCTSTR)m_serverIRC.m_sNickName) == 0)
		return true;

	// is it destined to us?
	if (pmsg->parameters[0].compare((LPCTSTR)m_serverIRC.m_sNickName) == 0) 
	{
		// is it not from the channel
		if (stricmp(pmsg->prefix.sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
			return true;

		if (pmsg->parameters.size() != 2)
			return true;

		// unknown origin - we don't allow anything of that sort
		std::string sNick = pmsg->prefix.sNick;
		if (sNick.empty())
			return true;

		sNick = pmsg->prefix.sUser;
		if (sNick.empty())
			return true;

		std::string sCmd = pmsg->parameters[1].c_str();

		// get rid of all styling codes
		int nPos;
		while ((nPos = sCmd.find_first_of("\x02\x0f\x16\x03\x1f")) != std::string::npos)
			sCmd.erase(nPos, 1);

		// we might allow DCC without allowing PM
		if (stricmp(sCmd.substr(0, 5).c_str(), "\001dcc ") == 0) {
			return ProcessIRCCTCPDCC(pmsg, sCmd.substr(5), &g_ircSession);
		}

		// we might allow VERSION without allowing PM
		if (stricmp(sCmd.c_str(), "\001VERSION\001") == 0) {
			return ProcessIRCCTCPVersion(pmsg, sCmd, &g_ircSession);
		}

		// no PM allowed -> we do nothing
		if (!settings.m_bPMFromIRC)
			return true;

		ProcessIRCCommand(pmsg, sCmd, &g_ircSession);
	} 
	// is it destined to the channel?
	else if (pmsg->parameters[0].compare((LPCTSTR)m_serverIRC.m_sChannelName) == 0) {
		// no Channel Msgs allowed -> we do nothing
		if (!settings.m_bCMFromIRC)
			return true;

		// is it not from us?
		if (stricmp(pmsg->prefix.sNick.c_str(), (LPCTSTR)m_serverIRC.m_sNickName) == 0)
			return true;

		if (pmsg->parameters.size() != 2)
			return true;

		// unknown origin - we don't allow anything of that sort
		std::string sNick = pmsg->prefix.sNick;
		if (sNick.empty())
			return true;

		sNick = pmsg->prefix.sUser;
		if (sNick.empty())
			return true;

		std::string sCmd = pmsg->parameters[1].c_str();

		int nPos;
		while ((nPos = sCmd.find_first_of("\x02\x0f\x16\x03\x1f")) != std::string::npos)
			sCmd.erase(nPos, 1);

		// ignore empty command
		if (sCmd.size() < 1)
			return true;

		// check prefix on command
		if (!settings.m_sCMFromIRCPrefix.IsEmpty())
		{
			if (sCmd[0] != settings.m_sCMFromIRCPrefix[0])
				// prefix does not match
				return true;

			// prefix matches. remove the prefix for proper processing
			sCmd.erase(0, 1);
		}

		// check for empty command, again.
		if (sCmd.size() < 1)
			return true;

		//modify message, as if it came from the channel, so command handlers will 
		//respond to the channel
		CIrcMessage msg(*pmsg);
		msg.prefix.sNick = pmsg->parameters[0].c_str();

		ProcessIRCCommand(&msg, sCmd, &g_ircSession);
	}

	return true;
}

bool CMainFrame::ProcessIRCRcon(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// We never run rcon from a channel!
	if (sNick.find('#') == 0)
		return true;
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	// are we connected to a CS server?
	if (!m_bConnected) {
		OutputIRCMessageImmediate("Error: CSBot is currently not connected to a game server", sNick.c_str(), pSession);
		return true;
	}
	
	// are we trusted + logged in?
	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (pPerson != NULL)
	{
		if (pPerson->IsCommandAllowed(IRCCOMM_RCON)) {
			IssueRcon(sCmd.c_str());
			OutputIRCMessageImmediate("RCON Ok", sNick.c_str(), pSession);
		} else
			OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);
		return true;
	}

	// do we allow this publicly?
	if (!settings.IsCommandAllowed(IRCCOMM_RCON)) {
		OutputIRCMessageImmediate("Error: This command is not currently allowed\n", sNick.c_str(), pSession);
		return true;
	}

	int nSpace = sCmd.find(' ');
	// do we have a space?
	if ((nSpace == std::string::npos) || (nSpace < 2)) {
		OutputIRCMessageImmediate("Usage: \x02RCON <rcon-password> <command>\x0f", sNick.c_str(), pSession);
		return true;
	}

	std::string sRconPassword = sCmd.substr(0, nSpace);
	sCmd = sCmd.substr(nSpace+1);

	// is the rcon password correct?
	if (sRconPassword.compare((LPCTSTR)m_serverGame.m_sRcon) != 0) {
		OutputIRCMessageImmediate("Error: Access denied!", sNick.c_str(), pSession);
		return true;
	}

	IssueRcon(sCmd.c_str());
	OutputIRCMessageImmediate("RCON Ok", sNick.c_str(), pSession);
	return true;
}

bool CMainFrame::ProcessIRCLogin(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// Never perform login from a channel
	if (sNick.find('#') == 0)
		return true;
	// cannot use LOGIN from channel interface
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	if (!settings.IsCommandAllowed(IRCCOMM_LOGIN))
	{
		OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);
		return true;
	}

	// do we have a space?
	int nPos = sCmd.find(' ');
	if (nPos == std::string::npos) {
		OutputIRCMessageImmediate("Usage: \x02LOGIN <username> <password>\x0f", sNick.c_str(), pSession);
		return true;
	}

	std::string sUser = sCmd.substr(0, nPos);
	std::string sPass = sCmd.substr(nPos+1);

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (pPerson != NULL)
	{
		OutputIRCMessageImmediate("You are already logged in. Please LOGOUT first, and try again.", pmsg->prefix.sNick.c_str(), pSession);
		return true;
	}

	pPerson = UserLogin(pmsg, sUser, sPass, pSession);
	return true;
}

bool CMainFrame::ProcessIRCLogout(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// Never perform logout from a channel
	if (sNick.find('#') == 0)
		return true;
	// cannot use LOGOUT from channel interface
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	if (pSession->m_bDCC)
		sNick = pSession->m_sNick;
	std::string sUserID = pmsg->prefix.sUser;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (pPerson != NULL)
	{
		pPerson->m_bLoggedIn = false;
		pPerson->m_sUserID = "";
		OutputIRCMessageImmediate("You are now logged out", sNick.c_str(), pSession);
		return true;
	}

	OutputIRCMessageImmediate("Error: You have not logged in", sNick.c_str(), pSession);

	return true;
}

bool CMainFrame::ProcessIRCHelp(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;
	bool bFromChannel = (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0);

	CString sMsg;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (((pPerson == NULL) && settings.IsCommandAllowed(IRCCOMM_HELP)) || bFromChannel)
	{
		OutputIRCMessageImmediate("CSBot quick help. Commands:", sNick.c_str(), pSession);
		OutputIRCMessageImmediate("  \x02HELP\x0f - Show this quick help message", sNick.c_str(), pSession);
		OutputIRCMessageImmediate("  \x02VERSION\x0f - CSBot version information", sNick.c_str(), pSession);
		if (settings.IsCommandAllowed(IRCCOMM_INFO))
			OutputIRCMessageImmediate("  \x02INFO\x0f - Game server info", sNick.c_str(), pSession);
		if (settings.IsCommandAllowed(IRCCOMM_PLAYER))
			OutputIRCMessageImmediate("  \x02PLAYER <nickname | id>\x0f - Player info", sNick.c_str(), pSession);
		if (settings.IsCommandAllowed(IRCCOMM_TEAMS))
			OutputIRCMessageImmediate("  \x02TEAMS\x0f - Teams info (player list)", sNick.c_str(), pSession);
		if (settings.IsCommandAllowed(IRCCOMM_RCON))
			OutputIRCMessageImmediate("  \x02RCON <rcon-pass> <command>\x0f - Send RCON command to server", sNick.c_str(), pSession);
		if (!bFromChannel)
			OutputIRCMessageImmediate("  \x02LOGIN <username> <password>\x0f - Login to CSBot", sNick.c_str(), pSession);
	} else if ((pPerson != NULL) && pPerson->IsCommandAllowed(IRCCOMM_HELP))
	{
		OutputIRCMessageImmediate("CSBot quick help for logged-in user. Commands:", sNick.c_str(), pSession);
		OutputIRCMessageImmediate("  \x02HELP\x0f - Show this quick help message", sNick.c_str(), pSession);
		OutputIRCMessageImmediate("  \x02VERSION\x0f - CSBot version information", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_INFO))
			OutputIRCMessageImmediate("  \x02" "INFO\x0f - Game server info", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_PLAYER))
			OutputIRCMessageImmediate("  \x02" "PLAYER <nickname | id>\x0f - Player info", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_TEAMS))
			OutputIRCMessageImmediate("  \x02" "TEAMS\x0f - Teams info (player list)", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_RCON))
			OutputIRCMessageImmediate("  \x02" "RCON <command>\x0f - Send RCON command to server", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_OP))
			OutputIRCMessageImmediate("  \x02" "OP [nickname]\x0f - sets +o for nickname in the channel, or self if ommited", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_DEOP))
			OutputIRCMessageImmediate("  \x02" "DEOP [nickname]\x0f - sets -o for nickname in the channel, or self if ommited", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_VOICE))
			OutputIRCMessageImmediate("  \x02" "VOICE [nickname]\x0f - sets +v for nickname in the channel, or self if ommited", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_DEVOICE))
			OutputIRCMessageImmediate("  \x02" "DEVOICE [nickname]\x0f - sets -v for nickname in the channel, or self if ommited", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_SAY))
			OutputIRCMessageImmediate("  \x02" "SAY <text>\x0f - makes CSBot say something on the channel", sNick.c_str(), pSession);
		if (pPerson->IsCommandAllowed(IRCCOMM_ACT))
			OutputIRCMessageImmediate("  \x02" "ACT <action>\x0f - makes CSBot do something in the channel", sNick.c_str(), pSession);
		OutputIRCMessageImmediate("  \x02LOGOUT\x0f - Logout from CSBot", sNick.c_str(), pSession);
	} else 
		OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);

	return true;
}

bool CMainFrame::ProcessIRCVersion(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;
	CString sMsg;

	OutputIRCMessageImmediate(CSBOT_VERSIONINFO ", " CSBOT_COPYRIGHT ". " CSBOT_WEBSITE "\n", sNick.c_str(), pSession);
	OutputIRCMessageImmediate("Original idea by Eternal_Bliss. A United Admins project: http://www.unitedadmins.com\n", sNick.c_str(), pSession);
	return true;
}

bool CMainFrame::ProcessIRCCTCPVersion(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;
	std::string sMsg = "NOTICE " + sNick + " :\001VERSION " CSBOT_VERSIONINFO ", " CSBOT_COPYRIGHT " " CSBOT_WEBSITE "\001";
	*pSession << sMsg.c_str();
	return true;
}

bool CMainFrame::ProcessIRCInfo(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	CString sMsg;
	std::string sNick = pmsg->prefix.sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (((pPerson != NULL) && pPerson->IsCommandAllowed(IRCCOMM_INFO)) ||
		((pPerson == NULL) && settings.IsCommandAllowed(IRCCOMM_INFO)))
	{
		// are we connected to a CS server?
		if (!m_bConnected) {
			OutputIRCMessageImmediate("Error: CSBot is currently not connected to a game server\n", sNick.c_str(), pSession);
			return true;
		}
		
		if ((m_pFrames[I_PLAYERS] == NULL) || (m_pFrames[I_PLAYERS]->m_pView == NULL)) {
			OutputIRCMessageImmediate("Error: Internal CSBot error 4\n", sNick.c_str(), pSession);
			return true;
		}

		CPlayerListView* pView = (CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView;

		sMsg.Format("Server: %s:%d\n", m_serverGame.m_sServerName, m_serverGame.m_uPort);
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		if (m_pParser != NULL)
		{
			sMsg.Format("Game: %s\n", m_pParser->GameTypeDescription());
			OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
			sMsg.Format("Server name: %s\n", m_pParser->GetServerName().c_str());
			OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
			sMsg.Format("Map: %s\n", m_pParser->GetMapName().c_str());
			OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		}
		sMsg.Format("Number of players: %d / %d\n", pView->m_ctlList.GetItemCount(), m_nMaxPlayers);
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
	} 
	else 
		OutputIRCMessageImmediate("Error: Access Denied\n", sNick.c_str(), pSession);

	return true;
}

bool CMainFrame::ProcessIRCPlayer(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	CString sMsg;
	std::string sNick = pmsg->prefix.sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (((pPerson != NULL) && pPerson->IsCommandAllowed(IRCCOMM_PLAYER)) ||
		((pPerson == NULL) && settings.IsCommandAllowed(IRCCOMM_PLAYER)))
	{
		// are we connected to a CS server?
		if (!m_bConnected) {
			OutputIRCMessageImmediate("Error: CSBot is currently not connected to a game server\n", sNick.c_str(), pSession);
			return true;
		}

		//find player
		if ((m_pFrames[I_PLAYERS] == NULL) || (m_pFrames[I_PLAYERS]->m_pView == NULL)) {
			OutputIRCMessageImmediate("Error: Internal CSBot error 1\n", sNick.c_str(), pSession);
			return true;
		}

		PlayerItemData data;
		{
			IGCLock lock(&m_lockPlayers, "ProcessIRCPlayer");

			PLAYERMAP::iterator iPlayer = m_mPlayers.find(atoi(sCmd.c_str()));
			if (iPlayer == m_mPlayers.end())
			{
				for (iPlayer = m_mPlayers.begin(); iPlayer != m_mPlayers.end(); iPlayer++)
				{
					if (((*iPlayer).second)->sName.CompareNoCase(sCmd.c_str()) == 0)
						break;
				}
			}

			if (iPlayer == m_mPlayers.end())
			{
				m_lockPlayers.Unlock();
				OutputIRCMessageImmediate("No such player\n", sNick.c_str(), pSession);
				return true;
			}

			data.Assign(*(*iPlayer).second);
		}

		CTimeSpan tSpan = CTime::GetCurrentTime() - data.time;

		sMsg.Format("Player name: %s\n", (LPCTSTR)data.sName);
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		sMsg.Format("Player ID: %d\n", data.uID);
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);

		CString sTeam;
		if (m_pParser != NULL)
			sTeam = m_pParser->TeamDescription(data.team);
		if (!sTeam.IsEmpty())
		{
			sMsg.Format("Team: %s\n", (LPCTSTR)sTeam);
			OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		}

		if (!data.sRole.IsEmpty())
		{
			sMsg.Format("Role: %s\n", (LPCTSTR)data.sRole);
			OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		}
		sMsg.Format("Time on server: %02dh:%02dm\n", tSpan.GetHours(), tSpan.GetMinutes());
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
		sMsg.Format("Frags/Deaths: %d / %d\n", data.nFrags, data.nDeaths);
		OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
	}
	else
		OutputIRCMessageImmediate("Error: Access Denied\n", sNick.c_str(), pSession);

	return true;
}

bool CMainFrame::ProcessIRCTeams(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	CString sMsg;
	std::string sNick = pmsg->prefix.sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if (((pPerson != NULL) && pPerson->IsCommandAllowed(IRCCOMM_TEAMS)) ||
		((pPerson == NULL) && settings.IsCommandAllowed(IRCCOMM_TEAMS)))
	{
		// are we connected to a CS server?
		if (!m_bConnected) {
			OutputIRCMessageImmediate("Error: CSBot is currently not connected to a game server\n", sNick.c_str(), pSession);
			return true;
		}

		if (m_pParser == NULL)
		{
			OutputIRCMessageImmediate("Error: Internal CSBot error 134\n", sNick.c_str(), pSession);
			return true;
		}

		//find player
		if ((m_pFrames[I_PLAYERS] == NULL) || (m_pFrames[I_PLAYERS]->m_pView == NULL)) {
			OutputIRCMessageImmediate("Error: Internal CSBot error 1\n", sNick.c_str(), pSession);
			return true;
		}

		CPlayerListView* pView = (CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView;

		int nCount = pView->m_ctlList.GetItemCount();
		PlayerItemData* pData;

		INT2STR mPlayers[TEAM_COUNT];
		for (int i=0; i < nCount; i++)
		{
			CString sName = pView->m_ctlList.GetItemText(i, 1);
			pData = (PlayerItemData*)pView->m_ctlList.GetItemData(i);
			mPlayers[(int)pData->team][pData->uID] = (LPCTSTR)sName;
		}

		for (int nTeam=1; nTeam<TEAM_COUNT; nTeam++)
		{
			if (!mPlayers[nTeam].empty())
			{
				LPCTSTR pTeamDesc = m_pParser->TeamDescription((Team)nTeam);
				if (*pTeamDesc != '\0')
				{
					sMsg.Format("%s team(%d):\n", m_pParser->TeamDescription((Team)nTeam), mPlayers[nTeam].size());
					OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
					for (INT2STR::iterator it = mPlayers[nTeam].begin(); it != mPlayers[nTeam].end(); it++)
					{
						sMsg.Format("  %d, %s\n", (*it).first, m_pParser->Decorate((*it).second.c_str(), (Team)nTeam).c_str());
						OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
					}
				}
			}
		}

		nTeam = 0;
		if (!mPlayers[nTeam].empty())
		{
			LPCTSTR pTeamDesc = m_pParser->TeamDescription((Team)nTeam);
			if (*pTeamDesc != '\0')
			{
				sMsg.Format("%s team(%d):\n", m_pParser->TeamDescription((Team)nTeam), mPlayers[nTeam].size());
				OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
				for (INT2STR::iterator it = mPlayers[nTeam].begin(); it != mPlayers[nTeam].end(); it++)
				{
					sMsg.Format("  %d, %s\n", (*it).first, m_pParser->Decorate((*it).second.c_str(), (Team)nTeam).c_str());
					OutputIRCMessageImmediate(sMsg, sNick.c_str(), pSession);
				}
			}
		}
	}
	else
		OutputIRCMessageImmediate("Error: Access Denied\n", sNick.c_str(), pSession);

	return true;
}

bool CMainFrame::ProcessIRCCTCPDCC(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;
	sCmd = sCmd.substr(0, sCmd.size()-1);
	if (stricmp(sCmd.substr(0, 5).c_str(), "chat ") == 0) {
		return ProcessIRCChat(pmsg, sCmd.substr(5), &g_ircSession);
	}
	return true;
}

bool CMainFrame::ProcessIRCChat(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	CString sMsg;
	std::string sNick = pmsg->prefix.sNick;

	// check protocol
	std::string sProtocol = sCmd.substr(0, sCmd.find(' '));
	if (sProtocol.compare("chat") != 0) {
		sMsg.Format("PRIVMSG %s :\001ERRMSG DCC %s unavailable\001\n", sNick.c_str(), sProtocol.c_str());
		*pSession << (LPCTSTR)sMsg;
		return true;
	}
	sCmd = sCmd.substr(5);

	// do we allow this?
	if (!settings.m_bDCCChatFromIRC) {
		sMsg.Format("PRIVMSG %s :\001ERRMSG DCC chat declined\001\n", sNick.c_str());
		*pSession << (LPCTSTR)sMsg;
		return true;
	}

	DWORD dwIP = atol(sCmd.c_str());
	sCmd = sCmd.substr(sCmd.find(' ')+1);
	UINT uPort = atol(sCmd.c_str());
	
	CIrcSession* pNewSession = new CIrcSession;

	CIrcSessionInfo info;
	in_addr addr;
	addr.S_un.S_addr = htonl(dwIP);
	info.sServer = inet_ntoa(addr);
	info.iPort = uPort;
	info.sNick = m_serverIRC.m_sNickName;
	info.sUserID = m_serverIRC.m_sUserID;
	info.sFullName = m_serverIRC.m_sFullName;
	info.bIdentServer = false;
	pNewSession->AddMonitor(this);
	pNewSession->m_bDCC = true;

	if (!pNewSession->Connect(info))
	{
		sMsg.Format("PRIVMSG %s :\001ERRMSG DCC chat declined\001\n", sNick.c_str());
		*pSession << (LPCTSTR)sMsg;
		return true;
	}

	m_lSessions.push_back(pNewSession);
	return true;
}

bool CMainFrame::ProcessIRCCommand(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sCommand = sCmd;
	int nSpace = sCommand.find(' ');
	if (nSpace != std::string::npos)
	{
		sCmd = sCommand.substr(nSpace+1);
		sCommand = sCommand.substr(0, nSpace);
	} else {
		sCmd = "";
	}

	for (int nCommand = 0; g_pCommandNames[nCommand] != NULL; nCommand++)
	{
		if (stricmp(sCommand.c_str(), g_pCommandNames[nCommand]) == 0)
		{
			IRCCommand cmd = (IRCCommand)nCommand;
			switch (cmd)
			{
			case IRCCOMM_VERSION:
				return ProcessIRCVersion(pmsg, sCmd, pSession);
			case IRCCOMM_RCON:
				return ProcessIRCRcon(pmsg, sCmd, pSession);
			case IRCCOMM_LOGIN:
				return ProcessIRCLogin(pmsg, sCmd, pSession);
			case IRCCOMM_LOGOUT:
				return ProcessIRCLogout(pmsg, sCmd, pSession);
			case IRCCOMM_INFO:
				return ProcessIRCInfo(pmsg, sCmd, pSession);
			case IRCCOMM_TEAMS:
				return ProcessIRCTeams(pmsg, sCmd, pSession);
			case IRCCOMM_PLAYER:
				return ProcessIRCPlayer(pmsg, sCmd, pSession);
			case IRCCOMM_HELP:
				return ProcessIRCHelp(pmsg, sCmd, pSession);
			case IRCCOMM_OP:
				return ProcessIRCOp(pmsg, sCmd, pSession);
			case IRCCOMM_DEOP:
				return ProcessIRCDeop(pmsg, sCmd, pSession);
			case IRCCOMM_VOICE:
				return ProcessIRCVoice(pmsg, sCmd, pSession);
			case IRCCOMM_DEVOICE:
				return ProcessIRCDevoice(pmsg, sCmd, pSession);
			case IRCCOMM_SAY:
				return ProcessIRCSay(pmsg, sCmd, pSession);
			case IRCCOMM_ACT:
				return ProcessIRCAct(pmsg, sCmd, pSession);
			}
		}
	}


	std::string sNick = pmsg->prefix.sNick;
	// is it a normal chat msg in the channel? then return.
	if (!pSession->m_bDCC && (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)) 
		return true;

	// some unknown command in PM or DCC...
	OutputIRCMessageImmediate("Invalid command. Type HELP for a short overview of commands\n", sNick.c_str(), pSession);

	return true;
}

void CMainFrame::Summon(SummonStruct* pSummon)
{
	PostMessage(UM_SUMMON, (WPARAM)pSummon);
}

LRESULT CMainFrame::OnSummon(WPARAM wParam, LPARAM lParam)
{
#if _TRACES >= 5
	::OutputDebugString("Handling summon\n");
#endif
	SummonStruct* pSummon = (SummonStruct*)wParam;

	if (!pSummon->m_bTeam && !settings.m_bAllowSummon)
	{
		delete pSummon;
		return 0;
	}
	if (pSummon->m_bTeam && !settings.m_bAllowSummonTeam)
	{
		delete pSummon;
		return 0;
	}

	if (pSummon->m_sLine.find(settings.m_sSummonWord) == std::string::npos)
	{
		delete pSummon;
		return 0;
	}

	if (settings.m_bSummonFlash) {
		if (!m_bHasFlashTimer) {
			// show - to restore the task-bar button
			ShowWindow(SW_SHOW);
			// flash continously until it is foreground
			SetTimer(IDT_FLASH, 500, NULL);
			m_bHasFlashTimer = true;
		}
	}
	if (settings.m_bSummonPlay) {
		::PlaySound((LPCTSTR)settings.m_sSummonPlayFile, NULL, SND_FILENAME | SND_NOWAIT | SND_ASYNC);
	}

	if (m_bConnectedIRC) {
		POSITION pos = settings.m_lSummonLines.GetHeadPosition();
		while (pos != NULL)
		{
			CString sCmd = settings.m_lSummonLines.GetNext(pos);
			sCmd.TrimLeft();
			sCmd.TrimRight();

			sCmd.Replace("<say>", pSummon->m_sLine.c_str());
			sCmd.Replace("<name>", pSummon->m_player.m_sName.c_str());
			if (m_pParser != NULL)
				sCmd.Replace("<team>", m_pParser->TeamName(pSummon->m_player.m_team));
			else
				sCmd.Replace("<team>", "");
			sCmd.Replace("<role>", pSummon->m_player.m_sRole.c_str());
			TranslateIRCCommand(sCmd);

			if (!sCmd.IsEmpty()) {
				sCmd += "\n";
				g_ircSession << (LPCTSTR)sCmd;
			}
		}
	}
	delete pSummon;
	return 0;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	if ((nState == WA_ACTIVE) || (nState = WA_CLICKACTIVE))
		StopFlash();
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CMainFrame::StopFlash()
{
	if (m_bHasFlashTimer) {
		FlashWindow(FALSE);
		KillTimer(IDT_FLASH);
		m_bHasFlashTimer = false;
	}
}

bool CMainFrame::OnIrc_513(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() < 2)
		return true;

	
	LPCTSTR pPong = strstr(pmsg->parameters[1].c_str(), "/QUOTE PONG ");
	if (pPong == NULL)
		return true;

	pPong += 12;

	char cMsg[1024];
	sprintf(cMsg, "PONG %s\n", pPong);

	g_ircSession << cMsg;

	return true;
}

CString MakeParameters(CString sStr, int nParams)
{
	if (nParams == 1)
	{
		sStr.Insert(0, ':');
		return sStr;
	}
	int nPos = 0;
	int nParam = 1;
	nPos = sStr.Find(' ');
	while (nPos >= 0) {
		nParam++;
		if (nParam >= nParams) {
			sStr.Insert(nPos+1, ':');
			return sStr;
		}
		nPos = sStr.Find(' ', nPos+1);
	}
	return sStr;
}

CString GetParameter(CString& sStr, int nParams)
{
	int nOld = 0;
	int nPos = 0;
	int nParam = 0;
	nPos = sStr.Find(' ');
	while (nPos >= 0) {
		nParam++;
		if (nParam >= nParams) {
			sStr.Insert(nPos+1, ':');
			if (nPos >= 0)
				return sStr.Mid(nOld, nPos-nOld);
			else
				return sStr.Mid(nOld);
		}
		nOld = nPos;
		nPos = sStr.Find(' ');
	}
	return "";
}

void CMainFrame::TranslateIRCCommand(CString& sCmd)
{
	if (sCmd.GetAt(0) == '/') {
		sCmd.Delete(0, 1);
		int nPos = sCmd.Find(' ');
		if (nPos >= 0) {
			CString sMeta = sCmd.Left(nPos);
			CString sAfterMeta = sCmd.Mid(nPos+1);
			sAfterMeta.TrimLeft();
			sAfterMeta.TrimRight();
			if (sMeta.CompareNoCase("msg") == 0) {
				sCmd = "PRIVMSG " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("notice") == 0) {
				sCmd = "NOTICE " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("oper") == 0) {
				sCmd = "OPER " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("quit") == 0) {
				sCmd = "QUIT " + MakeParameters(sAfterMeta, 1);
			} else if (sMeta.CompareNoCase("join") == 0) {
				sCmd = "JOIN " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("part") == 0) {
				sCmd = "PART " + MakeParameters(sAfterMeta, 1);
			} else if (sMeta.CompareNoCase("mode") == 0) {
				sCmd = "MODE " + MakeParameters(sAfterMeta, 5);
			} else if (sMeta.CompareNoCase("topic") == 0) {
				sCmd = "TOPIC " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("names") == 0) {
				sCmd = "NAMES " + MakeParameters(sAfterMeta, 1);
			} else if (sMeta.CompareNoCase("list") == 0) {
				sCmd = "LIST " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("invite") == 0) {
				sCmd = "INVITE " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("kick") == 0) {
				sCmd = "KICK " + MakeParameters(sAfterMeta, 3);
			} else if (sMeta.CompareNoCase("me") == 0) {
				sAfterMeta = m_serverIRC.m_sChannelName + " \001ACTION " + sAfterMeta + "\001";
				sCmd = "PRIVMSG " + MakeParameters(sAfterMeta, 2);
			} else if (sMeta.CompareNoCase("describe") == 0) {
				int nSpace = sAfterMeta.Find(' ');
				if (nSpace >= 0)
				{
					CString sTo = sAfterMeta.Left(nSpace);
					sAfterMeta.Delete(0, nSpace);
					sAfterMeta.TrimLeft();
					sAfterMeta.TrimRight();
					sAfterMeta = " \001ACTION " + sAfterMeta + "\001";
					sCmd = "PRIVMSG " + sTo + " " + MakeParameters(sAfterMeta, 1);
				}
			} else if (sMeta.CompareNoCase("ctcp") == 0) {
				sAfterMeta = MakeParameters(sAfterMeta, 2);
				int nPos = sAfterMeta.Find(" :");
				ASSERT(nPos >= 0);
				CString sNick = sAfterMeta.Left(nPos);
				CString sMsg = sAfterMeta.Mid(nPos+2);
				sMsg.MakeUpper();
				sCmd = "PRIVMSG " + sNick + " :\001" + sMsg + "\001";
			} else if (sMeta.CompareNoCase("quote") == 0)
				sCmd = sAfterMeta;
		}
	} else {
		CString sMeta = m_serverIRC.m_sChannelName + " " + sCmd;
		sCmd = "PRIVMSG " + MakeParameters(sMeta, 2);
	}
}

void CMainFrame::TranslateGameCommand(CString& sCmd)
{
	CString sTemp = sCmd;
	sTemp.MakeLower();
	if (sTemp.Find("admin_") == 0)
	{
		if (sTemp.Find("admin_command ") != 0)
		{
			sCmd = "admin_command " + sCmd;
		}
	}
}

void CMainFrame::SetMapName(SetMapNameStruct* pMapName)
{
	PostMessage(UM_SETMAPNAME, (WPARAM)pMapName);
}

LRESULT CMainFrame::OnSetMapName(WPARAM wParam, LPARAM lParam)
{
#if _TRACES >= 5
	::OutputDebugString("Handling map name\n");
#endif
	SetMapNameStruct* pMapName = (SetMapNameStruct*)wParam;
	CString sMap;
	m_wndStatusBar.GetPaneText(1, sMap);
	if (sMap.Compare(pMapName->sMapName.c_str()) != 0)
	{
		m_wndStatusBar.SetPaneText(1, pMapName->sMapName.c_str());
		ClearPlayerList();
	}
	delete pMapName;
	return 0;
}

BOOL CMainFrame::OnQueryEndSession() 
{
	if (!CMDIFrameWnd::OnQueryEndSession())
		return FALSE;

	return TRUE;
}

bool CMainFrame::OnIrc_CHANTOPIC(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->parameters.size() != 3)
		return true;

	if (stricmp(pmsg->parameters[1].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) != 0)
		return true;

	m_sLastTopic = pmsg->parameters[2].c_str();
	return true;
}

bool CMainFrame::OnIrc_CHANMODE(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() < 3)
		return true;

	if (stricmp(pmsg->parameters[1].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) != 0)
		return true;

	std::string sModes;
	sModes = pmsg->parameters[2];

	if (!sModes.empty())
	{
		if (sModes[0] == '+')
		{
			IGCLock lock(this, "OnIrc_CHANMODE");
			m_sChannelModes = sModes.c_str()+1;
		}
	}
	return true;
}

bool CMainFrame::OnIrc_CHANMODES(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() != 4)
		return true;

	if (stricmp(pmsg->parameters[1].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) != 0)
		return true;

	std::string sTopic;
	sTopic = pmsg->parameters[3];
/*	if (sTopic.size() > 2)
	{
		if (sTopic[0] == '[')
		{
			int nClose = sTopic.find(']');
			if ((nClose != std::string::npos) && (nClose > 3))
			{
				sTopic = sTopic.substr(nClose+2);
			}
		}
	}*/

	if (m_sLastTopic.IsEmpty())
		m_sLastTopic = sTopic.c_str();

	//322  Testing #testing 2 [+nt] Topic!
	//322  Testing #Testing 2  sdfsdf 
	//322  Testing #Testing 1
	return true;
}

bool CMainFrame::OnIrc_TOPIC(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;

	if (pmsg->parameters.size() != 2)
		return true;

	if (pmsg->parameters[0].compare(m_serverIRC.m_sChannelName) != 0)
		return true;

	m_sLastTopic = pmsg->parameters[1].c_str();

	return true;
}

bool CMainFrame::OnIrc_MODE(const CIrcMessage* pmsg)
{
	if (!pmsg->m_bIncoming)
		return true;
	if (pmsg->parameters.size() == 3)
	{
		// mode change with parameter (+b, +k, +o, etc)
		if (stricmp(pmsg->parameters[0].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) != 0)
			return true;
		if (stricmp(pmsg->parameters[2].c_str(), (LPCTSTR)m_serverIRC.m_sNickName) != 0)
			return true;
		std::string sModes = pmsg->parameters[1];
		if (sModes.size() >= 2)
		{
			if ((sModes[0] == '+') && (sModes.find('o') != std::string::npos))
			{
				// we were opped!
				if (m_sChannelModes.Find('t') >= 0)
					if (m_pParser != NULL)
						m_pParser->SetIRCTopic();
			}
		}
	} else if (pmsg->parameters.size() == 2)
	{
		// mode change in general (+t, +n, +c. etc)
		if (stricmp(pmsg->parameters[0].c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) != 0)
			return true;
		std::string sModes = pmsg->parameters[1];
		if (sModes.size() >= 2)
		{
			bool bPlus = (sModes[0] == '+');
			int nFind;
			IGCLock lock(this, "OnIrc_MODE");
			for (std::string::size_type uPos = 1; uPos < sModes.size(); uPos++)
			{
				nFind = m_sChannelModes.Find(sModes[uPos]);
				if (bPlus && (nFind < 0))
					m_sChannelModes.Insert(0, sModes[uPos]);
				else if (!bPlus && (nFind >= 0))
					m_sChannelModes.Delete(nFind, 1);
			}
		}
	}
	return true;
}

bool CMainFrame::ProcessIRCModeCommand(const char* pMode, int nCommand, const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// Never perform from a channel
	if (sNick.find('#') == 0)
		return true;
	// cannot use from channel interface
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	if (pSession->m_bDCC)
		sNick = pSession->m_sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if ((pPerson == NULL) || !pPerson->IsCommandAllowed(nCommand))
	{
		OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);
		return true;
	}

	std::string sTarget = sCmd;
	if (sCmd.empty())
		sTarget = sNick;

	CString sMsg;
	sMsg.Format("MODE %s %s :%s", (LPCTSTR)m_serverIRC.m_sChannelName, pMode, sTarget.c_str());
	g_ircSession << sMsg;

	return true;
}

bool CMainFrame::ProcessIRCOp(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	return ProcessIRCModeCommand("+o", IRCCOMM_OP, pmsg, sCmd, pSession);
}

bool CMainFrame::ProcessIRCDeop(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	return ProcessIRCModeCommand("-o", IRCCOMM_DEOP, pmsg, sCmd, pSession);
}

bool CMainFrame::ProcessIRCVoice(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	return ProcessIRCModeCommand("+v", IRCCOMM_VOICE, pmsg, sCmd, pSession);
}

bool CMainFrame::ProcessIRCDevoice(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	return ProcessIRCModeCommand("-v", IRCCOMM_DEVOICE, pmsg, sCmd, pSession);
}

bool CMainFrame::ProcessIRCSay(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// Never perform from a channel
	if (sNick.find('#') == 0)
		return true;
	// cannot use from channel interface
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	// can't say nothing...
	if (sCmd.empty())
		return true;

	if (pSession->m_bDCC)
		sNick = pSession->m_sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if ((pPerson == NULL) || !pPerson->IsCommandAllowed(IRCCOMM_SAY))
	{
		OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);
		return true;
	}

	OutputIRCMessageImmediate(sCmd.c_str());

	return true;
}

bool CMainFrame::ProcessIRCAct(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;

	// Never perform from a channel
	if (sNick.find('#') == 0)
		return true;
	// cannot use from channel interface
	if (stricmp(sNick.c_str(), (LPCTSTR)m_serverIRC.m_sChannelName) == 0)
		return true;

	// can't say nothing...
	if (sCmd.empty())
		return true;

	if (pSession->m_bDCC)
		sNick = pSession->m_sNick;

	TrustedPerson* pPerson = IsUserLoggedIn(pmsg);
	if ((pPerson == NULL) || !pPerson->IsCommandAllowed(IRCCOMM_SAY))
	{
		OutputIRCMessageImmediate("Error: Access denied", sNick.c_str(), pSession);
		return true;
	}

	CString sMsg = "\001ACTION ";
	sMsg += sCmd.c_str();
	sMsg += "\001";
	OutputIRCMessageImmediate(sMsg);

	return true;
}

TrustedPerson* CMainFrame::IsUserLoggedIn(const CIrcMessage* pmsg)
{
	std::string sUserID = pmsg->prefix.sUser;
	for (PERSONLIST::iterator i = settings.m_lTrusted.begin(); i != settings.m_lTrusted.end(); i++)
	{
		TrustedPerson* pPerson = (*i);
		if (pPerson != NULL)
		{
			if (pPerson->m_bLoggedIn && (pPerson->m_sUserID.Compare(pmsg->prefix.sUser.c_str()) == 0)) {
				return pPerson;
			}
		}
	}
	return NULL;
}

void CMainFrame::IssueRcon(const CString& sCommand)
{
	CString sCmd = sCommand;
	TranslateGameCommand(sCmd);
	if (m_pUdpServer != NULL)
		m_pUdpServer->SendRcon(sCmd, m_serverGame.m_sRcon);
}

TrustedPerson* CMainFrame::UserLogin(const CIrcMessage* pmsg, const std::string& sUser, const std::string& sPass, CIrcSession* pSession)
{
	std::string sNick = pmsg->prefix.sNick;
	for (PERSONLIST::iterator i = settings.m_lTrusted.begin(); i != settings.m_lTrusted.end(); i++)
	{
		TrustedPerson* pPerson = (*i);
		if (pPerson != NULL)
		{
			if ((pPerson->m_sUsername.Compare(sUser.c_str()) == 0) && (pPerson->m_sPass.Compare(sPass.c_str()) == 0)) {
				if (pPerson->m_bAllowLogin) {
					CString sMsg;
					if ((pPerson->m_bLoggedIn) && (pPerson->m_sUserID.Compare(pmsg->prefix.sUser.c_str()) != 0)) {
						// let the old logged in person know he was kicked
						sMsg.Format("%s has logged in instead of you. you are now logged out.", pmsg->prefix.sNick.c_str());
						OutputIRCMessageImmediate(sMsg, (LPCTSTR)pPerson->m_sCurrentNick, pSession);
					}
					pPerson->m_bLoggedIn = true;
					pPerson->m_sUserID = pmsg->prefix.sUser.c_str();
					pPerson->m_tLoggedIn = CTime::GetCurrentTime();
					pPerson->m_sCurrentNick = pmsg->prefix.sNick.c_str();
					if (pSession->m_bDCC)
					{
						pSession->m_sNick = sUser;
					}
					OutputIRCMessageImmediate("You are now logged in", sNick.c_str(), pSession);
				} else {
					CString sMsg;
					OutputIRCMessageImmediate("Error: You have no permission to login", sNick.c_str(), pSession);
				}
				return pPerson;
			}
		}
	}

	OutputIRCMessageImmediate("Error: Password incorrect", sNick.c_str(), pSession);
	return NULL;
}

void CMainFrame::OnMute() 
{
	m_bMute = !m_bMute;	

	TBBUTTONINFO info;
	info.cbSize = sizeof(info);
	info.dwMask = TBIF_IMAGE;
	info.iImage = m_bMute ? m_nMuteImage : m_nUnmuteImage;

	m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_MUTE, &info);
}

void CMainFrame::OnUpdateMute(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bMute ? TRUE : FALSE);
}

void CMainFrame::SetIRCTopic(const CString& sTopic)
{
	if (m_bConnectedIRC && m_bJoinedIRC && !m_bMute)
	{
		if (m_sLastTopic != sTopic) {
			CString s;
			s.Format("TOPIC %s :%s\n", (LPCTSTR)m_serverIRC.m_sChannelName, (LPCTSTR)sTopic);
			m_pIrcOutput->QueueOutputMsg(s);
		}
	}
}

CString CMainFrame::GetChannelModes()
{
	IGCLock lock(this, "GetChannelModes");
	return m_sChannelModes;
}

CString CMainFrame::GetServerName()
{
	// can't really change anywhere while logger is running
	return m_serverGame.m_sServerName;
}

UINT CMainFrame::GetServerPort()
{
	// can't really change anywhere while logger is running
	return m_serverGame.m_uPort;
}

CString CMainFrame::GetLastIP()
{
	// can't really change anywhere while logger is running
	return m_sLastIP;
}

void CMainFrame::UpdatePlayer(UpdatePlayerStruct* pUpdate)
{
	GIRLLogReceiver::UpdatePlayer(pUpdate);
	PostMessage(UM_UPDATEPLAYER, (WPARAM)pUpdate);
}

void CMainFrame::UpdateFrags(int nID, int nFrags)
{
	PostMessage(UM_UPDATEFRAG, nID, nFrags);
}

void CMainFrame::UpdateDeaths(int nID, int nDeaths)
{
	PostMessage(UM_UPDATEDEATH, nID, nDeaths);
}

LRESULT CMainFrame::OnUpdatePlayer(WPARAM wParam, LPARAM lParam)
{
	// UI thread
	UpdatePlayerStruct* pUpdate = (UpdatePlayerStruct*)wParam;

	if (settings.m_bUpdateWonID) {
		std::string sWonID = pUpdate->player.m_sWonID.c_str();
		for (STRLIST::iterator i = m_mWonIDs[sWonID].begin(); i != m_mWonIDs[sWonID].end(); i++)
		{
			if ((*i).compare(pUpdate->player.m_sName.c_str()) == 0)
				break;
		}
		if (i == m_mWonIDs[sWonID].end())
		{
			m_mWonIDs[sWonID].push_back(pUpdate->player.m_sName.c_str());
		}
	}

	IGCLock(&m_lockPlayers, "OnUpdatePlayer");
	PLAYERMAP::iterator iPlayer = m_mPlayers.find(pUpdate->player.m_nID);
	PlayerItemData* pData = NULL;
	if (iPlayer != m_mPlayers.end())
	{
		pData = (*iPlayer).second;
		if ((m_pFrames[I_PLAYERS] != NULL) && (m_pFrames[I_PLAYERS]->m_pView != NULL))
			((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->UpdatePlayer(pData);
	}

	delete pUpdate;
	return 0;
}

void CMainFrame::PlayerDisconnected(int nID)
{
	IGCLock(&m_lockPlayers, "PlayerDisconnected");
	PostMessage(UM_PLAYERDISCONNECTED, nID);
}

LRESULT CMainFrame::OnPlayerDisconnected(WPARAM wParam, LPARAM lParam)
{
	int nID = (int)wParam;

	PLAYERMAP::iterator iPlayer = m_mPlayers.find(nID);

	if (iPlayer != m_mPlayers.end())
	{
		PlayerItemData* pData = (*iPlayer).second;

		if ((m_pFrames[I_PLAYERS] != NULL) && (m_pFrames[I_PLAYERS]->m_pView != NULL))
			((CPlayerListView*)m_pFrames[I_PLAYERS]->m_pView)->PlayerDisconnected(nID);

		delete pData;
		m_mPlayers.erase(iPlayer);
	}

	return 0;
}

void CMainFrame::OnViewOptions() 
{
	COptionsSheet dlg("CSBot options", this);

	dlg.ReadSettings();
	// If the user presses OK or Apply, the sheet will call OnApplyOptions
	// Sadly this happens once for every initialized page... oh well...
	dlg.DoModal();
	settings.SaveToRegistry();
}

void CMainFrame::NotifyCSBotUpdate()
{
	if (!settings.m_bNotifyUpdates)
		return;

	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, &NotifyUpdateProc, (LPVOID)this, 0, &dwThreadID);

	if (hThread != NULL)
		::CloseHandle(hThread);
}

DWORD WINAPI CMainFrame::NotifyUpdateProc(LPVOID lpThis)
{
	CMainFrame* pThis = (CMainFrame*)lpThis;
	if (!pThis->DoNotifyUpdate() && pThis->m_bNotifyNoUpdate)
	{
		pThis->PostMessage(UM_NOTIFY_UPDATE, NULL, NULL);
	}
	return 0;
}

bool CMainFrame::DoNotifyUpdate()
{
	CString sMsg = settings.ReadHttp("www.csbot.org", "msgbox.php", 5);

	int nPos = sMsg.Find("\r\n");
	if (nPos < 0)
		return false;
	CString sMostRecent = settings.GetString("MostRecentVersion", CSBOT_VERSION);
	// "most recent version" in registry is correct
	if (sMsg.Left(nPos).Compare(sMostRecent) == 0)
		return false;
	// user downloaded a new version without updating the "most recent version" in registry
	if (sMsg.Left(nPos).Compare(CSBOT_VERSION) == 0)
	{
		settings.SetString("MostRecentVersion", CSBOT_VERSION);
		return false;
	}
	PostMessage(UM_NOTIFY_UPDATE, (WPARAM)new CString(sMsg.Left(nPos)), (LPARAM)new CString(sMsg.Mid(nPos+2)));
	return true;
}

LRESULT CMainFrame::OnUpdateNotify(WPARAM wParam, LPARAM lParam)
{
	CString* pString = (CString*)lParam;
	CString* pVer = (CString*)wParam;
	if ((pString == NULL) || (pVer == NULL))
	{
		AfxMessageBox("No update found!\nYou are probably using the latest version of CSBot.", MB_ICONINFORMATION | MB_OK);
	} else {
		CNotifyUpdateDlg dlg;
		dlg.m_bNotifyUpdates = !settings.m_bNotifyUpdates;
		dlg.m_sMsg = *pString;
		dlg.DoModal();
		settings.m_bNotifyUpdates = !dlg.m_bNotifyUpdates;
		settings.SetString("MostRecentVersion", *pVer);
		delete pString;
		delete pVer;
	}
	return 0;
}

void CMainFrame::OnHelpUpdate() 
{
	m_bNotifyNoUpdate = true;
	NotifyCSBotUpdate();
}
