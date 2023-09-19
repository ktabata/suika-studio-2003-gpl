/*****************************************************************************
 KSceneView_drag.cpp
 KSceneView�̃A�C�e���i�s�j����֘A�̃R�[�h

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

#define UNDO_MAX	(1024)

/***********************************
�@�I����Ԃ̊Ǘ�
***********************************/

void KSceneView::UpdateSelLineTable()
{
// �e�m�[�h�Ǝq�m�[�h�������ɑI������Ă���ꍇ�͎q�m�[�h�̑I�����������Ă���A
// �I������Ă���s�̔ԍ���sel_line_tbl�ɔ[�߂�B

	sel_line_tbl.DeleteAll();

	int line = GetFirstSelectedLine();
	int lines_cnt = document->GetLinesCount();
	int checked_cnt = 0;
	while(true) {
		sel_line_tbl.InsertAt(checked_cnt, line);

		// ���`�F�b�N���c��P�Ȃ�I��
		int sel_cnt = GetSelectedCount();
		if(checked_cnt+1 == sel_cnt)
			break;

		int indent = document->GetIndentAt(line);
		for(int i = line+1; i < lines_cnt; i++) {
			// line�ȍ~�ŁAline�Ǝ������������s�́A�P��O�̍s�܂ł�line�̎q�m�[�h
			if(document->GetIndentAt(i) == indent)
				break;
			UnselectLine(i);
		}
		checked_cnt++;

		// ���̑I�����ꂽ�A�C�e����T��
		while(!IsSelected(line)) {
			line++;
			K_ASSERT(line < lines_cnt);
		}
	}
}

/***********************************
  �N���b�N�ɂ��P���ڑI���̏���
***********************************/

// �q�b�g�e�X�g���s���ăJ�[�\�����ړ�����
void KSceneView::MoveCursorToHitLine(KPOINT point)
{
	// is_cur_moved: �ʏ�́A�s�̏�Ń}�E�X�{�^�������������Ƃ��̍s�ɃJ�[�\
	// �����ړ����邪�A�����I������Ă���s�̂����̂P�̏�Ń{�^������������
	// �Ƃ��ɂ́A�Ή�����{�^��������ɃJ�[�\�����ړ�����B���̃t���O�́A����
	// ����ȃP�[�X�����ʂ��邽�߂̂��́B�����I������Ă���s�̂����̂P�̏�
	// �Ń{�^�������������Ƃ��̌Ăяo�����Ƀt���O���Z�b�g���A�Ή�����������
	// �Ăяo���Ń`�F�b�N����B���Ȃ݂ɁA���̎d�g�݂����݂��闝�R�́A�����{�^
	// ���������ɃA�C�e���̑I����Ԃ�ς��Ă��܂�����A�����A�C�e�����h���b�O
	// �ł��Ȃ��Ȃ��Ă��܂�����ł���B

	int		line;
	if(is_cur_moved)
		line = last_hit_line;
	else {
		// �q�b�g�e�X�g���s���ăJ�[�\���ړ���̃A�C�e������肷��
		last_hit_line = line = DoHitTest(point);

		// �A�C�e���������I������Ă��āC���C�I������Ă���A�C�e�����������Ƃ�
		if(GetSelectedCount() > 1 && IsSelected(line)) {
			SetCursorPos(line);
			is_cur_moved = true;	// ��̐����Q��
			return;
		}
	}

	is_cur_moved = false;	// ��̐����Q��

	// �V�t�g�L�[��������Ă��Ȃ����
	bool is_shift_down = IsCtrlDown();
	if(!is_shift_down)
		SetAllLinesUnselected();	// ���ׂẴA�C�e���̑I����Ԃ𖳌��ɂ���

	// �J�[�\���ړ��悪�Ȃ���΃��^�[��
	if(line == -1)
		return;

	// �J�[�\�����ړ�����
	SetCursorPos(line);
	if(is_shift_down)
		SelectLine(line);
}

/***********************************
  �A�C�e���i���߁j�̕ҏW����
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
	// �J�[�\���ʒu���擾����i�J�[�\���s���Ȃ���Ζ����s�j
	int pos = GetCursorPos();
	pos = (pos != -1) ? pos : GetLinesCount();
	if(!document->IsInsertablePos(pos))
		return;	// �}���ł��Ȃ��ʒu�Ȃ̂Ń��^�[��

	// �}������I�y�R�[�h�̖��߃A�C�e�����쐬����
	KSceneDocItem *item = new KSceneDocItem(opcode);

	// �ҏW�p�̃_�C�A���O��\�����ă��[�U�ɕҏW���Ă��炤
	if(!DoEditDialog(item)) {
		// �L�����Z�����ꂽ
		delete item;
		return;
	}

	// �A�C�e�����h�L�������g�ɑ}������
	document->InsertAt(pos, item);

	// Undo�L�^
	StartUndoRecording();
	RecordInsertItem(pos);
	EndUndoRecording();

	ApplyEditLines();
	SetCursorPos(pos);
	EnsureVisible(pos);
}

bool KSceneView::EditItem()
{
	// �J�[�\���ʒu���擾����
	int pos = GetCursorPos();
	if(pos == -1 || pos == GetDummyLine())
		return false;

	KSceneDocItem *old_item = new KSceneDocItem(*document->GetItemAt(pos));

	// �ҏW�p�̃_�C�A���O��\�����ă��[�U�ɕҏW���Ă��炤
	int old_cnt = document->GetLinesCount();
	if(!DoEditDialog(document->GetItemAt(pos))) {
		delete old_item;
		return false;
	}

	// Undo�L�^
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
	// �J�[�\���ʒu���擾����
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
  �ҏW����̃A���h�D�[
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
