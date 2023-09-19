/*****************************************************************************
 image_draw.cpp
 イメージクラス・描画

 Suika Studio (core/80x86)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/graphic.h"
#include "klib/debug.h"
#include <memory.h>

bool KImage::Clip(
	KImage *src,
	int& dst_x,
	int& dst_y,
	int& src_x,
	int& src_y,
	int& w,
	int& h)
{
	if(dst_x >= width || dst_y >= height ||
	   src_x >= src->width || src_y >= src->height)
		return false;	// 領域外

	// 転送元領域のクリップ
	if(src_x < 0) {	// 左側のはみ出しを修正
		w += src_x;
		dst_x -= src_x;
		src_x = 0;
	}
	if(src_y < 0) {
		h += src_y;	// 上側のはみ出しを修正
		dst_y -= src_y;
		src_y = 0;
	}
	if(src_x + w > src->width)	// 右側のはみ出しを修正
		w = src->width - src_x;
	if(src_y + h > src->height)	// 下側のはみ出しを修正
		h = src->height - src_y;

	// 転送先領域のクリップ
	if(dst_x < 0) {	// 左側のはみ出しを修正
		w += dst_x;
		src_x -= dst_x;
		dst_x = 0;
	}
	if(dst_y < 0) {	// 上側のはみ出しを修正
		h += dst_y;
		src_y -= dst_y;
		dst_y = 0;
	}
	if(dst_x + w > width)	// 右側のはみ出しを修正
		w -= dst_x + w - width;
	if(dst_y + h > height)	// 下側のはみ出しを修正
		h -= dst_y + h - height;

	// 領域外か？
	if(w < 0)
		return false;
	if( h < 0 )
		return false;
	return true;
}


void KImage::DrawImage(
	KImage	*src,
	int		dst_x,
	int		dst_y,
	uint8	alpha,	// = 255
	RGBA32	color,	// = IMG_MAKE_RGBA(0,0,0,0)
	int		src_x,	// = -1
	int		src_y,	// = -1
	int		w,		// = -1
	int		h )		// = -1
{
	// デフォルト値を設定する
	if(h == -1)
		h = src->height;
	if(w == -1)
		w = src->width;
	if(src_y == -1)
		src_y = 0;
	if(src_x == -1)
		src_x = 0;

	if(!Clip(src, dst_x, dst_y, src_x, src_y, w, h))
		return;

	bool is_alpha = src->IsAlphaEnabled();

	// 描画関数を選択する
	if( IMG_GET_A(color) == 0 ) {
		if(alpha == 255) {
			// Normal DrawImage()
			if(is_alpha)
				KImage_DrawImage_RGBA(dst_x, dst_y, w, h, src_x, src_y, src, this);
			else
				KImage_DrawImage_RGB(dst_x, dst_y, w, h, src_x, src_y, src, this);
		} else {
			// Alpha version of DrawImage()
			if(is_alpha)
				KImage_DrawImageAlpha_RGBA(alpha, dst_x, dst_y, w, h, src_x, src_y, src, this);
			else
				KImage_DrawImageAlpha_RGB(alpha, dst_x, dst_y, w, h, src_x, src_y, src, this);
		}
	} else {
		if( alpha == 255 ) {
			// Color version DrawImage()
			if( is_alpha )
				KImage_DrawImageColor_RGBA(color, dst_x, dst_y, w, h, src_x, src_y, src, this );
			else
				KImage_DrawImageColor_RGB(color, dst_x, dst_y, w, h, src_x, src_y, src, this );
		} else {
			// Color and Alpha version of DrawImage()
			if( is_alpha )
				KImage_DrawImageColorAlpha_RGBA(color, alpha, dst_x, dst_y, w, h, src_x, src_y, src, this);
			else
				KImage_DrawImageColorAlpha_RGB(color, alpha, dst_x, dst_y, w, h, src_x, src_y, src, this);
		}
	}
}

void KImage::DrawPolygon(
	KImage		*src,
	KEdgeBuffer	*edge_buf,
	uint8		alpha,	// = 255
	RGBA32		color )	// = IMG_MAKE_RGBA(0,0,0,0)
{
	bool is_alpha = src->IsAlphaEnabled();

	// 描画関数を選択する
	if(IMG_GET_A(color) == 0) {
		if(alpha == 255) {
			// Normal DrawPolygon()
			if(is_alpha)
				KImage_DrawPolygon_RGBA(edge_buf, src, this);
			else
				KImage_DrawPolygon_RGB(edge_buf, src, this);
		} else {
			// Alpha version of DrawPolygon()
			if(is_alpha)
				KImage_DrawPolygonAlpha_RGBA(alpha, edge_buf, src, this);
			else
				KImage_DrawPolygonAlpha_RGB(alpha, edge_buf, src, this);
		}
	} else {
		if(alpha == 255) {
			// Color version of DrawPolygon
			if(is_alpha)
				KImage_DrawPolygonColor_RGBA(color, edge_buf, src, this);
			else
				KImage_DrawPolygonColor_RGB(color, edge_buf, src, this);
		} else {
			// Color Alpha version of DrawPolygon
			if(is_alpha)
				KImage_DrawPolygonColorAlpha_RGBA(color, alpha, edge_buf, src, this);
			else
				KImage_DrawPolygonColorAlpha_RGB(color, alpha, edge_buf, src, this);
		}
	}
}

void KImage::Clear()
{
	::memset(image, 0, width*height*4);
}

void KImage::White()
{
	::memset(image, 0xff, width*height*4);
}

void KImage::Fill(RGBA32 rgb)
{
#ifdef	_WIN32

	int cnt = width * height;
	uint32 *dst = image;
	_asm {
		pusha
		mov	eax, rgb;
		mov	edi, dst;
		mov	ecx, cnt;
		cld
		rep stosd
		popa
	}

#else

	K_ASSERT(false);	// for debug

	int cnt = width * height * 4;
	uint32 pix = rgb;
	uint32 *dst = image;
	for(int i = 0; i < cnt; i++)
		*image++ = pix;

#endif
}

#ifdef _WIN32

void KImage::DrawGlyphGrayscale(
	uint32 dst_x,
	uint32 dst_y,
	uint32 src_w,
	uint32 src_h,
	RGBA32 rgb,
	uint8 *src_img)
{
	KImage_DrawGlyphGrayscale(dst_x, dst_y, src_w, src_h, rgb, src_img, this);
}

#endif
