/*****************************************************************************
 SceneView.h
 �V�[���G�f�B�^�̃r���[ (GUI event wrapper)

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/
 
#include "SceneView.h"
#include "Resource.h"
#include "klib/debug.h"

#define DRAG_START_DISTANCE	(15)	// �h���b�O���J�n����}�E�X�ړ������i�N���b�N���덷�j
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

	// �w�b�_�����C�I�[�i�[�`��̃��|�[�g�R���g���[�����쐬����
	BOOL b = CListCtrl::Create(
				WS_CHILD|LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_OWNERDRAWFIXED,
				rect, pParent, nID);
	ASSERT(b);

	// �A�C�e���i�s�j�̍����𒲐߂��邽�߂ɃC���[�W���Z�b�g����
	image_list = new CImageList();
	b = image_list->Create("IDB_SCENEVIEWPAD", 32, 0, (COLORREF)0xff00ff);
	ASSERT(b);
	SetImageList(image_list, LVSIL_SMALL);

	// �O���t�B�b�N�R���e�L�X�g���쐬����
	gc = new KGraphicContext( m_hWnd );
	gc->SelectFont(6, 12);
	gc->EnableAntiAliasing(false);

	// �_�~�[�̗��ǉ�����
	InsertColumn(0, "dummy column", LVCFMT_LEFT, rect.right-rect.left+1);
	SetColumnWidth(0, VIEW_WIDTH);

	// �ŉ��s�̃_�~�[�A�C�e����ǉ�����
	SetLinesCount(1);
	SetCursorPos(0);

	return true;
}

BOOL CSceneView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return CListCtrl::PreCreateWindow(cs);
}

// owner-draw �̃R�[���o�b�N
void CSceneView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect	rc = lpDrawItemStruct->rcItem;
	KRECT	rect;
	rect.left =  rc.left;
	rect.top =  rc.top;
	rect.right =  rc.right;
	rect.bottom =  rc.bottom;

	// �A�C�e���C���[�W�ɕ`����s��
	KSceneView::OnDraw(lpDrawItemStruct->itemID, gc, rect);
}

/**************************************************
  �m�[�h�ҏW�_�C�A���O�̕\��
**************************************************/

#include "dialogs/EditMsgDlg.h"
#include "dialogs/EditCheckVarDlg.h"

bool CSceneView::DoEditDialog(KSceneDocItem *item)
{
	K_ASSERT(item != NULL);
	int opcode = item->GetOpCode();

	BOOL res = FALSE;
	if(opcode == SOP_MESSAGE) {	// �u���͂�\������v����
		CEditMsgDlg	dlg(this);
		res = dlg.DoModal(item);
	}
	if(opcode == SOP_CHECKVAR) {	// �u�ϐ��̒l�𔻒肷��v����
		CEditCheckVarDlg	dlg(this);
		res = dlg.DoModal(item);
	}

	SetFocus();
	return res == TRUE;		// convert 'BOOL' to 'bool'
}

/**************************************************
  �|�b�v�A�b�v�̕\��
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
  �h���b�O�֘A
**************************************************/

// �h���b�O�J�n����
void CSceneView::StartDrag(KPOINT point)
{
	//�@���̏ꍇ�h���b�O�͖���
	// �@�E�{�^�����������ꂽ�̂��R���g���[�����łȂ�
	// �@�E�{�^������������Ă���h���b�O�����܂łɃz�C�[���ŃX�N���[�����ꂽ
	if(!is_l_down || is_wheel_scrolled)
		return;

	// �}�E�X�ړ��덷�͈͂��H�i�N���b�N���̃}�E�X�ړ��덷���l���āj
	int dist = last_down_point.y - point.y;
	if(dist < 0)
		dist *= -1;	// abs
	if(dist	< DRAG_START_DISTANCE)
		return;	// �덷�͈͂Ȃ̂Ń��^�[��

	// �h���b�O�J�n��KSceneView�ɒm�点��
	if(!KSceneView::OnBeginDrag(point)) {
		is_drag_canceled = true;
		return;		// �h���b�O�J�n�����ۂ��ꂽ
	}

	is_dragging = true;
	m_hCursor = ::SetCursor(::LoadCursor(::GetModuleHandle(NULL),
					MAKEINTRESOURCE(IDC_DRAGCURSOR) ) );
}

