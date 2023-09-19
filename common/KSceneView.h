/*****************************************************************************
 KSceneView.h
 �V�[���G�f�B�^�̃r���[

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KSCENEVIEW_H_
#define _KSCENEVIEW_H_

#include "KSceneDoc.h"
#include "klib/graphic.h"
#include "klib/keycode.h"

// icons.bmp���̊e�A�C�R���̃T�C�Y
#define OP_ICON_WIDTH	(22)
#define OP_ICON_HEIGHT	(24)

/****************************************************************************
 KUndoData / KUndoUnit
 ===========================================================================
  KSceneView��Undo�f�[�^
****************************************************************************/

struct KUndoData {
	int code;
	int param0, param1;
	KSceneDocItem *item;

	KUndoData() { item = NULL; }
	KUndoData(int code, int p0, int p1, KSceneDocItem *item) :
		code(code), param0(p0), param1(p1), item(item) { }
	~KUndoData() { if(item!=NULL){delete item; item=NULL;} }
};

struct KUndoUnit : public KArray<KUndoData *> {
	~KUndoUnit() {
		for(int i = 0; i < GetItemsCount(); i++) {
			if(At(i) != NULL)
				delete At(i);
		}
	}
};

/****************************************************************************
 KSceneView
 ===========================================================================
  �V�[���h�L�������g�ɑ΂���r���[
****************************************************************************/

class KSceneView {
public:
	KSceneView();
	virtual ~KSceneView();
	void	Init();

	// �r���[�̑���Ώۂł���h�L�������g�֘A
	virtual bool	IsModified(void) const;
	virtual bool	Save(KOutputStream *os);
	virtual bool	Load(KInputStream *is);

	// �ҏW����
	void	InsertItem(int opcode);
	bool	EditItem();
	void	DeleteItems();
	void	CutItems();
	void	CopyItems();
	void	PasteItems();
	void	MoveItemsTo(int dst);
	void	Undo();

	// �f�o�b�K�p����
	KSceneDoc	*GetDocument();
	void		ReleaseDocument(bool need_delete);
	void		SetDocument(KSceneDoc *scene);

	// ���߂̃A�C�R��
	KImage		*GetIconImage();
	static int	GetIconOffsetX(int type);

protected:
	int		GetDummyLine();

	// �N���b�N�ɂ��P���ڑI���̏��� ( MoveCursorOn() )
	bool	is_cur_moved;	// ���O�̃{�^�������ŁA�����I������Ă���A�C�e����
							// �����P�������A���ꂪMoveCursorOn()�ŏ������ꂽ���H
	void	MoveCursorToHitLine(KPOINT point);

private:
	// �r���[���ێ�����h�L�������g
	KSceneDoc	*document;

	// �I�����
	KArray<int>	sel_line_tbl;
	void	UpdateSelLineTable();

	// �N���b�v�{�[�h
	KSceneDoc	*clipboard_doc;

	// �ҏW����(Undo�T�|�[�g)
	KArray<KUndoUnit *> undo_buffer;
	KUndoUnit	*cur_undo_unit;
	bool	PrepareSelState();
	void	ApplyEditLines();
	void	InitClipboard();
	void	StartUndoRecording();
	void	RecordInsertItem(int line);
	void	RecordEditItem(int line, KSceneDocItem *item);
	void	RecordDeleteItem(int line, KSceneDocItem *item);
	void	RecordMoveItem(int dst, int src);
	void	EndUndoRecording();

	// �A�C�e���`��
	KImage	*icon_image;
	KImage	*line_image;
	void	LoadImages();
	void	DrawItem(int line, KGraphicContext *gc, KRECT& rect);
	RGBA32	GetLineBkColor(int line);
	void	DrawItemHelper(int line, KGraphicContext *gc, KRECT& rect);
	KString GetItemExplanation(KSceneDocItem *item, RGBA32& color);

	// �L�[����̏���
	void	ProcessKey(int keycode);
	void	DoKeyScroll(int cur_pos, bool is_scrollup, bool is_page);

	// �h���b�O�i�A�C�e���ړ��j�̏���
	int		last_hit_line;		// �Ō�Ƀ{�^���ŉ������A�C�e��
	int		drop_pos;			// �h���b�v�ʒu�̃C���f�b�N�X
	bool	ProcessDragStart();
	void	ProcessDragMove(KPOINT point);
	void	ProcessDragEnd(bool is_drop);
	bool	AreSelLinesEditable();
	int		CalcLineToInsert(KPOINT point);
	bool	IsDroppablePos(int line);
	void	SetDropTarget(int line);
	void	DoDragScroll(KPOINT point);


	/*******************************************
  �ȉ��A�ڐA�p�̃C���^�t�F�[�X
*******************************************/
	protected:

	/*******************************************
	  �q�N���X����Ăяo�����C���^�t�F�[�X
	*******************************************/

	void	OnDraw(int line, KGraphicContext *gc, KRECT& rect);
	void	OnKeyDown(int keycode);
	void	OnButtonDown(KPOINT point);
	void	OnButtonUp(KPOINT point);
	void	OnMouseMove(KPOINT point);
	bool	OnBeginDrag(KPOINT point);
	void	OnDragMove(KPOINT point);
	void	OnEndDrag(KPOINT point, bool is_drop);
	void	OnDoubleClick(int line);

	/******************************************
	  �q�N���X�ɗv������C���^�t�F�[�X
	******************************************/

	// �L�[��Ԃ̎擾
	virtual bool	IsCtrlDown() = 0;
	virtual bool	IsShiftDown() = 0;

	// �r���[�̗̈�擾�i�}�E�X�Ɠ������W�n�Łj
	virtual void	GetViewRect(KRECT& rect) = 0;
	virtual void	GetLineRect(int line, KRECT& rect) = 0;

	// �s���̎擾�E�ݒ�
	virtual int		GetLinesCount() = 0;
	virtual void	SetLinesCount(int count) = 0;
	virtual int		GetTopVisibleLine() = 0;
	virtual int		GetVisibleCount() = 0;
	virtual int		GetSelectedCount() = 0;

	// �s�̑I����Ԃ̎擾�E�ݒ�
	virtual int		GetFirstSelectedLine() = 0;
	virtual bool	IsSelected(int line) = 0;
	virtual void	SelectLine(int line, bool need_redraw = true) = 0;
	virtual void	UnselectLine(int line, bool  need_redraw = true) = 0;
	virtual void	SetAllLinesUnselected(bool need_redraw = true) = 0;

	// �J�[�\���ʒu�̎擾�E�ݒ�
	public:
	virtual int		GetCursorPos() = 0;
	virtual void	SetCursorPos(int line, bool need_redraw = true) = 0;

	protected:
	// �s�̃h���b�v�Ώۑ����̐ݒ�
	virtual bool	IsDropHilited(int line) = 0;
	virtual void	SetDropHilited(int line, bool state, bool need_redraw = true) = 0;

	// �q�b�g�e�X�g
	virtual int		DoHitTest(KPOINT point) = 0;

	// �s�̑}���E�폜�E�`��
	virtual void	InsertLine(int pos) = 0;
	virtual void	DeleteLine(int line) = 0;
	virtual void	Redraw(int start = -1, int end = -1) = 0;
	virtual void	EnsureVisible(int line) = 0;
	virtual void	Scroll(int ofs) = 0;

	// �A�v���P�[�V�������\�[�X����̉摜�ǂݍ���
	virtual KImage	*LoadAppResourceImage(KString rname) = 0;

	// �m�[�h�ҏW�_�C�A���O�̕\��
	virtual bool	DoEditDialog(KSceneDocItem *item) = 0;
};

#endif
