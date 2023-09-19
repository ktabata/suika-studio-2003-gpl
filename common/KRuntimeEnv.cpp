/*****************************************************************************
 KRuntimeEnv.cpp
 ランタイム環境

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KRuntimeEnv.h"
#include "klib/debug.h"

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

KRuntimeEnv::KRuntimeEnv()
{
	scene = NULL;
	gc = NULL;
	back_image = NULL;
	need_quit = NULL;
}

KRuntimeEnv::~KRuntimeEnv()
{
	RELEASE(scene);
	RELEASE(gc);
	RELEASE(back_image);
}

/****************************************************************************
  初期化
****************************************************************************/

bool KRuntimeEnv::Init()
{
	// ・設定ファイルから解像度を読み込む

	width = 640;
	height = 480;

	gc = OnCreateGC(640, 480);
	if(gc == NULL)
		return false;

	back_image = new KImage();
	bool b = back_image->Create(640, 480);
	return b;

	/*********************************
	  未実装：サウンドなどの初期化
	*********************************/
}

/****************************************************************************
  シーン（実行位置）の設定・取得
****************************************************************************/

int KRuntimeEnv::Execute()
{
	return 0;
}

/****************************************************************************
  シーン（実行位置）の設定・取得
****************************************************************************/

bool KRuntimeEnv::LoadScene(KString fname, int line)
{
	// ファイルストリームを開く
	KInputStream *is = OpenFile(fname);
	if(is == NULL)
		return false;

	// シーンをロードする
	KSceneDoc *doc = new KSceneDoc();
	if(!doc->Load(is)) {
		delete doc;
		delete is;
		return false;
	}

	// 現在のシーンを破棄して新しいシーンをセットする
	if(scene != NULL)
		delete scene;
	delete is;
	scene_fname = fname;
	scene = doc;
	line = line;
	return true;
}

KString KRuntimeEnv::GetSceneFileName()
{
	return scene_fname;
}

void KRuntimeEnv::SetLineNum(int line)
{
	this->line = line;
}

int KRuntimeEnv::GetLineNum()
{
	return line;
}

KSceneDoc *KRuntimeEnv::GetScene()
{
	return scene;
}

/****************************************************************************
  エラーメッセージ設定・取得(実行関数によってセットされる)
****************************************************************************/
void KRuntimeEnv::SetErrMsg(KString msg)
{
	err_msg = msg;
}

KString KRuntimeEnv::GetErrMsg()
{
	return err_msg;
}

/****************************************************************************
  描画関係の値・オブジェクト取得
****************************************************************************/

KGraphicContext	*KRuntimeEnv::GetGC()
{
	return gc;
}

KImage *KRuntimeEnv::GetBackImage()
{
	return back_image;
}

int	KRuntimeEnv::GetScreenWidth()
{
	return width;
}

int KRuntimeEnv::GetScreenHeight()
{
	return height;
}

/****************************************************************************
  ダブルバッファのバックイメージをスクリーンに転送する
****************************************************************************/

void KRuntimeEnv::Update(int x, int y, int w, int h)
{
	if(gc == NULL)
		return;
	gc->DrawImage(back_image, x, y, w, h, x, y);
}

void KRuntimeEnv::Flip()
{
	Update(0, 0, width, height);
}

/****************************************************************************
  終了要求
****************************************************************************/

void KRuntimeEnv::PostQuitRequest()
{
	need_quit;
}

bool KRuntimeEnv::IsQuitRequestPosted()
{
	return need_quit;
}
