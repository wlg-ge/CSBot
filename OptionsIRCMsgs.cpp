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

// OptionsIRCMsgs.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsIRCMsgs.h"
#include "GIRLLogParserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgs dialog

enum TreeItemType {
	TI_Game,
	TI_Gameflow,
	TI_Objectives,
	TI_Colors,
	TI_Build,
	TI_Research,
	TI_Spawn,
	TI_Recycle,
	TI_Others
};

struct TreeItemData
{
	TreeItemData(TreeItemType typ, GIRLLogParser* prsr) :
		m_ItemType(typ), m_pParser(prsr) {};
	TreeItemData(const TreeItemData& other) :
		m_ItemType(other.m_ItemType), m_pParser(other.m_pParser) {};
	TreeItemType m_ItemType;
	GIRLLogParser* m_pParser;
};


COptionsIRCMsgs::COptionsIRCMsgs() : 
	COptionsPage(COptionsIRCMsgs::IDD)
{
	//{{AFX_DATA_INIT(COptionsIRCMsgs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pCurrentDialog = NULL;
	m_pageMsgs.m_pPage = this;
	m_pageColors.m_pPage = this;
}


void COptionsIRCMsgs::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate)
	{
		if (m_pCurrentDialog != NULL)
		{
			if (!m_pCurrentDialog->UpdateData(FALSE))
				return;
		}
	}
	//{{AFX_DATA_MAP(COptionsIRCMsgs)
	DDX_Control(pDX, IDC_TREE, m_ctlTree);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		if (m_pCurrentDialog != NULL)
		{
			if (!m_pCurrentDialog->UpdateData())
				return;
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsIRCMsgs, COptionsPage)
	//{{AFX_MSG_MAP(COptionsIRCMsgs)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsIRCMsgs message handlers

