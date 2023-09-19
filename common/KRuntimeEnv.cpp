/*****************************************************************************
 KRuntimeEnv.cpp
 �����^�C����

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KRuntimeEnv.h"
#include "klib/debug.h"

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

KRuntimeEnv::KRuntimeEnv()
{
	scene = NULL;
	gc = NULL;
	back_image = NULL;
	need_quit = NULL;
}

KRuntimeEnv::~KRuntimeEnv()
{
	RELEASE(scene);
	RELEASE(gc);
	RELEASE(back_image);
}

/****************************************************************************
  ������
****************************************************************************/

bool KRuntimeEnv::Init()
{
	// �E�ݒ�t�@�C������𑜓x��ǂݍ���

	width = 640;
	height = 480;

	gc = OnCreateGC(640, 480);
	if(gc == NULL)
		return false;

	back_image = new KImage();
	bool b = back_image->Create(640, 480);
	return b;

	/*********************************
	  �������F�T�E���h�Ȃǂ̏�����
	*********************************/
}

/****************************************************************************
  �V�[���i���s�ʒu�j�̐ݒ�E�擾
****************************************************************************/

int KRuntimeEnv::Execute()
{
	return 0;
}

/****************************************************************************
  �V�[���i���s�ʒu�j�̐ݒ�E�擾
****************************************************************************/

bool KRuntimeEnv::LoadScene(KString fname, int line)
{
	// �t�@�C���X�g���[�����J��
	KInputStream *is = OpenFile(fname);
	if(is == NULL)
		return false;

	// �V�[�������[�h����
	KSceneDoc *doc = new KSceneDoc();
	if(!doc->Load(is)) {
		delete doc;
		delete is;
		return false;
	}

	// ���݂̃V�[����j�����ĐV�����V�[�����Z�b�g����
	if(scene != NULL)
		delete scene;
	delete is;
	scene_fname = fname;
	scene = doc;
	line = line;
	return true;
}

KString KRuntimeEnv::GetSceneFileName()
{
	return scene_fname;
}

void KRuntimeEnv::SetLineNum(int line)
{
	this->line = line;
}

int KRuntimeEnv::GetLineNum()
{
	return line;
}

KSceneDoc *KRuntimeEnv::GetScene()
{
	return scene;
}

/****************************************************************************
  �G���[���b�Z�[�W�ݒ�E�擾(���s�֐��ɂ���ăZ�b�g�����)
****************************************************************************/
void KRuntimeEnv::SetErrMsg(KString msg)
{
	err_msg = msg;
}

KString KRuntimeEnv::GetErrMsg()
{
	return err_msg;
}

/****************************************************************************
  �`��֌W�̒l�E�I�u�W�F�N�g�擾
****************************************************************************/

KGraphicContext	*KRuntimeEnv::GetGC()
{
	return gc;
}

KImage *KRuntimeEnv::GetBackImage()
{
	return back_image;
}

int	KRuntimeEnv::GetScreenWidth()
{
	return width;
}

int KRuntimeEnv::GetScreenHeight()
{
	return height;
}

/****************************************************************************
  �_�u���o�b�t�@�̃o�b�N�C���[�W���X�N���[���ɓ]������
****************************************************************************/

void KRuntimeEnv::Update(int x, int y, int w, int h)
{
	if(gc == NULL)
		return;
	gc->DrawImage(back_image, x, y, w, h, x, y);
}

void KRuntimeEnv::Flip()
{
	Update(0, 0, width, height);
}

/****************************************************************************
  �I���v��
****************************************************************************/

void KRuntimeEnv::PostQuitRequest()
{
	need_quit;
}

bool KRuntimeEnv::IsQuitRequestPosted()
{
	return need_quit;
}
