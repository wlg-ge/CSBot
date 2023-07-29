/*******************************************************************************/
/* GHOutputWnd - an MFC based output text window, enabling selection and       */
/* copying (but not editing) of the text visible in the control                */
/*                                                                             */
/* GHOutputWnd.cpp : implementation file                                       */
/* Copyright (C) 2003, 2004 Guy H.                                             */
/* E-Mail: hguy@netvision.net.il                                               */
/*                                                                             */
/* This program is free software; you can redistribute it and/or               */
/* modify it under the terms of the GNU General Public License                 */
/* as published by the Free Software Foundation; either version 2              */
/* of the License, or (at your option) any later version.                      */
/*                                                                             */
/* This program is distributed in the hope that it will be useful,             */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of              */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               */
/* GNU General Public License for more details.                                */
/*                                                                             */
/* You should have received a copy of the GNU General Public License           */
/* along with this program; if not, write to the Free Software                 */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */
/*******************************************************************************/

#include "stdafx.h"
#include "GHOutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHARS_WIDTH			512
#define INITIAL_MAX_LINES	1000
#define CARET_WIDTH			2

/////////////////////////////////////////////////////////////////////////////
// CGHOutputWnd

#pragma warning (disable: 4200)
struct OutputLine
{
	int		nLen;
	int		nWidth;
	char	cData[0];
};
#pragma warning (default: 4200)

BOOL CGHOutputWnd::m_bGotScrollLines;
UINT CGHOutputWnd::m_uCachedScrollLines;
UINT CGHOutputWnd::m_msgGetScrollLines;
WORD CGHOutputWnd::m_nRegisteredMessage;

IMPLEMENT_DYNCREATE(CGHOutputWnd, CWnd)

CGHOutputWnd::CGHOutputWnd()
{
	m_ptOffset.x = 0;
	m_ptOffset.y = 0;
	m_sizeLetter.cx = 0;
	m_sizeLetter.cy = 0;
	m_sizePage.cx = 0;
	m_sizePage.cy = 0;
	m_ptCaret.x = 0;
	m_ptCaret.y = 0;
	m_nMaxLines = INITIAL_MAX_LINES;
	m_bAutoDestroy = FALSE;
	m_font = (HFONT)::GetStockObject(ANSI_FIXED_FONT);
	m_bAddToLast = TRUE;
	m_bShowSelection = FALSE;
	m_bCapture = FALSE;
	m_nMaxWidth = -1;
	m_nLinesSize = 10;
	m_pLines = (OutputLine**)new char[m_nLinesSize * sizeof(OutputLine*)];
	m_nLineCount = 1;
	m_pLines[0] = (OutputLine*)new char[sizeof(OutputLine)];
	m_pLines[0]->nLen = 0;
	m_pLines[0]->nWidth = 0;
	m_bHasCaret = FALSE;
}

CGHOutputWnd::~CGHOutputWnd()
{
	for (int i = 0; i < m_nLineCount; i++)
		delete [] m_pLines[i];
	if (m_pLines != NULL)
		delete [] m_pLines;
}


BEGIN_MESSAGE_MAP(CGHOutputWnd, CWnd)
	//{{AFX_MSG_MAP(CGHOutputWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_ENABLE()
	ON_WM_MOUSEWHEEL()
	ON_WM_GETDLGCODE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnWndSetFont)
	ON_MESSAGE(WM_COPY, OnCopy)
	ON_MESSAGE(WM_CLEAR, OnClear)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_ADDTEXT, OnAddText)
	ON_MESSAGE(WM_GETMAXLINES, OnGetMaxLines)
	ON_MESSAGE(WM_SETMAXLINES, OnSetMaxLines)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

BOOL CGHOutputWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
	return CWnd::PreCreateWindow(cs);
}


// CGHOutputWnd message handlers
//	General Messages
void CGHOutputWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (m_bHasCaret)
		HideCaret();

	CRect rectClip;
	dc.GetClipBox(&rectClip);
	CRect rect(0, 0, m_nMaxWidth, m_sizeLetter.cy), rectDraw;

	// Draw the text...
	dc.SetViewportOrg(-m_ptOffset.x, 0);

	dc.SaveDC();
	::SelectObject(dc.GetSafeHdc(), m_font);
	dc.SetBkMode(OPAQUE);
	int nY = min(m_ptOffset.y, m_nLineCount);
	BOOL bDisabled = !IsWindowEnabled();
	BOOL bInSelected = m_bShowSelection && (nY > m_ptSelectionStart.y) && (nY < m_ptSelectionEnd.y), bToDraw;
	dc.SetTextColor(::GetSysColor(bInSelected ? COLOR_WINDOW : bDisabled ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT));
	dc.SetBkColor(::GetSysColor(bInSelected ? COLOR_WINDOWTEXT : COLOR_WINDOW));
	char* pToDraw;
	int nDrawFrom;
	for (; nY < m_nLineCount; nY++, rect.OffsetRect(0, m_sizeLetter.cy))
	{
		bToDraw = !(rectClip & rect).IsRectEmpty();
		rectDraw = rect;
		pToDraw = m_pLines[nY]->nLen == 0 ? "" : m_pLines[nY]->cData;
		nDrawFrom = 0;
		if (m_bShowSelection && (nY == m_ptSelectionStart.y))
		{
			if (bToDraw && (m_ptSelectionStart.x > nDrawFrom))
			{
				dc.DrawText(pToDraw, m_ptSelectionStart.x - nDrawFrom, &rectDraw, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_VCENTER);
				rectDraw.left += dc.GetTextExtent(pToDraw, m_ptSelectionStart.x).cx;
				nDrawFrom = m_ptSelectionStart.x;
			}
			dc.SetTextColor(::GetSysColor(COLOR_WINDOW));
			dc.SetBkColor(::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT));
		}
		if (m_bShowSelection && (nY == m_ptSelectionEnd.y))
		{
			if (bToDraw && (m_ptSelectionEnd.x > nDrawFrom))
			{
				dc.DrawText(pToDraw + nDrawFrom, m_ptSelectionEnd.x - nDrawFrom, &rectDraw, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_VCENTER);
				rectDraw.left += dc.GetTextExtent(pToDraw + nDrawFrom, m_ptSelectionEnd.x - nDrawFrom).cx;
				nDrawFrom = m_ptSelectionEnd.x;
			}
			dc.SetTextColor(::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
		}
		if (nDrawFrom < m_pLines[nY]->nLen)
			dc.DrawText(pToDraw + nDrawFrom, m_pLines[nY]->nLen - nDrawFrom, &rectDraw, DT_SINGLELINE|DT_LEFT|DT_NOPREFIX|DT_VCENTER);
	}
	dc.RestoreDC(-1);
	if (m_bHasCaret)
		ShowCaret();
}

