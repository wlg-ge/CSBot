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

// GIRLLogParser.h: interface for the GIRLLogParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGPARSER_H__3A677B7A_F2D1_4B10_A156_863E7962002C__INCLUDED_)
#define AFX_GIRLLOGPARSER_H__3A677B7A_F2D1_4B10_A156_863E7962002C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIRLDefs.h"
#include "GIRLLogReceiver.h"
#include "IGCLockable.h"

// unknown line type
#define LT_UNKNOWN	0x80000000
#define LT_TOPIC	0x80000001

// Item data that is saved in the players list view.
struct PlayerItemData 
{
	PlayerItemData() :
		uID(0),
		time(CTime::GetCurrentTime()),
		team(TEAM_SPECTATOR),
		nFrags(0),
		nDeaths(0)
		{};
	virtual ~PlayerItemData()
	{
	}

	void Assign(const PlayerItemData& other)
	{
		uID = other.uID;
		sWonID = other.sWonID;
		team = other.team;
		time = other.time;
		sRole = other.sRole;
		sName = other.sName;
		nFrags = other.nFrags;
		nDeaths = other.nDeaths;
	}

	UINT uID;
	CString sWonID;
	Team team;
	CTime time;
	CString sRole;
	CString sName;
	int nFrags;
	int nDeaths;
};

// IRC message info. 
// m_sValue and m_bEnabled may change by the parser manager due to settings.
struct IRCCustomMsg
{
	IRCCustomMsg() : 
		m_uMsg(-1), 
		m_pDefault(NULL), 
		m_pMeaning(NULL), 
		m_bEnabled(false), 
		m_eType(MsgType_Other)
		{};
	IRCCustomMsg(const IRCCustomMsg& other) :
		m_uMsg(other.m_uMsg),
		m_pDefault(other.m_pDefault),
		m_pMeaning(other.m_pMeaning),
		m_sValue(other.m_sValue),
		m_bEnabled(other.m_bEnabled),
		m_eType(other.m_eType)
		{};
	IRCCustomMsg(IRCMSG uMsg, const char* pDefault, const char* pMeaning, std::string sValue, bool bEnabled, MsgType eType) :
		m_uMsg(uMsg),
		m_pDefault(pDefault),
		m_pMeaning(pMeaning),
		m_sValue(sValue),
		m_bEnabled(bEnabled),
		m_eType(eType)
		{};
	IRCMSG m_uMsg;
	const char* m_pDefault;
	const char* m_pMeaning;
	std::string m_sValue;
	bool m_bEnabled;
	MsgType m_eType;
	IRCCustomMsg& operator= (const IRCCustomMsg& other)
	{
		m_uMsg = other.m_uMsg;
		m_pDefault = other.m_pDefault;
		m_pMeaning = other.m_pMeaning;
		m_sValue = other.m_sValue;
		m_bEnabled = other.m_bEnabled;
		m_eType = other.m_eType;
		return *this;
	}
};
typedef std::map<IRCMSG, IRCCustomMsg> IRCMSGMAP;

// All known information about a player.
// The same player is assumed if it has the same m_nID and m_sWonID
class CStatsPlayer {
public:
	CStatsPlayer() :
		m_nID(-1),
		m_team(TEAM_NONE)
		{};
	CStatsPlayer(const CStatsPlayer& plyr) : 
		m_sName(plyr.m_sName), 
		m_nID(plyr.m_nID),
		m_sWonID(plyr.m_sWonID),
		m_team(plyr.m_team),
		m_sRole(plyr.m_sRole)
		{};
	std::string		m_sName;
	int				m_nID;
	std::string		m_sWonID;
	Team			m_team;
	std::string		m_sRole;
};

// Struct that is POSTed when player information should be updated
struct UpdatePlayerStruct
{
	UpdatePlayerStruct(CStatsPlayer& plyr, int nFrgs = -1, CTime* pT = NULL) :
		player(plyr), nFrags(nFrgs) 
	{
		if (pT == NULL)
			pTs = NULL;
		else
		{
			ts = *pT;
			pTs = &ts;
		}
	};

	CStatsPlayer player;
	int nFrags;
	CTime* pTs;
	CTime ts;
};

// Struct that is POSTed when a player summons an admin
struct SummonStruct
{
	SummonStruct(LPCTSTR pLine, const CStatsPlayer& player, bool bTeam) : m_sLine(pLine), m_player(player), m_bTeam(bTeam) {};
	std::string m_sLine;
	CStatsPlayer m_player;
	bool m_bTeam;
};

// Struct that is POSTed when map changes
struct SetMapNameStruct
{
	SetMapNameStruct(LPCTSTR pMapName) : sMapName(pMapName) {};
	std::string sMapName;
};

class GIRLLogParser : public CIGCLockable
{
public:
	// Constructor / Destructor
	GIRLLogParser();
	virtual ~GIRLLogParser() {};

private:
	// for own use only
	void ParseTime(std::string& sLine);
	std::string m_sServerName;
	std::string m_sMapName;
	std::string m_sTime;
	IRCMSGMAP	m_mMsgs;

public:
	// for use by framework
	IRCMSGMAP& LockMsgMap();
	void UnlockMsgMap();
	void SetServerName(const char* pName);
	std::string GetMapName();
	std::string GetServerName();
	bool ParseLogLine(std::string sLine);
	void SetActive(bool bActive = true);

