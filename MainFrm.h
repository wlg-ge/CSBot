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

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__017C1ACA_512F_4370_ABBD_58106F325A6F__INCLUDED_)
#define AFX_MAINFRM_H__017C1ACA_512F_4370_ABBD_58106F325A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Settings.h"
#include "irc.h"
#include "GIRLLogParser.h"
#include "IGCLockable.h"
#include "OptionsSheet.h"

// The IRC client is implemented in a namespace...
using namespace irc;

// constant IDs for the child views and frames
#define I_RAWLOG		0
#define I_PLAYERS		1
#define I_RCON			2
#define I_IRC			3
#define I_SCHED			4

#define I_MAX			5

// one global IRC session (client) is currently defined
extern	CIrcSession g_ircSession;

// Forward class and struct declarations (for lists and maps below)
class	CStatsPlayer;
class	CChildFrame;
class	IRCOutputThread;
class	GIRLUdpServer;
class	LogFilter;
struct	GIRLUdpMessageError;
struct	GIRLUdpMessageRcon;
struct	GIRLUdpMessageLog;
struct	GIRLUdpMessageInfo;
struct	GIRLUdpMessageRules;

// Type definitions for various lists and maps that we use later on
typedef std::list<std::string> STRLIST;
typedef std::map<std::string, STRLIST> STR2STRLIST;
typedef std::map<DWORD, std::string> DWORD2STR;
typedef std::list<CIrcSession*> SESSIONLIST;

// The Main Frame class itself, with all its glory
class CMainFrame : public CMDIFrameWnd, public CIrcDefaultMonitor, public CIGCLockable, public CIrcIdentServerMonitor, public GIRLLogReceiver
{
public:
	// Standard Constructor
	CMainFrame();

	// United Colors of mIRC. 16 predefined colors.
	static COLORREF		m_colorsIRC[16];

	// Current log parser and UDP server
	GIRLLogParser*		m_pParser;
	GIRLUdpServer*		m_pUdpServer;

	// Utility methods
	void				Connect(LPCTSTR pProfileName, bool bGuess, bool bGuessHarder);
	void				ConnectIRC(LPCTSTR pProfileName);
	void				Disconnect();

	void				TranslateGameCommand(CString& sCmd);
	void				TranslateIRCCommand(CString& sCmd);

	void				ShowFrame(int nID);
	void				FormatTime(CTime& time, CString& s);
	void				FetchPlayerList();
	void				IssueRcon(const CString& sCommand);

	bool				IsConnected() {return m_bConnected;};
	bool				IsConnectedIRC() {return m_bConnectedIRC;};
	bool				IsJoinedIRC() {return m_bJoinedIRC;};

private:
	// All our children (child frames for views), and their resource IDs
	friend class		CChildFrame;
	CChildFrame*		m_pFrames[I_MAX];
	UINT				m_ResourceIDs[I_MAX];

public:
	// GIRLLogReceiver overrides

	// Locked methods for use by parsers
	virtual CString	GetChannelModes();
	virtual CString	GetServerName();
	virtual UINT	GetServerPort();
	virtual CString	GetLastIP();
	virtual void	OutputIRCMessage(unsigned long uMsg, LPCTSTR pMessage, LPCTSTR pTo = NULL);
	virtual void	SetIRCTopic(const CString& sTopic);

	// Player list methods, inter-locked
	virtual void	UpdatePlayer(UpdatePlayerStruct* pUpdate);
	virtual void	UpdateFrags(int nID, int nFrags);
	virtual void	UpdateDeaths(int nID, int nDeaths);

	// Message-Posting methods  for use by prasers
	virtual void	PlayerDisconnected(int nID);
	virtual void	SetMapName(SetMapNameStruct* pMapName);
	virtual void	Summon(SummonStruct* pSummon);

private:
	// State members

	// State variables
	bool				m_bSentAutoCommands;

