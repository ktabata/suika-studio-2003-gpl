/*****************************************************************************
 config.h
 基本型宣言

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_CONFIG_H_
#define _KLIB_CONFIG_H_

typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;

typedef signed long		sint32;
typedef signed short	sint16;
typedef signed char		sint8;

typedef uint32	RGBA32;

struct KPOINT {
	int x, y;
};

struct KRECT {
	int left, top, right, bottom;
};

#define CHANGE_ORDER_2(s)	(((s&0xff)<<8) | (s>>8))
#define CHANGE_ORDER_4(l)	(((l&0xff)<<24) | ((l&0xff00)<<8) | ((l&0xff0000)>>8) | (l>>24))

// 数値のバイトオーダーをリトルエンディアンにする。
#define TO_LSB_FIRST_2(i)		i
#define TO_LSB_FIRST_4(i)		i

// 数値のバイトオーダーをビッグエンディアンにする。
#define TO_MSB_FIRST_2(s)		CHANGE_ORDER_2(s)
#define TO_MSB_FIRST_4(l)		CHANGE_ORDER_4(l)

// 数値がリトルエンディアンで格納されていることを明示する。
#define LSB_FIRST_OF_2(i)		i
#define LSB_FIRST_OF_4(i)		i

// 数値がビッグエンディアンで格納されていることを明示する。
#define MSB_FIRST_OF_2(s)		CHANGE_ORDER_2(s)
#define MSB_FIRST_OF_4(l)		CHANGE_ORDER_4(l)

typedef sint32 fixedpoint;

#define FIX_POWER			2048
#define FIX_ROUND			1024
#define fp_int2fix(x)		((x)*FIX_POWER)
#define fp_fix2int(x)		((x)/FIX_POWER)
#define fp_add(x,y)			((x)+(y))
#define fp_sub(x,y)			((x)-(y))
#define fp_mul(x,y)			((x)*(y)/FIX_POWER)
#define fp_div(x,y)			(((x)*FIX_POWER)/(y));

#endif
