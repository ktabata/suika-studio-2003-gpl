/*****************************************************************************
 MainWindow.h
 シーンエディタのウィンドウ

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "MainWindow.h"
#include "Resource.h"
#include "klib/io.h"
#include "klib/debug.h"

#define LEFT_SPACE		(2)
#define RIGHT_SPACE		(2)
#define BOTTOM_SPACE	(2)
#define WND_CX			(640)
#define WND_CY			(480)

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

CMainWindow::CMainWindow()
{
	m_hMenu = NULL;
	m_pView = NULL;
	m_pEditToolbar = NULL;
	m_pDebugToolbar = NULL;
	m_hAccel = NULL;
	m_bFullScreen = FALSE;
	m_bDebugging = FALSE;
	m_pRuntime = NULL;
}

CMainWindow::~CMainWindow()
{
	RELEASE(m_pView);
	RELEASE(m_pEditToolbar);
	RELEASE(m_pDebugToolbar);
	if(m_hMenu != NULL && !m_bDebugging)
		::DestroyMenu(m_hMenu);
}

BOOL CMainWindow::Create()
{
	HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);

	WNDCLASS	wc;
	::ZeroMemory(&wc, sizeof(wc));
//	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &AfxWndProc;
 	wc.hInstance = hInst;
    wc.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCENEEDIT) );
    wc.hCursor = ::LoadCursor(hInst, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "AfxWnd";

	BOOL res = AfxRegisterClass(&wc);
	if(!res)
		return FALSE;

	res = CreateEx(0, "AfxWnd", "シーンエディタ",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
			WND_CX, WND_CY, NULL, NULL);
	if(!res)
		return FALSE;

	DragAcceptFiles(TRUE);
	m_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
	m_hMenu = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINMENU));
	if(m_hMenu != NULL)
		::SetMenu(m_hWnd, m_hMenu);
	return TRUE;
}

BOOL CMainWindow::PreTranslateMessage(MSG* pMsg) 
{
	BOOL bResult = CWnd::PreTranslateMessage(pMsg);
	if(bResult)
		return TRUE;

	if(m_hAccel != NULL)
		return ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
	else
		return FALSE;
}

/****************************************************************************
  セーブ・ロード
****************************************************************************/