	bool				m_bSentRules;
	bool				m_bSentRconTest;
	bool				m_bSentRconCommands;

	// Game server state
	GameServerVersion	m_versionGame;
	int					m_nReconnectGameSecLeft;
	int					m_nReconnectGameAttemptsLeft;
	bool				m_bDuringConnection;
	DWORD				m_dwConnectionStart;
	bool				m_bConnected;
	CString				m_sLastIP;

	GameServerProfile	m_serverGame;
	STR2STRLIST			m_mWonIDs;

	CTime				m_tLastLogLine;
	int					m_nMaxPlayers;
	int					m_nPlayers;
	LogQueryType		m_eTypeUsed;

	// IRC server state
	int					m_nReconnectIRCSecLeft;
	int					m_nReconnectIRCAttemptsLeft;
	bool				m_bConnectedIRC;
	bool				m_bJoinedIRC;

	IRCServerProfile	m_serverIRC;
	CIrcSessionInfo		m_IrcInfo;
	SESSIONLIST			m_lSessions;
	IRCOutputThread*	m_pIrcOutput;
	LogFilter*			m_pLogFilter;
	bool				m_bIRCDisconnectRequest;
	CString				m_sLastTopic;
	CString				m_sChannelModes;
	bool				m_bMute;

	// Windows resources state
	bool				m_bHasTimers;
	bool				m_bHasFlashTimer;
	int					m_nMuteImage;
	int					m_nUnmuteImage;
	bool				m_bNotifyNoUpdate;

	// IRC MessageMap
	DEFINE_IRC_MAP()

	// Tray-icon stuff
	CMenu			m_TrayMenu;
	HICON			m_hIconTray;
	NOTIFYICONDATA	m_notify;
	void			Deiconize();
	bool			CreateSystemTrayIcon();


	// Helper methods.

	// Connection/disconnection related
	void			Connect();
	void			DoConnect(bool bAutomatic);
	bool			DoConnectSteps(bool bAutomatic);
	void			SendFirstRconCommands();
	void			OnSuddenDisconnect(bool bAllowReconnect = true, LPCTSTR pErrorMessage = NULL);
	bool			CheckInactivityTimeout();
	in_addr			ResolveServerName(bool bAutomatic);

	void			ConnectIRC();
	void			DisconnectIRC();
	void			ConnectToIRCServer(bool bAutomatic);
	void			SendPassNickUser();

	// UI reflection of what happens inside CSBot
	void			EnableControls();
	void			DisableControls();
	void			EnableIRCControls();
	void			DisableIRCControls();

	// Cleanup helpers
	void			ClearPlayerList();
	void			ClearSchedList();
	void			CloseEverything();

	// IRC Output
	void			OutputIRCMessageDelayed(LPCTSTR pMessage, LPCTSTR pTo = NULL, CIrcSession* pSession = NULL);
	void			OutputIRCMessageImmediate(LPCTSTR pMessage, LPCTSTR pTo = NULL, CIrcSession* pSession = NULL);

	// Player list (status) parsing
	bool			ParsePlayerList(std::string sRes);
	CString			GetNextString(CString& str);
	void			ParseTime(CString s, int& nDays, int& nHours, int& nMins, int& nSecs);
	bool			ParseServerVersion(const char* pLines);


	// More UI/Other
	void			KillTimers();
	void			StopFlash();
	void			CommitWonIDs();
	void			NotifyCSBotUpdate();
	static DWORD WINAPI NotifyUpdateProc(LPVOID lpThis);
	bool			DoNotifyUpdate();

	// Ident monitor overrides
	virtual void	OnIdentResponded(LPCTSTR pResponse);

