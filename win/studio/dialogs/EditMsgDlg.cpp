// EditMsgDlg.cpp
// 「文章を表示する」命令を編集するダイアログ
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#include "EditMsgDlg.h"
#include "klib/debug.h"

UINT CEditMsgDlg::m_nLastFocus = IDC_COMBO;

CEditMsgDlg::CEditMsgDlg(CWnd* pParent) : CDialog(CEditMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditMsgDlg)
	m_strVoice = _T("");
	m_strMsg = _T("");
	m_strSpeaker = _T("");
	m_iMsgbox = -1;
	//}}AFX_DATA_INIT
}

BOOL CEditMsgDlg::DoModal(KSceneDocItem *item)
{
	m_strSpeaker = (const char *)(KString)item->ParamAt(OPP_MESSAGE_SPEAKER);
	m_strVoice = (const char *)(KString)item->ParamAt(OPP_MESSAGE_VOICE);
	m_strMsg = (const char *)(KString)item->ParamAt(OPP_MESSAGE_MESSAGE);
	m_iMsgbox = item->ParamAt(OPP_MESSAGE_MSGBOX);

	if(CDialog::DoModal() == IDOK)
	{
		item->ParamAt(OPP_MESSAGE_SPEAKER) = (KString)(const char *)m_strSpeaker;
		item->ParamAt(OPP_MESSAGE_VOICE) = (KString)(const char *)m_strVoice;
		item->ParamAt(OPP_MESSAGE_MESSAGE) = (KString)(const char *)m_strMsg;
		item->ParamAt(OPP_MESSAGE_MSGBOX) = m_iMsgbox;
		return TRUE;
	}
	return FALSE;
}

BOOL CEditMsgDlg::OnInitDialog() 
{
	/*
		ここでキャラ名リストをコンボに追加する
	*/

	// 最後にフォーカスのあったコントロールにフォーカスを設定する
	GetDlgItem(m_nLastFocus)->SetFocus();

	CDialog::OnInitDialog();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CEditMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditMsgDlg)
	DDX_Text(pDX, IDC_EDITVOICE, m_strVoice);
	DDV_MaxChars(pDX, m_strVoice, 1024);
	DDX_Text(pDX, IDC_EDITMSG, m_strMsg);
	DDV_MaxChars(pDX, m_strMsg, 65534);
	DDX_CBString(pDX, IDC_CBSPEAKER, m_strSpeaker);
	DDX_CBIndex(pDX, IDC_CBMSGBOX, m_iMsgbox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CEditMsgDlg)
	ON_CBN_SETFOCUS(IDC_CBSPEAKER, OnSpeakerSetFocus)
	ON_EN_SETFOCUS(IDC_EDITVOICE, OnVoiceSetFocus)
	ON_EN_SETFOCUS(IDC_EDITMSG, OnMsgSetFocus)
	ON_EN_SETFOCUS(IDC_CBMSGBOX, OnMsgboxSetFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditMsgDlg::OnSpeakerSetFocus(void) { m_nLastFocus = IDC_CBSPEAKER; }
void CEditMsgDlg::OnVoiceSetFocus(void) { m_nLastFocus = IDC_EDITVOICE; }
void CEditMsgDlg::OnMsgSetFocus(void) { m_nLastFocus = IDC_EDITMSG; }
void CEditMsgDlg::OnMsgboxSetFocus(void) { m_nLastFocus = IDC_CBMSGBOX; }
