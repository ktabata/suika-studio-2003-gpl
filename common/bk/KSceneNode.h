/*****************************************************************************
 KSceneNode.h
 シーンノード（ツリーノード）

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KSCENENODE_H_
#define _KSCENENODE_H_

#include "KRuntimeEnv.h"
#include "klib/io.h"
#include "klib/debug.h"

class KRuntimeEnv;

/********************************
  命令ノードタイプ
********************************/
#define KSCENENODE_MSG			(0)
#define KSCENENODE_BK			(1)
#define KSCENENODE_CHARA		(2)
#define KSCENENODE_SOUND		(3)
#define KSCENENODE_CALL			(4)
#define KSCENENODE_SELECT		(5)
#define KSCENENODE_SELECTGROUP	(6)
#define KSCENENODE_BLOCKSTART	(7)
#define KSCENENODE_BLOCKEND		(8)
#define KSCENENODE_SETVAR		(9)
#define KSCENENODE_CHECKVAR		(10)
#define KSCENENODE_IFTRUE		(11)
#define KSCENENODE_IFFALSE		(12)
#define KSCENENODE_LOOP			(13)
#define KSCENENODE_BREAK		(14)
#define KSCENENODE_EXTENDED		(15)
#define KSCENENODE_COMMENT		(16)

/********************************
  変数判定の条件
********************************/
#define CHECKVAR_COND_FIRST		(0)
#define CHECKVAR_COND_EQ		(0)		// =  equal
#define CHECKVAR_COND_NOTEQ		(1)		// != not equal
#define CHECKVAR_COND_GT		(2)		// >  greater than
#define CHECKVAR_COND_EQGT		(3)		// >= equal or greater than
#define CHECKVAR_COND_LT		(4)		// <  lesser than
#define CHECKVAR_COND_EQLT		(5)		// <= equal or lesser than
#define CHECKVAR_COND_LAST		(5)

/********************************
  実行結果
********************************/
#define EXEC_SUCCESS			(0)		// 正常に実行できた
#define EXEC_ERROR				(1)		// エラーが発生した（実行は継続可能）
#define EXEC_CRITICALERROR		(2)		// 致命的なエラーが発生した（実行は継続不可）
#define EXEC_END				(3)		// 最後まで実行した
#define EXEC_QUITREQUESTED		(4)		// 終了要求で中断した

/********************************
  値の正当性チェック
********************************/
#define KSCENENODE_IS_VALID_TYPE(type) \
					(type>=KSCENENODE_MSG&&type<=KSCENENODE_COMMENT)
#define KSCENENODE_IS_INSERTABLE_TYPE(type) \
					(type!=KSCENENODE_SELECTGROUP&&\
					 type!=KSCENENODE_BLOCKSTART&&\
					 type!=KSCENENODE_BLOCKEND&&\
					 type!=KSCENENODE_IFTRUE&&\
					 type!=KSCENENODE_IFFALSE)
#define KSCENENODE_IS_DELETABLE_TYPE	KSCENENODE_IS_INSERTABLE_TYPE
#define CHECKVAR_IS_VALID_CONDITION(cond) \
					(cond>=CHECKVAR_COND_EQ&&cond<=CHECKVAR_COND_EQLT)

/****************************************************************************
	KSceneNode
	　シーンに含まれる命令ノードの基本クラスです。
****************************************************************************/
class KSceneNode : public KTreeNode {
public:
	inline KSceneNode(int type) : instr_type(type) { }
	virtual ~KSceneNode() { }
	static KSceneNode *CreateInstance(int type);
	virtual KTreeNode *CreateClone(void);

	inline int GetType(void) const { return instr_type; }
	virtual	KString GetExplanation(RGBA32& color) = 0;
	static KString GetInstrString(int type);

	bool Save(KOutputStream * os);
	static KSceneNode *Load(KInputStream * is);

