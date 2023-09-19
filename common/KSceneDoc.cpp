/*****************************************************************************
 KSceneDoc.cpp
 �V�[���h�L�������g

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneDoc.h"
#include "klib/io.h"
#include "klib/debug.h"

#define MAGIC	"scn"		// �V�[���t�@�C���̃V�O�l�`��

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

/****************************************************************************
 KSceneDocItem
****************************************************************************/

char *g_cvcond[] = {"��","��","��","��","��","��"};

KSceneDocItem::KSceneDocItem(int opcode)
{
	this->opcode = opcode;
	SetDefaultValue();
}

KSceneDocItem::KSceneDocItem(KSceneDocItem& src)
{
	int i;

	// copy opcode
	opcode = src.opcode;

	// copy parameters
	int cnt = src.params_tbl.GetItemsCount();
	for(i = 0; i < cnt; i++)
		params_tbl.InsertAt(i, src.params_tbl[i]);

	// copy children
	cnt = src.children_tbl.GetItemsCount();
	for(i = 0; i < cnt; i++)
		children_tbl.InsertAt(i, new KSceneDocItem(*src.children_tbl[i]));
}

KSceneDocItem::~KSceneDocItem()
{
	int cnt = children_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++)
		delete children_tbl[i];
}

int	KSceneDocItem::GetOpCode()
{
	return opcode;
}

KIntStrValue& KSceneDocItem::ParamAt(int index)
{
	return params_tbl[index];
}

void KSceneDocItem::SetDefaultValue()
{
	switch(opcode) {
	case SOP_MESSAGE:
		params_tbl.SetItemsCount(OPP_COUNT_MESSAGE);
		params_tbl[OPP_MESSAGE_SPEAKER] = (KString)"";
		params_tbl[OPP_MESSAGE_VOICE] = (KString)"";
		params_tbl[OPP_MESSAGE_MESSAGE] = (KString)"";
		params_tbl[OPP_MESSAGE_MSGBOX] = 0;
		break;
	case SOP_CHECKVAR:
		params_tbl.SetItemsCount(OPP_COUNT_CHECKVAR);
		params_tbl[OPP_CHECKVAR_VARIABLE] = (KString)"";
		params_tbl[OPP_CHECKVAR_CONDITION] = 0;
		params_tbl[OPP_CHECKVAR_VALUE] = (KString)"";
		children_tbl.InsertAt(0, new KSceneDocItem(SOP_IFTRUE));
		children_tbl.InsertAt(1, new KSceneDocItem(SOP_IFFALSE));
		break;
	case SOP_IFTRUE:
	case SOP_IFFALSE:
		children_tbl.InsertAt(0, new KSceneDocItem(SOP_BLOCKEND));
		break;
	}
}

KSceneDocItem *KSceneDocItem::Load(KInputStream *is)
{
	return new KSceneDocItem(0);
}

bool KSceneDocItem::Save(KOutputStream *os)
{
	return true;
}

/****************************************************************************
 KSceneDoc
****************************************************************************/

KSceneDoc::KSceneDoc()
{
	is_modified = false;
	line_desc_tbl = new KArray<KSceneDocLineDesc>();
}

KSceneDoc::~KSceneDoc()
{
	RELEASE(line_desc_tbl);
}

bool KSceneDoc::Save(KOutputStream *os)
{
	KTypedOutputStream tos(os);

	// �}�W�b�N���o�͂���
	if(!tos.PutString(MAGIC))
		return false;			// �������݂Ɏ��s

	// �g�b�v���x���̃A�C�e�������o�͂���
	int cnt = line_desc_tbl->GetItemsCount();
	if(!tos.PutInt((uint32)cnt))
		return false;			// �������݂Ɏ��s

	// �g�b�v���x���̃m�[�h���o�͂���
	for(int i = 0; i < cnt; i++) {
		KSceneDocLineDesc& desc = line_desc_tbl->At(i);
		if(desc.indent != 0)
			continue;
		if(!desc.item->Save(os))
			return false;		// �������݂Ɏ��s
	}

	is_modified = false;
	return true;	// ����
}

bool KSceneDoc::Load(KInputStream *is)
{
	KTypedInputStream tis(is);
	uint32 u;

	// �}�W�b�N����͂���
	KString magic;
	if(!tis.GetString(magic))
		return false;			// �ǂݍ��݂Ɏ��s
	if(!magic.Equals(MAGIC))
		return false;			// �}�W�b�N�s��

	// �g�b�v���x���̃A�C�e������ǂݍ���
	if(!tis.GetInt(u))
		return false;			// �ǂݍ��݂Ɏ��s
	int cnt = (int) u;

	// �g�b�v���x���̃m�[�h����͂���
	KArray<KSceneDocLineDesc>	*old_tbl = line_desc_tbl;
	line_desc_tbl = new KArray<KSceneDocLineDesc>();
	bool success = true;
	for(int i = 0; i < cnt; i++) {
		KSceneDocItem *item = KSceneDocItem::Load(is);
		if(item == NULL) {		// �ǂݍ��݂Ɏ��s
			delete line_desc_tbl;
			line_desc_tbl = old_tbl;
			return false;
		}
		InsertAt(line_desc_tbl->GetItemsCount(), item);
	}

	delete old_tbl;
	is_modified = false;
	return true;
}

