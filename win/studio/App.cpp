/*****************************************************************************
 App.h
 シーンエディタ

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "App.h"
#include "klib/debug.h"

BEGIN_MESSAGE_MAP(CSceneEditApp, CWinApp)
	//{{AFX_MSG_MAP(CSceneEditApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CSceneEditApp::CSceneEditApp()
{
}

CSceneEditApp theApp;

BOOL CSceneEditApp::InitInstance()
{
	Enable3dControls();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	ProcessShellCommand(cmdInfo);

	EnableShellOpen();


	m_wndMain.Create();
//	m_wndMain.Load(cmdInfo.m_strFileName);
	m_pMainWnd = &m_wndMain;
	return TRUE;
}

BOOL CSceneEditApp::OnDDECommand(LPTSTR lpszCommand)
{
	return FALSE;
}
