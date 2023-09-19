/*****************************************************************************
 App.h
 シーンエディタ

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _APP_H_
#define _APP_H_

#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "MainWindow.h"
#include "Resource.h"

class CSceneEditApp : public CWinApp
{
public:
	CSceneEditApp();

private:
	CMainWindow		m_wndMain;

///////////////////////////////////////////////////////////////////////////////
	//{{AFX_VIRTUAL(CSceneEditApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnDDECommand(LPTSTR lpszCommand);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSceneEditApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
