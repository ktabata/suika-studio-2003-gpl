/*****************************************************************************
 KSceneDoc.cpp
 �V�[���h�L�������g

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KSCENEDOC_H_
#define _KSCENEDOC_H_

#include "klib/array.h"
#include "klib/io.h"
#include "opconst.h"

/****************************************************************************
 KSceneDocItem
 ===========================================================================
    �V�[���h�L�������g���̖��߃A�C�e���B�q�A�C�e���������A�c���[�\���ƂȂ�B
****************************************************************************/
class KSceneDocItem {
	friend class KSceneDoc;
public:
	KSceneDocItem(int opecode);
	KSceneDocItem(KSceneDocItem& src);
	~KSceneDocItem();

	int	GetOpCode();
	KIntStrValue&	ParamAt(int index);

	void	SetDefaultValue();

	// �V���A���C�Y
	static KSceneDocItem *Load(KInputStream *is);
	bool Save(KOutputStream *os);

protected:
	int	opcode;
	KArray<KIntStrValue>	params_tbl;
	KArray<KSceneDocItem *>	children_tbl;
};

/****************************************************************************
 KSceneDocLineDesc
 ===========================================================================
    KSceneDocItem�̃c���[���̗v�f�ɍs�ԍ��ŃA�N�Z�X���邽�߂̕ϊ��e�[�u����
  �p����\���̂ł��B
****************************************************************************/
struct KSceneDocLineDesc {
	int		indent;
	KSceneDocItem	*item;

	KSceneDocLineDesc() { item = NULL; }
	KSceneDocLineDesc(int indent, KSceneDocItem *item) :
		indent(indent), item(item) { }
};

/****************************************************************************
 KSceneDoc
 ===========================================================================
    �V�[���h�L�������g�B
****************************************************************************/
class KSceneDoc {
public:
	KSceneDoc();
	~KSceneDoc();

	bool Save(KOutputStream *os);
	bool Load(KInputStream *is);

	bool	IsModified();
	bool	IsInsertablePos(int line);

	int		GetLinesCount();
	int		GetIndentAt(int line);
	KSceneDocItem *GetItemAt(int line);
	int		GetItemLines(int line);

	void	InsertAt(int line, KSceneDocItem *item);
	void	DeleteAt(int line);
	void	Move(int dst, int src);



private:
	KArray<KSceneDocLineDesc>	*line_desc_tbl;
	bool		is_modified;

	void	InsertChild(KSceneDocItem *item, int& line, int indent);
	int		GetParentLevelLineBefore(int line);
};

#endif