// �h���b�O�I������
void CSceneView::EndDrag(KPOINT point, bool is_drop)
{
	is_dragging = false;
	::SetCursor(m_hCursor);
	KSceneView::OnEndDrag(point, is_drop);
}

/**************************************************
  �e�N���XKSceneView����v�������C���^�t�F�[�X
**************************************************/

// �L�[��Ԃ̎擾

bool CSceneView::IsCtrlDown()
{
	return (::GetKeyState(VK_CONTROL) & 0x80000000) != 0;
}

bool CSceneView::IsShiftDown()
{
	return (::GetKeyState(VK_SHIFT) & 0x80000000) != 0;
}

// �r���[�̗̈�擾�i�}�E�X�Ɠ������W�n�Łj

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

// �s���̎擾�E�ݒ�

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

// �s�̑I����Ԃ̎擾�E�ݒ�

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

// �J�[�\���ʒu�̎擾�E�ݒ�

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

// �s�̃h���b�v�Ώۑ����̐ݒ�

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

// �q�b�g�e�X�g

int CSceneView::DoHitTest(KPOINT point)
{
	CPoint pt(point.x, point.y);
	pt.x = 10;
	return HitTest(pt);
}

// �s�̑}���E�폜�E�ĕ`��

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

// �A�v���P�[�V�������\�[�X����̉摜�ǂݍ���

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

	// �h���b�O�����H
	if(is_dragging) {
		KSceneView::OnDragMove(point);
		return;
	}

	// ���{�^������������Ă��邩
	if((nFlags & MK_LBUTTON) && !is_drag_canceled)
		StartDrag(point);
}

void CSceneView::OnLButtonDown(UINT nFlags, CPoint pt)
{
	SetFocus();

	is_l_down = true;
	last_down_point = pt;
	/* is_l_down: �R���g���[���O�ŉ������ꂽ�܂܃}�E�X���R���g���[�����ɓ������Ƃ�
	   �h���b�O���J�n���Ȃ��悤�ɂ��邽�� */

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

	// �h���b�O�����H
	if(is_dragging) {
		// �h���b�O���L�����Z������
		EndDrag(point, false);
		is_drag_canceled = true;		// �Ή�����{�^������ŎQ�Ƃ����t���O���Z�b�g����
		return;
	}

	last_down_point = pt;
	KSceneView::MoveCursorToHitLine(point);
}

void CSceneView::OnRButtonUp(UINT nFlags, CPoint pt)
{
	// �Ή����钼�O�̉����Ńh���b�O���L�����Z������Ă����
	if(is_drag_canceled)
		return;

	// �Ή����钼�O�̃{�^��������MoveCursorToHitLine()���Ă΂�C
	// ����̏����iKSceneView::MoveCursorToHitLine()�Q�Ɓj�𖞂����Ȃ�
	if(KSceneView::is_cur_moved) {
		KPOINT	point;
		point.x = pt.x, point.y = pt.y;
		MoveCursorToHitLine(point);
		return;
	}

	// �|�b�v�A�b�v���j���[��\������
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
// �A�C�e�����\������Ȃ����������h��Ԃ�

	int		iPageCount = GetCountPerPage();
	int		iCount = GetItemCount();
	int		iTop = GetTopIndex();

	// �u���V���擾����
	CBrush *pBrush = pDC->GetCurrentBrush();

// for debug
/*	CBrush brush;
	brush.CreateSolidBrush(0);
	CBrush *pBrush = &brush;
*/
	// �N���C�A���g�̈���擾����
	RECT rcClient;
	GetClientRect(&rcClient);

	// �㑤�̃A�C�e�����Ȃ��̈�����߂�
	RECT	rc;
	CListCtrl::GetItemRect((iTop < 0) ? 0 : iTop, &rc, LVIR_BOUNDS);

	// �㑤�̃A�C�e�����Ȃ��̈��h��Ԃ�
	if(rc.top > 0)
	{
		RECT rcErase;
		rcErase.left = 0;
		rcErase.top = 0;
		rcErase.right = rcClient.right;
		rcErase.bottom = rc.top;
		pDC->FillRect(&rcErase, pBrush);
	}

	// �����̃A�C�e�����Ȃ��̈�����߂�
	int		iExpect = iTop + iPageCount - 1;
	int		iBottom = (iExpect >= iCount) ? iCount-1 : iExpect;
	CListCtrl::GetItemRect(iBottom, &rc, LVIR_BOUNDS);

	// �����̃A�C�e�����Ȃ��̈��h��Ԃ�
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
