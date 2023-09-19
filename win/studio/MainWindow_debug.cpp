/*****************************************************************************
 MainWindow_debug.cpp
 CMainWindowのデバッグパート

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "MainWindow.h"
#include "klib/io.h"
#include "klib/debug.h"
#include <shlobj.h>

/****************************************************************************
  ゲームフォルダの選択
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
	binfo.lpszTitle = "ゲームフォルダを選択してください。";
	binfo.ulFlags = BIF_RETURNONLYFSDIRS;
	binfo.lpfn = NULL;
	binfo.lParam = 0;
	binfo.iImage = 0;

	// フォルダを選択する	
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
  デバッグの開始
****************************************************************************/
void CMainWindow::OnDebugStartCur() 
{
	K_ASSERT(!m_bDebugging);	// この関数がデバッグ中に呼ばれることはない

	// 開始前の準備を行う
	if(!PrepareStartDebug())
		return;

	// 編集中のファイルと行を保存する
	m_strPrevScene = (const char *)m_strFileName;
	m_iPrevLine = ((KSceneView *)m_pView)->GetCursorPos();
	m_iPrevLine = (m_iPrevLine == -1) ? 0 : m_iPrevLine;

	// ランタイム環境を子ウィンドウとして作成する
	m_pRuntime = new KRuntimeEnvWin32();
	bool b = m_pRuntime->Init("西瓜 Studio", m_strRootPath, m_hWnd);
	K_ASSERT(b);

	// 実行するファイルをロードする
	m_pRuntime->SetDebugPathMode(true);		// 絶対パスを有効にする
	if(!m_pRuntime->LoadScene(m_strPrevScene, m_iPrevLine))
	{
		MessageBox("シーンファイルをオープンできません。", "西瓜 Studio", MB_ICONEXCLAMATION);
		delete m_pRuntime;
		m_pRuntime = NULL;
		return;
	}
	m_pRuntime->SetDebugPathMode(true);		// 絶対パスを無効にする

	// デバッグウィンドウに切り替える
	m_bDebugging = TRUE;
	m_bDebugWindow = FALSE;
	SwitchToDebugWindow();

	// シーンビューにシーンをセットする
	m_pView->ReleaseDocument(true);
	m_pView->SetDocument(m_pRuntime->GetScene());
}

BOOL CMainWindow::PrepareStartDebug()
{
	// ドキュメントを保存する
	bool is_modified = m_pView->IsModified();
	if(is_modified)
		if(!AskAndSave(FALSE))	// "OK/キャンセル"で訪ねる
			return FALSE;
	if(m_strFileName.IsEmpty())
	{
		MessageBox("実行するデータがありません。", "西瓜 Studio", MB_ICONINFORMATION);
		return FALSE;
	}

	// ゲームフォルダが選択されていなければダイアログを表示して選択してもらう
	if(m_strRootPath.IsEmpty())
		if(!SelRootDir())
			return FALSE;

	// フルスクリーンモード中ならウィンドウモードに移行する
	if(m_bFullScreen)
		OnChangeScreenMode();

	return TRUE;
}

/****************************************************************************
  デバッグの終了
****************************************************************************/

void CMainWindow::OnDebugEndReturn() 
{
	K_ASSERT(m_bDebugging);
	K_ASSERT(m_pRuntime != NULL);

	// 編集ウィンドウに切り替える
	SwitchToEditWindow(FALSE);

	// デバッグを開始する前に編集していたシーンをロードする
	RestoreScene();

	// デバッグ終了
	m_pRuntime->ReleaseAll();
	delete m_pRuntime;
	m_pRuntime = NULL;
	m_bDebugging = FALSE;
}

void CMainWindow::RestoreScene()
{
	// シーンビューからシーンを切り離す
	// （デバッグ中のシーンはKRuntimeEnvが管理しているのでdeleteしない）
	m_pView->ReleaseDocument(false);
	m_pView->SetDocument(new KSceneDoc());

	K_ASSERT(!m_strPrevScene.IsEmpty());

	// デバッグ開始前に編集していたファイルをロードする
	KFileInputStream fis(m_strPrevScene);
	if(fis.IsOpen())
	{
		// ロードする
		if(m_pView->Load(&fis))
		{
			((KSceneView *)m_pView)->SetCursorPos(m_iPrevLine);
			return;
		}
	}

	// ロードに失敗した
	MessageBox("デバッグ開始前に編集していたシーンを"
		"ロードできませんでした。", "西瓜 Studio", MB_ICONEXCLAMATION);
}

/****************************************************************************
  ウィンドウ切り替えボタンのハンドラ
****************************************************************************/

void CMainWindow::OnDebugChangeWindow()
{
	if(m_bDebugWindow)
		SwitchToEditWindow(TRUE);
	else
		SwitchToDebugWindow();
}

/****************************************************************************
  プレイボタンのハンドラ
****************************************************************************/

void CMainWindow::OnDebugPlay()
{
/*	int res = KDebugger::Execute();

	resの値でエラー報告
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