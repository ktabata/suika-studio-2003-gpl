// EditCheckVarDlg.cpp
// �u�ϐ��̒l�𔻒肷��v���߂�ҏW����_�C�A���O
//
// Suika Studio (Win32)
// (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.

#include "EditCheckVarDlg.h"
#include "klib/debug.h"

#define CB_COND_EQ (0)

CEditCheckVarDlg::CEditCheckVarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditCheckVarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditCheckVarDlg)
	m_strVar = _T("");
	m_iCondition = -1;
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}

BOOL CEditCheckVarDlg::DoModal(KSceneDocItem *item)
{
	m_strVar = (const char *)(KString)item->ParamAt(OPP_CHECKVAR_VARIABLE);
	m_iCondition = item->ParamAt(OPP_CHECKVAR_CONDITION);
	m_strValue = (const char *)(KString)item->ParamAt(OPP_CHECKVAR_VALUE);

	K_ASSERT( IS_VALID_CVCOND(m_iCondition) );

	if(CDialog::DoModal() == IDCANCEL)
		return FALSE;

	K_ASSERT( IS_VALID_CVCOND(m_iCondition) );

	item->ParamAt(OPP_CHECKVAR_VARIABLE) = (KString)(const char *)m_strVar;
	item->ParamAt(OPP_CHECKVAR_CONDITION) = m_iCondition;
	item->ParamAt(OPP_CHECKVAR_VALUE) = (KString)(const char *)m_strValue;
	return TRUE;
}

void CEditCheckVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCheckVarDlg)
	DDX_Control(pDX, IDC_CBCONDITION, m_wndCbCond);
	DDX_CBString(pDX, IDC_CBVALUE, m_strValue);
	DDV_MaxChars(pDX, m_strValue, 1024);
	DDX_CBString(pDX, IDC_CBVAR, m_strVar);
	DDV_MaxChars(pDX, m_strVar, 1024);
	DDX_CBIndex(pDX, IDC_CBCONDITION, m_iCondition);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CEditCheckVarDlg, CDialog)
	//{{AFX_MSG_MAP(CEditCheckVarDlg)
	ON_CBN_DROPDOWN(IDC_CBVALUE, OnDropdownCbValue)
	ON_CBN_DROPDOWN(IDC_CBVAR, OnDropdownCbVar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEditCheckVarDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// �����R���{�{�b�N�X�ɃA�C�e����ǉ�����
	for(int i = 0; i <= 5; i ++)
		m_wndCbCond.AddString( g_cvcond[i] );
	m_wndCbCond.SetCurSel(m_iCondition);

	return TRUE;
}

void CEditCheckVarDlg::OnDropdownCbValue() 
{
	// �ϐ��e�[�u�����Q�Ƃ��Ĉꗗ�\������
}

void CEditCheckVarDlg::OnDropdownCbVar() 
{
	// �ϐ��e�[�u�����Q�Ƃ��Ĉꗗ�\������
}
