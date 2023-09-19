// EditCheckVarDlg.h
// 「変数の値を判定する」命令を編集するダイアログ
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#ifndef _EDITCHECKVARDLG_H_
#define _EDITCHECKVARDLG_H_

#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "common/KSceneDoc.h"
#include "Resource.h"

class CEditCheckVarDlg : public CDialog
{
public:
	CEditCheckVarDlg(CWnd* pParent = NULL);

	BOOL	DoModal(KSceneDocItem *item);

/////////////////////////////////////////////////////////////////////////////

	//{{AFX_DATA(CEditCheckVarDlg)
	enum { IDD = IDD_EDITCHECKVAR };
	CComboBox	m_wndCbCond;
	CString	m_strValue;
	CString	m_strVar;
	int		m_iCondition;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CEditCheckVarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CEditCheckVarDlg)
	afx_msg void OnDropdownCbValue();
	afx_msg void OnDropdownCbVar();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
