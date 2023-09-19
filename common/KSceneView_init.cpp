/*****************************************************************************
 KSceneView_drag.cpp
 KSceneViewの初期化・イベント処理関連のコード

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

KSceneView::KSceneView()
{
	document = new KSceneDoc();
	icon_image = NULL;
	line_image = NULL;
	clipboard_doc = NULL;
	cur_undo_unit = NULL;
	is_cur_moved = false;
	last_hit_line = -1;
	drop_pos = -1;
}

KSceneView::~KSceneView()
{
	RELEASE(document);
	RELEASE(clipboard_doc);
	RELEASE(cur_undo_unit);
	RELEASE(icon_image);
	RELEASE(line_image);
}

void KSceneView::Init()
{
	// コンストラクタ内で行えない初期化を行う。
	icon_image = LoadAppResourceImage("sceneview_icons.bmp");
	line_image = LoadAppResourceImage("sceneview_line.bmp");
	K_ASSERT(icon_image != NULL);
	K_ASSERT(line_image != NULL);
}

int KSceneView::GetDummyLine()
{
	return GetLinesCount()-1;
}

/****************************************
  関連づけられているドキュメントの操作
****************************************/

bool KSceneView::IsModified(void) const
{
	return document->IsModified();
}

bool KSceneView::Save(KOutputStream *os)
{
	return document->Save(os);
}

bool KSceneView::Load(KInputStream *is)
{
	if(!document->Load(is))
		return false;		// 読み込みに失敗

	SetLinesCount(document->GetLinesCount()+1);
	SetAllLinesUnselected();
	SetCursorPos(0);
	return true;
}

KSceneDoc *KSceneView::GetDocument()
{
	return document;
}

void KSceneView::ReleaseDocument(bool need_delete)
{
	if(need_delete && document != NULL) {
		delete document;
		document = NULL;
	}
}

void KSceneView::SetDocument(KSceneDoc *doc)
{
	document = doc;
	SetLinesCount(document->GetLinesCount()+1);
	SetAllLinesUnselected();
	SetCursorPos(0);
}

/**********************************************
  以下、GUIイベントの処理
**********************************************/

/***********************************
　アイテム描画要求の処理
***********************************/

void KSceneView::OnDraw(int line, KGraphicContext *gc, KRECT& rect)
{
	DrawItem(line, gc, rect);
}

/***********************************
　キーボードイベントの処理
***********************************/

void KSceneView::OnKeyDown(int keycode)
{
	ProcessKey(keycode);
}

/***********************************
　マウスイベントの処理
***********************************/

void KSceneView::OnButtonDown(KPOINT point)
{
	// 押された行にカーソルを移動する
	MoveCursorToHitLine(point);
}

void KSceneView::OnButtonUp(KPOINT point)
{
	if(is_cur_moved)	// MoveCursorToHitLine()のフラグがセットされていれば
		MoveCursorToHitLine(point);
}

void KSceneView::OnMouseMove(KPOINT point)
{
}

bool KSceneView::OnBeginDrag(KPOINT point)
{
	return ProcessDragStart();
}

void KSceneView::OnDragMove(KPOINT point)
{
	ProcessDragMove(point);
}

void KSceneView::OnEndDrag(KPOINT point, bool is_drop)
{
	ProcessDragEnd(is_drop);
}

void KSceneView::OnDoubleClick(int line)
{
	// アイテムを編集する
	EditItem();
}
