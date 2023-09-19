/*****************************************************************************
 SceneView.h
 シーンエディタのビュー (GUI event wrapper)

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/
 
#include "SceneView.h"
#include "Resource.h"
#include "klib/debug.h"

#define DRAG_START_DISTANCE	(15)	// ドラッグを開始するマウス移動距離（クリック時誤差）
#define ITEM_HEIGHT			(25)
#define VIEW_WIDTH			(1024)

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

CSceneView::CSceneView()
{
	gc = NULL;
	image_list = NULL;
	m_hCursor = NULL;
	is_l_down = false;
	is_dragging = false;
	is_wheel_scrolled = false;
	is_drag_canceled = false;
}

CSceneView::~CSceneView()
{
	RELEASE(gc);
	RELEASE(image_list);
}

BOOL CSceneView::Create(const RECT& rect, CWnd* pParent, UINT nID)
{
	KSceneView::Init();

	// ヘッダ無し，オーナー描画のレポートコントロールを作成する
	BOOL b = CListCtrl::Create(
				WS_CHILD|LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_OWNERDRAWFIXED,
				rect, pParent, nID);
	ASSERT(b);

	// アイテム（行）の高さを調節するためにイメージをセットする
	image_list = new CImageList();
	b = image_list->Create("IDB_SCENEVIEWPAD", 32, 0, (COLORREF)0xff00ff);
	ASSERT(b);
	SetImageList(image_list, LVSIL_SMALL);

	// グラフィックコンテキストを作成する
	gc = new KGraphicContext( m_hWnd );
	gc->SelectFont(6, 12);
	gc->EnableAntiAliasing(false);

	// ダミーの列を追加する
	InsertColumn(0, "dummy column", LVCFMT_LEFT, rect.right-rect.left+1);
	SetColumnWidth(0, VIEW_WIDTH);

	// 最下行のダミーアイテムを追加する
	SetLinesCount(1);
	SetCursorPos(0);

	return true;
}

BOOL CSceneView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return CListCtrl::PreCreateWindow(cs);
}

// owner-draw のコールバック
void CSceneView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect	rc = lpDrawItemStruct->rcItem;
	KRECT	rect;
	rect.left =  rc.left;
	rect.top =  rc.top;
	rect.right =  rc.right;
	rect.bottom =  rc.bottom;

	// アイテムイメージに描画を行う
	KSceneView::OnDraw(lpDrawItemStruct->itemID, gc, rect);
}

/**************************************************
  ノード編集ダイアログの表示
**************************************************/

#include "dialogs/EditMsgDlg.h"
#include "dialogs/EditCheckVarDlg.h"

bool CSceneView::DoEditDialog(KSceneDocItem *item)
{
	K_ASSERT(item != NULL);
	int opcode = item->GetOpCode();

	BOOL res = FALSE;
	if(opcode == SOP_MESSAGE) {	// 「文章を表示する」命令
		CEditMsgDlg	dlg(this);
		res = dlg.DoModal(item);
	}
	if(opcode == SOP_CHECKVAR) {	// 「変数の値を判定する」命令
		CEditCheckVarDlg	dlg(this);
		res = dlg.DoModal(item);
	}

	SetFocus();
	return res == TRUE;		// convert 'BOOL' to 'bool'
}

/**************************************************
  ポップアップの表示
**************************************************/

void CSceneView::ProcessPopup(CPoint point)
{
	CPoint screen = point;
	ClientToScreen(&screen);

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_SCENEVIEWMENU));
	CMenu* pPopup = menu.GetSubMenu((GetSelectedCount()>0) ? 0 : 1);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		screen.x, screen.y, this);
}

/**************************************************
  ドラッグ関連
**************************************************/

// ドラッグ開始処理
void CSceneView::StartDrag(KPOINT point)
{
	//　次の場合ドラッグは無効
	// 　・ボタンが押下されたのがコントロール内でない
	// 　・ボタンが押下されてからドラッグされるまでにホイールでスクロールされた
	if(!is_l_down || is_wheel_scrolled)
		return;

	// マウス移動誤差範囲か？（クリック時のマウス移動誤差を考えて）
	int dist = last_down_point.y - point.y;
	if(dist < 0)
		dist *= -1;	// abs
	if(dist	< DRAG_START_DISTANCE)
		return;	// 誤差範囲なのでリターン

	// ドラッグ開始をKSceneViewに知らせる
	if(!KSceneView::OnBeginDrag(point)) {
		is_drag_canceled = true;
		return;		// ドラッグ開始が拒否された
	}

	is_dragging = true;
	m_hCursor = ::SetCursor(::LoadCursor(::GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDC_DRAGCURSOR) ) );
}

// ドラッグ終了処理
void CSceneView::EndDrag(KPOINT point, bool is_drop)
{
	is_dragging = false;
	::SetCursor(m_hCursor);
	KSceneView::OnEndDrag(point, is_drop);
}