void CGHOutputWnd::PostNcDestroy() 
{
	CWnd::PostNcDestroy();
	if (m_bAutoDestroy)
		delete this;
}

int CGHOutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DoSetFont();
	
	return 0;
}

void CGHOutputWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if ((nType == WS_MINIMIZE) || (cx == 0) || (cy == 0))
		return;
	SIZE szScroll = {0,0};
	if (m_sizeLetter.cy > 0)
	{
		m_sizePage.cy = cy / m_sizeLetter.cy;
		if (m_ptOffset.y + m_sizePage.cy > m_nLineCount)
			szScroll.cy = m_sizePage.cy - m_nLineCount;
	}
	if (m_sizeLetter.cx > 0)
	{
		m_sizePage.cx = cx;
		if (m_ptOffset.x + m_sizePage.cx > m_nMaxWidth)
			szScroll.cx = m_sizePage.cx - m_nMaxWidth;
	}
	if ((szScroll.cx != 0) || (szScroll.cy != 0))
	{
		if (m_bHasCaret)
			HideCaret();
		ScrollWindow(m_ptOffset.x + szScroll.cx, (m_ptOffset.y + szScroll.cy) * m_sizeLetter.cy);
		if (szScroll.cx != 0)
			m_ptOffset.x = max(0, m_nMaxWidth - m_sizePage.cx);
		if (szScroll.cy != 0)
			m_ptOffset.y = max(0, m_nLineCount - m_sizePage.cy);
		MoveCaret(FALSE);
		if (m_bHasCaret)
			ShowCaret();
	}
	SetVScroll();
	SetHScroll();
}

void CGHOutputWnd::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	Invalidate();
}

void CGHOutputWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar != NULL)
	{
		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	int nNewPos = m_ptOffset.y;
	switch (nSBCode)
	{
	case SB_BOTTOM:
		nNewPos = max(0, m_nLineCount - 1);
		break;
	case SB_LINEDOWN:
		if (nNewPos < m_nLineCount - 1)
			nNewPos++;
		break;
	case SB_LINEUP:
		if (nNewPos > 0)
			nNewPos--;
		break;
	case SB_PAGEDOWN:
		nNewPos += m_sizePage.cy;
		if (nNewPos >= m_nLineCount)
			nNewPos = max(0, m_nLineCount - 1);
		break;
	case SB_PAGEUP:
		nNewPos -= m_sizePage.cy;
		if (nNewPos < 0)
			nNewPos = 0;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewPos = nPos;
		break;
	case SB_TOP:
		nNewPos = 0;
		break;
	}

	ScrollY(nNewPos - m_ptOffset.y);
}

void CGHOutputWnd::ScrollY(int nAmount)
{
	if (nAmount == 0)
		return;

	if (abs(nAmount) < m_sizePage.cy)
	{
		ScrollWindow(0, -nAmount * m_sizeLetter.cy);
		m_ptOffset.y += nAmount;	// Otherwise clip is wrong
		UpdateWindow();
	}
	else
	{
		Invalidate();
		m_ptOffset.y += nAmount;
	}
	SetScrollPos(SB_VERT, m_ptOffset.y);
	MoveCaret(FALSE);
}

void CGHOutputWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar != NULL)
	{
		CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	int nNewPos = m_ptOffset.x;
	switch (nSBCode)
	{
	case SB_LEFT:
		nNewPos = 0;
		break;
	case SB_LINERIGHT:
		nNewPos += m_sizeLetter.cx;
		if (nNewPos >= m_nMaxWidth - m_sizePage.cx)
			nNewPos = max(m_nMaxWidth - m_sizePage.cx, 0);
		break;
	case SB_LINELEFT:
		if (nNewPos > 0)
			nNewPos--;
		break;
	case SB_PAGERIGHT:
		nNewPos += m_sizePage.cx;
		if (nNewPos >= m_nMaxWidth - m_sizePage.cx)
			nNewPos = max(m_nMaxWidth - m_sizePage.cx, 0);
		break;
	case SB_PAGELEFT:
		nNewPos -= m_sizePage.cx;
		if (nNewPos < 0)
			nNewPos = 0;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewPos = nPos;
		break;
	case SB_RIGHT:
		nNewPos = max(m_nMaxWidth - m_sizePage.cx, 0);
		break;
	}

	ScrollX(nNewPos - m_ptOffset.x);
}

void CGHOutputWnd::ScrollX(int nAmount)
{
	if (nAmount == 0)
		return;

	if (abs(nAmount) < m_sizePage.cx)
	{
		ScrollWindow(-nAmount, 0);
		m_ptOffset.x += nAmount;	// Otherwise clip is wrong
		UpdateWindow();
	}
	else
	{
		Invalidate();
		m_ptOffset.x += nAmount;
	}
	SetScrollPos(SB_HORZ, m_ptOffset.x);
	MoveCaret(FALSE);
}

