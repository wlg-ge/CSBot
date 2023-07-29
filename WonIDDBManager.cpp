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

#include "stdafx.h"
#include "WonIDDBManager.h"
#include "Settings.h"

#define WONID_MAGICNUM	0xCBCBD412

int WonIDDBManager::AddWonIDToDatabase(DWORD dwWonID, LPCTSTR pName)
{
	if (dwWonID == 0xffffffff)
		return WONID_OK;
	CString sFilename;
	sFilename.Format("%s\\%u.WonID", (LPCTSTR)settings.m_sWonIDFolder, dwWonID);

	FILE* pFile = fopen(sFilename, "rb");
	if (pFile == NULL) {
		// new file
		pFile = fopen(sFilename, "wb");
		if (pFile == NULL)
			return WONID_CANTOPENFILE;
		
		DWORD dw = WONID_MAGICNUM;
		if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		dw = strlen(pName);
		if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		DWORD dwFlags = 1;
		if (fwrite(&dwFlags, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		if (fwrite(pName, 1, dw, pFile) != dw) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		dw = 0;
		if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		fclose(pFile);
	} else {
		// existing file
		// this is going to be frustrating... here goes...
		// 1. open file for r/w
		// 2. search for this name. exists -> quit&stop
		// 3. seek to end-4, and add
		fclose(pFile);
		pFile = fopen(sFilename, "r+b");
		if (pFile == NULL) {
			return WONID_CANTOPENFILE;
		}
		if (fseek(pFile, 0, SEEK_SET) != 0) {
			fclose(pFile);
			return WONID_CANTSEEKFILE;
		}
		DWORD dw;
		if (fread(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (dw != WONID_MAGICNUM) {
			fclose(pFile);
			return WONID_INVALIDFILE;
		}
		DWORD dwLen;
		char c[256];
		do
		{
			if (fread(&dwLen, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
				fclose(pFile);
			return WONID_CANTREADFILE;
			}
			if (dwLen > 0) {
				if (fread(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
					fclose(pFile);
					return WONID_CANTREADFILE;
				}
				if (fread(c, 1, dwLen, pFile) != dwLen) {
					fclose(pFile);
					return WONID_CANTREADFILE;
				}
				c[dwLen] = '\0';
				
				if ((dw & 1) == 1) {
					if (strcmp(pName, c) == 0) {
						fclose(pFile);
						return WONID_OK;
					}
				}
			}
		} while (dwLen != 0);
		if (fseek(pFile, -4, SEEK_END) != 0) {
			fclose(pFile);
			return WONID_CANTSEEKFILE;
		}
		dwLen = strlen(pName);
		if (fwrite(&dwLen, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		dw = 1;
		if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		if (fwrite(pName, 1, dwLen, pFile) != dwLen) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		dw = 0;
		if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTWRITEFILE;
		}
		fclose(pFile);
	}
	return WONID_OK;
}

int WonIDDBManager::WonIDAdd(DWORD dwWonID)
{
	CString sFilename;
	sFilename.Format("%s\\%u.WonID", settings.m_sWonIDFolder, dwWonID);
	FILE* pFile = fopen(sFilename, "rb");
	if (pFile != NULL) {
		fclose(pFile);
		return WONID_EXISTS;
	}
	pFile = fopen(sFilename, "wb");
	if (pFile == NULL) {
		return WONID_CANTCREATEFILE;
	}
	DWORD dwLen = WONID_MAGICNUM;
	if (fwrite(&dwLen, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
		return WONID_CANTWRITEFILE;
	}
	dwLen = 0;
	if (fwrite(&dwLen, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
		return WONID_CANTWRITEFILE;
	}
	fclose(pFile);
	return WONID_OK;
}

int WonIDDBManager::GetAllNames(DWORD dwWonID, WonIDNameCallback* pCallback, LPARAM lParam)
{
	CString sFilename;
	sFilename.Format("%s\\%u.WonID", settings.m_sWonIDFolder, dwWonID);
	FILE* pFile = fopen(sFilename, "rb");
	if (pFile == NULL)
		return WONID_CANTOPENFILE;
	DWORD dwSize;
	DWORD dwFlags;
	char c[256];

	if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
	{
		fclose(pFile);
		return WONID_CANTREADFILE;
	}
	if (dwSize != WONID_MAGICNUM)
		return WONID_INVALIDFILE;
	do {
		if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
		{
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (dwSize > 0)
		{
			if (fread(&dwFlags, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
			{
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			if (fread(c, 1, dwSize, pFile) != dwSize) {
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			c[dwSize] = '\0';
			if (dwFlags & 1) {
				if (!pCallback->FoundName(c, dwWonID, lParam))
				{
					fclose(pFile);
					return WONID_OK;
				}
			}
		}
	} while (dwSize > 0);
	fclose(pFile);
	return WONID_OK;
}

void WonIDDBManager::MsgBox(int nRet)
{
	switch (nRet) 
	{
	case WONID_EXISTS:
		AfxMessageBox("WonID file already exists in the database");
		break;
	case WONID_CANTCREATEFILE:
		AfxMessageBox("Cannot create WonID file. (disk full? no permissions?)");
		break;
	case WONID_CANTWRITEFILE:
		AfxMessageBox("Cannot write to WonID file. (disk full? no permissions?)");
		break;
	case WONID_CANTOPENFILE:
		AfxMessageBox("Cannot open WonID file. (no permissions?)");
		break;
	case WONID_CANTREADFILE:
		AfxMessageBox("Cannot read WonID file. (corrupt? no permissions?)");
		break;
	case WONID_CANTSEEKFILE:
		AfxMessageBox("Cannot seek in WonID file. (corrupt? no permissions?)");
		break;
	case WONID_INVALIDFILE:
		AfxMessageBox("Invalid file. Not the correct type of file! (corrupt?)");
		break;
	case WONID_CANTENUMFILES:
		AfxMessageBox("Error enumerating WonID files");
		break;
	}
}

int WonIDDBManager::DeleteName(DWORD dwWonID, LPCTSTR pName)
{
	// long procedure... here goes...
	// 1. open file for r/w
	// 2. search for name
	// 3. found -> seek, and remove flag
	CString sFilename;
	sFilename.Format("%s\\%u.WonID", (LPCTSTR)settings.m_sWonIDFolder, dwWonID);
	FILE* pFile = fopen(sFilename, "r+b");
	if (pFile == NULL)
		return WONID_CANTOPENFILE;
	DWORD dw;
	if (fseek(pFile, 0, SEEK_SET))
		return WONID_CANTSEEKFILE;
	if (fread(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
		fclose(pFile);
		return WONID_CANTREADFILE;
	}
	if (dw != WONID_MAGICNUM) {
		fclose(pFile);
		return WONID_INVALIDFILE;
	}
	DWORD dwLen;
	char c[256];
	do {
		if (fread(&dwLen, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (fread(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (dwLen > 0) {
			if (fread(c, 1, dwLen, pFile) != dwLen) {
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			c[dwLen] = '\0';
			if ((dw & 1) == 1) {
				if (strcmp(pName, c) == 0) {
					//found. delete it.
					fseek(pFile, -4 - dwLen, SEEK_CUR);
					dw = dw & 0xfffffffe;
					if (fwrite(&dw, 1, sizeof(DWORD), pFile) != sizeof(DWORD)) {
						fclose(pFile);
						return WONID_CANTWRITEFILE;
					}
					break;
				}
			}
		}
	} while (dwLen > 0);
	fclose(pFile);
	return WONID_OK;
}

int WonIDDBManager::FindNextMatch(CString& sSub, DWORD& dwStartWonID, CString& sStartName, bool bMatchCase)
{
	CString sFilename;

	if (sStartName.IsEmpty())
	{
		CString sWonID;
		sWonID.Format("%u", dwStartWonID);
		if (sWonID.Find(sSub) >= 0)
		{
			sStartName.Empty();
			return WONID_OK;
		}
	}

	// step 1: Search within the start file.
	if (dwStartWonID > 0) {
		sFilename.Format("%s\\%u.WonID", settings.m_sWonIDFolder, dwStartWonID);
		FILE* pFile = fopen(sFilename, "rb");
		if (pFile == NULL)
			return WONID_CANTOPENFILE;

		DWORD dwSize;
		DWORD dwFlags;
		char c[256];
		bool bStarted = sStartName.IsEmpty() ? true : false;

		if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
		{
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (dwSize != WONID_MAGICNUM)
			return -1;
		do {
			if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
			{
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			if (dwSize > 0)
			{
				if (fread(&dwFlags, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
				{
					fclose(pFile);
					return WONID_CANTREADFILE;
				}
				if (fread(c, 1, dwSize, pFile) != dwSize) {
					fclose(pFile);
					return WONID_CANTREADFILE;
				}
				c[dwSize] = '\0';
				if (dwFlags & 1) {
					CString s = c;
					if (bStarted) {
						if (bMatchCase) {
							if (s.Find(sSub) >= 0) {
								sStartName = c;
								return WONID_OK;
							}
						} else {
							s.MakeLower();
							if (s.Find(sSub) >= 0) {
								sStartName = c;
								return WONID_OK;
							}
						}
					} else {
						if (s.Compare(sStartName) == 0)
							bStarted = true;
					}
				}
			}
		} while (dwSize > 0);
		fclose(pFile);
	}	
	// step 2: find the next file, and return its wonid.
	CString sName;
	CString sFolder = settings.m_sWonIDFolder;
	sFolder += "\\*.WonID";
	WIN32_FIND_DATA fd;
	HANDLE hSearch = ::FindFirstFile(sFolder, &fd);
	if (hSearch == INVALID_HANDLE_VALUE)
		return WONID_CANTENUMFILES;
	DWORD dwNextID = 0xffffffff;
	DWORD dwMinID = 0xffffffff;
	do {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			sName = fd.cFileName;
			sName = sName.Left(sName.GetLength()-6);
			if (IsDigitsOnly(sName)) {
				DWORD dwID = (DWORD)atol(sName);
				if ((dwID > dwStartWonID) && (dwID < dwNextID)) {
					dwNextID = dwID;
				}
				if (dwID < dwMinID)
					dwMinID = dwID;
			}	
		}
	} while (::FindNextFile(hSearch, &fd));
	if (GetLastError() != ERROR_NO_MORE_FILES)
		return WONID_CANTENUMFILES;
	if (dwNextID == 0xffffffff) {
		// nothing was found... start from 0.
		dwStartWonID = 0;//dwMinID;
	} else {
		dwStartWonID = dwNextID;
	}
	sStartName.Empty();
	return WONID_NOTFOUND;
}

int WonIDDBManager::Import(FILE* pImportFile)
{
	char c[32768];
	if (fgets(c, 32767, pImportFile) == NULL)
		return WONID_CANTREADFILE;
	if (strcmp("WonID\tNames...\n", c) != 0)
		return WONID_INVALIDFILE;

	char* pTab;
	while (!feof(pImportFile))
	{
		// read next line and parse it.
		if (fgets(c, 32767, pImportFile) == NULL)
		{
			if (feof(pImportFile))
				break;
			return WONID_CANTREADFILE;
		}
		int nLen = strlen(c);
		if (nLen < 2)
			continue;
		if (c[nLen-1] == '\n') 
		{
			c[nLen-1] = '\0';
			nLen--;
		}

		// find end of WonID
		pTab = strchr(c, '\t');
		if (pTab == NULL)
			continue;

		*pTab = '\0';
		pTab++;
		// get the WonID
		DWORD dwWonID = strtoul(c, NULL, 10);
		char* pPrev = pTab;
		do {
			pTab = strchr(pTab, '\t');
			if (pTab != NULL)
			{
				*pTab = '\0';
				pTab++;
			}
			int nRet = AddWonIDToDatabase(dwWonID, pPrev);
			if ((nRet != WONID_OK) && (nRet != WONID_EXISTS))
				return nRet;
		} while (pTab != NULL);
	}
	return WONID_OK;
}

int WonIDDBManager::Export(FILE* pExportFile)
{
	DWORDARRAY lIDs;
	CString sName;
	CString sFolder = settings.m_sWonIDFolder;
	sFolder += "\\*.WonID";
	WIN32_FIND_DATA fd;
	HANDLE hSearch = ::FindFirstFile(sFolder, &fd);
	if (hSearch == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		return WONID_CANTENUMFILES;
	}
	do {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			sName = fd.cFileName;
			sName = sName.Left(sName.GetLength()-6);
			if (IsDigitsOnly(sName)) {
				lIDs.push_back((DWORD)atol(sName));
			}	
		}
	} while (::FindNextFile(hSearch, &fd));
	if (GetLastError() != ERROR_NO_MORE_FILES)
		return WONID_CANTENUMFILES;

	int i, nMax = lIDs.size();
	for (i=0; i<nMax-1; i++)
	{
		for (int j=i+1; j<nMax; j++)
		{
			if (lIDs[i] > lIDs[j])
			{
				DWORD dwTemp = lIDs[i];
				lIDs[i] = lIDs[j];
				lIDs[j] = dwTemp;
			}
		}
	}
	for (i=0; i<nMax; i++)
	{
		fprintf(pExportFile, "%u", lIDs[i]);
		int nRet = ExportNames(lIDs[i], pExportFile);
		if (nRet != WONID_OK)
			return nRet;
		fprintf(pExportFile, "\n");
	}
	return WONID_OK;
}

int WonIDDBManager::ExportNames(DWORD dwWonID, FILE* pExportFile)
{
	CString sFilename;
	sFilename.Format("%s\\%u.WonID", settings.m_sWonIDFolder, dwWonID);
	FILE* pFile = fopen(sFilename, "rb");
	if (pFile == NULL)
		return WONID_CANTOPENFILE;

	DWORD dwSize;
	DWORD dwFlags;
	char c[256];
	int nSelItem = -1;

	if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
	{
		fclose(pFile);
		return WONID_CANTREADFILE;
	}
	if (dwSize != WONID_MAGICNUM)
		return WONID_INVALIDFILE;
	do {
		if (fread(&dwSize, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
		{
			fclose(pFile);
			return WONID_CANTREADFILE;
		}
		if (dwSize > 0)
		{
			if (fread(&dwFlags, 1, sizeof(DWORD), pFile) != sizeof(DWORD))
			{
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			if (fread(c, 1, dwSize, pFile) != dwSize) {
				fclose(pFile);
				return WONID_CANTREADFILE;
			}
			c[dwSize] = '\0';
			if (dwFlags & 1)
				fprintf(pExportFile, "\t%s", c);
		}
	} while (dwSize > 0);
	fclose(pFile);
	return WONID_OK;
}

bool WonIDDBManager::IsDigitsOnly(LPCTSTR pString)
{
	while (*pString != '\0') {
		if ((*pString > '9') || (*pString < '0'))
			return false;
		pString++;
	}
	return true;
}

