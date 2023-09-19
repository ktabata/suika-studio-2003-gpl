/*****************************************************************************
 EditToolbar.h
 �ҏW�c�[���o�[

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "EditToolbar.h"
#include "SceneView.h"
#include "Resource.h"
#include "klib/debug.h"

#define TOOLBAR_WIDTH	(600)
#define TOOLBAR_HEIGHT	(33)
#define TYPEBOX_CX		(180)
#define TYPEBOX_CY		(28)
#define LEFT_SPACE		(2)
#define TOP_SPACE		(4)
#define BTN_CX			(70)
#define BTN_CY			(28)

#define IDC_BTNINSERT	(32900)

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

CEditToolbar::CEditToolbar(CSceneView *view)
{
	m_pView = view;
	icon_image = view->GetIconImage();
	m_pInsertBtn = NULL;

	// ���ߑI���{�b�N�X�`��p�C���[�W���쐬����
	typebox_image = new KImage();
	BOOL b = typebox_image->Create(TYPEBOX_CX, TYPEBOX_CY);
	ASSERT(b);
	gc = new KGraphicContext(typebox_image);
	gc->SelectFont(6, 12, "default");
	gc->SetColor(IMG_MAKE_RGB(0,0,0));
	gc->EnableAntiAliasing(true);

	m_iType = SOP_MESSAGE;
}

CEditToolbar::~CEditToolbar()
{
	RELEASE(m_pInsertBtn);
	RELEASE(typebox_image);
	RELEASE(gc);
}

BOOL CEditToolbar::Create(CWnd *pParent, UINT nID)
{
	RECT rc = {0, 0, TOOLBAR_WIDTH, TOOLBAR_HEIGHT};
	return CStatic::Create(NULL, WS_CHILD|SS_USERITEM|SS_NOTIFY,
				rc, pParent,nID);
}

int CEditToolbar::GetHeight()
{
	return TOOLBAR_HEIGHT;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CEditToolbar, CStatic)
	//{{AFX_MSG_MAP(CEditToolbar)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTNINSERT, OnBtnInsert)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_INSERT_MSG, ID_INSERT_MSG, OnSetType)
	ON_COMMAND_RANGE(ID_INSERT_CHECKVAR, ID_INSERT_CHECKVAR, OnSetType)
END_MESSAGE_MAP()

int CEditToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT rc = {TYPEBOX_CX+8, TOP_SPACE, TYPEBOX_CX+8+BTN_CX, TOP_SPACE+BTN_CY};
	m_pInsertBtn = new CSuikaButton();
	m_pInsertBtn->Create("���߂�}�����܂�", rc, this, IDC_BTNINSERT);
	m_pInsertBtn->LoadBitmap("IDB_BTNINSERT");

	// ���߃^�C�v�{�b�N�X�ɖ��߃^�C�v��ݒ肷��
	SetType(SOP_MESSAGE);

	return 0;
}

void CEditToolbar::OnPaint(void) 
{
	CPaintDC dc(this);

	// ���߃{�b�N�X��`�悷��(typebox_image�̓��e��]������)
	::BitBlt(dc, LEFT_SPACE, TOP_SPACE, TYPEBOX_CX, TYPEBOX_CY,
		typebox_image->GetDC(), 0, 0, SRCCOPY);
}

void CEditToolbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// ���ߑI���{�b�N�X�����H
	if(point.x < TYPEBOX_CX && point.y < TYPEBOX_CY)
		Popup();
	else
		CStatic::OnLButtonDown(nFlags, point);
}

void CEditToolbar::Popup()
{
	CPoint point(0, TYPEBOX_CY);
	ClientToScreen(&point);
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_TYPEBOXMENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
		point.x, point.y, this);
}

void CEditToolbar::OnSetType(UINT nID)
{
	int type = GetItemTypeFromCmdID(nID);
	K_ASSERT(type != -1);
	SetType(type);
}

void CEditToolbar::OnBtnInsert()
{
	((KSceneView *)m_pView)->InsertItem(m_iType);
}

////////////////////////////////////////////////////////////////////////////
// Helpers

// ���߃{�b�N�X�̖��߃^�C�v��ύX����
void CEditToolbar::SetType(int type)
{
	m_iType = type;
	DrawTypeBox();
	RedrawWindow();
}

// ���߃{�b�N�X��`�悷��
void CEditToolbar::DrawTypeBox()
{
	// �w�i�𔒂œh��Ԃ�
	typebox_image->White();

	// �A�C�R����`�悷��
	int x = KSceneView::GetIconOffsetX(m_iType);
	gc->DrawImage(icon_image, 2, 2, OP_ICON_WIDTH, OP_ICON_HEIGHT, x, 0);

	// ���ߖ��̕������`�悷��
	gc->DrawText(OP_ICON_WIDTH+8, 8, GetOpString(m_iType));

	// ���̓I�ȃt���[����`�悷��
	CDC		dc;
	CRect	rc(0, 0, TYPEBOX_CX, TYPEBOX_CY);
	BOOL	b;

	b = dc.Attach(typebox_image->GetDC());	// KImage�ɐڑ�����
	ASSERT(b);
	dc.Draw3dRect(rc,						// �`�悷��
		::GetSysColor(COLOR_BTNSHADOW),
		::GetSysColor(COLOR_BTNHIGHLIGHT));
	dc.Detach();							// KImage����؂藣��
}

// ���j���[ID�𖽗߃R�[�h�ɕϊ�����
int	CEditToolbar::GetItemTypeFromCmdID(UINT nID)
{
	switch(nID){
	case ID_INSERT_MSG:			return SOP_MESSAGE;
	case ID_INSERT_CHECKVAR:	return SOP_CHECKVAR;
	}
	return -1;
}

KString CEditToolbar::GetOpString(int opcode)
{
	switch(opcode){
	case SOP_MESSAGE:	return "���͂�\������";
	case SOP_CHECKVAR:	return "�ϐ��̒l�𔻒肷��";
	}
	return "���肦�Ȃ� by ����]���~";
}
