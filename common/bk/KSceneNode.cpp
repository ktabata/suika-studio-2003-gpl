/*****************************************************************************
 KSceneNode.cpp
 �V�[���m�[�h�i�c���[�m�[�h�j

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include <stddef.h>		// NULL
#include "KSceneNode.h"
#include "klib/io.h"
#include "klib/debug.h"

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

/***************************
 KSceneNode
***************************/

bool KSceneNode::Save(KOutputStream * os)
{
	KTypedOutputStream tos(os);

	// ���߃^�C�v���o�͂���
	if(!tos.PutInt((uint32)GetType()))
		return false;			// �������݂Ɏ��s

#ifdef _DEBUG
	RGBA32 color;
	char buf[1024];
	sprintf(buf, ", [%d:%s]", GetType(), (const char *)GetExplanation(color));
	OutputDebugString(buf);
#endif

	// �^�C�v�ʂɃZ�[�u����
	if(!SaveNode(os))
		return false;			// �������݂Ɏ��s

	// �q�m�[�h���Z�[�u����
	return SaveChildren(os);
}

KSceneNode *KSceneNode::Load(KInputStream *is)
{
	KTypedInputStream tis(is);
	uint32	u;

	// ���߃^�C�v����͂���
	if(!tis.GetInt(u))
		return NULL;			// �ǂݍ��݂Ɏ��s

	// �m�[�h�𐶐�����
	KSceneNode *node = CreateInstanceNoInit((int)u);
	assert(node != NULL);
	if(node == NULL)
		return false;			// �s���ȃt�@�C���`��

	// �^�C�v�ʂɃ��[�h����
	if(node->LoadNode(is)) {
		// �q�m�[�h�����[�h����
		if(node->LoadChildren(is))
			return node;		// ����
	}

	// ���s����
	delete node;
	return NULL;
}

bool KSceneNode::SaveNode(KOutputStream *os) { return true; }
bool KSceneNode::LoadNode(KInputStream *is) { return true; }

bool KSceneNode::SaveChildren(KOutputStream *os)
{
	KTypedOutputStream	tos(os);
	KSceneNode	*ch = (KSceneNode *)GetChild();
	int	children_cnt = (ch != NULL) ? ch->GetSiblingsCount() : 0;

	// �q�m�[�h�̐����o�͂���
	if(!tos.PutInt(children_cnt))
		return NULL;			// �������݂Ɏ��s

#ifdef _DEBUG
	OutputDebugString("{");
#endif

	for(int i = 0; i < children_cnt; i++) {
		assert(ch != NULL);		// �����N���X�g�֘A�̃o�O
		if(ch == NULL)
			return false;

		if(!ch->Save(os))
			return false;		// �������݂Ɏ��s
		ch = (KSceneNode *)ch->GetNext();
	}

#ifdef _DEBUG
	OutputDebugString("}");
#endif

	return true;	// ����
}

bool KSceneNode::LoadChildren(KInputStream *is)
{
	KTypedInputStream	tis(is);
	uint32	u;

	// �q�m�[�h�̐�����͂���
	if(!tis.GetInt(u))
		return NULL;		// �ǂݍ��݂Ɏ��s
	int children_cnt = (int) u;

	// �ŏ��̎q�m�[�h�����[�h����
	KSceneNode *ch;
	if(u > 0) {
		ch = Load(is);
		if(ch == NULL)
			return false;	// �ǂݍ��݂Ɏ��s
		InsertChild(ch);
	}

	// �Q�ڈȍ~�̎q�m�[�h�����[�h����
	for(int i = 1; i < children_cnt; i++) {
		KSceneNode *node = Load(is);
		if(node == NULL)
			return false;	// �ǂݍ��݂Ɏ��s
		ch->InsertNext(node);
		ch = node;
	}
	return true;
}

KTreeNode *KSceneNode::CreateClone(void)
{
	KSceneNode *res = NULL;
	switch(instr_type) {
	case KSCENENODE_MSG:
		res = new KSceneNodeMsg((KSceneNodeMsg&)*this);
		break;
	}
	assert(res != NULL);
	return res;
}

KString KSceneNode::GetInstrString(int type)
{
	assert(type >= KSCENENODE_MSG && type <= KSCENENODE_COMMENT);

	static char *msg[] = {
		"���͂�\������", "�w�i��\������", "�L������\������",
		"�T�E���h���Đ�����", "�V�[���t�@�C�����Ăяo��", "�I������\������",
		"�I�����O���[�v", "blockstart", "blockend",
		"�ϐ��ɒl��ݒ肷��", "�ϐ��̒l�𔻒肷��", "iftrue", "iffalse",
		"���[�v��}������", "���[�v���甲���o��", "�g������", "�R�����g"};
	return msg[type];
}

KSceneNode *KSceneNode::CreateInstance(int type)
{
	KSceneNode *node = CreateInstanceNoInit(type);
	if(node == NULL)
		return NULL;
	node->InsertDefaultChildren();
	return node;
}

KSceneNode *KSceneNode::CreateInstanceNoInit(int type)
{
	switch(type)
	{
	case KSCENENODE_MSG:		return new KSceneNodeMsg();
/*	case KSCENENODE_BK:			return;
	case KSCENENODE_CHARA:		return;
	case KSCENENODE_SOUND:		return;
	case KSCENENODE_SELECT:		return;
*/	case KSCENENODE_BLOCKSTART:	return new KSceneNodeBlockStart();
	case KSCENENODE_BLOCKEND:	return new KSceneNodeBlockEnd();
/*	case KSCENENODE_SETVAR:		return;
*/
	case KSCENENODE_CHECKVAR:	return new KSceneNodeCheckVar();
	case KSCENENODE_IFTRUE:		return new KSceneNodeIfTrue();
	case KSCENENODE_IFFALSE:	return new KSceneNodeIfFalse();
/*	case KSCENENODE_LOOP:		return;
	case KSCENENODE_BREAK:		return;
	case KSCENENODE_EXTENDED:	return;
	case KSCENENODE_COMMENT:	return;
*/	}
	return NULL;	// type error
}