//	Specific messages
LRESULT CGHOutputWnd::OnWndSetFont(WPARAM wParam, LPARAM lParam)
{
	::DeleteObject(m_font);
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	::GetObject((HFONT)wParam, sizeof(lf), &lf);
	m_font = ::CreateFontIndirect(&lf);

	DoSetFont();

	if (LOWORD(lParam))
		Invalidate();
	return 0;
}

LRESULT CGHOutputWnd::OnSetMaxLines(WPARAM wLines, LPARAM /*lParam*/)
{
	m_nMaxLines = wLines;
	if (m_nLineCount <= m_nMaxLines)
		return 0;

	int nDelete = m_nLineCount - m_nMaxLines;
	DeleteLines(nDelete);
	if ((GetSafeHwnd() != NULL) && (m_ptOffset.y < m_sizePage.cy))
		Invalidate();
	return 0;
}

LRESULT CGHOutputWnd::OnClear(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	for (int i=0;i<m_nLineCount;i++)
		delete [] m_pLines[i];

	m_pLines[0] = (OutputLine*)new char[sizeof(OutputLine)];
	m_pLines[0]->nLen = 0;
	m_pLines[0]->nWidth = 0;
	m_nLineCount = 1;

	m_bAddToLast = TRUE;
	m_bShowSelection = FALSE;
	m_ptCaret.x = 0;
	m_ptCaret.y = 0;
	SetVScroll();
	SetHScroll();
	Invalidate();
	return 0;
}

LRESULT CGHOutputWnd::OnCopy(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (!m_bShowSelection || ((m_ptSelectionStart.x == m_ptSelectionEnd.x) && (m_ptSelectionStart.y == m_ptSelectionEnd.y)))
		return 0;

	// Get selection text:
	int nSelLength = 1;
	if (m_ptSelectionStart.y == m_ptSelectionEnd.y)
		nSelLength += m_ptSelectionEnd.x - m_ptSelectionStart.x;
	else
	{
		nSelLength += m_pLines[m_ptSelectionStart.y]->nLen - m_ptSelectionStart.x;
		for (int nY = m_ptSelectionStart.y + 1; nY < m_ptSelectionEnd.y; nY++)
			nSelLength += 2 + m_pLines[nY]->nLen;
		if (m_ptSelectionEnd.x > 0)
			nSelLength += m_ptSelectionEnd.x;
		nSelLength += 2;
	}

	if (nSelLength == 0)
		return 0;

	if (!OpenClipboard())
		return 0;

	EmptyClipboard();

    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, nSelLength * sizeof(TCHAR)); 
    if (hglbCopy == NULL) 
    { 
        CloseClipboard(); 
		return 0;
    } 

    // Lock the handle and copy the text to the buffer. 
    LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
	lptstrCopy[nSelLength - 1] = (TCHAR)0;	// null char
	if (m_ptSelectionStart.y == m_ptSelectionEnd.y)
		memcpy(lptstrCopy, m_pLines[m_ptSelectionStart.y]->cData + m_ptSelectionStart.x, m_ptSelectionEnd.x - m_ptSelectionStart.x);
	else
	{
		if (m_pLines[m_ptSelectionStart.y]->nLen > m_ptSelectionStart.x)
		{
			memcpy(lptstrCopy, m_pLines[m_ptSelectionStart.y]->cData + m_ptSelectionStart.x, m_pLines[m_ptSelectionStart.y]->nLen - m_ptSelectionStart.x);
			lptstrCopy += m_pLines[m_ptSelectionStart.y]->nLen - m_ptSelectionStart.x;
		}
		for (int nY = m_ptSelectionStart.y + 1; nY < m_ptSelectionEnd.y; nY++)
		{
			lptstrCopy[0] = '\r';
			lptstrCopy[1] = '\n';
			lptstrCopy += 2;
			if (m_pLines[nY]->nLen > 0)
			{
				memcpy(lptstrCopy, m_pLines[nY]->cData, m_pLines[nY]->nLen);
				lptstrCopy += m_pLines[nY]->nLen;
			}
		}
		lptstrCopy[0] = '\r';
		lptstrCopy[1] = '\n';
		lptstrCopy += 2;
		if (m_ptSelectionEnd.x > 0)
			memcpy(lptstrCopy, m_pLines[nY]->cData, m_ptSelectionEnd.x);
	}
    GlobalUnlock(hglbCopy); 

    // Place the handle on the clipboard. 
    SetClipboardData(CF_TEXT, hglbCopy);

	CloseClipboard();
	return 0;
}

LRESULT CGHOutputWnd::OnGetText(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pString = (LPTSTR)lParam;
	WPARAM wSize = wParam;
	for (int nLine = 0; (wSize > 0) && (nLine < m_nLineCount); nLine++)
	{
		int nCopy = min((int)wSize - 1, m_pLines[nLine]->nLen);
		if (nCopy > 0)
		{
			memcpy(pString, m_pLines[nLine]->cData, nCopy);
			wSize -= nCopy;
			pString += nCopy;
		}
		if (wSize > 0)
		{
			memcpy(pString, _T("\n"), sizeof(TCHAR));
			wSize--;
			pString++;
		}
	}
	if ((wSize == 0) && (wParam > 0))
	{
		pString--;
		wSize = 1;
	}
	if (wParam > 0)
		*pString = '\0';
	return wParam - wSize;
}

LRESULT CGHOutputWnd::OnSetText(WPARAM /*wParam*/, LPARAM lParam)
{
	Clear();
	return OnAddText(0, lParam);
}