BOOL COptionsIRCMsgs::OnInitDialog() 
{
	COptionsPage::OnInitDialog();

	CBitmap bmp;
	bmp.LoadBitmap(IDB_IMAGELIST);
	m_imageList.Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 8);
	m_imageList.Add(&bmp, RGB(255,255,255));
	
	m_ctlTree.SetImageList(&m_imageList, TVSIL_NORMAL);

	CRect rcSheet;

	ColorInfo info;
	m_pageColors.m_pColors = &info;
	m_pageColors.Create(this);
	GetDlgItem( IDC_TABS )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_pageColors.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	m_pageMsgs.Create(this);
	GetDlgItem( IDC_TABS )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_pageMsgs.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	HTREEITEM hItem, hGame;
	PARSERLIST* pList = g_parserManager.GetParserList();
	TreeItemData* pData;

	bool* pMsgTypes = new bool[(int)MsgType_Last];
	bool bColors = false;

	for (PARSERLIST::iterator iParser = pList->begin(); iParser != pList->end(); iParser++)
	{
		GIRLLogParser* pParser = (*iParser);

		// find out which types of msgs exist
		for (int nMsgType = 0; nMsgType < (int)MsgType_Last; nMsgType++)
			pMsgTypes[nMsgType] = false;

		IRCMSGMAP& mMap = pParser->LockMsgMap();
		for (IRCMSGMAP::iterator iMap = mMap.begin(); iMap != mMap.end(); iMap++)
		{
			IRCCustomMsg& msg = (*iMap).second;
			pMsgTypes[msg.m_eType] = true;
		}
		pParser->UnlockMsgMap();

		bColors = false;
		for (int nTeam = 0; nTeam < TEAM_COUNT; nTeam++)
		{
			if (strlen(pParser->TeamName((Team)nTeam)) > 0)
			{
				bColors = true;
				break;
			}
		}

		// create game's tree item
		hGame = m_ctlTree.InsertItem(pParser->GameTypeDescription());
		pData = new TreeItemData(TI_Game, pParser);
		m_lTIDs.push_back(pData);
		m_ctlTree.SetItemData(hGame, (DWORD)pData);
		m_ctlTree.SetItemImage(hGame, pParser->GameTypeImage(), pParser->GameTypeImage());

		// create other tree items, if exist
		if (bColors)
		{
			hItem = m_ctlTree.InsertItem("Colors", hGame);
			pData = new TreeItemData(TI_Colors, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_COLORS, IMAGE_COLORS);
		}

		if (pMsgTypes[MsgType_Gameflow]) 
		{
			hItem = m_ctlTree.InsertItem("Gameflow", hGame);
			pData = new TreeItemData(TI_Gameflow, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_GAMEFLOW, IMAGE_GAMEFLOW);
		}

		if (pMsgTypes[MsgType_Objectives]) 
		{
			hItem = m_ctlTree.InsertItem("Objectives", hGame);
			pData = new TreeItemData(TI_Objectives, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_OBJECTIVES, IMAGE_OBJECTIVES);
		}

		if (pMsgTypes[MsgType_Build]) 
		{
			hItem = m_ctlTree.InsertItem("Build", hGame);
			pData = new TreeItemData(TI_Build, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_BUILD, IMAGE_BUILD);
		}
		
		if (pMsgTypes[MsgType_Spawn]) 
		{
			hItem = m_ctlTree.InsertItem("Recycle", hGame);
			pData = new TreeItemData(TI_Recycle, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_RECYCLE, IMAGE_RECYCLE);
		}

		if (pMsgTypes[MsgType_Research]) 
		{
			hItem = m_ctlTree.InsertItem("Research", hGame);
			pData = new TreeItemData(TI_Research, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_RESEARCH, IMAGE_RESEARCH);
		}

		if (pMsgTypes[MsgType_Spawn]) 
		{
			hItem = m_ctlTree.InsertItem("Equipment", hGame);
			pData = new TreeItemData(TI_Spawn, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_SPAWN, IMAGE_SPAWN);
		}

		if (pMsgTypes[MsgType_Other]) 
		{
			hItem = m_ctlTree.InsertItem("Others", hGame);
			pData = new TreeItemData(TI_Others, pParser);
			m_lTIDs.push_back(pData);
			m_ctlTree.SetItemData(hItem, (DWORD)pData);
			m_ctlTree.SetItemImage(hItem, IMAGE_OTHERS, IMAGE_OTHERS);
		}
		m_ctlTree.Expand(hGame, TVE_EXPAND);
	}

	delete [] pMsgTypes;

	m_ctlTree.SelectItem(m_ctlTree.GetRootItem());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsIRCMsgs::ReadSettings()
{
	m_mMsgs.clear();
	m_mColors.clear();
	
	PARSERLIST* pParserList = g_parserManager.GetParserList();
	for (PARSERLIST::iterator iParser = pParserList->begin(); iParser != pParserList->end(); iParser++)
	{
		GIRLLogParser* pParser = (*iParser);

		// create the messagelist map
		IRCMSGMAP& mMsgs = pParser->LockMsgMap();
		IRCMSGLIST& lMsgs = m_mMsgs[pParser];
		// and fill it with copies of the messages
		for (IRCMSGMAP::iterator j = mMsgs.begin(); j != mMsgs.end(); j++)
		{
			IRCCustomMsg& msg = (*j).second;
			lMsgs.push_back(msg);
		}
		pParser->UnlockMsgMap();
		
		// now create the color map as well
		ColorInfo& colors = m_mColors[pParser];
		colors.pParser = pParser;
		for (int nTeam=0; nTeam<TEAM_COUNT; nTeam++)
		{
			colors.sTeam[nTeam] = pParser->TeamDescription((Team)nTeam);
			colors.sColor[nTeam] = pParser->GetTeamColor((Team)nTeam);
			colors.sColorDefault[nTeam] = pParser->GetTeamColorDefault((Team)nTeam);
		}
	}
}

void COptionsIRCMsgs::WriteSettings()
{
	PARSERLIST* pParserList = g_parserManager.GetParserList();
	for (PARSERLIST::iterator iParser = pParserList->begin(); iParser != pParserList->end(); iParser++)
	{
		GIRLLogParser* pParser = (*iParser);

		PARSER2MSGS::iterator iMsgList = m_mMsgs.find(pParser);
		ASSERT(iMsgList != m_mMsgs.end());

		IRCMSGLIST& lMsgs = (*iMsgList).second;

		for (IRCMSGLIST::iterator i = lMsgs.begin(); i != lMsgs.end(); i++)
		{
			IRCCustomMsg& msg = (*i);
			pParser->SetIRCMessage(msg.m_uMsg, msg.m_sValue.c_str());
			pParser->EnableIRCMessage(msg.m_uMsg, msg.m_bEnabled);
		}

		PARSER2COLORS::iterator iColor = m_mColors.find(pParser);
		ASSERT(iColor != m_mColors.end());
		ColorInfo& colors = (*iColor).second;

		for (int nTeam=0; nTeam<TEAM_COUNT; nTeam++)
			pParser->SetTeamColor((Team)nTeam, colors.sColor[nTeam]);
		if (pParser->IsActive())
			pParser->SetIRCTopic();
	}
}

