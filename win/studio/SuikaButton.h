/*****************************************************************************
 SuikaButton.h
 カスタムボタン

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _SUIKABUTTON_H_
#define _SUIKABUTTON_H_

#include "klib/graphic.h"

class CSuikaButton : public CStatic
{
public:
	CSuikaButton();
	~CSuikaButton();

	BOOL	LoadBitmap(LPCSTR lpszResource);
	BOOL	Create(LPCSTR lpszCaption, const RECT& rc, CWnd *pParent, UINT nID);

	void	Enable();
	void	Disable();

private:
	CString	m_strCaption;
	CRect	m_rc;
	CWnd	*m_pParentWnd;
	UINT	m_nID;
	int		m_iState;
	KImage	*button_image;

	void	ShowPopup();
	void	ClosePopup();


/////////////////////////////////////////////////////////////////////////////
private:
	//{{AFX_VIRTUAL(CSuikaButton)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSuikaButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