void KSceneNode::InsertDefaultChildren() { }

void KSceneNode::DebugOutputTree(int indent /* = 0 */)
{
	// �����̏����o�͂���
	for(int i = 0; i < indent; i++)
		OutputDebugString("    ");
	RGBA32 color;
	char buf[1024];
	sprintf(buf, "[%d:%s]\r\n", GetType(), (const char *)GetExplanation(color));
	OutputDebugString(buf);

	// �q�m�[�h�̏����o�͂���
	KSceneNode *node = (KSceneNode *)GetChild();
	if(node != NULL) {
		for(i = 0; i < indent; i++)
			OutputDebugString("    ");
		OutputDebugString("{\r\n");

		node->DebugOutputTree(indent + 1);

		for(i = 0; i < indent; i++)
			OutputDebugString("    ");
		OutputDebugString("}\r\n");
	}


	// ���̌Z��m�[�h�̏����o�͂���
	node = (KSceneNode *)GetNext();
	if(node != NULL)
		node->DebugOutputTree(indent);
}

/***************************
 KSceneNodeMsg
***************************/

bool KSceneNodeMsg::SaveNode(KOutputStream * os)
{
	KTypedOutputStream tos(os);

	if(!tos.PutString(speaker))
		return false;
	if(!tos.PutString(voice_fname))
		return false;
	return tos.PutString(msg);
}

bool KSceneNodeMsg::LoadNode(KInputStream * is)
{
	KTypedInputStream tis(is);

	if(!tis.GetString(speaker))
		return false;
	if(!tis.GetString(voice_fname))
		return false;
	return tis.GetString(msg);
}

KString KSceneNodeMsg::GetExplanation(RGBA32& color)
{
	// �{�C�X����Ȃ�C�����łȂ���΍�
	color = voice_fname.IsEmpty() ? IMG_MAKE_RGB(0,0,0) : IMG_MAKE_RGB(0,0,0xff);
//	color = voice_fname.IsEmpty() ? IMG_MAKE_RGB(0xff,0xff,0xff) : IMG_MAKE_RGB(0x80,0xf80,0xff);

	// �b�҂ƕ��͂��Ȃ���
	return (!speaker.IsEmpty() ? (speaker+"�u") : "") + msg;
}

/***************************
 KSceneNodeBlockStart
***************************/

KString KSceneNodeBlockStart::GetExplanation(RGBA32& color)
{
	return "";
}

/***************************
 KSceneNodeBlockEnd
***************************/

KString KSceneNodeBlockEnd::GetExplanation(RGBA32& color)
{
	return "";
}

/***************************
 KSceneNodeCheckVar
***************************/

KSceneNodeCheckVar::KSceneNodeCheckVar()
 : KSceneNode(KSCENENODE_CHECKVAR)
{
	// �����[�h�����ꂪ�]�v�ł���B

	cond_type = CHECKVAR_COND_EQ;
}

void KSceneNodeCheckVar::InsertDefaultChildren()
{
	KSceneNodeIfTrue *iftrue = new KSceneNodeIfTrue();
	KSceneNodeIfFalse *iffalse = new KSceneNodeIfFalse();
	iftrue->InsertChild( new KSceneNodeBlockEnd() );
	iftrue->InsertChild( new KSceneNodeBlockStart() );
	iffalse->InsertChild( new KSceneNodeBlockEnd() );
	iffalse->InsertChild( new KSceneNodeBlockStart() );
	InsertChild( iftrue );
	iftrue->InsertNext ( iffalse );
}

bool KSceneNodeCheckVar::SaveNode(KOutputStream * os)
{
	KTypedOutputStream tos(os);

	if(!tos.PutString(var_name))
		return false;
	if(!tos.PutInt((uint32)cond_type))
		return false;
	return tos.PutString(value);
}

bool KSceneNodeCheckVar::LoadNode(KInputStream * is)
{
	KTypedInputStream tis(is);
	uint32 u;

	if(!tis.GetString(var_name))
		return false;
	if(!tis.GetInt(u))
		return false;
	cond_type = (int) u;
	return tis.GetString(value);
}

KString KSceneNodeCheckVar::GetExplanation(RGBA32& color)
{
	color = IMG_MAKE_RGB(0xff,0x00,0x00);
	return "�ϐ��̒l�𔻒肷�� <" + var_name + " " +
			GetCondString(cond_type) + " " + value + ">";
}

KString KSceneNodeCheckVar::GetCondString(int type)
{
	K_ASSERT(CHECKVAR_IS_VALID_CONDITION(type));
	const static char *str[] = {"��","��","��","��","��","��"};
	return str[type];
}

/***************************
 KSceneNodeIfTrue
***************************/

KString KSceneNodeIfTrue::GetExplanation(RGBA32& color)
{
	color = IMG_MAKE_RGB(0xff,0,0xff);
	return "�����ɓ��Ă͂܂�Ƃ�";
}

/***************************
 KSceneNodeIfFalse
***************************/

KString KSceneNodeIfFalse::GetExplanation(RGBA32& color)
{
	color = IMG_MAKE_RGB(0xff,0,0xff);
	return "�����ɓ��Ă͂܂�Ȃ��Ƃ�";
}