void COptionsIRCMsgs::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;
	*pResult = 0;

	if (m_pCurrentDialog != NULL)
	{
		if (!m_pCurrentDialog->UpdateData())
			return;
		m_pCurrentDialog->ShowWindow(SW_HIDE);
	}
	m_pCurrentDialog = NULL;

	TreeItemData* pData = (TreeItemData*)m_ctlTree.GetItemData(pNMTreeView->itemNew.hItem);
	switch (pData->m_ItemType)
	{
	case TI_Game:
		ShowList(pData->m_pParser);
		break;
	case TI_Colors:
		ShowColors(pData->m_pParser);
		break;
	case TI_Gameflow:
		ShowList(pData->m_pParser, MsgType_Gameflow);
		break;
	case TI_Objectives:
		ShowList(pData->m_pParser, MsgType_Objectives);
		break;
	case TI_Build:
		ShowList(pData->m_pParser, MsgType_Build);
		break;
	case TI_Research:
		ShowList(pData->m_pParser, MsgType_Research);
		break;
	case TI_Spawn:
		ShowList(pData->m_pParser, MsgType_Spawn);
		break;
	case TI_Recycle:
		ShowList(pData->m_pParser, MsgType_Recycle);
		break;
	case TI_Others:
		ShowList(pData->m_pParser, MsgType_Other);
		break;
	default:
		ASSERT(FALSE);
	};

	if (m_pCurrentDialog != NULL)
	{
		m_pCurrentDialog->UpdateData(FALSE);
		m_pCurrentDialog->ShowWindow(SW_SHOW);
	}
}

void COptionsIRCMsgs::ShowList(GIRLLogParser* pParser, MsgType msgType)
{
	m_pCurrentDialog = &m_pageMsgs;
	m_pageMsgs.m_bSoft = true;
	m_pageMsgs.m_ctlList.DeleteAllItems();

	PARSER2MSGS::iterator iMsgList = m_mMsgs.find(pParser);
	ASSERT(iMsgList != m_mMsgs.end());

	IRCMSGLIST& lMsgs = (*iMsgList).second;
	for (IRCMSGLIST::iterator i = lMsgs.begin(); i != lMsgs.end(); i++)
	{
		IRCCustomMsg& msg = (*i);
		if ((msgType == (MsgType)-1) || (msg.m_eType == msgType))
		{
			int nItem = m_pageMsgs.m_ctlList.InsertItem(m_pageMsgs.m_ctlList.GetItemCount(), msg.m_sValue.c_str());
			m_pageMsgs.m_ctlList.SetItemText(nItem, 1, msg.m_pMeaning);
			m_pageMsgs.m_ctlList.SetCheck(nItem, msg.m_bEnabled ? TRUE : FALSE);
			m_pageMsgs.m_ctlList.SetItemData(nItem, (DWORD)&msg);
		}
	}
	m_pageMsgs.m_pParser = pParser;
	m_pageMsgs.m_bSoft = false;
}

void COptionsIRCMsgs::ShowColors(GIRLLogParser* pParser)
{
	m_pCurrentDialog = &m_pageColors;

	PARSER2COLORS::iterator i = m_mColors.find(pParser);
	ASSERT(i != m_mColors.end());
	ColorInfo& colors = (*i).second;

	m_pageColors.SetColors(&(*i).second, pParser);
}

void COptionsIRCMsgs::OnDestroy() 
{
	for (TIDLIST::iterator i = m_lTIDs.begin(); i != m_lTIDs.end(); i++)
		delete (*i);

	m_lTIDs.clear();
	COptionsPage::OnDestroy();
}