bool KSceneDoc::IsModified()
{
	return is_modified;
}

bool KSceneDoc::IsInsertablePos(int line)
{
	K_ASSERT(line >= 0 && line <= line_desc_tbl->GetItemsCount());

	if(line == 0 || line == line_desc_tbl->GetItemsCount())
		return true;

	switch(GetItemAt(line)->GetOpCode()) {
	case SOP_IFTRUE:
	case SOP_IFFALSE:
		return false;
	}

	return true;
}

int	KSceneDoc::GetLinesCount()
{
	int res = line_desc_tbl->GetItemsCount();
	K_ASSERT(res >= 0);
	return res;
}

int KSceneDoc::GetIndentAt(int line)
{
	K_ASSERT(line >= 0 && line <= line_desc_tbl->GetItemsCount());
	int indent = line_desc_tbl->At(line).indent;
	K_ASSERT(indent >= 0);
	return indent;
}

KSceneDocItem *KSceneDoc::GetItemAt(int line)
{
	K_ASSERT(line >= 0 && line <= line_desc_tbl->GetItemsCount());
	KSceneDocItem *item = line_desc_tbl->At(line).item;
	K_ASSERT(item != NULL);
	return item;
}

int	KSceneDoc::GetItemLines(int line)
{
	int indent = GetIndentAt(line);
	int cnt = GetLinesCount();
	for(int i = line+1, res = 1; i < cnt; i++, res++) {
		if(GetIndentAt(i) == indent)
			return res;
	}
	return cnt - line;
}

void KSceneDoc::InsertAt(int line, KSceneDocItem *item)
{
	K_ASSERT(item != NULL);
	K_ASSERT(IsInsertablePos(line));
	int cnt = line_desc_tbl->GetItemsCount();
	K_ASSERT(line >= 0 && line <= cnt);

	int indent = 0;
	bool toplevel = false;
	if(line == 0 || line == cnt)
		toplevel = true;
	else if( (indent = GetIndentAt(line)) == 0 )
			toplevel = true;

	// �s�e�[�u���ɑ}������
	line_desc_tbl->InsertAt(line, KSceneDocLineDesc(indent, item));

	if(!toplevel) {
		// �c���[�ɑ}������
		int parent = GetParentLevelLineBefore(line);
		line_desc_tbl->At(parent).item->children_tbl.InsertAt(line-parent-1, item);
	}

	// �q�m�[�h������΍s�ɒǉ�����
	InsertChild(item, line, GetIndentAt(line));

	is_modified = true;
}

void KSceneDoc::InsertChild(KSceneDocItem *item, int& line, int indent)
{
	int cnt = item->children_tbl.GetItemsCount();
	for(int i = 0; i < cnt; i++) {
		KSceneDocItem *p = item->children_tbl[i];
		line_desc_tbl->InsertAt(++line, KSceneDocLineDesc(indent+1, p));
		InsertChild(p, line, indent+1);
	}
}

void KSceneDoc::DeleteAt(int line)
{
	int lines = GetItemLines(line);

	// �e�A�C�e���̎q�e�[�u������폜����
	if(GetIndentAt(line) != 0) {
		int parent = GetParentLevelLineBefore(line);
		K_ASSERT(parent != -1);
		line_desc_tbl->At(parent).item->children_tbl.DeleteAt(line-parent-1);
	}

	// �A�C�e�����폜����
	delete line_desc_tbl->At(line).item;

	// �s���폜����
	for(int i = 0; i < lines; i++)
		line_desc_tbl->DeleteAt(line);
}

void KSceneDoc::Move(int dst, int src)
{
	KSceneDocItem *item = GetItemAt(src);
	int lines = GetItemLines(src);

	// �ړ����̍s���폜����
	for(int i = 0; i < lines; i++)
		line_desc_tbl->DeleteAt(src);

	// �ړ���ɑ}������
	InsertAt((src<dst) ? dst-lines : dst, item);
}

int KSceneDoc::GetParentLevelLineBefore(int line)
{
	int indent = GetIndentAt(line);
	for(int i = line-1; i >= 0; i--) {
		if(GetIndentAt(i) == indent-1)
			return i;
	}
	return -1;
}
