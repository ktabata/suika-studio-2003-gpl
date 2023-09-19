/*****************************************************************************
 KSceneView_key.cpp
 KSceneView�̃L�[����֘A�̃R�[�h

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

/***********************************
�@�L�[����̏���
***********************************/

void KSceneView::ProcessKey(int keycode)
{
	int cur_pos = GetCursorPos();

	// Enter�L�[����������
	if(keycode == KEYCODE_RETURN) {
		EditItem();
		return;
	}

	// Delete�L�[����������
	if(keycode == KEYCODE_DELETE) {
		DeleteItems();
		return;
	}

	// Undo
	if((keycode == 'Z' || keycode == 'z') && IsCtrlDown()) {
		Undo();
	}

	// �e�}���L�[����������
	if(keycode == 'm' || keycode == 'M') {
		InsertItem(SOP_MESSAGE);
		return;
	}
	if(keycode == 'j' || keycode == 'J') {
		InsertItem(SOP_CHECKVAR);
		return;
	}

	// �㉺�L�[�A�y�[�W����������
	bool is_up = (keycode == KEYCODE_UP);
	bool is_pgup = (keycode == KEYCODE_PAGEUP);
	bool is_pgdn = (keycode == KEYCODE_PAGEDOWN);
	if(is_up || keycode == KEYCODE_DOWN || is_pgup || is_pgdn)
		DoKeyScroll(cur_pos, is_up, (is_pgup || is_pgdn));
}

void KSceneView::DoKeyScroll(int cur_pos, bool is_scrollup, bool is_page)
{
	int prev_cur = cur_pos;
	int	new_cur = prev_cur;
	int top = GetTopVisibleLine();
	int scroll_unit = is_page ? 10 : 1;	// �X�N���[������P��
	int scroll_lines = 0;		

	if(prev_cur == -1)
		new_cur = GetTopVisibleLine();
	else if(is_scrollup) {
		if(prev_cur == 0)
			return;
		new_cur -= scroll_unit;
		if(new_cur <= top)
			scroll_lines = -scroll_unit;
	} else {
		if(prev_cur == GetLinesCount()-1)
			return;
		new_cur += scroll_unit;
		if(new_cur >= top+GetVisibleCount()-1)
			scroll_lines = scroll_unit;
	}

	// Shift�L�[��������Ă��Ȃ���΁A���ׂẴA�C�e���̑I����Ԃ���������
	bool is_shift_down = IsShiftDown();
	if(!is_shift_down)
		SetAllLinesUnselected();

	// Shift�L�[����������Ă���ΑI����Ԃ𔽓]����
	int dummy = GetDummyLine();
	if(is_shift_down && prev_cur != dummy)
		SelectLine(prev_cur, !IsSelected(prev_cur));

	SetCursorPos(new_cur, false);
	Scroll(scroll_lines);
}
