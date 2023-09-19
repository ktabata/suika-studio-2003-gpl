/*****************************************************************************
 debug.h
 デバッグ用マクロ定義

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_DEBUG_H_
#define _KLIB_DEBUG_H_

#ifdef _WIN32
	#include <afxcmn.h>
	#define new DEBUG_NEW
#endif

#include <assert.h>
#define K_ASSERT(p)		assert(p)

#endif
