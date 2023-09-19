// KRuntimeEnvWin32.h
// ランタイム環境Win32版実装
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#include "KRuntimeEnvWin32.h"
#include "klib/io.h"
#include "klib/debug.h"
#include "Resource.h"

KRuntimeEnvWin32::KRuntimeEnvWin32()
{
	hWnd = NULL;
	hParentWnd = NULL;
	hMenu = NULL;
	is_fullscreen = false;
}

KRuntimeEnvWin32::~KRuntimeEnvWin32()
{
	ReleaseAll();
}

/****************************************************************************
  初期化
****************************************************************************/

bool KRuntimeEnvWin32::Init(
	KString title,
	KString root_path,
	HWND hParentWnd/*=NULL*/)
{
	this->title = title;
	this->root_path = root_path;
	this->hParentWnd = hParentWnd;
	return KRuntimeEnv::Init();
}

void KRuntimeEnvWin32::SetVisible(bool is_visible)
{
	K_ASSERT(hWnd != NULL);
	::ShowWindow(hWnd, is_visible ? SW_SHOW : SW_HIDE);
}

void KRuntimeEnvWin32::Move(int x, int y)
{
	K_ASSERT(hWnd != NULL);
	::MoveWindow(hWnd, x, y, GetScreenWidth(), GetScreenHeight(), TRUE);
}

void KRuntimeEnvWin32::ReleaseAll()
{
	if(hWnd == NULL)
		return;

	SetFullScreen(false);
	::DestroyWindow(hWnd);
	MSG		msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	hWnd = NULL;
}

/****************************************************************************
  デバッグ用制御
****************************************************************************/

void KRuntimeEnvWin32::SetDebugPathMode(bool is_debug_mode)
{
	this->is_debug_path_mode = is_debug_mode;
}

/****************************************************************************
  キーボード、マウス状態の取得
****************************************************************************/

bool KRuntimeEnvWin32::IsLButtonDown()
{
	return (::GetKeyState(VK_LBUTTON) < 0);
}

bool KRuntimeEnvWin32::IsRButtonDown()
{
	return (::GetKeyState(VK_RBUTTON) < 0);
}

bool KRuntimeEnvWin32::IsCtrlDown()
{
	return (::GetKeyState(VK_CONTROL) < 0);
}

void KRuntimeEnvWin32::GetMousePos(KPOINT& pt)
{
	POINT point;
	::GetCursorPos(&point);
	::ScreenToClient(hWnd, &point);
	pt.x = point.x;
	pt.y = point.y;
}

bool KRuntimeEnvWin32::IsUpArrowDown()
{
	return (::GetKeyState(VK_UP) < 0);
}

bool KRuntimeEnvWin32::IsDownArrowDown()
{
	return (::GetKeyState(VK_DOWN) < 0);
}

/****************************************************************************
  実行権の譲渡（この実装では単なるイベント処理）
****************************************************************************/

void KRuntimeEnvWin32::YieldCPU() {
	MSG		msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
}

/****************************************************************************
  KRuntimeEnvからコールされるコールバック
****************************************************************************/

KGraphicContext *KRuntimeEnvWin32::OnCreateGC(int w, int h)
{
	if(!CreateWin(w, h))
		return false;
	return new KGraphicContext(hWnd);
}

KInputStream *KRuntimeEnvWin32::OpenFile(KString fname)
{
	/********************************************
	  未実装：アーカイブへのアクセスを提供する
	********************************************/

	KString path;
	if(is_debug_path_mode)
		path = fname;
	else
		path = root_path + "\\" + fname;

	KFileInputStream *fis = new KFileInputStream();
	if(!fis->Open(path)) {
		delete fis;
		return NULL;
	}
	return fis;
}

/****************************************************************************
  helpers
****************************************************************************/

bool KRuntimeEnvWin32::CreateWin(int w, int h)
{
	// アプリケーションのインスタンスハンドルを取得する
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	// ウィンドウクラスを登録する
	char		classname[] = "KRuntimeEnv";
	WNDCLASS	wc;
	::ZeroMemory(&wc, sizeof(wc));
	wc.style			= 0;
	wc.lpfnWndProc		= &KRuntimeEnvWin32_WndProc;
	wc.cbWndExtra		= sizeof( KRuntimeEnvWin32 * );	// 補足メモリ
	wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.lpszClassName	= classname;
	RegisterClass (&wc);

	// 子ウィンドウでなければ
	int		x = 0, y = 0, width = w, height = h;
	DWORD	style = WS_CHILD;
	if(hParentWnd == NULL) {
		style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
		x = y = CW_USEDEFAULT;

		width += GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
		height += GetSystemMetrics(SM_CYCAPTION) +
					GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
					GetSystemMetrics(SM_CYMENU);
	}

	// ウィンドウを作成する
	hWnd = ::CreateWindow(classname, title, style, x, y, width, height,
			hParentWnd, NULL, hInstance, this);
	if(hWnd == NULL)
		return false;

	// 子ウィンドウでなければメニューをセットする
	if(hParentWnd == NULL) {
		hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_PLAYERMENU) );
		if(hMenu != NULL)
			::SetMenu(hWnd, hMenu);
	}

	return true;
}