/**************************************************
  親クラスKSceneViewから要求されるインタフェース
**************************************************/

// キー状態の取得

bool CSceneView::IsCtrlDown()
{
	return (::GetKeyState(VK_CONTROL) & 0x80000000) != 0;
}

bool CSceneView::IsShiftDown()
{
	return (::GetKeyState(VK_SHIFT) & 0x80000000) != 0;
}

// ビューの領域取得（マウスと同じ座標系で）

void CSceneView::GetViewRect(KRECT& rect)
{
	RECT rc;
	GetClientRect(&rc);
	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right;
	rect.bottom = rc.bottom;
}

void CSceneView::GetLineRect(int line, KRECT& rect)
{
	RECT rc;
	GetItemRect(line, &rc, LVIR_BOUNDS);
	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right;
	rect.bottom = rc.bottom;
}

// 行数の取得・設定

int CSceneView::GetLinesCount()
{
	return GetItemCount();
}

void CSceneView::SetLinesCount(int count)
{
	int diff = count - GetItemCount();

	if(diff >= 0) {
		for(int i = 0; i < diff; i++)
			CListCtrl::InsertItem(0, "dummy");
	} else {
		for(int i = diff; i < 0; i++)
			CListCtrl::DeleteItem(0);
	}
	RedrawWindow();
}

int CSceneView::GetTopVisibleLine()
{
	return GetTopIndex();
}

int CSceneView::GetVisibleCount()
{
	return GetCountPerPage();
}

int CSceneView::GetSelectedCount()
{
	return CListCtrl::GetSelectedCount();
}

// 行の選択状態の取得・設定

int CSceneView::GetFirstSelectedLine(void)
{
	int items_count = GetItemCount();
	for(int i = 0; i < items_count; i++)
		if(IsSelected(i))
			return i;
	return -1;
}

bool CSceneView::IsSelected(int line)
{
	if(line == -1)
		return false;
	return (GetItemState(line, LVIS_SELECTED) == LVIS_SELECTED);
}

void CSceneView::SelectLine(int line, bool need_redraw /*=true*/)
{
	SetItemState(line, LVIS_SELECTED, LVIS_SELECTED);
	if(need_redraw)
		RedrawItems(line, line);
}

void CSceneView::UnselectLine(int line, bool need_redraw /*=true*/)
{
	SetItemState(line, 0, LVIS_SELECTED);
	if(need_redraw)
		RedrawItems(line, line);
}

void CSceneView::SetAllLinesUnselected(bool need_redraw /*=true*/)
{
	int sel_count = CListCtrl::GetSelectedCount();
	int items_count = CListCtrl::GetItemCount();
	for(int i = 0; i < items_count && sel_count > 0; i++)
	{
		if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			UnselectLine(i, need_redraw);
			sel_count--;
		}
	}
}

// カーソル位置の取得・設定

int CSceneView::GetCursorPos()
{
	int items_count = GetItemCount();
	for(int i = 0; i < items_count; i++)
		if(GetItemState(i, LVIS_FOCUSED) == LVIS_FOCUSED)
			return i;
	return -1;
}

void CSceneView::SetCursorPos(int line, bool need_redraw /*=true*/)
{
	SetItemState(line, LVIS_FOCUSED, LVIS_FOCUSED);
	if(need_redraw)
		RedrawItems(line, line);
}

// 行のドロップ対象属性の設定

bool CSceneView::IsDropHilited(int line)
{
	if(line == -1)
		return false;
	return (GetItemState(line, LVIS_DROPHILITED)== LVIS_DROPHILITED);
}

void CSceneView::SetDropHilited(int line, bool light, bool need_redraw /*=true*/)
{
	SetItemState(line, light ? LVIS_DROPHILITED : 0, LVIS_DROPHILITED);
	if(need_redraw)
		RedrawItems(line, line);
}

// ヒットテスト

int CSceneView::DoHitTest(KPOINT point)
{
	CPoint pt(point.x, point.y);
	pt.x = 10;
	return HitTest(pt);
}

// 行の挿入・削除・再描画

void CSceneView::InsertLine(int pos)
{
	CListCtrl::InsertItem(pos, "dummy");
	UpdateWindow();
}

void CSceneView::DeleteLine(int pos)
{
	CListCtrl::DeleteItem(pos);
	UpdateWindow();
}

void CSceneView::Redraw(int start /*= -1*/, int end /*= -1*/)
{
	if(start == -1)
		start = GetTopIndex();
	if(end == -1)
		end = start + GetCountPerPage() - 1;

	RedrawItems(start, end);
	UpdateWindow();
}

void CSceneView::EnsureVisible(int line)
{
	CListCtrl::EnsureVisible(line, FALSE);
	UpdateWindow();
}

void CSceneView::Scroll(int ofs)
{
	CListCtrl::Scroll( CSize(0, ITEM_HEIGHT*ofs) );
	UpdateWindow();
}

