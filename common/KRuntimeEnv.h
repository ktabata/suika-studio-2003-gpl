/*****************************************************************************
 KRuntimeEnv.h
 ランタイム環境

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KRUNTIMEENV_H_
#define _KRUNTIMEENV_H_

#include "KSceneDoc.h"
#include "KAnimator.h"
#include "klib/io.h"

class KRuntimeEnv;
class KSceneDoc;
class KSceneNode;

class KRuntimeEnv {
public:
	KRuntimeEnv();
	virtual ~KRuntimeEnv();

	// 初期化
	virtual bool	Init();

	// 実行
	int	Execute();

	// デバッガ用：シーン・実行位置の設定・取得
	bool	LoadScene(KString fname, int line);
	void	SetLineNum(int line);
	KString		GetSceneFileName();
	int			GetLineNum();
	KSceneDoc	*GetScene();

	// エラーメッセージ設定・取得（KSceneNodeによってセットされる）
	void	SetErrMsg(KString msg);
	KString GetErrMsg();

	// スクリーンサイズの取得
	int		GetScreenWidth();
	int		GetScreenHeight();

	// スクリーンのGC、ダブルバッファのバックイメージの取得
	KGraphicContext	*GetGC();
	KImage			*GetBackImage();

	// キーボード、マウス状態の取得
	virtual bool IsLButtonDown() = 0;
	virtual bool IsRButtonDown() = 0;
	virtual bool IsCtrlDown() = 0;
	virtual void GetMousePos(KPOINT& pt) = 0;
	virtual bool IsUpArrowDown() = 0;
	virtual bool IsDownArrowDown() = 0;

	// ダブルバッファのバックイメージをスクリーンに転送する
	virtual void Update(int x, int y, int w, int h);
	virtual void Flip();		// 実装できればフリップ、できなければ転送

	// 終了要求の有無
	bool	IsQuitRequestPosted();

	// 実行権譲渡
	virtual void YieldCPU() = 0;

protected:
	// スクリーンのGC、ダブルバッファのバックイメージの作成
	virtual KGraphicContext	*OnCreateGC(int w, int h) = 0;
	virtual KInputStream	*OpenFile(KString fname) = 0;

	// 各種状態の設定
	void	PostQuitRequest();

private:
	KSceneDoc		*scene;			// シーン
	KString			scene_fname;	// シーンファイルの名前
	int				line;			// 行番号
	KGraphicContext	*gc;			// スクリーンのGC
	KImage			*back_image;	// ダブルバッファのバックイメージ
//	KAnimator		
	int				width, height;	// スクリーンのサイズ
	bool			need_quit;		// 終了リクエストフラグ
	KString			err_msg;
};

#endif
