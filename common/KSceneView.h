/*****************************************************************************
 KSceneView.h
 シーンエディタのビュー

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KSCENEVIEW_H_
#define _KSCENEVIEW_H_

#include "KSceneDoc.h"
#include "klib/graphic.h"
#include "klib/keycode.h"

// icons.bmp中の各アイコンのサイズ
#define OP_ICON_WIDTH	(22)
#define OP_ICON_HEIGHT	(24)

/****************************************************************************
 KUndoData / KUndoUnit
 ===========================================================================
  KSceneViewのUndoデータ
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
  シーンドキュメントに対するビュー
****************************************************************************/

class KSceneView {
public:
	KSceneView();
	virtual ~KSceneView();
	void	Init();

	// ビューの操作対象であるドキュメント関連
	virtual bool	IsModified(void) const;
	virtual bool	Save(KOutputStream *os);
	virtual bool	Load(KInputStream *is);

	// 編集操作
	void	InsertItem(int opcode);
	bool	EditItem();
	void	DeleteItems();
	void	CutItems();
	void	CopyItems();
	void	PasteItems();
	void	MoveItemsTo(int dst);
	void	Undo();

	// デバッガ用操作
	KSceneDoc	*GetDocument();
	void		ReleaseDocument(bool need_delete);
	void		SetDocument(KSceneDoc *scene);

	// 命令のアイコン
	KImage		*GetIconImage();
	static int	GetIconOffsetX(int type);

protected:
	int		GetDummyLine();

	// クリックによる単項目選択の処理 ( MoveCursorOn() )
	bool	is_cur_moved;	// 直前のボタン押下で、複数選択されているアイテムの
							// うち１つを押し、それがMoveCursorOn()で処理されたか？
	void	MoveCursorToHitLine(KPOINT point);

private:
	// ビューが保持するドキュメント
	KSceneDoc	*document;

	// 選択状態
	KArray<int>	sel_line_tbl;
	void	UpdateSelLineTable();

	// クリップボード
	KSceneDoc	*clipboard_doc;

	// 編集操作(Undoサポート)
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

	// アイテム描画
	KImage	*icon_image;
	KImage	*line_image;
	void	LoadImages();
	void	DrawItem(int line, KGraphicContext *gc, KRECT& rect);
	RGBA32	GetLineBkColor(int line);
	void	DrawItemHelper(int line, KGraphicContext *gc, KRECT& rect);
	KString GetItemExplanation(KSceneDocItem *item, RGBA32& color);

	// キー操作の処理
	void	ProcessKey(int keycode);
	void	DoKeyScroll(int cur_pos, bool is_scrollup, bool is_page);

	// ドラッグ（アイテム移動）の処理
	int		last_hit_line;		// 最後にボタンで押したアイテム
	int		drop_pos;			// ドロップ位置のインデックス
	bool	ProcessDragStart();
	void	ProcessDragMove(KPOINT point);
	void	ProcessDragEnd(bool is_drop);
	bool	AreSelLinesEditable();
	int		CalcLineToInsert(KPOINT point);
	bool	IsDroppablePos(int line);
	void	SetDropTarget(int line);
	void	DoDragScroll(KPOINT point);


	/*******************************************
  以下、移植用のインタフェース
*******************************************/
	protected:

	/*******************************************
	  子クラスから呼び出されるインタフェース
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
	  子クラスに要求するインタフェース
	******************************************/

	// キー状態の取得
	virtual bool	IsCtrlDown() = 0;
	virtual bool	IsShiftDown() = 0;

	// ビューの領域取得（マウスと同じ座標系で）
	virtual void	GetViewRect(KRECT& rect) = 0;
	virtual void	GetLineRect(int line, KRECT& rect) = 0;

	// 行数の取得・設定
	virtual int		GetLinesCount() = 0;
	virtual void	SetLinesCount(int count) = 0;
	virtual int		GetTopVisibleLine() = 0;
	virtual int		GetVisibleCount() = 0;
	virtual int		GetSelectedCount() = 0;

	// 行の選択状態の取得・設定
	virtual int		GetFirstSelectedLine() = 0;
	virtual bool	IsSelected(int line) = 0;
	virtual void	SelectLine(int line, bool need_redraw = true) = 0;
	virtual void	UnselectLine(int line, bool  need_redraw = true) = 0;
	virtual void	SetAllLinesUnselected(bool need_redraw = true) = 0;

	// カーソル位置の取得・設定
	public:
	virtual int		GetCursorPos() = 0;
	virtual void	SetCursorPos(int line, bool need_redraw = true) = 0;

	protected:
	// 行のドロップ対象属性の設定
	virtual bool	IsDropHilited(int line) = 0;
	virtual void	SetDropHilited(int line, bool state, bool need_redraw = true) = 0;

	// ヒットテスト
	virtual int		DoHitTest(KPOINT point) = 0;

	// 行の挿入・削除・描画
	virtual void	InsertLine(int pos) = 0;
	virtual void	DeleteLine(int line) = 0;
	virtual void	Redraw(int start = -1, int end = -1) = 0;
	virtual void	EnsureVisible(int line) = 0;
	virtual void	Scroll(int ofs) = 0;

	// アプリケーションリソースからの画像読み込み
	virtual KImage	*LoadAppResourceImage(KString rname) = 0;

	// ノード編集ダイアログの表示
	virtual bool	DoEditDialog(KSceneDocItem *item) = 0;
};

#endif
