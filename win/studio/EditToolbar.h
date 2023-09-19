/*****************************************************************************
 EditToolbar.h
 編集ツールバー

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _EDITTOOLBAR_H_
#define _EDITTOOLBAR_H_

#include <afxcmn.h>
#include <afxext.h>
#include "SceneView.h"
#include "SuikaButton.h"

class CEditToolbar : public CStatic
{
public:
	CEditToolbar(CSceneView *view);
	~CEditToolbar();

	BOOL	Create(CWnd *pParent, UINT nID);
	int		GetHeight();

private:
	CSuikaButton	*m_pInsertBtn;
	CSceneView		*m_pView;
	int				m_iType;

	KImage			*icon_image;
	KImage			*typebox_image;
	KGraphicContext *gc;

	void	DrawTypeBox();
	void	Popup();
	void	SetType(int type);

	static int GetItemTypeFromCmdID(UINT nID);
	static KString GetOpString(int opcode);

/////////////////////////////////////////////////////////////////////////////

	//{{AFX_VIRTUAL(CEditToolbar)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CEditToolbar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtnInsert();
	//}}AFX_MSG
	afx_msg void OnSetType(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
