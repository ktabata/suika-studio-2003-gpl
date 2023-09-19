// EditMsgDlg.h
// 「文章を表示する」命令を編集するダイアログ
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#ifndef _EDITMSGDLG_H_
#define _EDITMSGDLG_H_

#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "common/KSceneDoc.h"
#include "Resource.h"

class CEditMsgDlg : public CDialog
{
public:
	CEditMsgDlg(CWnd* pParent);

	BOOL	DoModal(KSceneDocItem *item);

private:
	static UINT	m_nLastFocus;

/////////////////////////////////////////////////////////////////////////////
private:
	//{{AFX_DATA(CEditMsgDlg)
	enum { IDD = IDD_EDITMSG };
	CString		m_strVoice;
	CString		m_strMsg;
	CString	m_strSpeaker;
	int		m_iMsgbox;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CEditMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CEditMsgDlg)
	virtual BOOL OnInitDialog(void);
	afx_msg void OnSpeakerSetFocus(void);
	afx_msg void OnVoiceSetFocus(void);
	afx_msg void OnMsgSetFocus(void);
	afx_msg void OnMsgboxSetFocus(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
