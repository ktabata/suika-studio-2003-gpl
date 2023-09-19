/*****************************************************************************
 KSceneView_draw.cpp
 KSceneView�̕`��֘A�̃R�[�h

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneView.h"
#include "klib/debug.h"

#define INDENT	(OP_ICON_WIDTH+5)

/***********************************
�@�A�C�e���`��
***********************************/

void KSceneView::DrawItem(int line, KGraphicContext *gc, KRECT& rect)
{
	// �w�i��h��Ԃ��i�I���E�t�H�[�J�X�̏�Ԃœh�蕪����j
	RGBA32 bk_color = GetLineBkColor(line);
	gc->SetColor(bk_color);
	gc->FillRect(rect);

	// �ŉ��s�̃_�~�[�A�C�e���łȂ����
	if(line != GetDummyLine())
		DrawItemHelper(line, gc, rect);

	// �h���b�v�Ώۂ̍s�Ȃ�h���b�v�ʒu����������`�悷��
	if(IsDropHilited(line)) {
		gc->DrawImage(line_image,rect.left, rect.top,
			line_image->GetWidth(), line_image->GetHeight(), 0, 0);
	}
}

RGBA32 KSceneView::GetLineBkColor(int line)
{
	if(line == GetCursorPos())					// �J�[�\���s�Ȃ�
		return IMG_MAKE_RGB(0xd0,0xd0,0xff);
	if(IsSelected(line))						// �I�����ꂽ�s�Ȃ�
		return IMG_MAKE_RGB(0xff,0xd0,0xd0);
	return IMG_MAKE_RGB(0xff, 0xff, 0xff);		// �ʏ��Ԃ̍s
}

void KSceneView::DrawItemHelper(int line, KGraphicContext *gc, KRECT& rect)
{
	KSceneDocItem *item = document->GetItemAt(line);
	K_ASSERT(item != NULL);
	int indent = document->GetIndentAt(line);

	if(line != GetDummyLine()) {
		// �A�C�R����`�悷��
		gc->DrawImage(
			icon_image,						// src
			rect.left + INDENT*indent+1,	// dst x
			rect.top + 1,					// dst y
			OP_ICON_WIDTH,				// w
			OP_ICON_HEIGHT,				// h
			GetIconOffsetX(item->GetOpCode()),// src x
			0);								// src y

		// �����̕�����𐶐����ĕ`�悷��
		RGBA32 color;
		KString msg = GetItemExplanation(item, color);
		gc->SetColor(color);
		gc->DrawText( rect.left + INDENT * (indent+1), rect.top + 8, msg);
	}
}

int KSceneView::GetIconOffsetX(int opcode)
{
	// icons.bmp���̃A�C�R���̊J�n�w���W��Ԃ�
	switch(opcode) {
	case SOP_MESSAGE:
		return 0;
	case SOP_BLOCKEND:
		return OP_ICON_WIDTH;
	case SOP_CHECKVAR:
		return OP_ICON_WIDTH*2;
	case SOP_IFTRUE:
		return OP_ICON_WIDTH*3;
	case SOP_IFFALSE:
		return OP_ICON_WIDTH*4;
	}
	return -1;
}

KImage *KSceneView::GetIconImage()
{
	return icon_image;
}

KString KSceneView::GetItemExplanation(KSceneDocItem *item, RGBA32& color)
{
	switch(item->GetOpCode()) {
	case SOP_MESSAGE:
	{
		KString speaker = item->ParamAt(OPP_MESSAGE_SPEAKER);
		KString voice	= item->ParamAt(OPP_MESSAGE_VOICE);
		KString msg		= item->ParamAt(OPP_MESSAGE_MESSAGE);
		color = voice.IsEmpty() ? IMG_MAKE_RGB(0,0,0) : IMG_MAKE_RGB(0,0,0xff);
		return (!speaker.IsEmpty() ? (speaker+"�u") : "") + msg;
	}
	case SOP_CHECKVAR:
	{
		KString	var_name = item->ParamAt(OPP_CHECKVAR_VARIABLE);
		int		cond	 = item->ParamAt(OPP_CHECKVAR_CONDITION);
		KString	value	 = item->ParamAt(OPP_CHECKVAR_VALUE);
		K_ASSERT(IS_VALID_CVCOND(cond));
		color = IMG_MAKE_RGB(0xff,0x00,0x00);
		return "�ϐ��̒l�𔻒肷�� <" + var_name + " " + g_cvcond[cond] +
					" " + value + ">";
	}
	case SOP_IFTRUE:
		color = IMG_MAKE_RGB(0xff, 0, 0xff);
		return "�����ɓ��Ă͂܂�Ƃ�";
	case SOP_IFFALSE:
		color = IMG_MAKE_RGB(0xff, 0, 0xff);
		return "�����ɓ��Ă͂܂�Ȃ��Ƃ�";
	}
	return "";
}