BOOL CMainWindow::Load(CString strFileName)
{
	// 現在のファイルを保存するか確認して必要なら保存する
	if(m_pView->IsModified())
		if(!AskAndSave(TRUE))	// "はい・いいえ・キャンセル"で表示、いいえでもTRUE
			return FALSE;		// キャンセルされたかセーブに失敗した

	KFileInputStream fis;
	if(!fis.Open((const char *)strFileName))
	{
		MessageBox("ファイルをオープンできません。", "エラー",
			MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(!m_pView->Load(&fis))
	{
		MessageBox("ファイルのロードに失敗しました。ファイル形式が不正です。",
			"エラー", MB_ICONEXCLAMATION);
		return FALSE;
	}
	m_strFileName = strFileName;
	return TRUE;
}

BOOL CMainWindow::AskAndSave(BOOL bShowNo)
{
	UINT nType = bShowNo ? MB_YESNOCANCEL : MB_OKCANCEL;
	int id = MessageBox("ファイルへの変更を保存しますか？", "セーブの確認",
							nType|MB_ICONQUESTION);
	if(id == IDCANCEL)
		return FALSE;
	if(id == IDNO)
		return TRUE;

	return Save(m_strFileName);
}

BOOL CMainWindow::Save(CString strFileName)
{
	if(strFileName.IsEmpty())
	{
		// ファイルダイアログを表示してファイル名を決定する
		CFileDialog wndDlg(FALSE, "scn", "*.scn", OFN_OVERWRITEPROMPT,
			"シーンファイル(*.scn)\0*.scn\0", this);
		if(wndDlg.DoModal() == IDCANCEL)
			return false;
		strFileName = wndDlg.GetPathName();
	}

	// セーブする
	KFileOutputStream fos;
	if(!fos.Create((const char *)strFileName))
	{
		MessageBox("ファイルを作成できません。", "エラー",
			MB_ICONEXCLAMATION);
		return false;
	}
	if(!m_pView->Save(&fos))
	{
		MessageBox("ファイルの書き込みに失敗しました。",
			"エラー", MB_ICONEXCLAMATION);
		return false;
	}
	m_strFileName = strFileName;
	return true;
}

/****************************************************************************
  ウィンドウスタイル・位置・サイズの保存・復元
****************************************************************************/

void CMainWindow::SaveStyleAndRect()
{
	// 現在のウィンドウのスタイルを保存する
	m_dwOldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	m_dwOldExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	// 現在のウィンドウの位置・サイズを保存する
	GetWindowRect(m_rcWindow);
}

void CMainWindow::RestoreStyleAndRect()
{
	// 保存していたウィンドウスタイルを復元する
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, m_dwOldExStyle);
	::SetWindowLong(m_hWnd, GWL_STYLE, m_dwOldStyle);

	// 保存していたウィンドウの位置・サイズを復元する
	SetWindowPos(&CWnd::wndTop, m_rcWindow.left, m_rcWindow.top,
		m_rcWindow.Width(), m_rcWindow.Height(), SWP_SHOWWINDOW);
}

void CMainWindow::SwitchToDebugWindow()
{
	K_ASSERT(m_bDebugging);
	K_ASSERT(m_pRuntime != NULL);
	K_ASSERT(!m_bDebugWindow);

	m_bDebugWindow = TRUE;

	// シーンビューと編集ツールバーを隠し、デバッグツールバーを表示する
	m_pView->ShowWindow(SW_HIDE);
	m_pEditToolbar->ShowWindow(SW_HIDE);
	m_pDebugToolbar->ShowWindow(SW_SHOW);

	// メインウィンドウのスタイル・位置・サイズを保存する
	SaveStyleAndRect();

	// メインウィンドウのサイズとスタイルを変更する
	DWORD dwStyle = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	int top = m_pDebugToolbar->GetHeight();
	int w = m_pRuntime->GetScreenWidth() +
				GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	int h = top + m_pRuntime->GetScreenHeight() +
				GetSystemMetrics(SM_CYCAPTION) +
				GetSystemMetrics(SM_CYFIXEDFRAME) * 2;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	::SetMenu(m_hWnd, NULL);
	SetWindowPos(&CWnd::wndTop, 0, 0, w, h, SWP_NOMOVE);

	// ランタイム環境の子ウィンドウを表示する
	m_pRuntime->Move(0, m_pDebugToolbar->GetHeight());
	m_pRuntime->SetVisible(true);
}

void CMainWindow::SwitchToEditWindow(BOOL bShowDebugToolbar)
{
	K_ASSERT(m_bDebugging);
	K_ASSERT(m_pRuntime != NULL);
	K_ASSERT(m_bDebugWindow);

	m_bDebugWindow = FALSE;

	// ランタイムの子ウィンドウを非表示にする
	m_pRuntime->SetVisible(false);

	// 保存していたメインウィンドウのスタイル・位置・サイズを復元する
	RECT rc;
	GetWindowRect(&rc);
	m_rcWindow.OffsetRect(rc.left-m_rcWindow.left, rc.top-m_rcWindow.top);
	RestoreStyleAndRect();
	::SetMenu(m_hWnd, m_hMenu);

	// ツールバーを切り替え、シーンビューを表示する
	if(!bShowDebugToolbar)
	{
		m_pDebugToolbar->ShowWindow(SW_HIDE);
		m_pEditToolbar->ShowWindow(SW_SHOW);
	}
	m_pView->ShowWindow(SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainWindow, CWnd)
	//{{AFX_MSG_MAP(CMainWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_CHANGESCREENMODE, OnChangeScreenMode)
	ON_COMMAND(ID_DEBUG_START_CUR, OnDebugStartCur)
	ON_COMMAND(ID_DEBUG_SELROOT, OnDebugSelRoot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// シーンビューを作成する
	RECT rc = {0,0,0,0};
	m_pView = new CSceneView();
	m_pView->Create(rc, this, 0);
	m_pView->ShowWindow(SW_SHOW);
	m_pView->SetFocus();

	// 編集ツールバーを作成する
	m_pEditToolbar = new CEditToolbar(m_pView);
	m_pEditToolbar->Create(this, 0);
	m_pEditToolbar->ShowWindow(SW_SHOW);

	// デバッグツールバーを作成する
	m_pDebugToolbar = new CDebugToolbar(this);
	m_pDebugToolbar->Create(this, 0);

	return 0;
}

void CMainWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	if(nState != WA_INACTIVE && m_pView != NULL)
		m_pView->SetFocus();
}

void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RECT rcView;
	rcView.left = LEFT_SPACE;
	rcView.top = m_pEditToolbar->GetHeight() + 3;
	rcView.right = cx - RIGHT_SPACE;
	rcView.bottom = cy - BOTTOM_SPACE;
	m_pView->MoveWindow(&rcView, TRUE);
	m_pView->SetFocus();

	// ビューの微調整（CSceneView::OnSize()内で行うとなぜか表示が乱れるのでここで行う）
	m_pView->GetClientRect(&rcView);
	m_pView->SetColumnWidth(0, rcView.right);
}