LRESULT CGHOutputWnd::OnAddText(WPARAM /*wParam*/, LPARAM lParam)
{
	LPCTSTR pText = (LPCTSTR)lParam;
	int nLength = strlen(pText);
	if (nLength == 0)
		return TRUE;

	// Remember if end was in view:
	BOOL bCaretAtEnd = (m_ptCaret.y == m_nLineCount - 1) || (m_nLineCount == 0);
	BOOL bScrollEnd = (m_nLineCount - m_ptOffset.y <= m_sizePage.cy);

	// Add the text
	HDC hDC = NULL;
	HFONT hOldFont;
	if (GetSafeHwnd() != NULL)
	{
		hDC = ::GetDC(GetSafeHwnd());
		hOldFont = (HFONT)::SelectObject(hDC, m_font);
	}
	// Remove the last line...
	delete [] m_pLines[m_nLineCount-1];
	m_nLineCount--;

	int nStop;
	BOOL bEndLine;
	while (nLength > 0)
	{
		nStop = 0;
		bEndLine = FALSE;
		while (nStop < nLength)
		{
			switch (pText[nStop])
			{
				case '\n':
				case '\r':
					bEndLine = TRUE;
					break;
				default:
					nStop++;
					break;
			}
			if (bEndLine)
				break;
		}
		AddLine(pText, nStop, hDC);
		m_bAddToLast = !bEndLine;
		if (bEndLine)
		{
			if ((nStop < nLength - 1) && (pText[nStop] == '\r') && (pText[nStop + 1] == '\n'))
				nStop++;
			nStop++;
		} 
		pText += nStop;
		nLength -= nStop;
	}
	// Create empty last line
	ActualAddLine("", 0, hDC);
	if (hDC != NULL)
	{
		::SelectObject(hDC, hOldFont);
		::ReleaseDC(GetSafeHwnd(), hDC);
	}
	
	// Check if we need to delete some lines
	if (m_nLineCount > m_nMaxLines)
	{
		int nDelete = (m_nLineCount - m_nMaxLines) + m_sizePage.cy;
		DeleteLines(nDelete);
		if (m_ptOffset.y < m_sizePage.cy)
			Invalidate();
	}

	if (bCaretAtEnd)
	{
		// Make sure end is still in view!
		m_ptCaret.y = m_nLineCount - 1;
		m_ptCaret.x = 0;
		if (bScrollEnd)
		{
			Invalidate();
			if (m_nLineCount >= m_ptOffset.y + m_sizePage.cy)
				m_ptOffset.y = max(0, m_nLineCount - 1 - (m_sizePage.cy / 2));
		}
	}
	else if (bScrollEnd)
		Invalidate();

	MoveCaret(FALSE);

	SetVScroll();
	SetHScroll();

	return TRUE;
}

//	Mouse messages
void CGHOutputWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bCapture)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	SetCapture();
	m_bCapture = TRUE;
	m_ptCaret = LocationFromPoint(point);
	m_ptDragStart = m_ptCaret;
	SetSelection(m_ptCaret, m_ptCaret, TRUE);
	MoveCaret(FALSE);
	UpdateWindow();
	m_ptLastMove = point;
}

void CGHOutputWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bCapture)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}
	if (point == m_ptLastMove)
		return;
	m_ptCaret = LocationFromPoint(point);
	SetSelection(m_ptDragStart, m_ptCaret, TRUE);
	ScrollToCaret();
	UpdateWindow();
	m_ptLastMove = point;
}

void CGHOutputWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bCapture)
	{
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}
	ReleaseCapture();
	m_bCapture = FALSE;
	if (point == m_ptLastMove)
		return;
	m_ptCaret = LocationFromPoint(point);
	SetSelection(m_ptDragStart, m_ptCaret, TRUE);
	ScrollToCaret();
	UpdateWindow();
}

//	Focus messages
void CGHOutputWnd::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	m_bHasCaret = TRUE;
	::CreateCaret(GetSafeHwnd(), NULL, CARET_WIDTH, m_sizeLetter.cy);
	ShowCaret();
	MoveCaret(FALSE);
	SetActiveWindow();
}

void CGHOutputWnd::OnKillFocus(CWnd* pNewWnd) 
{
	HideCaret();
	::DestroyCaret();
	m_bHasCaret = FALSE;
	CWnd::OnKillFocus(pNewWnd);
}

int CGHOutputWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	int nRet = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	if ((nRet == MA_ACTIVATE) || (nRet == MA_ACTIVATEANDEAT))
		SetFocus();
	return nRet;
}

