/*****************************************************************************
 KSceneView_drag.cpp
 KSceneView�̏������E�C�x���g�����֘A�̃R�[�h

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
	// �R���X�g���N�^���ōs���Ȃ����������s���B
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
  �֘A�Â����Ă���h�L�������g�̑���
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
		return false;		// �ǂݍ��݂Ɏ��s

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
  �ȉ��AGUI�C�x���g�̏���
**********************************************/

/***********************************
�@�A�C�e���`��v���̏���
***********************************/

void KSceneView::OnDraw(int line, KGraphicContext *gc, KRECT& rect)
{
	DrawItem(line, gc, rect);
}

/***********************************
�@�L�[�{�[�h�C�x���g�̏���
***********************************/

void KSceneView::OnKeyDown(int keycode)
{
	ProcessKey(keycode);
}

/***********************************
�@�}�E�X�C�x���g�̏���
***********************************/

void KSceneView::OnButtonDown(KPOINT point)
{
	// �����ꂽ�s�ɃJ�[�\�����ړ�����
	MoveCursorToHitLine(point);
}

void KSceneView::OnButtonUp(KPOINT point)
{
	if(is_cur_moved)	// MoveCursorToHitLine()�̃t���O���Z�b�g����Ă����
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
	// �A�C�e����ҏW����
	EditItem();
}
