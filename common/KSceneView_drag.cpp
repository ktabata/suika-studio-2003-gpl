/*****************************************************************************
 KSceneView_drag.cpp
 KSceneViewのドラッグ＆ドロップ関連のコード

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

/***********************************
  ドラッグ（アイテム移動）の処理
***********************************/

bool KSceneView::ProcessDragStart()
{
	if(last_hit_line == -1 || last_hit_line == GetDummyLine())
		return false;

	SelectLine(last_hit_line);
	UpdateSelLineTable();

	// 選択されたすべての行がドラッグ可能か調べる
	if(!AreSelLinesEditable())
		return false;		// ドラッグできないのでリターン

	// 初期状態を設定してドラッグを開始する
	is_cur_moved = false;
	drop_pos = -1;
	SetDropTarget(-1);

	return true;
}

bool KSceneView::AreSelLinesEditable()
{
	int cnt = sel_line_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		switch(document->GetItemAt(sel_line_tbl[i])->GetOpCode()) {
		case SOP_BLOCKEND:
		case SOP_IFTRUE:
		case SOP_IFFALSE:
			return false;
		}
	}
	return true;
}

void KSceneView::ProcessDragMove(KPOINT point)
{
	// マウス位置によるスクロールを行う
	DoDragScroll(point);

	// マウスの座標からドロップ位置（挿入位置）を計算する
	int line = CalcLineToInsert(point);

	// ドロップ位置が変更されたか？
	if(line != drop_pos) {
		// ドロップ可能な行か？
		if(!IsDroppablePos(line))
			line = -1;	// ドロップ位置を無効化する
		// 新しいドロップ位置を設定して描画する
		SetDropTarget(line);
	}
}

// ドラッグ中のマウス位置によるスクロールを行う
void KSceneView::DoDragScroll(KPOINT point)
{
	KRECT rc;
	GetViewRect(rc);	// ビューの領域をマウスと同じ座標系で取得する

	int new_visible_line = -1;
	int top = GetTopVisibleLine();

	// マウス座標がビュー領域上方にあるか？（スクロールアップ）
	if(point.y <= rc.top)
		new_visible_line = top - 1;
	// マウス座標がビュー領域下方にあるか？（スクロールダウン）
	else if(point.y >= rc.bottom)
		new_visible_line = top + GetVisibleCount();

	// インデックスが無効ならリターン
	if(new_visible_line < 0 || new_visible_line >= GetLinesCount())
		return;

	// アイテムが見えるようにスクロールする
	EnsureVisible(new_visible_line);
}

// ドラッグ中に，マウス座標からドロップ位置を計算する
int KSceneView::CalcLineToInsert(KPOINT point)
{
	int line = DoHitTest(point);
	if(line == -1)
		return -1;

	KRECT rc;
	GetLineRect(line, rc);

	// 行の下半分の領域なら、次の行をターゲットにする
	if(point.y >= (rc.top+rc.bottom)/2 && line != GetLinesCount()-1)
		line++;
	return line;
}

bool KSceneView::IsDroppablePos(int line)
{
	if(line == -1)
		return false;
	if(line == GetDummyLine())
		return true;
	if(!document->IsInsertablePos(line))
		return false;

	// lineが、選択行の子ノードでないか調べる。方法としては、
	// lineからトップレベル行(indent=0)までツリーをさかのぼって、
	// そのパス中のノードの行番号が選択行に一致しないかを調べる。
	while(true) {
		int indent = document->GetIndentAt(line);
		if(indent == 0)
			break;	// トップレベル行まで調べたので終了

		// 親ノードの行を探す（indentが減るまで行をさかのぼる）
		int i, prev_line = line;
		for(i = line - 1; i >= 0; i--) {
			if(document->GetIndentAt(i) != indent) {
				line = i;
				break;
			}
		}
		K_ASSERT(line != prev_line);

		// linear-search
		int cnt = sel_line_tbl.GetItemsCount();
		for(i = 0; i < cnt; i++) {
			if(sel_line_tbl[i] == line)
				return false;	// 一致したので挿入不能な位置
		}
	}
	return true;
}

// ドロップ位置を設定する（アイテムの状態を変更する）
void KSceneView::SetDropTarget(int line)
{
	K_ASSERT(line >= -1 && line < GetLinesCount());

	// ドロップ対象に変更がなければ
	if(line == drop_pos)
		return;

	// 前のドロップ位置drop_posが無効でなければ
	if(drop_pos != -1) {
		// 前のドロップ位置のアイテムを通常状態に戻す
		SetDropHilited(drop_pos, false, true);
	}

	// 新しいドロップ位置lineが無効でなければ
	if(line != -1) {
		// 新しいドロップ位置のアイテムをドロップ対象の状態する
		SetDropHilited(line, true, true);
	}

	// ドロップ位置を変更する
	drop_pos = line;
}

void KSceneView::ProcessDragEnd(bool is_drop)
{
	// ドロップ対象属性を解除する
	if(drop_pos != -1)
		SetDropHilited(drop_pos, false, true);
	else
		return;	// ドロップ位置が向こうなのでリターン

	// キャンセルならリターン
	if(!is_drop)
		return;

	// 移動を行う
	MoveItemsTo(drop_pos);

	// 再描画する
	Redraw();
}