//	Keyboard messages
void CGHOutputWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_nLineCount == 0)
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	BOOL bControl = ::GetKeyState(VK_CONTROL) & 0x8000;
	CPoint ptNew = m_ptCaret;
	switch (nChar)
	{
		case VK_UP:
			ptNew.y--;
			break;
		case VK_DOWN:
			ptNew.y++;
			break;
		case VK_LEFT:
			if (bControl && (m_nLineCount > 0))
			{
				if (ptNew.x == 0)
				{
					if (ptNew.y > 0)
					{
						ptNew.y--;
						ptNew.x = m_pLines[ptNew.y]->nLen;
					}
				}
				while (IsSpaceLeft(ptNew) && (ptNew.x > 0))
					ptNew.x--;
				if (ptNew.x == 0)
					break;
				BOOL bCharLeft = IsCharLeft(ptNew);
				if (ptNew.x > 0)
					do
						ptNew.x--;
					while ((IsCharLeft(ptNew) == bCharLeft) && (ptNew.x > 0));
			}
			else
			{
				ptNew.x--;
				if (ptNew.x < 0)
				{
					if (ptNew.y > 0)
					{
						ptNew.x = CHARS_WIDTH;
						ptNew.y--;
					}
					else
						ptNew.x = 0;
				}
			}
			break;
		case VK_RIGHT:
			if (bControl && (m_nLineCount > 0))
			{
				if (ptNew.x == m_pLines[ptNew.y]->nLen)
				{
					if (ptNew.y < m_nLineCount - 1)
					{
						ptNew.y++;
						ptNew.x = 0;
					}
				}
				int nLen = m_pLines[ptNew.y]->nLen;
				while (IsSpaceRight(ptNew) && (ptNew.x < nLen))
					ptNew.x++;
				BOOL bCharRight = IsCharRight(ptNew);
				if (ptNew.x < nLen)
					do
						ptNew.x++;
					while ((IsCharRight(ptNew) == bCharRight) && (ptNew.x < nLen));
			}
			else
			{
				ptNew.x++;
				if ((ptNew.x > m_pLines[ptNew.y]->nLen) && (ptNew.y < m_nLineCount - 1))
				{
					ptNew.x = 0;
					ptNew.y++;
				}
			}
			break;
		case VK_END:
			if (bControl)
			{
				if (m_nLineCount == 0)
					ptNew = CPoint(0,0);
				else
					ptNew = CPoint(m_pLines[m_nLineCount - 1]->nLen, m_nLineCount - 1);
			}
			else
				ptNew.x = m_nLineCount == 0 ? 0 : m_pLines[ptNew.y]->nLen;
			break;
		case VK_HOME:
			if (bControl)
				ptNew = CPoint(0,0);
			else
				ptNew.x = 0;
			break;
		case VK_PRIOR:
			ptNew.y -= m_sizePage.cy;
			break;
		case VK_NEXT:
			ptNew.y += m_sizePage.cy;
			break;
		case 'c':
		case 'C':
		case VK_INSERT:
			if (bControl)
			{
				Copy();
				return;
			}
			break;
		default:
			CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
	}
	if (ptNew.y < 0)
		ptNew.y = 0;
	else if (ptNew.y >= m_nLineCount)
		ptNew.y = max(m_nLineCount - 1, 0);
	ptNew.x = min(ptNew.x, m_pLines[ptNew.y]->nLen);
	if (ptNew == m_ptCaret)
		return;
	if (::GetKeyState(VK_SHIFT) & 0x8000)
	{
		// Extend the selection!
		SetSelection(m_ptDragStart, ptNew, TRUE);
	}
	else
	{
		m_ptDragStart = ptNew;
		if (m_bShowSelection)
			SetSelection(ptNew, ptNew, TRUE);
	}
	m_ptCaret = ptNew;
	ScrollToCaret();
	UpdateWindow();
}


// Helper functions
void CGHOutputWnd::DoSetFont()
{
	if (GetSafeHwnd() == NULL)
		return;

	HDC hDC = ::GetDC(GetSafeHwnd());
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_font);

	// Set letter size
	TEXTMETRIC metric;
	::GetTextMetrics(hDC, &metric);
	m_sizeLetter = CSize(metric.tmAveCharWidth, metric.tmHeight + metric.tmExternalLeading);

	// Calculate text width...
	SIZE sz;
	for (int i=0;i<m_nLineCount;i++)
		if ((m_pLines[i]->nLen > 0) && ::GetTextExtentPoint32(hDC, m_pLines[i]->cData, m_pLines[i]->nLen, &sz))
			m_pLines[i]->nWidth = sz.cx;

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(GetSafeHwnd(), hDC);

	CRect rect;
	GetClientRect(&rect);
	if (m_sizeLetter.cy == 0)
		m_sizePage.cy = 0;
	else
		m_sizePage.cy = (rect.Height() / m_sizeLetter.cy) + (rect.Height() % m_sizeLetter.cy == 0 ? 0 : 1);
	if (m_sizeLetter.cx == 0)
		m_sizePage.cx = 0;
	else
		m_sizePage.cx = rect.Width();

	SetVScroll();
	SetHScroll();
	MoveCaret(TRUE);
}

void CGHOutputWnd::SetVScroll()
{
	ShowScrollBar(SB_VERT, TRUE);
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_PAGE|SIF_POS|SIF_RANGE;
	info.nMin = 0;
	info.nMax = m_nLineCount + m_sizePage.cy - 2;
	info.nPage = m_sizePage.cy;
	info.nPos = m_ptOffset.y;
	SetScrollInfo(SB_VERT, &info);
}

void CGHOutputWnd::SetHScroll()
{
	if (m_nMaxWidth == -1)
	{
		if (m_nLineCount == 0)
			ShowScrollBar(SB_HORZ, FALSE);
		for (int i=0;i<m_nLineCount;i++)
			if (m_pLines[i]->nWidth + CARET_WIDTH> m_nMaxWidth)
				m_nMaxWidth = m_pLines[i]->nWidth + CARET_WIDTH;
	}
	if (m_sizePage.cx >= m_nMaxWidth)
	{
		ShowScrollBar(SB_HORZ, FALSE);
		return;
	}
	ShowScrollBar(SB_HORZ, TRUE);
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_PAGE|SIF_POS|SIF_RANGE;
	info.nMin = 0;
	info.nMax = m_nMaxWidth;
	info.nPage = m_sizePage.cx;
	info.nPos = m_ptOffset.x;
	SetScrollInfo(SB_HORZ, &info);
}

void CGHOutputWnd::DeleteLines(int nDelete)
{
	for (int i=0;i<nDelete;i++)
		delete [] m_pLines[i];
	memmove(m_pLines, m_pLines + nDelete, (m_nLineCount - nDelete) * sizeof(OutputLine*));
	m_nLineCount -= nDelete;

	if (m_bShowSelection)
	{
		InvalidateSelection();
		m_ptSelectionStart.y -= nDelete;
		if (m_ptSelectionStart.y < 0)
		{
			m_ptSelectionStart.x = 0;
			m_ptSelectionStart.y = 0;
			m_ptSelectionEnd.x = 0;
			m_ptSelectionEnd.y = 0;
		}
		else
			m_ptSelectionEnd.y -= nDelete;
		m_bShowSelection = (m_ptSelectionStart.x != m_ptSelectionEnd.x) || (m_ptSelectionStart.y != m_ptSelectionEnd.y);
	}
	if (m_bCapture)
	{
		if ((m_ptDragStart.y < nDelete) || (m_ptCaret.y < nDelete))
		{
			::ReleaseCapture();
			m_bCapture = FALSE;
		}
		else
		{
			m_ptDragStart.y -= nDelete;
		}
	}
	m_ptCaret.y -= nDelete;
	if (m_ptCaret.y < 0)
	{
		m_ptCaret.y = 0;
		if (m_nLineCount == 0)
			m_ptCaret.x = 0;
		else
			m_ptCaret.x = min(m_ptCaret.x, m_pLines[m_ptCaret.y]->nLen);
	}
	m_ptOffset.y -= nDelete;
	if (m_ptOffset.y < 0)
		m_ptOffset.y = 0;
	m_nMaxWidth = -1;
	if (GetSafeHwnd() != NULL)
	{
		SetVScroll();
		SetHScroll();
		MoveCaret(FALSE);
		InvalidateSelection();
	}
}

