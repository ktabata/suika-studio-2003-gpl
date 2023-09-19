/*****************************************************************************
 MainWindow_debug.cpp
 CMainWindow�̃f�o�b�O�p�[�g

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "MainWindow.h"
#include "klib/io.h"
#include "klib/debug.h"
#include <shlobj.h>

/****************************************************************************
  �Q�[���t�H���_�̑I��
****************************************************************************/
void CMainWindow::OnDebugSelRoot()
{
	SelRootDir();
}

BOOL CMainWindow::SelRootDir()
{
	char strName[4096];
	BROWSEINFO  binfo;
    binfo.hwndOwner = NULL;
	binfo.pidlRoot = NULL;
	binfo.pszDisplayName = strName;
	binfo.lpszTitle = "�Q�[���t�H���_��I�����Ă��������B";
	binfo.ulFlags = BIF_RETURNONLYFSDIRS;
	binfo.lpfn = NULL;
	binfo.lParam = 0;
	binfo.iImage = 0;

	// �t�H���_��I������	
	LPITEMIDLIST pList= ::SHBrowseForFolder(&binfo);
	if(pList == NULL)
		return FALSE;

	char strDir[4096];
	::SHGetPathFromIDList(pList, strDir);
	::CoTaskMemFree(pList);

	m_strRootPath = strDir;
	return TRUE;
}

/****************************************************************************
  �f�o�b�O�̊J�n
****************************************************************************/
void CMainWindow::OnDebugStartCur() 
{
	K_ASSERT(!m_bDebugging);	// ���̊֐����f�o�b�O���ɌĂ΂�邱�Ƃ͂Ȃ�

	// �J�n�O�̏������s��
	if(!PrepareStartDebug())
		return;

	// �ҏW���̃t�@�C���ƍs��ۑ�����
	m_strPrevScene = (const char *)m_strFileName;
	m_iPrevLine = ((KSceneView *)m_pView)->GetCursorPos();
	m_iPrevLine = (m_iPrevLine == -1) ? 0 : m_iPrevLine;

	// �����^�C�������q�E�B���h�E�Ƃ��č쐬����
	m_pRuntime = new KRuntimeEnvWin32();
	bool b = m_pRuntime->Init("���Z Studio", m_strRootPath, m_hWnd);
	K_ASSERT(b);

	// ���s����t�@�C�������[�h����
	m_pRuntime->SetDebugPathMode(true);		// ��΃p�X��L���ɂ���
	if(!m_pRuntime->LoadScene(m_strPrevScene, m_iPrevLine))
	{
		MessageBox("�V�[���t�@�C�����I�[�v���ł��܂���B", "���Z Studio", MB_ICONEXCLAMATION);
		delete m_pRuntime;
		m_pRuntime = NULL;
		return;
	}
	m_pRuntime->SetDebugPathMode(true);		// ��΃p�X�𖳌��ɂ���

	// �f�o�b�O�E�B���h�E�ɐ؂�ւ���
	m_bDebugging = TRUE;
	m_bDebugWindow = FALSE;
	SwitchToDebugWindow();

	// �V�[���r���[�ɃV�[�����Z�b�g����
	m_pView->ReleaseDocument(true);
	m_pView->SetDocument(m_pRuntime->GetScene());
}

BOOL CMainWindow::PrepareStartDebug()
{
	// �h�L�������g��ۑ�����
	bool is_modified = m_pView->IsModified();
	if(is_modified)
		if(!AskAndSave(FALSE))	// "OK/�L�����Z��"�ŖK�˂�
			return FALSE;
	if(m_strFileName.IsEmpty())
	{
		MessageBox("���s����f�[�^������܂���B", "���Z Studio", MB_ICONINFORMATION);
		return FALSE;
	}

	// �Q�[���t�H���_���I������Ă��Ȃ���΃_�C�A���O��\�����đI�����Ă��炤
	if(m_strRootPath.IsEmpty())
		if(!SelRootDir())
			return FALSE;

	// �t���X�N���[�����[�h���Ȃ�E�B���h�E���[�h�Ɉڍs����
	if(m_bFullScreen)
		OnChangeScreenMode();

	return TRUE;
}

/****************************************************************************
  �f�o�b�O�̏I��
****************************************************************************/

void CMainWindow::OnDebugEndReturn() 
{
	K_ASSERT(m_bDebugging);
	K_ASSERT(m_pRuntime != NULL);

	// �ҏW�E�B���h�E�ɐ؂�ւ���
	SwitchToEditWindow(FALSE);

	// �f�o�b�O���J�n����O�ɕҏW���Ă����V�[�������[�h����
	RestoreScene();

	// �f�o�b�O�I��
	m_pRuntime->ReleaseAll();
	delete m_pRuntime;
	m_pRuntime = NULL;
	m_bDebugging = FALSE;
}

void CMainWindow::RestoreScene()
{
	// �V�[���r���[����V�[����؂藣��
	// �i�f�o�b�O���̃V�[����KRuntimeEnv���Ǘ����Ă���̂�delete���Ȃ��j
	m_pView->ReleaseDocument(false);
	m_pView->SetDocument(new KSceneDoc());

	K_ASSERT(!m_strPrevScene.IsEmpty());

	// �f�o�b�O�J�n�O�ɕҏW���Ă����t�@�C�������[�h����
	KFileInputStream fis(m_strPrevScene);
	if(fis.IsOpen())
	{
		// ���[�h����
		if(m_pView->Load(&fis))
		{
			((KSceneView *)m_pView)->SetCursorPos(m_iPrevLine);
			return;
		}
	}

	// ���[�h�Ɏ��s����
	MessageBox("�f�o�b�O�J�n�O�ɕҏW���Ă����V�[����"
		"���[�h�ł��܂���ł����B", "���Z Studio", MB_ICONEXCLAMATION);
}

/****************************************************************************
  �E�B���h�E�؂�ւ��{�^���̃n���h��
****************************************************************************/

void CMainWindow::OnDebugChangeWindow()
{
	if(m_bDebugWindow)
		SwitchToEditWindow(TRUE);
	else
		SwitchToDebugWindow();
}

/****************************************************************************
  �v���C�{�^���̃n���h��
****************************************************************************/

void CMainWindow::OnDebugPlay()
{
/*	int res = KDebugger::Execute();

	res�̒l�ŃG���[��
*/
}
/*
int KDebugger::ExecuteOne()
{
	int res = env->GetNextNode()->Execute(env);
	if(res != EXEC_SUCCESS)
		return res;

	return (env->GetLineNum() == env->GetScene()->GetLinesCount()) ?
			 EXEC_END : EXEC_SUCCESS;
}

int KDebugger::Execute()
{
	int res;
	while( (res = ExecuteOne()) == EXEC_SUCCESS)
		;
	return res;
}
*/