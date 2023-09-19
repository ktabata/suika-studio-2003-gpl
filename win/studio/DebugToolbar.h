/*****************************************************************************
 DebugToolBar.h
 デバッグツールバー

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _DEBUGTOOLBAR_H_
#define _DEBUGTOOLBAR_H_

#include "MainWindow.h"
#include "SuikaButton.h"

class CMainWindow;

class CDebugToolbar : public CStatic
{
public:
	CDebugToolbar(CMainWindow *pWnd);
	~CDebugToolbar();

	BOOL	Create(CWnd *pParent, UINT nID);
	int		GetHeight();

private:
	CMainWindow		*m_pMainWnd;
	CSuikaButton	*m_btnDebugPlay;
	CSuikaButton	*m_btnDebugPause;
	CSuikaButton	*m_btnDebugPlayOne;
	CSuikaButton	*m_btnDebugChangeWin;
	CSuikaButton	*m_btnDebugEndReturn;
	CSuikaButton	*m_btnDebugEndAt;

/////////////////////////////////////////////////////////////////////////////

	//{{AFX_VIRTUAL(CDebugToolbar)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDebugToolbar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnDebugEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
