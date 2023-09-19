/*****************************************************************************
 MainWindow.h
 �V�[���G�f�B�^�̃E�B���h�E

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

	// ���[�h
	BOOL	Load(CString strFileName);

private:
	// �ҏW���̃t�@�C���̃t���p�X
	CString			m_strFileName;

	// �R���g���[��
	HMENU			m_hMenu;
	CSceneView		*m_pView;
	CSceneView		*m_pSaveView;
	CEditToolbar	*m_pEditToolbar;
	CDebugToolbar	*m_pDebugToolbar;

	// �A�N�Z�����[�^
	HACCEL	m_hAccel;

	// �E�B���h�E�̃X�^�C���E�ʒu�E�T�C�Y�̕ۑ��E����
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
  �f�o�b�O�p�[�g(CMainWindow���番�����������ǂ����@��������Ȃ�)
**********************************************************************/
public:
	// CDebugToolBar����Ăяo�����f�o�b�O����
	void	OnDebugEndReturn();
	void	OnDebugPlay();
	void	OnDebugChangeWindow();

private:
	// �f�o�b�O
	KString				m_strRootPath;
	BOOL				m_bDebugging;
	KString				m_strPrevScene;	// �f�o�b�O�J�n�O�ɕҏW���Ă����t�@�C��
	int					m_iPrevLine;
	KRuntimeEnvWin32	*m_pRuntime;
	BOOL	SelRootDir();
	BOOL	PrepareStartDebug();
	void	RestoreScene();
/*********************
 �f�o�b�O�p�[�g�I���
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
