// KRuntimeEnv.h
// ランタイム環境Win32版実装
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#ifndef _KRUNTIMEENVWIN32_H_
#define _KRUNTIMEENVWIN32_H_

#include "common/KRuntimeEnv.h"

#ifndef _MFC_VER
#include <windows.h>
#else
#include <afxcmn.h>
#endif

long WINAPI KRuntimeEnvWin32_WndProc(HWND, UINT, WPARAM, LPARAM);

class KRuntimeEnvWin32 : public KRuntimeEnv {
	friend long WINAPI KRuntimeEnvWin32_WndProc(HWND, UINT, WPARAM, LPARAM);
public:
	KRuntimeEnvWin32();
	virtual ~KRuntimeEnvWin32();

	// 初期化
	bool	Init(KString title, KString root_path, HWND hParentWnd=NULL);
	void	SetVisible(bool is_visible);
	void	Move(int x, int y);
	void	ReleaseAll();

	// デバッグ用制御
	void	SetDebugPathMode(bool is_offset);

	// キーボード、マウス状態の取得
	bool	IsLButtonDown();
	bool	IsRButtonDown();
	bool	IsCtrlDown();
	void	GetMousePos(KPOINT& pt);
	bool	IsUpArrowDown();
	bool	IsDownArrowDown();

	// 実行権譲渡
	void	YieldCPU();

protected:
	// スクリーンのGC、ダブルバッファのバックイメージの作成
	KGraphicContext	*OnCreateGC(int w, int h);

	// ファイルストリームの作成（アーカイブ操作）
	KInputStream	*OpenFile(KString fname);

private:
	HWND	hWnd;
	HWND	hParentWnd;
	KString title;
	KString	root_path;
	HMENU	hMenu;
	bool	is_fullscreen;
	bool	is_debug_path_mode;

	bool	CreateWin(int w, int h);
	void	OnPaint();
	void	OnMouseMove(int x, int y);
	void	OnCommand(UINT nID);
	void	OnClose();
	void	ChangeScreenMode();
	void	SetFullScreen(bool is_full);
};

#endif
