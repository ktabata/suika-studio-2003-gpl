/*****************************************************************************
 KRuntimeEnv.h
 �����^�C����

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KRUNTIMEENV_H_
#define _KRUNTIMEENV_H_

#include "KSceneDoc.h"
#include "KAnimator.h"
#include "klib/io.h"

class KRuntimeEnv;
class KSceneDoc;
class KSceneNode;

class KRuntimeEnv {
public:
	KRuntimeEnv();
	virtual ~KRuntimeEnv();

	// ������
	virtual bool	Init();

	// ���s
	int	Execute();

	// �f�o�b�K�p�F�V�[���E���s�ʒu�̐ݒ�E�擾
	bool	LoadScene(KString fname, int line);
	void	SetLineNum(int line);
	KString		GetSceneFileName();
	int			GetLineNum();
	KSceneDoc	*GetScene();

	// �G���[���b�Z�[�W�ݒ�E�擾�iKSceneNode�ɂ���ăZ�b�g�����j
	void	SetErrMsg(KString msg);
	KString GetErrMsg();

	// �X�N���[���T�C�Y�̎擾
	int		GetScreenWidth();
	int		GetScreenHeight();

	// �X�N���[����GC�A�_�u���o�b�t�@�̃o�b�N�C���[�W�̎擾
	KGraphicContext	*GetGC();
	KImage			*GetBackImage();

	// �L�[�{�[�h�A�}�E�X��Ԃ̎擾
	virtual bool IsLButtonDown() = 0;
	virtual bool IsRButtonDown() = 0;
	virtual bool IsCtrlDown() = 0;
	virtual void GetMousePos(KPOINT& pt) = 0;
	virtual bool IsUpArrowDown() = 0;
	virtual bool IsDownArrowDown() = 0;

	// �_�u���o�b�t�@�̃o�b�N�C���[�W���X�N���[���ɓ]������
	virtual void Update(int x, int y, int w, int h);
	virtual void Flip();		// �����ł���΃t���b�v�A�ł��Ȃ���Γ]��

	// �I���v���̗L��
	bool	IsQuitRequestPosted();

	// ���s�����n
	virtual void YieldCPU() = 0;

protected:
	// �X�N���[����GC�A�_�u���o�b�t�@�̃o�b�N�C���[�W�̍쐬
	virtual KGraphicContext	*OnCreateGC(int w, int h) = 0;
	virtual KInputStream	*OpenFile(KString fname) = 0;

	// �e���Ԃ̐ݒ�
	void	PostQuitRequest();

private:
	KSceneDoc		*scene;			// �V�[��
	KString			scene_fname;	// �V�[���t�@�C���̖��O
	int				line;			// �s�ԍ�
	KGraphicContext	*gc;			// �X�N���[����GC
	KImage			*back_image;	// �_�u���o�b�t�@�̃o�b�N�C���[�W
//	KAnimator		
	int				width, height;	// �X�N���[���̃T�C�Y
	bool			need_quit;		// �I�����N�G�X�g�t���O
	KString			err_msg;
};

#endif
