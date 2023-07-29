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

#ifndef __WonIDDBManager_H__
#define __WonIDDBManager_H__

#include <vector>

typedef std::vector<DWORD> DWORDARRAY;

#define WONID_EXISTS			1
#define WONID_CANTCREATEFILE	2
#define WONID_CANTWRITEFILE		3
#define WONID_CANTOPENFILE		4
#define WONID_CANTREADFILE		5
#define WONID_CANTSEEKFILE		6
#define WONID_INVALIDFILE		7
#define WONID_NOTFOUND			8
#define WONID_CANTENUMFILES		9
#define WONID_OK				100

class WonIDNameCallback
{
public:
	virtual bool FoundName(LPCTSTR pName, DWORD dwWonID, LPARAM lParam) = 0;
};

class WonIDDBManager
{
public:
	static int	AddWonIDToDatabase(DWORD dwWonID, LPCTSTR pName);
	static int	WonIDAdd(DWORD dwWonID);
	static int	GetAllNames(DWORD dwWonID, WonIDNameCallback* pCallback, LPARAM lParam);
	static int	DeleteName(DWORD dwWonID, LPCTSTR pName);
	static void MsgBox(int nRet);
	static int	FindNextMatch(CString& sSub, DWORD& dwStartWonID, CString& sStartName, bool bMatchCase);
	static int	Export(FILE* pExportFile);
	static int	Import(FILE* pImportFile);
	static bool IsDigitsOnly(LPCTSTR pString);

private:
	static int ExportNames(DWORD dwWonID, FILE* pExportFile);

};

#endif
