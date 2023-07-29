/*******************************************************************************/
/* GHOutputWnd - an MFC based output text window, enabling selection and       */
/* copying (but not editing) of the text visible in the control                */
/*                                                                             */
/* GHOutputWnd.h : header file                                                 */
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

#if !defined(AFX_OUTPUTWND_H__C0B22E1A_C0B5_409C_B234_95FEAE47C327__INCLUDED_)
#define AFX_OUTPUTWND_H__C0B22E1A_C0B5_409C_B234_95FEAE47C327__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGHOutputWnd window
#define WM_ADDTEXT		(WM_USER + 0x18a)
#define WM_SETMAXLINES	(WM_USER + 0x18b)
#define WM_GETMAXLINES	(WM_USER + 0x18c)

struct OutputLine;

class CGHOutputWnd : public CWnd
{
// Construction
public:
	CGHOutputWnd();

	DECLARE_DYNCREATE(CGHOutputWnd)

protected:
	// Members
	BOOL			m_bAutoDestroy;

	//	Data
	BOOL			m_bAddToLast;
	OutputLine**	m_pLines;
	int				m_nLineCount;
	int				m_nLinesSize;
	int				m_nMaxWidth;

	//	View
	int				m_nMaxLines;
	SIZE			m_sizeLetter;		// Pixels
	HFONT			m_font;
	POINT			m_ptOffset;			// Y in char units, X in pixels
	SIZE			m_sizePage;			// Y in char units, X in pixels
	BOOL			m_bHasCaret;

	//	Selection
	POINT			m_ptSelectionStart;	// Char units
	POINT			m_ptSelectionEnd;	// Char units
	POINT			m_ptDragStart;		// Char units
	BOOL			m_bShowSelection;

	//	Mouse selection
	BOOL			m_bCapture;
	POINT			m_ptCaret;			// Char units
	POINT			m_ptLastMove;		// Pixels

	//	Mouse wheel - statics
	static BOOL		m_bGotScrollLines;
	static UINT		m_uCachedScrollLines;
	static UINT		m_msgGetScrollLines;
	static WORD		m_nRegisteredMessage;

// Operations
public:
	// Data Access
	void			SetMaxLines(int nLines) {SendMessage(WM_SETMAXLINES, nLines);};
	void			SetAutoDestroy(BOOL bAuto = TRUE) {m_bAutoDestroy = bAuto;};

	int				GetMaxLines() const {return m_nMaxLines;};

	// Methods
	BOOL			Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void			AddText(LPCTSTR pText);
	void			Clear() {SendMessage(WM_CLEAR);};
	void			Copy() {SendMessage(WM_COPY);};

	// Overrides
	virtual void	PreSubclassWindow();

protected:
	// Helpers
	//	Display
	void			DoSetFont();

	//	Scrolls
	void			SetVScroll();
	void			SetHScroll();

	//	Text changes
	void			AddLine(LPCTSTR pLine, int nLength, HDC hDC);
	void			ActualAddLine(LPCTSTR pLine, int nLength, HDC hDC);
	void			DeleteLines(int nDelete);

	//	Positioning
	POINT			LocationFromPoint(POINT pt);
	POINT			WndFromText(POINT pt);

	//	Selection
	void			SetSelection(CPoint pt1, CPoint pt2, BOOL bInvalidate);
	void			InvalidateSelection();
	void			InvalidateText(CPoint ptStart, CPoint ptEnd, BOOL bOrdered = TRUE);

	//	Caret
	void			MoveCaret(BOOL bShow);
	void			ScrollToCaret();

	//	Moving the window
	void			ScrollY(int nAmount);
	void			ScrollX(int nAmount);

	//	Text Helpers
	BOOL			IsCharRight(CPoint pt);
	BOOL			IsCharLeft(CPoint pt);
	BOOL			IsSpaceLeft(CPoint pt);
	BOOL			IsSpaceRight(CPoint pt);

	// Static helpers
	//	Mouse wheel
	static UINT		GetMouseScrollLines();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGHOutputWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGHOutputWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGHOutputWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnWndSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCopy(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnClear(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnAddText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetMaxLines(WPARAM /*wParam*/, LPARAM /*lParam*/) {return m_nMaxLines;};
	afx_msg LRESULT OnSetMaxLines(WPARAM wParam, LPARAM /*lParam*/);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTWND_H__C0B22E1A_C0B5_409C_B234_95FEAE47C327__INCLUDED_)
