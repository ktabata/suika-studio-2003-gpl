/*****************************************************************************
 KSceneDoc.cpp
 シーンドキュメント

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
    シーンドキュメント中の命令アイテム。子アイテムを持ち、ツリー構造となる。
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

	// シリアライズ
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
    KSceneDocItemのツリー中の要素に行番号でアクセスするための変換テーブルで
  用いる構造体です。
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
    シーンドキュメント。
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
