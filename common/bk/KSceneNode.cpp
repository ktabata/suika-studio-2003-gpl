/*****************************************************************************
 KSceneNode.cpp
 シーンノード（ツリーノード）

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

	// 命令タイプを出力する
	if(!tos.PutInt((uint32)GetType()))
		return false;			// 書き込みに失敗

#ifdef _DEBUG
	RGBA32 color;
	char buf[1024];
	sprintf(buf, ", [%d:%s]", GetType(), (const char *)GetExplanation(color));
	OutputDebugString(buf);
#endif

	// タイプ別にセーブする
	if(!SaveNode(os))
		return false;			// 書き込みに失敗

	// 子ノードをセーブする
	return SaveChildren(os);
}

KSceneNode *KSceneNode::Load(KInputStream *is)
{
	KTypedInputStream tis(is);
	uint32	u;

	// 命令タイプを入力する
	if(!tis.GetInt(u))
		return NULL;			// 読み込みに失敗

	// ノードを生成する
	KSceneNode *node = CreateInstanceNoInit((int)u);
	assert(node != NULL);
	if(node == NULL)
		return false;			// 不正なファイル形式

	// タイプ別にロードする
	if(node->LoadNode(is)) {
		// 子ノードをロードする
		if(node->LoadChildren(is))
			return node;		// 成功
	}

	// 失敗した
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

	// 子ノードの数を出力する
	if(!tos.PutInt(children_cnt))
		return NULL;			// 書き込みに失敗

#ifdef _DEBUG
	OutputDebugString("{");
#endif

	for(int i = 0; i < children_cnt; i++) {
		assert(ch != NULL);		// リンクリスト関連のバグ
		if(ch == NULL)
			return false;

		if(!ch->Save(os))
			return false;		// 書き込みに失敗
		ch = (KSceneNode *)ch->GetNext();
	}

#ifdef _DEBUG
	OutputDebugString("}");
#endif

	return true;	// 成功
}

bool KSceneNode::LoadChildren(KInputStream *is)
{
	KTypedInputStream	tis(is);
	uint32	u;

	// 子ノードの数を入力する
	if(!tis.GetInt(u))
		return NULL;		// 読み込みに失敗
	int children_cnt = (int) u;

	// 最初の子ノードをロードする
	KSceneNode *ch;
	if(u > 0) {
		ch = Load(is);
		if(ch == NULL)
			return false;	// 読み込みに失敗
		InsertChild(ch);
	}

	// ２つ目以降の子ノードをロードする
	for(int i = 1; i < children_cnt; i++) {
		KSceneNode *node = Load(is);
		if(node == NULL)
			return false;	// 読み込みに失敗
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
		"文章を表示する", "背景を表示する", "キャラを表示する",
		"サウンドを再生する", "シーンファイルを呼び出す", "選択肢を表示する",
		"選択肢グループ", "blockstart", "blockend",
		"変数に値を設定する", "変数の値を判定する", "iftrue", "iffalse",
		"ループを挿入する", "ループから抜け出す", "拡張命令", "コメント"};
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
	// 自分の情報を出力する
	for(int i = 0; i < indent; i++)
		OutputDebugString("    ");
	RGBA32 color;
	char buf[1024];
	sprintf(buf, "[%d:%s]\r\n", GetType(), (const char *)GetExplanation(color));
	OutputDebugString(buf);

	// 子ノードの情報を出力する
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


	// 次の兄弟ノードの情報を出力する
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
	// ボイスありなら青，そうでなければ黒
	color = voice_fname.IsEmpty() ? IMG_MAKE_RGB(0,0,0) : IMG_MAKE_RGB(0,0,0xff);
//	color = voice_fname.IsEmpty() ? IMG_MAKE_RGB(0xff,0xff,0xff) : IMG_MAKE_RGB(0x80,0xf80,0xff);

	// 話者と文章をつなげる
	return (!speaker.IsEmpty() ? (speaker+"「") : "") + msg;
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
	// ↓ロード時これが余計である。

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
	return "変数の値を判定する <" + var_name + " " +
			GetCondString(cond_type) + " " + value + ">";
}

KString KSceneNodeCheckVar::GetCondString(int type)
{
	K_ASSERT(CHECKVAR_IS_VALID_CONDITION(type));
	const static char *str[] = {"＝","≠","＞","≧","＜","≦"};
	return str[type];
}

/***************************
 KSceneNodeIfTrue
***************************/

KString KSceneNodeIfTrue::GetExplanation(RGBA32& color)
{
	color = IMG_MAKE_RGB(0xff,0,0xff);
	return "条件に当てはまるとき";
}

/***************************
 KSceneNodeIfFalse
***************************/

KString KSceneNodeIfFalse::GetExplanation(RGBA32& color)
{
	color = IMG_MAKE_RGB(0xff,0,0xff);
	return "条件に当てはまらないとき";
}
