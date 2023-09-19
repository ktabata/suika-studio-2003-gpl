/*****************************************************************************
 KSceneView_drag.cpp
 KSceneViewのアイテム（行）操作関連のコード

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

#define UNDO_MAX	(1024)

/***********************************
　選択状態の管理
***********************************/

void KSceneView::UpdateSelLineTable()
{
// 親ノードと子ノードが同時に選択されている場合は子ノードの選択を解除してから、
// 選択されている行の番号をsel_line_tblに納める。

	sel_line_tbl.DeleteAll();

	int line = GetFirstSelectedLine();
	int lines_cnt = document->GetLinesCount();
	int checked_cnt = 0;
	while(true) {
		sel_line_tbl.InsertAt(checked_cnt, line);

		// 未チェックが残り１つなら終了
		int sel_cnt = GetSelectedCount();
		if(checked_cnt+1 == sel_cnt)
			break;

		int indent = document->GetIndentAt(line);
		for(int i = line+1; i < lines_cnt; i++) {
			// line以降で、lineと字下げが同じ行の、１つ手前の行までがlineの子ノード
			if(document->GetIndentAt(i) == indent)
				break;
			UnselectLine(i);
		}
		checked_cnt++;

		// 次の選択されたアイテムを探す
		while(!IsSelected(line)) {
			line++;
			K_ASSERT(line < lines_cnt);
		}
	}
}

/***********************************
  クリックによる単項目選択の処理
***********************************/

// ヒットテストを行ってカーソルを移動する
void KSceneView::MoveCursorToHitLine(KPOINT point)
{
	// is_cur_moved: 通常は、行の上でマウスボタンが押下されるとその行にカーソ
	// ルを移動するが、複数選択されている行のうちの１つの上でボタンを押下した
	// ときには、対応するボタン解放時にカーソルを移動する。このフラグは、この
	// 特殊なケースを識別するためのもの。複数選択されている行のうちの１つの上
	// でボタンを押下したときの呼び出し時にフラグをセットし、対応する解放時の
	// 呼び出しでチェックする。ちなみに、この仕組みが存在する理由は、もしボタ
	// ン押下時にアイテムの選択状態を変えてしまったら、複数アイテムをドラッグ
	// できなくなってしまうからである。

	int		line;
	if(is_cur_moved)
		line = last_hit_line;
	else {
		// ヒットテストを行ってカーソル移動先のアイテムを特定する
		last_hit_line = line = DoHitTest(point);

		// アイテムが複数選択されていて，かつ，選択されているアイテムを押したとき
		if(GetSelectedCount() > 1 && IsSelected(line)) {
			SetCursorPos(line);
			is_cur_moved = true;	// 上の説明参照
			return;
		}
	}

	is_cur_moved = false;	// 上の説明参照

	// シフトキーが押されていなければ
	bool is_shift_down = IsCtrlDown();
	if(!is_shift_down)
		SetAllLinesUnselected();	// すべてのアイテムの選択状態を無効にする

	// カーソル移動先がなければリターン
	if(line == -1)
		return;

	// カーソルを移動する
	SetCursorPos(line);
	if(is_shift_down)
		SelectLine(line);
}

/***********************************
  アイテム（命令）の編集操作
***********************************/

bool KSceneView::PrepareSelState()
{
	bool is_selected = false;
	int cur_pos = GetCursorPos();
	if(cur_pos != -1) {
		is_selected = IsSelected(cur_pos);
		if(!is_selected)
			SelectLine(cur_pos);
	}

	UpdateSelLineTable();
	if(cur_pos != -1 || !is_selected)
		UnselectLine(cur_pos);
	if(sel_line_tbl.GetItemsCount() == 0)
		return false;
	return AreSelLinesEditable();
}

void KSceneView::ApplyEditLines()
{
	SetLinesCount(document->GetLinesCount() + 1);
	SetAllLinesUnselected(false);
}

void KSceneView::InitClipboard()
{
	if(clipboard_doc != NULL)
		delete clipboard_doc;
	clipboard_doc = new KSceneDoc();
}

void KSceneView::InsertItem(int opcode)
{
	// カーソル位置を取得する（カーソル行がなければ末尾行）
	int pos = GetCursorPos();
	pos = (pos != -1) ? pos : GetLinesCount();
	if(!document->IsInsertablePos(pos))
		return;	// 挿入できない位置なのでリターン

	// 挿入するオペコードの命令アイテムを作成する
	KSceneDocItem *item = new KSceneDocItem(opcode);

	// 編集用のダイアログを表示してユーザに編集してもらう
	if(!DoEditDialog(item)) {
		// キャンセルされた
		delete item;
		return;
	}

	// アイテムをドキュメントに挿入する
	document->InsertAt(pos, item);

	// Undo記録
	StartUndoRecording();
	RecordInsertItem(pos);
	EndUndoRecording();

	ApplyEditLines();
	SetCursorPos(pos);
	EnsureVisible(pos);
}

bool KSceneView::EditItem()
{
	// カーソル位置を取得する
	int pos = GetCursorPos();
	if(pos == -1 || pos == GetDummyLine())
		return false;

	KSceneDocItem *old_item = new KSceneDocItem(*document->GetItemAt(pos));

	// 編集用のダイアログを表示してユーザに編集してもらう
	int old_cnt = document->GetLinesCount();
	if(!DoEditDialog(document->GetItemAt(pos))) {
		delete old_item;
		return false;
	}

	// Undo記録
	StartUndoRecording();
	RecordEditItem(pos, old_item);
	EndUndoRecording();

	ApplyEditLines();
	SetCursorPos(pos);
	EnsureVisible(pos);
	return true;
}