void KRuntimeEnvWin32::OnPaint()
{
	PAINTSTRUCT ps;
	::BeginPaint (hWnd, &ps);
	Update(ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left + 1,
			ps.rcPaint.bottom - ps.rcPaint.top + 1);
	::EndPaint (hWnd, &ps);
}

void KRuntimeEnvWin32::OnMouseMove(int x, int y)
{
	// ここでフルスクリーン時のメニュー表示を行う
}

void KRuntimeEnvWin32::OnCommand(UINT nID)
{
	switch(nID) {
	case ID_SCREENMODE:
		if(hParentWnd == NULL)	// 子ウィンドウでなければ
			ChangeScreenMode();
		return;
	case ID_QUIT:
		OnClose();
		return;
	}
}

void KRuntimeEnvWin32::OnClose()
{
	int res = ::MessageBox(hWnd, "終了してもよろしいですか？", "確認",
					MB_OKCANCEL | MB_ICONQUESTION);
	if(res == IDOK) {
		SetFullScreen(false);
		PostQuitRequest();
	}
}

long WINAPI KRuntimeEnvWin32_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CREATE) {
		::SetWindowLong(hWnd, GWL_USERDATA,
			(LONG)((LPCREATESTRUCT)lParam)->lpCreateParams);
		return 0;
	}

	KRuntimeEnvWin32 *obj = IsWindow(hWnd) ? 
			(KRuntimeEnvWin32 *)::GetWindowLong(hWnd, GWL_USERDATA) : NULL;

	if(obj != NULL) {
	    switch(uMsg) {
		case WM_PAINT:
			obj->OnPaint();
			return 0;
		case WM_MOUSEMOVE:
			if(obj->is_fullscreen)
				obj->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_COMMAND:
			obj->OnCommand(HIWORD(wParam));
			return 0;
		case WM_CLOSE:
			obj->OnClose();
			return 0;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void KRuntimeEnvWin32::ChangeScreenMode() {
/*	// 現在ウィンドウモードか？
	if( is_full_screen == false ) {
		::GetWindowRect(hWnd, &rcWindow);
		::SetWindowLong (hWnd, GWL_STYLE, WS_POPUP);
		::SetWindowLong (hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		::SetWindowPos (hWnd, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);
		SetFullScreen( true );
		SetMenuVisible( false );
	}

	// 現在フルスクリーンモード

	SetFullScreen( false );
	::SetWindowLong (hWnd, GWL_EXSTYLE, 0);
	::SetWindowLong (hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU |
					 WS_MINIMIZEBOX | WS_OVERLAPPED);
	int w = width + ::GetSystemMetrics (SM_CXFIXEDFRAME) * 2;
	int h = height + ::GetSystemMetrics (SM_CYCAPTION) + ::GetSystemMetrics (SM_CYFIXEDFRAME) * 2 + ::GetSystemMetrics (SM_CYMENU);

	// ２段階でウィンドウを移動する（１回で移動すると画面に黒い領域が残る）
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, width, height, SWP_SHOWWINDOW );
	::SetWindowPos(hWnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top, w, h, SWP_SHOWWINDOW );
	SetMenuVisible( true );
	Flip();
*/
}

void KRuntimeEnvWin32::SetFullScreen( bool b ) {
/*
	if( b ) {
		HDC		hDCDisp = ::GetDC( NULL );
		int		bpp = ::GetDeviceCaps( hDCDisp, BITSPIXEL );
		ReleaseDC( NULL, hDCDisp );

		DEVMODE dm;
		DWORD	n = 0;
		while( ::EnumDisplaySettings( NULL, n, &dm ) ) {
			if( dm.dmPelsWidth == (DWORD)width && dm.dmPelsHeight == (DWORD)height && dm.dmBitsPerPel == (DWORD)bpp )	{
				::ChangeDisplaySettings( &dm, 4 );
				is_full_screen = true;
				is_menu_visible = false;
				return;
			}
			n++;
		}
	} else {
		// 解像度を元に戻す
		is_full_screen = false;
		is_menu_visible = true;

		DEVMODE dm;
		dm.dmSize = sizeof(dm);
		dm.dmFields = 0;
		::ChangeDisplaySettings( &dm, 0x00000004);  // CDS_FULLSCREEN
	}
*/
}
