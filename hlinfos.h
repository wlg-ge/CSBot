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

#ifndef __HLINFOS_H__
#define __HLINFOS_H__

#include <string>

class ServerInfoStruct // 'C' response
{
public:
	ServerInfoStruct() : uPlayerCount(0), uMaxPlayers(0), uVersion(0) {};
	ServerInfoStruct(const ServerInfoStruct& other) :
		sAddress(other.sAddress),
		sHostname(other.sHostname),
		sMapName(other.sMapName),
		sGame(other.sGame),
		sGameDescription(other.sGameDescription),
		uPlayerCount(other.uPlayerCount),
		uMaxPlayers(other.uMaxPlayers),
		uVersion(other.uVersion)
		{};
		
	std::string		sAddress;
	std::string		sHostname;
	std::string		sMapName;
	std::string		sGame;
	std::string		sGameDescription;
	unsigned char	uPlayerCount;
	unsigned char	uMaxPlayers;
	unsigned char	uVersion;
};

class PlayerInfoStruct
{
public:
	PlayerInfoStruct() : uIndex(0), lFrags(0), fTime(0) {};
	PlayerInfoStruct(const PlayerInfoStruct& other) : 
		uIndex(other.uIndex), 
		lFrags(other.lFrags), 
		fTime(other.fTime),
		sName(other.sName)
		{};
	unsigned char	uIndex;
	std::string		sName;
	long			lFrags;
	float			fTime;
};

class RuleInfoStruct
{
public:
	RuleInfoStruct() {};
	RuleInfoStruct(const RuleInfoStruct& other) :
		sName(other.sName),
		sValue(other.sValue)
		{};
	std::string sName;
	std::string sValue;
};

#endif