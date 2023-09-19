/*****************************************************************************
 image_ext_c.h
 Ｃリンケージ用グラフィック関連

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_IMGAE_EXT_C_H_
#define _KLIB_IMGAE_EXT_C_H_

#include "klib/config.h"

/* アルファブレンド用の計算テーブル */
extern uint8 KImage_AlphaTable[256][256];

/* RGBAのバイト順を変更するには，次のマクロを修正してください。 */
#define IMG_MAKE_RGBA(r,g,b,a)	(b | (g<<8) | (r<<16) | (a<<24))
#define IMG_MAKE_RGB(r,g,b)		(b | (g<<8) | (r<<16))
#define IMG_MAKE_A(a)		(a<<24)
#define IMG_GET_B(rgb)		(rgb&0xff)
#define IMG_GET_G(rgb)		((rgb>>8)&0xff)
#define IMG_GET_R(rgb)		((rgb>>16)&0xff)
#define IMG_GET_A(rgb)		((rgb>>24)&0xff)

/* ＣリンケージでアクセスするためのKImageのデータ(KImageの基本クラス) */
struct KImageData {
	int		width;
	int		height;
	RGBA32	*image;
	int		bpl;
	int		is_alpha_enabled;
};

/* スキャンコンバージョンで使う無効値 */
#define SCBUF_MIN_INVALID	(fp_int2fix(65536))
#define SCBUF_MAX_INVALID	(fp_fix2int(-65536))

/* スキャンコンバージョンの１スキャンライン分のデータ */
struct KEdgeLineBuffer {
	fixedpoint	min;
	fixedpoint	min_tx;
	fixedpoint	min_ty;
	fixedpoint	max;
	fixedpoint	max_tx;
	fixedpoint	max_ty;
};

/* スキャンコンバージョンのエッジバッファ */
struct KEdgeBuffer {
	int lines_count;
	struct KEdgeLineBuffer *lines;
};

/*********************************
 イメージの転送関数
*********************************/

void KImage_DrawImage_RGB(int, int, int, int, int, int, struct KImageData *,
		struct KImageData *);
void KImage_DrawImage_RGBA(int, int, int, int, int, int, struct KImageData *,
		struct KImageData *);
void KImage_DrawImageAlpha_RGB(uint32, int, int, int, int, int, int,
		struct KImageData *, struct KImageData *);
void KImage_DrawImageAlpha_RGBA(uint32, int, int, int, int, int, int,
		struct KImageData *, struct KImageData *);
void KImage_DrawImageColor_RGB(uint32, int, int, int, int, int, int,
		struct KImageData *, struct KImageData *);
void KImage_DrawImageColor_RGBA(uint32, int, int, int, int, int, int,
		struct KImageData *, struct KImageData *);
void KImage_DrawImageColorAlpha_RGB(uint32, uint32, int, int, int, int, int,
		int, struct KImageData *, struct KImageData *);
void KImage_DrawImageColorAlpha_RGBA(uint32, uint32, int, int, int, int, int,
		int, struct KImageData *, struct KImageData *);
void KImage_DrawGlyphGrayscale(uint32, uint32, uint32, uint32, uint32, uint8 *,
		struct KImageData *);

/*********************************
 ポリゴンの描画関数
*********************************/

void KImage_DrawPolygon_RGB(struct KEdgeBuffer *, struct KImageData *,
		struct KImageData *);
void KImage_DrawPolygon_RGBA(struct KEdgeBuffer *, struct KImageData *,
		struct KImageData *);
void KImage_DrawPolygonAlpha_RGB(uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);
void KImage_DrawPolygonAlpha_RGBA(uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);
void KImage_DrawPolygonColor_RGB(uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);
void KImage_DrawPolygonColor_RGBA(uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);
void KImage_DrawPolygonColorAlpha_RGB(uint32, uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);
void KImage_DrawPolygonColorAlpha_RGBA(uint32, uint32, struct KEdgeBuffer *,
		struct KImageData *, struct KImageData *);

#endif
