/*****************************************************************************
 MainWindow.h
 シーンエディタのウィンドウ

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "SceneView.h"
#include "EditToolbar.h"
#include "DebugToolbar.h"
#include "Resource.h"
#include "player/KRuntimeEnvWin32.h"

class CDebugToolbar;

class CMainWindow : public CWnd
{
public:
	CMainWindow();
	virtual ~CMainWindow();

	// ロード
	BOOL	Load(CString strFileName);

private:
	// 編集中のファイルのフルパス
	CString			m_strFileName;

	// コントロール
	HMENU			m_hMenu;
	CSceneView		*m_pView;
	CSceneView		*m_pSaveView;
	CEditToolbar	*m_pEditToolbar;
	CDebugToolbar	*m_pDebugToolbar;

	// アクセラレータ
	HACCEL	m_hAccel;

	// ウィンドウのスタイル・位置・サイズの保存・復元
	BOOL	m_bFullScreen;
	BOOL	m_bDebugWindow;
	CRect	m_rcWindow;
	DWORD	m_dwOldStyle;
	DWORD	m_dwOldExStyle;
	void	SaveStyleAndRect();
	void	RestoreStyleAndRect();
	void	SwitchToDebugWindow();
	void	SwitchToEditWindow(BOOL bShowDebugToolbar);

	// save/load helpers
	BOOL	AskAndSave(BOOL bShowNo);
	BOOL	Save(CString strFileName);

/**********************************************************************
  デバッグパート(CMainWindowから分離したいが良い方法が見つからない)
**********************************************************************/
public:
	// CDebugToolBarから呼び出されるデバッグ操作
	void	OnDebugEndReturn();
	void	OnDebugPlay();
	void	OnDebugChangeWindow();

private:
	// デバッグ
	KString				m_strRootPath;
	BOOL				m_bDebugging;
	KString				m_strPrevScene;	// デバッグ開始前に編集していたファイル
	int					m_iPrevLine;
	KRuntimeEnvWin32	*m_pRuntime;
	BOOL	SelRootDir();
	BOOL	PrepareStartDebug();
	void	RestoreScene();
/*********************
 デバッグパート終わり
*********************/

/////////////////////////////////////////////////////////////////////////////

	//{{AFX_VIRTUAL(CMainWindow)
	public:
	virtual BOOL Create();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMainWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnChangeScreenMode();
	afx_msg void OnDebugStartCur();
	afx_msg void OnDebugSelRoot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