void CGHOutputWnd::AddText(LPCTSTR pText)
{
	if (GetSafeHwnd() != NULL)
		SendMessage(WM_ADDTEXT, 0, (LPARAM)pText);
}

void CGHOutputWnd::ActualAddLine(LPCTSTR pLine, int nLength, HDC hDC)
{
	if (m_nLinesSize == m_nLineCount)
	{
		int nNewSize = m_nLinesSize < m_nMaxLines ? m_nLinesSize + 100 : m_nLinesSize + 10;
		OutputLine** pNewLines = (OutputLine**)new char[nNewSize * sizeof(OutputLine*)];
		if (m_nLineCount > 0)
			memcpy(pNewLines, m_pLines, m_nLineCount * sizeof(OutputLine*));
		if (m_nLinesSize > 0)
			delete [] m_pLines;
		m_nLinesSize = nNewSize;
		m_pLines = pNewLines;
	}

	m_pLines[m_nLineCount] = (OutputLine*)new char[sizeof(OutputLine) + nLength];
	m_pLines[m_nLineCount]->nLen = nLength;
	SIZE sz;
	if ((nLength > 0) && (hDC != NULL) && ::GetTextExtentPoint32(hDC, pLine, nLength, &sz))
	{
		m_pLines[m_nLineCount]->nWidth = sz.cx;
		if ((m_nMaxWidth > -1) && (m_nMaxWidth < sz.cx + CARET_WIDTH))
			m_nMaxWidth = sz.cx + CARET_WIDTH;
	}
	else
		m_pLines[m_nLineCount]->nWidth = 0;
	if (nLength > 0)
		memcpy(m_pLines[m_nLineCount]->cData, pLine, nLength);
	m_nLineCount++;
}

void CGHOutputWnd::AddLine(LPCTSTR pLine, int nLength, HDC hDC)
{
	if ((nLength == 0) && m_bAddToLast)
	{
		m_bAddToLast = FALSE;
		return;
	}
	BOOL bCopied = FALSE;
	if (m_bAddToLast && (m_nLineCount > 0) && (m_pLines[m_nLineCount - 1]->nLen > 0))
	{
		LPTSTR pNewLine = new char[m_pLines[m_nLineCount - 1]->nLen + nLength];
		if (m_pLines[m_nLineCount - 1]->nLen > 0)
			memcpy(pNewLine, m_pLines[m_nLineCount - 1]->cData, m_pLines[m_nLineCount - 1]->nLen);
		if (nLength > 0)
			memcpy(pNewLine + m_pLines[m_nLineCount - 1]->nLen, pLine, nLength);
		pLine = pNewLine;
		nLength += m_pLines[m_nLineCount - 1]->nLen;
		delete [] m_pLines[m_nLineCount - 1];
		m_nLineCount--;
		bCopied = TRUE;
	}
	LPCTSTR pText = pLine;
	while (nLength > CHARS_WIDTH)
	{
		ActualAddLine(pText, CHARS_WIDTH, hDC);
		pText += CHARS_WIDTH;
		nLength -= CHARS_WIDTH;
	}
	ActualAddLine(pText, nLength, hDC);
	m_bAddToLast = TRUE;
	if (bCopied)
		delete [] (LPTSTR)pLine;
}

POINT CGHOutputWnd::LocationFromPoint(POINT pt)
{
	POINT ptRet = {0,0};
	if ((m_sizeLetter.cy == 0) || (m_nLineCount == 0))
		return ptRet;
	ptRet.y = m_ptOffset.y + pt.y / m_sizeLetter.cy;
	if (ptRet.y < 0)
	{
		ptRet.y = 0;
		ptRet.x = 0;
	}
	else if (ptRet.y >= m_nLineCount)
	{
		ptRet.y = m_nLineCount - 1;
		ptRet.x = m_pLines[ptRet.y]->nLen;
	}
	else
	{
		int nX = m_ptOffset.x + pt.x;
		if (nX < 0)
			ptRet.x = 0;
		else if (m_pLines[ptRet.y]->nLen == 0)
			ptRet.x = 0;
		else
		{
			HDC hDC = ::GetDC(GetSafeHwnd());
			HFONT hOldFont = (HFONT)::SelectObject(hDC, m_font);
			CSize sz;
			if (!::GetTextExtentExPoint(hDC, m_pLines[ptRet.y]->cData, m_pLines[ptRet.y]->nLen, nX, (int*)&ptRet.x, NULL, &sz))
				ptRet.x = 0;
			else if (ptRet.x < m_pLines[ptRet.y]->nLen - 1)
			{
				nX -= sz.cx;
				if (::GetTextExtentPoint32(hDC, m_pLines[ptRet.y]->cData + ptRet.x, 1, &sz) && (nX > sz.cx / 2))
					ptRet.x++;
			}
			::SelectObject(hDC, hOldFont);
			::ReleaseDC(GetSafeHwnd(), hDC);
		}
	}
	return ptRet;
}

