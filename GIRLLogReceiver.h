// GIRLLogReceiver.h: interface for the GIRLLogReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLLOGRECEIVER_H__88EBC1AE_CFD8_4227_88DD_846F1F7AA69C__INCLUDED_)
#define AFX_GIRLLOGRECEIVER_H__88EBC1AE_CFD8_4227_88DD_846F1F7AA69C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IGCLockable.h"

struct PlayerItemData;
struct UpdatePlayerStruct;
struct SetMapNameStruct;
struct SummonStruct;

typedef std::map<int, PlayerItemData*> PLAYERMAP;

class GIRLLogReceiver  
{
public:
	GIRLLogReceiver();
	virtual ~GIRLLogReceiver();

protected:
	// Player list
	PLAYERMAP		m_mPlayers;
	// Lockable object for player list
	CIGCLockable	m_lockPlayers;

	void			ClearPlayerMap();

public:
	virtual CString	GetChannelModes() = 0;
	virtual CString	GetServerName() = 0;
	virtual UINT	GetServerPort() = 0;
	virtual CString	GetLastIP() = 0;
	virtual void	OutputIRCMessage(unsigned long uMsg, LPCTSTR pMessage, LPCTSTR pTo = NULL) = 0;
	virtual void	SetIRCTopic(const CString& sTopic) = 0;

	virtual void	PlayerDisconnected(int nID) = 0;
	virtual void	SetMapName(SetMapNameStruct* pMapName) = 0;
	virtual void	Summon(SummonStruct* pSummon) = 0;

	virtual void	UpdateFrags(int nID, int nFrags) = 0;
	virtual void	UpdateDeaths(int nID, int nDeaths) = 0;

	virtual bool	GetPlayerItemData(int nID, PlayerItemData* pData);
	virtual void	UpdateFrag(int nID);
	virtual void	UpdateDeath(int nID);
	virtual void	UpdatePlayer(UpdatePlayerStruct* pUpdate);
};

#endif // !defined(AFX_GIRLLOGRECEIVER_H__88EBC1AE_CFD8_4227_88DD_846F1F7AA69C__INCLUDED_)
