/*****************************************************************************
 DebugToolBar.h
 デバッグツールバー

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "DebugToolbar.h"
#include "SceneView.h"
#include "Resource.h"
#include "klib/debug.h"

#define TOOLBAR_WIDTH			(300)
#define TOOLBAR_HEIGHT			(33)
#define IDC_BTN_ENDRETURN	(10)

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

CDebugToolbar::CDebugToolbar(CMainWindow *pWnd)
{
	m_pMainWnd = pWnd;
	m_btnDebugEndReturn = NULL;
}

CDebugToolbar::~CDebugToolbar()
{
	RELEASE(m_btnDebugEndReturn);
}

BOOL CDebugToolbar::Create(CWnd *pParent, UINT nID)
{
	RECT rc = {0, 0, TOOLBAR_WIDTH, TOOLBAR_HEIGHT};
	return CStatic::Create(NULL, WS_CHILD|SS_USERITEM|SS_NOTIFY,
				rc, pParent, nID);
}

int CDebugToolbar::GetHeight()
{
	return TOOLBAR_HEIGHT;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDebugToolbar, CStatic)
	//{{AFX_MSG_MAP(CDebugToolbar)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_ENDRETURN, OnBtnDebugEnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CDebugToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL b;

	m_btnDebugEndReturn = new CSuikaButton();
	b = m_btnDebugEndReturn->LoadBitmap("IDB_DEBUG_ENDRETURN");
	K_ASSERT(b);

	RECT rc = {2,2,30,30};
	b = m_btnDebugEndReturn->Create(
		"デバッグを終了して元の位置に戻る",
		rc, this, IDC_BTN_ENDRETURN);
	K_ASSERT(b);

	return 0;
}

void CDebugToolbar::OnBtnDebugEnd()
{
	m_pMainWnd->OnDebugEndReturn();
}
