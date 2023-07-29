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
#include "GIRLWindowing.h"

// Ripped off the MFC sources :)
struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
		{ return (WORD*)(this+1); }
};

// Ripped off MFC sources and changed so it will work on 256-color bitmaps, with correct
// background color for toolbars
HBITMAP AFXAPI GIRLLoadSysColorBitmap(LPCTSTR lpszResourceName)
{
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_BITMAP);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hglb;
	if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 256;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	DWORD dwColor = ::GetSysColor(COLOR_BTNFACE);
	dwColor = ((dwColor & 0xff0000) >> 16) | (dwColor & 0xff00) | ((dwColor & 0xff) << 16);
	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		if (pColorTable[iColor] == RGB(192,192,192))
		{
			pColorTable[iColor] = dwColor;
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);


	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

// Ripped off MFC sources and changed so it will work on 256-color bitmaps
BOOL GIRLLoadToolBar(CToolBar& bar, LPCTSTR lpszResourceName)
{
	ASSERT_VALID(&bar);
	ASSERT(lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];
	for (int i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];
	BOOL bResult = bar.SetButtons(pItems, pData->wItemCount);
	delete[] pItems;

	if (bResult)
	{
		// set new sizes of the buttons
		CSize sizeImage(pData->wWidth, pData->wHeight);
		CSize sizeButton(pData->wWidth + 7, pData->wHeight + 7);
		bar.SetSizes(sizeButton, sizeImage);

		// load bitmap now that sizes are known by the toolbar control
		bResult = bar.AddReplaceBitmap(GIRLLoadSysColorBitmap(lpszResourceName));
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return bResult;
}


/*
void AttachMenuImagesFromToolbar(CMenu* pMenu, CToolBarCtrl* pToolbar)
{
	UINT nItems = pMenu->GetMenuItemCount();

	TBBUTTONINFO tbi;
//	IMAGEINFO imginf;
	MENUITEMINFO mif;
	for (UINT nItem = 0; nItem < nItems; nItem++)
	{
		UINT nID = pMenu->GetMenuItemID(nItem);
		// separator
		if (nID == 0)
			continue;

		// sub-menu
		if (nID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(nItem);
//			MENUINFO mi;
//			mi.cbSize = sizeof(mi);
//			mi.fMask = MIM_STYLE;
//			if (::GetMenuInfo(pSubMenu->GetSafeHmenu(), &mi))
//			{
//				mi.dwStyle |= MNS_CHECKORBMP;
//				::SetMenuInfo(pSubMenu->GetSafeHmenu(), &mi);
//			}
			if (pSubMenu != NULL)
				AttachMenuImagesFromToolbar(pSubMenu, pToolbar);
			continue;
		}

		// menu item
		tbi.cbSize = sizeof(tbi);
		tbi.dwMask = TBIF_IMAGE;
		if (pToolbar->GetButtonInfo(nID, &tbi) != -1)
		{
			HICON hIcon = pToolbar->GetImageList()->ExtractIcon(tbi.iImage);
			if (hIcon != NULL)
			{
				ICONINFO ii;
				if (::GetIconInfo(hIcon, &ii))
				{
					mif.cbSize = sizeof(mif);
					mif.fMask = MIIM_BITMAP | MIIM_CHECKMARKS;
					mif.hbmpItem = ii.hbmColor;
					mif.hbmpChecked = ii.hbmColor;
					mif.hbmpUnchecked = ii.hbmColor;
					::SetMenuItemInfo(pMenu->GetSafeHmenu(), nItem, TRUE, &mif);
				}
			}
		}
	}
}
*/