	// Message-Mapped handlers to IRC commands
	bool			OnIrc_JOIN(const CIrcMessage* pmsg);
	bool			OnIrc_PART(const CIrcMessage* pmsg);
	bool			OnIrc_VERSION(const CIrcMessage* pmsg);
	bool			OnIrc_WELCOME(const CIrcMessage* pmsg);
	bool			OnIrc_PRIVMSG(const CIrcMessage* pmsg);
	bool			OnIrc_ERROR(const CIrcMessage* pmsg);
	bool			OnIrc_ENDOFNAMES(const CIrcMessage* pmsg);
	bool			OnIrc_ENDOFLIST(const CIrcMessage* pmsg);
	bool			OnIrc_QUIT(const CIrcMessage* pmsg);
	bool			OnIrc_NICK(const CIrcMessage* pmsg);
	bool			OnIrc_DCCMSG(const CIrcMessage* pmsg);
	bool			OnIrc_513(const CIrcMessage* pmsg);
	bool			OnIrc_CHANMODES(const CIrcMessage* pmsg);
	bool			OnIrc_CHANMODE(const CIrcMessage* pmsg);
	bool			OnIrc_CHANTOPIC(const CIrcMessage* pmsg);
	bool			OnIrc_MODE(const CIrcMessage* pmsg);
	bool			OnIrc_TOPIC(const CIrcMessage* pmsg);

	// IRC Monitor overrides
	virtual void	OnIrcDefault(const CIrcMessage* pmsg);
	virtual void	OnIrcDisconnected(const CIrcMessage* pmsg);
	virtual void	OnIrcAll(const CIrcMessage* pmsg);
	virtual void	OnFirstPing(LPCTSTR pID);

	// IRC Interface command processors
	bool			ProcessIRCRcon(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCLogin(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCLogout(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCHelp(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCVersion(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCInfo(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCPlayer(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCTeams(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCCTCPDCC(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCCTCPVersion(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCChat(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);

	bool			ProcessIRCOp(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCDeop(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCVoice(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCDevoice(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCSay(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCAct(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);

	bool			ProcessIRCModeCommand(const char* pMode, int nCommand, const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);
	bool			ProcessIRCCommand(const CIrcMessage* pmsg, std::string sCmd, CIrcSession* pSession);

	// Helpers for IRC Interface
	TrustedPerson*	IsUserLoggedIn(const CIrcMessage* pmsg);
	TrustedPerson*	UserLogin(const CIrcMessage* pmsg, const std::string& sUser, const std::string& sPass, CIrcSession* pSession);

	// UDP server message handlers
	void			OnUdpError(GIRLUdpMessageError* pMsg);
	void			OnUdpRcon(GIRLUdpMessageRcon* pRcon);
	void			OnUdpLog(GIRLUdpMessageLog* pMsg);
	void			OnUdpInfo(GIRLUdpMessageInfo* pMsg);
	void			OnUdpRules(GIRLUdpMessageRules* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CBitmap			m_bmpToolBar;
	CImageList		m_imageList;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateServerConnectCs(CCmdUI* pCmdUI);
	afx_msg void OnServerConnectCs();
	afx_msg void OnUpdateServerConnectIrc(CCmdUI* pCmdUI);
	afx_msg void OnServerConnectIrc();
	afx_msg void OnDestroy();
	afx_msg void OnViewRawlog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewPlayers();
	afx_msg void OnViewOptions();
	afx_msg void OnViewRcon();
	afx_msg void OnViewIrc();
	afx_msg void OnServerWonids();
	afx_msg void OnUpdateServerWonids(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateViewIrc(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPlayers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRawlog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRcon(CCmdUI* pCmdUI);
	afx_msg void OnViewSched();
	afx_msg void OnUpdateViewSched(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnMute();
	afx_msg void OnUpdateMute(CCmdUI* pCmdUI);
	afx_msg void OnHelpUpdate();
	//}}AFX_MSG
	afx_msg LRESULT OnUdp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePlayer(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMapName(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateFrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateDeath(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayerDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSummon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIdentResponded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateNotify(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTrayRestore();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__017C1ACA_512F_4370_ABBD_58106F325A6F__INCLUDED_)
