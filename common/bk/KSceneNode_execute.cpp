/*****************************************************************************
 KSceneNode_execute.cpp
 KSceneNode�Ƃ��̔h���N���X��Execute()�����t�@�C��

 Suika Studio (Core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "KSceneNode.h"

int KSceneNode::Execute(KRuntimeEnv *env)
{
	return EXEC_SUCCESS;
}

int KSceneNodeMsg::Execute(KRuntimeEnv *env)
{
	return EXEC_SUCCESS;
}

int KSceneNodeCheckVar::Execute(KRuntimeEnv *env)
{
	return EXEC_SUCCESS;
}