void CGHOutputWnd::SetSelection(CPoint pt1, CPoint pt2, BOOL bInvalidate)
{
	CPoint ptSelStart = m_ptSelectionStart, ptSelEnd = m_ptSelectionEnd;
	BOOL bOldShow = m_bShowSelection;
	m_bShowSelection = pt1 != pt2;
	if ((pt1.y < pt2.y) || ((pt1.y == pt2.y) && (pt1.x <= pt2.x)))
	{
		m_ptSelectionStart = pt1;
		m_ptSelectionEnd = pt2;
	}
	else
	{
		m_ptSelectionStart = pt2;
		m_ptSelectionEnd = pt1;
	}
	if (bInvalidate)
	{
		if (bOldShow)
		{
			if (m_bShowSelection)
			{
				InvalidateText(ptSelStart, m_ptSelectionStart, FALSE);
				InvalidateText(m_ptSelectionEnd, ptSelEnd, FALSE);
			}
			else
				InvalidateText(ptSelStart, ptSelEnd, FALSE);
		}
		else
			InvalidateSelection();
	}		
}

void CGHOutputWnd::InvalidateSelection()
{
	if (!m_bShowSelection)
		return;
	InvalidateText(m_ptSelectionStart, m_ptSelectionEnd);
}

void CGHOutputWnd::InvalidateText(CPoint ptStart, CPoint ptEnd, BOOL bOrdered /* = TRUE */)
{
	if (!bOrdered)
	{
		if ((ptStart.y > ptEnd.y) || ((ptStart.y == ptEnd.y) && (ptStart.x > ptEnd.x)))
		{
			CPoint pt = ptStart;
			ptStart = ptEnd;
			ptEnd = pt;
		}
	}
	CRect rect(0,0,0,0);
	if (ptEnd.y == ptStart.y)
	{
		rect.left = WndFromText(ptStart).x;
		ptEnd.x++;
		rect.right = WndFromText(ptEnd).x;
	}
	else
		GetClientRect(&rect);
	rect.bottom = (ptEnd.y - ptStart.y + 2) * m_sizeLetter.cy;
	rect.OffsetRect(0, (ptStart.y - m_ptOffset.y) * m_sizeLetter.cy);
	InvalidateRect(&rect, FALSE);
}

POINT CGHOutputWnd::WndFromText(POINT pt)
{
	POINT ptRet;
	ptRet.y = (pt.y - m_ptOffset.y) * m_sizeLetter.cy;
	if (pt.x == 0)
		ptRet.x = 0;
	else if (m_pLines[pt.y]->nLen == 0)
		ptRet.x = 0;
	else
	{
		HDC hDC = ::GetDC(GetSafeHwnd());
		HFONT hOldFont = (HFONT)::SelectObject(hDC, m_font);
		SIZE sz;
		if (!::GetTextExtentPoint32(hDC, m_pLines[pt.y]->cData, pt.x, &sz))
			ptRet.x = 0;
		else
			ptRet.x = sz.cx - m_ptOffset.x;
		::SelectObject(hDC, hOldFont);
		::ReleaseDC(GetSafeHwnd(), hDC);
	}
	return ptRet;
}

void CGHOutputWnd::MoveCaret(BOOL bNow)
{
	if (!m_bHasCaret)
		return;

	if (bNow)
		HideCaret();
	CPoint pt = WndFromText(m_ptCaret);
	SetCaretPos(pt);
	if (bNow)
		ShowCaret();
	else
	{
		CRect rect(pt, CSize(CARET_WIDTH, m_sizeLetter.cy));
		InvalidateRect(&rect, TRUE);
	}
}

BOOL CGHOutputWnd::IsCharRight(CPoint pt)
{
	if (m_nLineCount == 0)
		return FALSE;
	if (pt.x >= m_pLines[pt.y]->nLen)
		return FALSE;
	return isalnum(m_pLines[pt.y]->cData[pt.x]) != 0;
}

BOOL CGHOutputWnd::IsCharLeft(CPoint pt)
{
	if (m_nLineCount == 0)
		return FALSE;
	
	if ((pt.x == 0) || (m_pLines[pt.y]->nLen == 0))
		return FALSE;
	return isalnum(m_pLines[pt.y]->cData[pt.x - 1]) != 0;
}

BOOL CGHOutputWnd::IsSpaceLeft(CPoint pt)
{
	if (m_nLineCount == 0)
		return FALSE;
	
	if ((pt.x == 0) || (m_pLines[pt.y]->nLen == 0))
		return FALSE;
	return m_pLines[pt.y]->cData[pt.x - 1] == ' ';
}

BOOL CGHOutputWnd::IsSpaceRight(CPoint pt)
{
	if (m_nLineCount == 0)
		return FALSE;
	if (pt.x >= m_pLines[pt.y]->nLen)
		return FALSE;
	return m_pLines[pt.y]->cData[pt.x] == ' ';
}

void CGHOutputWnd::ScrollToCaret()
{
	// Make sure that the caret is visible:
	POINT ptNew = m_ptOffset;
	int nXCaret;
	if ((m_ptCaret.x == 0) || (m_nLineCount == 0))
		nXCaret = 0;
	else if (m_ptCaret.x >= m_pLines[m_ptCaret.y]->nLen)
		nXCaret = m_pLines[m_ptCaret.y]->nWidth;
	else
	{
		HDC hDC = ::GetDC(GetSafeHwnd());
		HFONT hOldFont = (HFONT)::SelectObject(hDC, m_font);
		SIZE sz;
		if (!::GetTextExtentPoint32(hDC, m_pLines[m_ptCaret.y]->cData, m_ptCaret.x, &sz))
			nXCaret = 0;
		else
			nXCaret = sz.cx;
		::SelectObject(hDC, hOldFont);
		::ReleaseDC(GetSafeHwnd(), hDC);
	}

	if (m_ptCaret.y < m_ptOffset.y)
		ptNew.y = m_ptCaret.y;
	else if (m_ptCaret.y >= m_ptOffset.y + m_sizePage.cy)
		ptNew.y = max(0, m_ptCaret.y - m_sizePage.cy + 1);
	if (nXCaret < m_ptOffset.x)
		ptNew.x = nXCaret;
	else if (nXCaret >= m_ptOffset.x + m_sizePage.cx)
		ptNew.x = max(0, nXCaret - m_sizePage.cx + 1);
	if ((ptNew.x != m_ptOffset.x) || (ptNew.y != m_ptOffset.y))
	{
		if ((abs(ptNew.y - m_ptOffset.y) < m_sizePage.cy) && (abs(ptNew.x - m_ptOffset.x) < m_sizePage.cx))
		{
			ScrollWindow(m_ptOffset.x - ptNew.x, (m_ptOffset.y - ptNew.y) * m_sizeLetter.cy);
			m_ptOffset = ptNew;
			UpdateWindow();
		}
		else
			Invalidate();
		m_ptOffset = ptNew;
		SetScrollPos(SB_VERT, m_ptOffset.y);
		SetScrollPos(SB_HORZ, m_ptOffset.x);
	}
	MoveCaret(FALSE);
}

