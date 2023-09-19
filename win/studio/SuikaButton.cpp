/*****************************************************************************
 SuikaButton.h
 �J�X�^���{�^��

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "SuikaButton.h"
#include "Resource.h"
#include "klib/debug.h"

#define STATE_NORMAL		(0)
#define STATE_MOUSEOVER		(1)
#define STATE_PUSHED		(2)
#define STATE_DISABLED		(3)

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

CSuikaButton::CSuikaButton()
{
	m_iState = STATE_NORMAL;
}

CSuikaButton::~CSuikaButton()
{
	RELEASE(button_image);
}

BOOL CSuikaButton::LoadBitmap(LPCSTR lpszResource)
{
	button_image = KImage::LoadResourceBitmap(lpszResource);
	return(button_image != NULL);
}

BOOL CSuikaButton::Create(
	LPCSTR lpszCaption,
	const RECT& rc,
	CWnd *pParent,
	UINT nID)
{
	m_strCaption = lpszCaption;
	m_rc = rc;
	m_rc.OffsetRect(-rc.left, -rc.top);
	m_pParentWnd = pParent;
	m_nID = nID;
	return CStatic::Create(NULL, WS_CHILD|WS_VISIBLE|SS_USERITEM|SS_NOTIFY,
				rc, pParent, nID);
}

void CSuikaButton::Enable()
{
	if(m_iState == STATE_DISABLED)
	{
		m_iState = STATE_NORMAL;
		RedrawWindow();
	}
}

void CSuikaButton::Disable()
{
	if(m_iState != STATE_DISABLED)
	{
		if(m_iState == STATE_MOUSEOVER)
			ClosePopup();
		m_iState = STATE_DISABLED;
		RedrawWindow();
	}
}

void CSuikaButton::ShowPopup()
{
	SetCapture();
}

void CSuikaButton::ClosePopup()
{
	ReleaseCapture();
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSuikaButton, CStatic)
	//{{AFX_MSG_MAP(CSuikaButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSuikaButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_iState == STATE_DISABLED || m_iState == STATE_PUSHED)
		return;

	// �|�b�v�A�b�v�\�������H
	if(m_iState == STATE_MOUSEOVER)
	{
		// �}�E�X���N���C�A���g�̈�ɂ��邩�H
		if(m_rc.PtInRect(point))
			return;

		// �N���C�A���g�̈悩��O�ꂽ�̂Ń|�b�v�A�b�v������
		ClosePopup();
		m_iState = STATE_NORMAL;
		RedrawWindow();
		return;
	}

	// ���ݒʏ��ԁB�|�b�v�A�b�v��\������	
	ShowPopup();
	m_iState = STATE_MOUSEOVER;
	RedrawWindow();
}

void CSuikaButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// �|�b�v�A�b�v�\�������H
	if(m_iState == STATE_MOUSEOVER)
		ClosePopup();

	m_iState = STATE_PUSHED;
	SetCapture();
	RedrawWindow();
}

void CSuikaButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// �{�^�����������ꂽ�̂��N���C�A���g�̈�O�Ȃ烊�^�[��
	if(m_iState != STATE_PUSHED)
		return;

	// �}�E�X�L���v�`����������Ēʏ��Ԃōĕ`��
	m_iState = STATE_NORMAL;
	ReleaseCapture();
	RedrawWindow();

	// �{�^����������ꂽ�̂��N���C�A���g�̈���Ȃ�e�E�B���h�E�ɒʒm
	if(m_rc.PtInRect(point))
		m_pParentWnd->SendMessage(WM_COMMAND, MAKEWPARAM(m_nID, BN_CLICKED)); 
}

void CSuikaButton::OnPaint() 
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

	int w = m_rc.Width();
	int h = m_rc.Height();
	int x = w;
	switch(m_iState)
	{
	case STATE_NORMAL:		x = 0;	break;
	case STATE_MOUSEOVER:	x *= 1;	break;	// *= 1 �͖�����������
	case STATE_PUSHED:		x *= 2;	break;
	case STATE_DISABLED:	x *= 3;	break;
	}

	KGraphicContext	gc( dc.m_hDC );
	gc.DrawImage(button_image, 0, 0, w, h, x, 0);
}