void CMainWindow::OnClose() 
{
	if(m_bDebugging)
	{
		MessageBox("デバッグ中は終了できません。", "西瓜 Studio", MB_ICONINFORMATION);
		return;
	}
	if(m_pView->IsModified())
		if(!AskAndSave(TRUE))	// "はい・いいえ・キャンセル"で表示、いいえでもTRUE
			return;				// キャンセルされたかセーブに失敗した
	CWnd::OnClose();
}

void CMainWindow::OnDropFiles(HDROP hDrop)
{
	// ウィンドウをWS_EX_TOPMOSTに設定する
	//（ドロップ時になぜか他のウィンドウの下に隠れてしまうのでその対策）
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

	// ドロップされたファイルを１つだけ取得する
	int iBufSize = ::DragQueryFile(hDrop, 0, NULL, 0);
	ASSERT(iBufSize > 0);
	CHAR *strBuf = new CHAR[iBufSize+1];
	::DragQueryFile(hDrop, 0, strBuf, iBufSize+1);
	::DragFinish(hDrop);

	// ロードする
	Load(strBuf);
	delete[] strBuf;

	// ウィンドウのWS_EX_TOPMOSTを解除する
	SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

void CMainWindow::OnFileOpen()
{
	// ファイルダイアログを表示してファイル名取得する
	CFileDialog wndDlg(TRUE, "scn", "*.scn", 0,
		"シーンファイル(*.scn)\0*.scn\0", this);
	if(wndDlg.DoModal() == IDCANCEL)
		return;

	// データをロードする
	Load(wndDlg.GetPathName());
}

void CMainWindow::OnFileSave()
{
	Save(m_strFileName);
}

void CMainWindow::OnFileSaveAs()
{
	Save("");
}

void CMainWindow::OnChangeScreenMode() 
{
	if(m_bDebugging)	// デバッグ中ならリターン
		return;

	// 現在ウィンドウモードか？
	if(!m_bFullScreen)
	{
		// 現在のウィンドウスタイルと位置・サイズを保存する
		SaveStyleAndRect();

		// ウィンドウのスタイルを変更する
		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

		// ウィンドウの位置・サイズを変更する
		CRect	rcDesktop;
		::GetWindowRect(::GetDesktopWindow(), rcDesktop);
		SetWindowPos(&CWnd::wndTop, 0, 0, 
			rcDesktop.Width(), rcDesktop.Height(), SWP_SHOWWINDOW);

		m_bFullScreen = TRUE;
		return;
	}

	// 現在フルスクリーンモード　→　ウィンドウモードに変更

	// 保存していたウィンドウスタイルと位置・サイズを復元する
	RestoreStyleAndRect();
	m_bFullScreen = FALSE;
}