// アプリケーションリソースからの画像読み込み

KImage *CSceneView::LoadAppResourceImage(KString rname)
{
	return KImage::LoadResourceBitmap(rname);
}

/////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSceneView, CListCtrl)
	//{{AFX_MSG_MAP(CSceneView)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSceneView::OnMouseMove(UINT nFlags, CPoint pt)
{
	KPOINT point;
	point.x = pt.x, point.y = pt.y;

	// ドラッグ中か？
	if(is_dragging) {
		KSceneView::OnDragMove(point);
		return;
	}

	// 左ボタンが押下されているか
	if((nFlags & MK_LBUTTON) && !is_drag_canceled)
		StartDrag(point);
}

void CSceneView::OnLButtonDown(UINT nFlags, CPoint pt)
{
	SetFocus();

	is_l_down = true;
	last_down_point = pt;
	/* is_l_down: コントロール外で押下されたままマウスがコントロール内に入ったとき
	   ドラッグを開始しないようにするため */

	KPOINT	point;
	point.x = pt.x, point.y = pt.y;

	SetCapture();
	OnButtonDown(point);
}

void CSceneView::OnLButtonUp(UINT nFlags, CPoint pt)
{
	KPOINT	point;
	point.x = pt.x, point.y = pt.y;
	is_wheel_scrolled = FALSE;
	is_l_down = FALSE;
	is_drag_canceled = false;

	ReleaseCapture();

	if(is_dragging)	{
		EndDrag(point, true);
		return;
	}

	KSceneView::OnButtonUp(point);
}

void CSceneView::OnRButtonDown(UINT nFlags, CPoint pt)
{
	SetFocus();

	KPOINT	point;
	point.x = pt.x, point.y = pt.y;
	is_wheel_scrolled = FALSE;

	// ドラッグ中か？
	if(is_dragging) {
		// ドラッグをキャンセルする
		EndDrag(point, false);
		is_drag_canceled = true;		// 対応するボタン解放で参照されるフラグをセットする
		return;
	}

	last_down_point = pt;
	KSceneView::MoveCursorToHitLine(point);
}

void CSceneView::OnRButtonUp(UINT nFlags, CPoint pt)
{
	// 対応する直前の押下でドラッグがキャンセルされていれば
	if(is_drag_canceled)
		return;

	// 対応する直前のボタン押下でMoveCursorToHitLine()が呼ばれ，
	// 特定の条件（KSceneView::MoveCursorToHitLine()参照）を満たすなら
	if(KSceneView::is_cur_moved) {
		KPOINT	point;
		point.x = pt.x, point.y = pt.y;
		MoveCursorToHitLine(point);
		return;
	}

	// ポップアップメニューを表示する
	ProcessPopup(pt);
}

BOOL CSceneView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	is_wheel_scrolled = TRUE;
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CSceneView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	*pResult = 0;
	SetFocus();

	int item = pNMListView->iItem;
	if(item != -1)
		KSceneView::OnDoubleClick(item);
}

void CSceneView::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	int keycode = pLVKeyDown->wVKey;
	*pResult = 1;

	KSceneView::OnKeyDown(keycode);
}

BOOL CSceneView::OnEraseBkgnd(CDC* pDC)
{
// アイテムが表示されない部分だけ塗りつぶす

	int		iPageCount = GetCountPerPage();
	int		iCount = GetItemCount();
	int		iTop = GetTopIndex();

	// ブラシを取得する
	CBrush *pBrush = pDC->GetCurrentBrush();

// for debug
/*	CBrush brush;
	brush.CreateSolidBrush(0);
	CBrush *pBrush = &brush;
*/
	// クライアント領域を取得する
	RECT rcClient;
	GetClientRect(&rcClient);

	// 上側のアイテムがない領域を求める
	RECT	rc;
	CListCtrl::GetItemRect((iTop < 0) ? 0 : iTop, &rc, LVIR_BOUNDS);

	// 上側のアイテムがない領域を塗りつぶす
	if(rc.top > 0)
	{
		RECT rcErase;
		rcErase.left = 0;
		rcErase.top = 0;
		rcErase.right = rcClient.right;
		rcErase.bottom = rc.top;
		pDC->FillRect(&rcErase, pBrush);
	}

	// 下側のアイテムがない領域を求める
	int		iExpect = iTop + iPageCount - 1;
	int		iBottom = (iExpect >= iCount) ? iCount-1 : iExpect;
	CListCtrl::GetItemRect(iBottom, &rc, LVIR_BOUNDS);

	// 下側のアイテムがない領域を塗りつぶす
	if(rcClient.bottom > rc.bottom )
	{
		RECT rcErase;
		rcErase.left = 0;
		rcErase.top = rc.bottom;
		rcErase.right = rcClient.right;
		rcErase.bottom = rcClient.bottom;
		pDC->FillRect(&rcErase, pBrush);
	}

	return TRUE;
}