	void	DebugOutputTree(int indent = 0);
	virtual int	Execute(KRuntimeEnv *env);

protected:
	static KSceneNode *CreateInstanceNoInit(int type);

	bool	SaveChildren(KOutputStream * os);
	bool	LoadChildren(KInputStream * is);

	virtual void InsertDefaultChildren();
	virtual bool SaveNode(KOutputStream * os);
	virtual bool LoadNode(KInputStream * is);


private:
	int	instr_type;
};

/****************************************************************************
	KSceneNodeMsg
	　「文章を表示する」命令のノードです。
****************************************************************************/
class KSceneNodeMsg : public KSceneNode {
public:
	inline KSceneNodeMsg() : KSceneNode(KSCENENODE_MSG) { }
	~KSceneNodeMsg() { }

	inline KString GetSpeaker(void) const { return speaker; }
	inline KString GetMsg(void) const { return msg; }
	inline KString GetVoiceFileName(void) const { return voice_fname; };

	inline void SetSpeaker(KString speaker) { this->speaker = speaker; }
	inline void SetMsg(KString msg) { this->msg = msg; }
	inline void SetVoiceFileName(KString fname) { voice_fname = fname; };

	bool	SaveNode(KOutputStream * os);
	bool	LoadNode(KInputStream * is);
	KString	GetExplanation(RGBA32& color);

	int	Execute(KRuntimeEnv *env);

private:
	KString	speaker;
	KString msg;
	KString voice_fname;
};

/****************************************************************************
	KSceneNodeCheckVar
	　「変数の値を判定する」命令のノードです。
****************************************************************************/
class KSceneNodeCheckVar : public KSceneNode {
public:
	KSceneNodeCheckVar();

	inline KString	GetVarName(void) const { return var_name; }
	inline int		GetCondType(void) const { return cond_type; }
	inline KString	GetValue(void) const { return value; };

	inline void SetVarName(KString var) { var_name = var; }
	inline void SetCondType(int type) { cond_type = type; }
	inline void SetValue(KString val) { value = val; };

	KString	GetExplanation(RGBA32& color);
	static KString GetCondString(int type);

	int	Execute(KRuntimeEnv *env);

protected:
	void InsertDefaultChildren();
	bool SaveNode(KOutputStream * os);
	bool LoadNode(KInputStream * is);

private:
	KString	var_name;
	int		cond_type;
	KString	value;
};

/****************************************************************************
	KSceneNodeIfTrue
	　「変数の値を判定する」命令の子ノードで，
	　条件式の値が真のとき実行されるノードです。
****************************************************************************/
class KSceneNodeIfTrue : public KSceneNode {
public:
	KSceneNodeIfTrue() : KSceneNode(KSCENENODE_IFTRUE) {}
	KString GetExplanation(RGBA32& color);
};

/****************************************************************************
	KSceneNodeIfFalse
	　「変数の値を判定する」命令の子ノードで，
	　条件式の値が偽のとき実行されるノードです。
****************************************************************************/
class KSceneNodeIfFalse : public KSceneNode {
public:
	KSceneNodeIfFalse() : KSceneNode(KSCENENODE_IFFALSE) {}
	KString GetExplanation(RGBA32& color);
};

/****************************************************************************
	KSceneNodeBlockStart
	　「ブロック開始コメント」のクラスです。
****************************************************************************/
class KSceneNodeBlockStart : public KSceneNode {
public:
	KSceneNodeBlockStart() : KSceneNode(KSCENENODE_BLOCKSTART) {}
	KString GetExplanation(RGBA32& color);
};

/****************************************************************************
	KSceneNodeBlockEnd
	　「ブロック終了コメント」のクラスです。
****************************************************************************/
class KSceneNodeBlockEnd : public KSceneNode {
public:
	KSceneNodeBlockEnd() : KSceneNode(KSCENENODE_BLOCKEND) {}
	KString GetExplanation(RGBA32& color);
};

#endif
