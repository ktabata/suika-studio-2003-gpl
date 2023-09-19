/*****************************************************************************
 SceneView.h
 �V�[���G�f�B�^�̃r���[ (GUI event wrapper)

 Suika Studio (Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _SCENEVIEW_H_
#define _SCENEVIEW_H_

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "common/KSceneView.h"
#include "Resource.h"

class CSceneView : public CListCtrl, public KSceneView
{
public:
	CSceneView();
	~CSceneView();

	BOOL	Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

private:
	// �A�C�e���`��
	CImageList			*image_list;
	KGraphicContext		*gc;

	// �}�E�X�C�x���g�̏���
	HCURSOR	m_hCursor;			// ���������n���K���A���\�L�c�c�B
	CPoint	last_down_point;
	bool	is_l_down;			// �R���g���[�����ō��{�^�����������ꂽ���H
	bool	is_wheel_scrolled;	// �{�^������������h���b�O�J�n�܂łɃz�C�[���ŃX�N���[�����ꂽ���H
	bool	is_dragging;		// �h���b�O�����H
	bool	is_drag_canceled;	// �E�{�^�������Ńh���b�O�����~���ꂽ���H
	void	StartDrag(KPOINT point);
	void	EndDrag(KPOINT point, bool is_drop);

	// �|�b�v�A�b�v�̕\��
	void	ProcessPopup(CPoint point);

private:

	/**************************************************
	  �e�N���XKSceneView����v�������C���^�t�F�[�X
	**************************************************/

	// �L�[��Ԃ̎擾
	bool	IsCtrlDown();
	bool	IsShiftDown();

	// �r���[�̗̈�擾�i�}�E�X�Ɠ������W�n�Łj
	void	GetViewRect(KRECT& rect);
	void	GetLineRect(int line, KRECT& rect);

	// �A�C�e�����̎擾�E�ݒ�
	int		GetLinesCount();
	void	SetLinesCount(int count);
	int		GetTopVisibleLine();
	int		GetVisibleCount();
	int		GetSelectedCount();

	// �A�C�e���I����Ԃ̎擾�E�ݒ�
	virtual int		GetFirstSelectedLine();
	virtual bool	IsSelected(int line);
	virtual void	SelectLine(int line, bool need_redraw = true);
	virtual void	UnselectLine(int line, bool  need_redraw = true);
	virtual void	SetAllLinesUnselected(bool need_redraw = true);

	// �J�[�\���ʒu�̎擾�E�ݒ�
	virtual int		GetCursorPos();
	virtual void	SetCursorPos(int line, bool need_redraw = true);

	// �s�̃h���b�v�Ώۑ����̐ݒ�
	virtual bool	IsDropHilited(int line);
	virtual void	SetDropHilited(int line, bool state, bool need_redraw = true);

	// �q�b�g�e�X�g
	virtual int		DoHitTest(KPOINT point);

	// �s�̑}���E�폜�E�`��
	virtual void	InsertLine(int pos);
	virtual void	DeleteLine(int pos);
	virtual void	Redraw(int start = -1, int end = -1);
	virtual void	EnsureVisible(int line);
	virtual void	Scroll(int ofs);

	// �A�v���P�[�V�������\�[�X����̉摜�ǂݍ���
	virtual KImage	*LoadAppResourceImage(KString rname);

	// �m�[�h�ҏW�_�C�A���O�̕\��
	virtual bool	DoEditDialog(KSceneDocItem *item);


/////////////////////////////////////////////////////////////////////////////
public:
	//{{AFX_VIRTUAL(CSceneView)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSceneView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
#endif
