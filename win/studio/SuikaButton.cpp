/*****************************************************************************
 SuikaButton.h
 カスタムボタン

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

	// ポップアップ表示中か？
	if(m_iState == STATE_MOUSEOVER)
	{
		// マウスがクライアント領域にあるか？
		if(m_rc.PtInRect(point))
			return;

		// クライアント領域から外れたのでポップアップを消す
		ClosePopup();
		m_iState = STATE_NORMAL;
		RedrawWindow();
		return;
	}

	// 現在通常状態。ポップアップを表示する	
	ShowPopup();
	m_iState = STATE_MOUSEOVER;
	RedrawWindow();
}

void CSuikaButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// ポップアップ表示中か？
	if(m_iState == STATE_MOUSEOVER)
		ClosePopup();

	m_iState = STATE_PUSHED;
	SetCapture();
	RedrawWindow();
}

void CSuikaButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// ボタンが押下されたのがクライアント領域外ならリターン
	if(m_iState != STATE_PUSHED)
		return;

	// マウスキャプチャを解放して通常状態で再描画
	m_iState = STATE_NORMAL;
	ReleaseCapture();
	RedrawWindow();

	// ボタンが解放されたのがクライアント領域内なら親ウィンドウに通知
	if(m_rc.PtInRect(point))
		m_pParentWnd->SendMessage(WM_COMMAND, MAKEWPARAM(m_nID, BN_CLICKED)); 
}

void CSuikaButton::OnPaint() 
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	int w = m_rc.Width();
	int h = m_rc.Height();
	int x = w;
	switch(m_iState)
	{
	case STATE_NORMAL:		x = 0;	break;
	case STATE_MOUSEOVER:	x *= 1;	break;	// *= 1 は明示しただけ
	case STATE_PUSHED:		x *= 2;	break;
	case STATE_DISABLED:	x *= 3;	break;
	}

	KGraphicContext	gc( dc.m_hDC );
	gc.DrawImage(button_image, 0, 0, w, h, x, 0);
}