void KSceneView::DeleteItems()
{
	if(!PrepareSelState())
		return;
	StartUndoRecording();

	int cnt = sel_line_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		int line = sel_line_tbl[i] - i;
		RecordDeleteItem(
			line,
			new KSceneDocItem(*document->GetItemAt(line))
		);
		document->DeleteAt(line);
	}

	EndUndoRecording();
	ApplyEditLines();
}

void KSceneView::CutItems()
{
	if(!PrepareSelState())
		return;
	InitClipboard();
	StartUndoRecording();

	int cnt = sel_line_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		int line = sel_line_tbl[i] - i;
		KSceneDocItem *item = document->GetItemAt(line);

		clipboard_doc->InsertAt(
			clipboard_doc->GetLinesCount(),
			new KSceneDocItem(*item));
		RecordDeleteItem(
			line,
			new KSceneDocItem(*item));
		document->DeleteAt(line);
	}

	EndUndoRecording();
	ApplyEditLines();
}

void KSceneView::CopyItems()
{
	if(!PrepareSelState())
		return;
	InitClipboard();
	StartUndoRecording();

	int cnt = sel_line_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		int line = sel_line_tbl[i] - i;
		clipboard_doc->InsertAt(
			clipboard_doc->GetLinesCount(),
			new KSceneDocItem(*document->GetItemAt(line))
		);
	}

	EndUndoRecording();
	ApplyEditLines();
}

void KSceneView::PasteItems()
{
	// カーソル位置を取得する
	int pos = GetCursorPos();
	if(pos == -1 || pos == GetDummyLine())
		return;

	if(clipboard_doc == NULL)
		return;

	StartUndoRecording();

	int inserted_lines = 0;
	int cnt = clipboard_doc->GetLinesCount();
	for(int i = 0; i < cnt; i++) {
		document->InsertAt(
			pos + inserted_lines,
			new KSceneDocItem(*clipboard_doc->GetItemAt(i))
		);
		RecordInsertItem(pos + inserted_lines);
		inserted_lines += document->GetItemLines(pos + inserted_lines);
	}

	EndUndoRecording();
	ApplyEditLines();
}

void KSceneView::MoveItemsTo(int dst)
{
	if(!PrepareSelState())
		return;
	InitClipboard();
	StartUndoRecording();

	int cnt = sel_line_tbl.GetItemsCount();
	int inserted = 0;
	int deleted_before_dst = 0;
	int original_dst = dst;

	for(int i = cnt-1; i >= 0; i--) {
		int src = sel_line_tbl[i];
		if(src > original_dst)
			src = src - deleted_before_dst + inserted;

		int lines = document->GetItemLines(src);

		document->Move(dst, src);
		RecordMoveItem(dst, src);

		inserted += lines;
		if(src < original_dst)
			deleted_before_dst += lines;
		dst = original_dst - deleted_before_dst + inserted;
	}

	EndUndoRecording();
	ApplyEditLines();
	SetCursorPos(original_dst-deleted_before_dst);
	EnsureVisible(GetCursorPos());
}

/***********************************
  編集操作のアンドゥー
***********************************/

#define UNDO_CODE_INSERT	(0)
#define UNDO_CODE_EDIT		(1)
#define UNDO_CODE_DELETE	(2)
#define UNDO_CODE_MOVE		(3)

void KSceneView::StartUndoRecording()
{
	if(undo_buffer.GetItemsCount() >= UNDO_MAX) {
		delete undo_buffer[0];
		undo_buffer.DeleteAt(0);
	}
	cur_undo_unit = new KUndoUnit();
}

void KSceneView::RecordInsertItem(int line)
{
	K_ASSERT(cur_undo_unit != NULL);
	cur_undo_unit->InsertAt(
		cur_undo_unit->GetItemsCount(),
		new KUndoData(UNDO_CODE_INSERT, line, 0, NULL) );
}

void KSceneView::RecordEditItem(int line, KSceneDocItem *item)
{
	K_ASSERT(cur_undo_unit != NULL);
	cur_undo_unit->InsertAt(
		cur_undo_unit->GetItemsCount(),
		new KUndoData(UNDO_CODE_EDIT, line, 0, item) );
}

void KSceneView::RecordDeleteItem(int line, KSceneDocItem *item)
{
	K_ASSERT(cur_undo_unit != NULL);
	cur_undo_unit->InsertAt(
		cur_undo_unit->GetItemsCount(),
		new KUndoData(UNDO_CODE_DELETE, line, 0, item) );
}

void KSceneView::RecordMoveItem(int dst, int src)
{
	K_ASSERT(cur_undo_unit != NULL);
	cur_undo_unit->InsertAt(
		cur_undo_unit->GetItemsCount(),
		new KUndoData(UNDO_CODE_MOVE, dst, src, NULL) );
}

void KSceneView::EndUndoRecording()
{
	undo_buffer.InsertAt(undo_buffer.GetItemsCount(), cur_undo_unit);
	cur_undo_unit = NULL;
}

void KSceneView::Undo()
{
	int index = undo_buffer.GetItemsCount() - 1;
	if(index == -1)
		return;

	KUndoUnit *unit = undo_buffer[index];
	undo_buffer.DeleteAt(index);

	int cnt = unit->GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		KUndoData *data = unit->At(i);
		switch(data->code) {
		case UNDO_CODE_INSERT:
			document->DeleteAt(data->param0);
			break;
		case UNDO_CODE_EDIT:
			document->DeleteAt(data->param0);
			document->InsertAt(data->param0, data->item);
			data->item = NULL;
			break;
		case UNDO_CODE_DELETE:
			document->InsertAt(data->param0, data->item);
			data->item = NULL;
			break;
		case UNDO_CODE_MOVE:
			document->Move(data->param1, data->param0);
			break;
		}
	}

	delete unit;
	ApplyEditLines();
}