	// for use by anyone (framework and derived classes)
	const IRCCustomMsg* GetIRCMessageStruct(IRCMSG eMsg);
	void SetIRCMessage(IRCMSG eMsg, const char* pValue);
	const char* GetIRCMessageDefault(IRCMSG eMsg);
	const char* GetIRCMessageMeaning(IRCMSG eMsg);
	bool IsIRCMessageEnabled(IRCMSG eMsg);
	void EnableIRCMessage(IRCMSG eMsg, bool bEnable = true);
	const char* GetIRCMessage(IRCMSG eMsg);
	void ResetDefaultIRCMessages();
	void SetTeamColorDefaults();
	void SetIRCTopic();
	std::string DecorateScore(Team t, int nScore);
	std::string Decorate(LPCTSTR pText, Team t);
	bool IsActive();

protected:
	// for use by derived classes
	GIRLLogReceiver* m_pReceiver;
	bool		m_bActive;

	void RegisterMessage(IRCMSG uMsg, const char* pDefault, const char* pMeaning, bool bEnabled, MsgType eType);
	void UnregisterMessage(IRCMSG uMsg);
	bool IsMessageRegistered(IRCMSG uMsg);

	std::string FixPlayerName(std::string sName);
	std::string GetNextName(std::string& sLine, CStatsPlayer& player, bool bAutoUpdate = true);
	std::string GetNextVar(std::string& sLine);
	std::string GetWeapon(std::string& sLine);
	std::string GetDetail(std::string& sLine, LPCTSTR pDetail);
	std::string GetRole(std::string& sLine);
	Team GetTeam(std::string& sLine);

	void OutputWithRestOnly(std::string& sLine, IRCMSG uMsg);
	void OutputWithPlayerOnly(std::string& sLine, IRCMSG uMsg);
	void OutputWithTwoPlayers(std::string& sLine, IRCMSG uMsg, std::string sBefore2 = "");
	void OutputIRCMessage(IRCMSG uMsg, const char* pLine);

	void ReplacePlayer(CString& sMsg, const CStatsPlayer& player);
	void ReplacePlayer2(CString& sMsg, const CStatsPlayer& player);
	void ReplaceName(CString& sMsg, const std::string& sReplacement);
	void ReplaceName2(CString& sMsg, const std::string& sReplacement);
	void ReplaceTeam(CString& sMsg, const std::string& sReplacement);
	void ReplaceTeam2(CString& sMsg, const std::string& sReplacement);
	void ReplaceRole(CString& sMsg, const std::string& sReplacement);
	void ReplaceRole2(CString& sMsg, const std::string& sReplacement);
	void ReplaceFrags(CString& sMsg, const std::string& sReplacement);
	void ReplaceFrags2(CString& sMsg, const std::string& sReplacement);
	void ReplaceDeaths(CString& sMsg, const std::string& sReplacement);
	void ReplaceDeaths2(CString& sMsg, const std::string& sReplacement);
	void ReplaceWeapon(CString& sMsg, const std::string& sReplacement);
	void ReplaceServer(CString& sMsg, const std::string& sReplacement);
	void ReplaceSay(CString& sMsg, const std::string& sReplacement);
	void ReplaceMap(CString& sMsg, const std::string& sReplacement);
	void ReplaceOther(CString& sMsg, const std::string& sReplacement);
	virtual void ReplaceRest(CString& sMsg);

	// for implementation by derived classes

	// recognizes the line, and then process it
	virtual void ProcessLine(std::string& sLine) = 0;

public:
	// for implementation by derived classes

	// initializes members, after manager and main frame have initialized 
	virtual void InitParser();
	// initializes members, before reconnecting to the game server
	virtual void Init() = 0;

	// returns the color of the specified team, or "" if the team does not exist
	virtual std::string GetTeamColor(Team t) = 0;
	// returns the default color of the specified team
	virtual const char* GetTeamColorDefault(Team t) = 0;
	// sets the color of the specified team
	virtual void SetTeamColor(Team t, const std::string& sColor) = 0;

	// returns the team's short name
	virtual const char* TeamName(Team t) = 0;
	// returns the team's long name
	virtual const char* TeamDescription(Team t) = 0;
	// returns the team by its name in a log file
	virtual Team NameTeam(std::string& sTeam) = 0;

	// returns all of the possible game's mod names in a NULL terminated array of NULL terminated strings
	// (eg. "cstrike", "tfc", "ns", etc.)
	virtual const char** GameTypeName() = 0;
	// returns the game's long name ("Counter Strike", "Natural-Selection", etc.)
	virtual const char* GameTypeDescription() = 0;
	// returns help message for additional supported tags in IRC messages
	virtual CString GetAdditionalTags() {return "";};
	// returns the game's image number in the image list (0 for no image)
	virtual int GameTypeImage() = 0;
	// notifies the parser that the framework detected a mapchange
	virtual void SetMapName(const char* pName);
};

// utility macros for use inside RecognizeLine()
#define CONTAINS(SubStr) (strstr(sLine.c_str(), SubStr) != NULL)
#define EQUALS(SubStr) (strcmp(sLine.c_str(), SubStr) == 0)
#define EQUALSNOCASE(SubStr) (stricmp(sLine.c_str(), SubStr) == 0)
#define STARTSWITH(SubStr) (strstr(sLine.c_str(), SubStr) == sLine.c_str())
#define ENDSWITH(SubStr) (sLine.rfind(SubStr) == (sLine.size()-strlen(SubStr)))

#endif // !defined(AFX_GIRLLOGPARSER_H__3A677B7A_F2D1_4B10_A156_863E7962002C__INCLUDED_)
