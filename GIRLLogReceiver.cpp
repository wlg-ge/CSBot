// GIRLLogReceiver.cpp: implementation of the GIRLLogReceiver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLLogReceiver.h"
#include "GIRLLogParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GIRLLogReceiver::GIRLLogReceiver() : m_lockPlayers("PlayerListLock")
{

}

GIRLLogReceiver::~GIRLLogReceiver()
{
	ClearPlayerMap();
}

void GIRLLogReceiver::UpdateFrag(int nID)
{
	IGCLock(&m_lockPlayers, "UpdateFrag");
	PLAYERMAP::iterator iPlayer = m_mPlayers.find(nID);

	if (iPlayer != m_mPlayers.end())
	{
		PlayerItemData* pData = (*iPlayer).second;
		pData->nFrags++;
		UpdateFrags(nID, pData->nFrags);
	}
}

void GIRLLogReceiver::UpdateDeath(int nID)
{
	IGCLock(&m_lockPlayers, "UpdateDeath");
	PLAYERMAP::iterator iPlayer = m_mPlayers.find(nID);

	if (iPlayer != m_mPlayers.end())
	{
		PlayerItemData* pData = (*iPlayer).second;
		pData->nDeaths++;
		UpdateDeaths(nID, pData->nDeaths);
	}
}

bool GIRLLogReceiver::GetPlayerItemData(int nID, PlayerItemData* pNewData)
{
	// logger's thread
	IGCLock lock(&m_lockPlayers, "GetPlayerItemData");

	PLAYERMAP::iterator iPlayer = m_mPlayers.find(nID);
	if (iPlayer == m_mPlayers.end())
		return false;

	PlayerItemData* pData = (*iPlayer).second;

	pNewData->nDeaths = pData->nDeaths;
	pNewData->nFrags = pData->nFrags;
	pNewData->sName = pData->sName;
	pNewData->sRole = pData->sRole;
	pNewData->sWonID = pData->sWonID;
	pNewData->team = pData->team;
	pNewData->time = pData->time;
	pNewData->uID = pData->uID;
	return true;
}

void GIRLLogReceiver::UpdatePlayer(UpdatePlayerStruct* pUpdate)
{
	IGCLock(&m_lockPlayers, "UpdatePlayer");
	PLAYERMAP::iterator iPlayer = m_mPlayers.find(pUpdate->player.m_nID);
	PlayerItemData* pData = NULL;
	if (iPlayer == m_mPlayers.end())
	{
		pData = new PlayerItemData;
		pData->uID = pUpdate->player.m_nID;
		m_mPlayers[pUpdate->player.m_nID] = pData;
	} else {
		pData = (*iPlayer).second;
	}

	if (pUpdate->nFrags >= 0)
		pData->nFrags = pUpdate->nFrags;

	if (!pUpdate->player.m_sName.empty())
		pData->sName = pUpdate->player.m_sName.c_str();

	if (pUpdate->player.m_team != TEAM_NONE)
	{
		if (pData->team != pUpdate->player.m_team)
		{
			pData->team = pUpdate->player.m_team;
			pData->sRole = "";
		}
	}
	if (!pUpdate->player.m_sRole.empty())
		pData->sRole = pUpdate->player.m_sRole.c_str();

	pData->team = pUpdate->player.m_team;
	if (pData->team == TEAM_SPECTATOR)
		pData->sRole = "";

	if (!pUpdate->player.m_sWonID.empty() && (pUpdate->player.m_sWonID != "0"))
		pData->sWonID = pUpdate->player.m_sWonID.c_str();

	if (pUpdate->pTs != NULL) {
		pData->time = *pUpdate->pTs;
	}
}

void GIRLLogReceiver::ClearPlayerMap()
{
	// UI thread
	IGCLock(&m_lockPlayers, "ClearPlayerMap");

	for (PLAYERMAP::iterator i = m_mPlayers.begin(); i != m_mPlayers.end(); i++)
		delete (*i).second;

	m_mPlayers.clear();
}