BOOL CGHOutputWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID);
}

BOOL CGHOutputWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_VSCROLL);

	pBar = GetScrollBarCtrl(SB_HORZ);
	BOOL bHasHorzBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_HSCROLL);

	if (!bHasVertBar && !bHasHorzBar)
		return FALSE;

	BOOL bResult = FALSE;
	UINT uWheelScrollLines = GetMouseScrollLines();
	int nToScroll, nDisplacement, nNewPos;

	if (bHasVertBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_sizePage.cy;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = min(nToScroll, m_sizePage.cy);
		}
		nNewPos = m_ptOffset.y + nDisplacement;
		if (nNewPos < 0)
			nNewPos = 0;
		else if (nNewPos > m_nLineCount - 1)
			nNewPos = m_nLineCount - 1;

		if (nNewPos != m_ptOffset.y)
		{
			ScrollY(nNewPos - m_ptOffset.y);
			SetScrollPos(SB_VERT, nNewPos);
		}
	}
	else if (bHasHorzBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_sizePage.cx;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = min(nToScroll, m_sizePage.cx);
		}
		nNewPos = m_ptOffset.x + nDisplacement;
		if (nNewPos < 0)
			nNewPos = 0;
		else if (nNewPos > m_nMaxWidth - m_sizePage.cx)
			nNewPos = m_nMaxWidth - m_sizePage.cx;

		if (nNewPos != m_ptOffset.x)
		{
			ScrollX(nNewPos - m_ptOffset.x);
			SetScrollPos(SB_VERT, nNewPos);
		}
	}

	return TRUE;
}

UINT CGHOutputWnd::GetMouseScrollLines()
{
	// if we've already got it and we're not refreshing,
	// return what we've already got

	if (m_bGotScrollLines)
		return m_uCachedScrollLines;

	// see if we can find the mouse window

	m_bGotScrollLines = TRUE;


	if (m_nRegisteredMessage == 0)
	{
		m_msgGetScrollLines = ::RegisterWindowMessage(MSH_SCROLL_LINES);
		if (m_msgGetScrollLines == 0)
			m_nRegisteredMessage = 1;     // couldn't register!  never try again
		else
			m_nRegisteredMessage = 2;     // it worked: use it
	}

	if (m_nRegisteredMessage == 2)
	{
		HWND hwMouseWheel = NULL;
		hwMouseWheel = ::FindWindow(MSH_WHEELMODULE_CLASS, MSH_WHEELMODULE_TITLE);
		if (hwMouseWheel && m_msgGetScrollLines)
		{
			m_uCachedScrollLines = (UINT) ::SendMessage(hwMouseWheel, m_msgGetScrollLines, 0, 0);
			return m_uCachedScrollLines;
		}
	}

	// couldn't use the window -- try system settings
	m_uCachedScrollLines = 3; // reasonable default
	DWORD dwVersion = ::GetVersion();
	if ((BYTE)dwVersion < 4)
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
				0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			TCHAR szData[128];
			DWORD dwKeyDataType;
			DWORD dwDataBufSize = sizeof(szData)/sizeof(szData[0]);

			if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
					(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
			{
				m_uCachedScrollLines = _tcstoul(szData, NULL, 10);
			}
			RegCloseKey(hKey);
		}
	}
	else if ((dwVersion & 0x80000000) == 0)
	{
		::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &m_uCachedScrollLines, 0);
	}

	return m_uCachedScrollLines;
}

void CGHOutputWnd::PreSubclassWindow()
{
	DoSetFont();
	CRect rect;
	GetClientRect(&rect);
	PostMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()));
}

UINT CGHOutputWnd::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void CGHOutputWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	HMENU hMenu = ::CreatePopupMenu();
	if (hMenu == NULL)
		return;
	MENUITEMINFO info;
	memset(&info, 0, sizeof(info));
	info.cbSize = sizeof(info);
	info.fMask = MIIM_ID|MIIM_TYPE|MIIM_STATE;
	info.fType = MFT_STRING;
	info.fState = (m_bShowSelection) ? MFS_ENABLED|MFS_DEFAULT : MFS_DISABLED|MFS_GRAYED;
	info.wID = WM_COPY;
	info.dwTypeData = "&Copy";
	info.cch = 4;
	::InsertMenuItem(hMenu, 0, TRUE, &info);
	info.fState = MFS_ENABLED;
	info.wID = WM_CLEAR;
	info.dwTypeData = "C&lear";
	info.cch = 5;
	::InsertMenuItem(hMenu, 1, TRUE, &info);
	UINT uSelected = ::TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, 0, GetSafeHwnd(), NULL);
	switch (uSelected)
	{
		case WM_COPY:
			PostMessage(WM_COPY);
			break;
		case WM_CLEAR:
			PostMessage(WM_CLEAR);
			break;
	}
}
