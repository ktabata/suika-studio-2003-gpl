/*****************************************************************************
 SceneView.h
 シーンエディタのビュー (GUI event wrapper)

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _SCENEVIEW_H_
#define _SCENEVIEW_H_

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "common/KSceneView.h"
#include "Resource.h"

class CSceneView : public CListCtrl, public KSceneView
{
public:
	CSceneView();
	~CSceneView();

	BOOL	Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

private:
	// アイテム描画
	CImageList			*image_list;
	KGraphicContext		*gc;

	// マウスイベントの処理
	HCURSOR	m_hCursor;			// ここだけハンガリアン表記……。
	CPoint	last_down_point;
	bool	is_l_down;			// コントロール内で左ボタンが押下されたか？
	bool	is_wheel_scrolled;	// ボタンが押下からドラッグ開始までにホイールでスクロールされたか？
	bool	is_dragging;		// ドラッグ中か？
	bool	is_drag_canceled;	// 右ボタン押下でドラッグが中止されたか？
	void	StartDrag(KPOINT point);
	void	EndDrag(KPOINT point, bool is_drop);

	// ポップアップの表示
	void	ProcessPopup(CPoint point);

private:

	/**************************************************
	  親クラスKSceneViewから要求されるインタフェース
	**************************************************/

	// キー状態の取得
	bool	IsCtrlDown();
	bool	IsShiftDown();

	// ビューの領域取得（マウスと同じ座標系で）
	void	GetViewRect(KRECT& rect);
	void	GetLineRect(int line, KRECT& rect);

	// アイテム数の取得・設定
	int		GetLinesCount();
	void	SetLinesCount(int count);
	int		GetTopVisibleLine();
	int		GetVisibleCount();
	int		GetSelectedCount();

	// アイテム選択状態の取得・設定
	virtual int		GetFirstSelectedLine();
	virtual bool	IsSelected(int line);
	virtual void	SelectLine(int line, bool need_redraw = true);
	virtual void	UnselectLine(int line, bool  need_redraw = true);
	virtual void	SetAllLinesUnselected(bool need_redraw = true);

	// カーソル位置の取得・設定
	virtual int		GetCursorPos();
	virtual void	SetCursorPos(int line, bool need_redraw = true);

	// 行のドロップ対象属性の設定
	virtual bool	IsDropHilited(int line);
	virtual void	SetDropHilited(int line, bool state, bool need_redraw = true);

	// ヒットテスト
	virtual int		DoHitTest(KPOINT point);

	// 行の挿入・削除・描画
	virtual void	InsertLine(int pos);
	virtual void	DeleteLine(int pos);
	virtual void	Redraw(int start = -1, int end = -1);
	virtual void	EnsureVisible(int line);
	virtual void	Scroll(int ofs);

	// アプリケーションリソースからの画像読み込み
	virtual KImage	*LoadAppResourceImage(KString rname);

	// ノード編集ダイアログの表示
	virtual bool	DoEditDialog(KSceneDocItem *item);


/////////////////////////////////////////////////////////////////////////////
public:
	//{{AFX_VIRTUAL(CSceneView)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSceneView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
#endif
