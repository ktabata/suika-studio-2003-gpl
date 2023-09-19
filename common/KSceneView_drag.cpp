/*****************************************************************************
 KSceneView_drag.cpp
 KSceneView�̃h���b�O���h���b�v�֘A�̃R�[�h

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

/***********************************
  �h���b�O�i�A�C�e���ړ��j�̏���
***********************************/

bool KSceneView::ProcessDragStart()
{
	if(last_hit_line == -1 || last_hit_line == GetDummyLine())
		return false;

	SelectLine(last_hit_line);
	UpdateSelLineTable();

	// �I�����ꂽ���ׂĂ̍s���h���b�O�\�����ׂ�
	if(!AreSelLinesEditable())
		return false;		// �h���b�O�ł��Ȃ��̂Ń��^�[��

	// ������Ԃ�ݒ肵�ăh���b�O���J�n����
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
	// �}�E�X�ʒu�ɂ��X�N���[�����s��
	DoDragScroll(point);

	// �}�E�X�̍��W����h���b�v�ʒu�i�}���ʒu�j���v�Z����
	int line = CalcLineToInsert(point);

	// �h���b�v�ʒu���ύX���ꂽ���H
	if(line != drop_pos) {
		// �h���b�v�\�ȍs���H
		if(!IsDroppablePos(line))
			line = -1;	// �h���b�v�ʒu�𖳌�������
		// �V�����h���b�v�ʒu��ݒ肵�ĕ`�悷��
		SetDropTarget(line);
	}
}

// �h���b�O���̃}�E�X�ʒu�ɂ��X�N���[�����s��
void KSceneView::DoDragScroll(KPOINT point)
{
	KRECT rc;
	GetViewRect(rc);	// �r���[�̗̈���}�E�X�Ɠ������W�n�Ŏ擾����

	int new_visible_line = -1;
	int top = GetTopVisibleLine();

	// �}�E�X���W���r���[�̈����ɂ��邩�H�i�X�N���[���A�b�v�j
	if(point.y <= rc.top)
		new_visible_line = top - 1;
	// �}�E�X���W���r���[�̈扺���ɂ��邩�H�i�X�N���[���_�E���j
	else if(point.y >= rc.bottom)
		new_visible_line = top + GetVisibleCount();

	// �C���f�b�N�X�������Ȃ烊�^�[��
	if(new_visible_line < 0 || new_visible_line >= GetLinesCount())
		return;

	// �A�C�e����������悤�ɃX�N���[������
	EnsureVisible(new_visible_line);
}

// �h���b�O���ɁC�}�E�X���W����h���b�v�ʒu���v�Z����
int KSceneView::CalcLineToInsert(KPOINT point)
{
	int line = DoHitTest(point);
	if(line == -1)
		return -1;

	KRECT rc;
	GetLineRect(line, rc);

	// �s�̉������̗̈�Ȃ�A���̍s���^�[�Q�b�g�ɂ���
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

	// line���A�I���s�̎q�m�[�h�łȂ������ׂ�B���@�Ƃ��ẮA
	// line����g�b�v���x���s(indent=0)�܂Ńc���[�������̂ڂ��āA
	// ���̃p�X���̃m�[�h�̍s�ԍ����I���s�Ɉ�v���Ȃ����𒲂ׂ�B
	while(true) {
		int indent = document->GetIndentAt(line);
		if(indent == 0)
			break;	// �g�b�v���x���s�܂Œ��ׂ��̂ŏI��

		// �e�m�[�h�̍s��T���iindent������܂ōs�������̂ڂ�j
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
				return false;	// ��v�����̂ő}���s�\�Ȉʒu
		}
	}
	return true;
}

// �h���b�v�ʒu��ݒ肷��i�A�C�e���̏�Ԃ�ύX����j
void KSceneView::SetDropTarget(int line)
{
	K_ASSERT(line >= -1 && line < GetLinesCount());

	// �h���b�v�ΏۂɕύX���Ȃ����
	if(line == drop_pos)
		return;

	// �O�̃h���b�v�ʒudrop_pos�������łȂ����
	if(drop_pos != -1) {
		// �O�̃h���b�v�ʒu�̃A�C�e����ʏ��Ԃɖ߂�
		SetDropHilited(drop_pos, false, true);
	}

	// �V�����h���b�v�ʒuline�������łȂ����
	if(line != -1) {
		// �V�����h���b�v�ʒu�̃A�C�e�����h���b�v�Ώۂ̏�Ԃ���
		SetDropHilited(line, true, true);
	}

	// �h���b�v�ʒu��ύX����
	drop_pos = line;
}

void KSceneView::ProcessDragEnd(bool is_drop)
{
	// �h���b�v�Ώۑ�������������
	if(drop_pos != -1)
		SetDropHilited(drop_pos, false, true);
	else
		return;	// �h���b�v�ʒu���������Ȃ̂Ń��^�[��

	// �L�����Z���Ȃ烊�^�[��
	if(!is_drop)
		return;

	// �ړ����s��
	MoveItemsTo(drop_pos);

	// �ĕ`�悷��
	Redraw();
}
