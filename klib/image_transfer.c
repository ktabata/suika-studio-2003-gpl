/*****************************************************************************
 image_tranfer.cpp
 ÉCÉÅÅ[ÉWÉfÅ[É^ì]ëóÉRÅ[Éh

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include <memory.h>
#include "image_ext_c.h"

void
KImage_DrawImage_RGB(
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 src_width = src->width;
	uint32 dst_width = dst->width;
	uint32 *src_pixel = src->image + src_width * src_y + src_x;
	uint32 *dst_pixel = dst->image + dst_width * dst_y + dst_x;
	int line_bytes = w * 4;
	int y;

	for(y = 0; y < h; y++, src_pixel += src_width, dst_pixel += dst_width)
		memcpy(dst_pixel, src_pixel, line_bytes);
}

void
KImage_DrawImage_RGBA(
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 src_right_pad = src->width - src_x - w;
	uint32 dst_right_pad = dst->width - dst_x - w;
	register uint32 *src_pixel = src->image + src->width * src_y + src_x;
	register uint32 *dst_pixel = dst->image + dst->width * dst_y + dst_x;
	register int y, x;

	for(y = 0; y < h; y++,
			src_pixel += src_right_pad, dst_pixel += dst_right_pad,
			src_pixel += src_x, dst_pixel += dst_x)
	{
		for(x = 0; x < w; x++, src_pixel++, dst_pixel++) {
			uint32	src_pix = *src_pixel;
			uint32	dst_pix = *dst_pixel;
			uint32	r, g, b, a;
			uint8	*src_alpha_table, *dst_alpha_table;

			a = IMG_GET_A(src_pix);
			if(a == 0)
				continue;
			else if(a == 255) {
				r = IMG_GET_R(src_pix);
				g = IMG_GET_G(src_pix);
				b = IMG_GET_B(src_pix);
			} else {
				src_alpha_table = KImage_AlphaTable[a];
				dst_alpha_table = KImage_AlphaTable[255-a];
				r = src_alpha_table[IMG_GET_R(src_pix)] + dst_alpha_table[IMG_GET_R(dst_pix)];
				g = src_alpha_table[IMG_GET_G(src_pix)] + dst_alpha_table[IMG_GET_G(dst_pix)];
				b = src_alpha_table[IMG_GET_B(src_pix)] + dst_alpha_table[IMG_GET_B(dst_pix)];
			}
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawImageAlpha_RGB(
	uint32	alpha,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register int y, x;
	uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	uint8	*dst_alpha_table = KImage_AlphaTable[255-alpha];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b;

			r = src_alpha_table[ IMG_GET_R(src_pix) ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = src_alpha_table[ IMG_GET_G(src_pix) ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = src_alpha_table[ IMG_GET_B(src_pix) ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawImageAlpha_RGBA(
	uint32	alpha,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register int y, x;
	uint8	*src_alpha_table = KImage_AlphaTable[alpha];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A( src_pix );
			if( a == 0 )
				continue;
			a = src_alpha_table[a];

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ IMG_GET_R(src_pix) ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ IMG_GET_G(src_pix) ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ IMG_GET_B(src_pix) ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawImageColor_RGB(
	uint32	color,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*color_alpha_table = KImage_AlphaTable[ca];
	register int y, x;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b;
			r = color_alpha_table[IMG_GET_R(src_pix)] + cr;
			g = color_alpha_table[IMG_GET_G(src_pix)] + cg;
			b = color_alpha_table[IMG_GET_B(src_pix)] + cb;
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawImageColor_RGBA(
	uint32	color,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*color_alpha_table = KImage_AlphaTable[ca];
	register int y, x;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A( src_pix );
			if( a == 0 )
				continue;

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ color_alpha_table[IMG_GET_R(src_pix)] + cr ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ color_alpha_table[IMG_GET_G(src_pix)] + cg ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ color_alpha_table[IMG_GET_B(src_pix)] + cb ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawImageColorAlpha_RGB(
	uint32	color,
	uint32	alpha,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*color_alpha_table = KImage_AlphaTable[ca];
	register uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	register uint8	*dst_alpha_table = KImage_AlphaTable[255-alpha];
	register int y, x;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b;
			r = src_alpha_table[ color_alpha_table[IMG_GET_R(src_pix)] + cr ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = src_alpha_table[ color_alpha_table[IMG_GET_G(src_pix)] + cg ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = src_alpha_table[ color_alpha_table[IMG_GET_B(src_pix)] + cb ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawImageColorAlpha_RGBA(
	uint32	color,
	uint32	alpha,
	int	dst_x,
	int	dst_y,
	int	w,
	int	h,
	int	src_x,
	int	src_y,
	struct KImageData *src,
	struct KImageData *dst)
{
	uint32 dst_bpl = dst->bpl;
	uint32 src_bpl = src->bpl;
	uint32 src_left_pad = src_x * 4;
	uint32 src_right_pad = src_bpl - (src_x + w) * 4;
	uint32 dst_left_pad = dst_x * 4;
	uint32 dst_right_pad = dst_bpl - (dst_x + w) * 4;
	register uint32 src_pixel = (uint32)(src->image) + src_bpl * src_y;
	register uint32 dst_pixel = (uint32)(dst->image) + dst_bpl * dst_y;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*color_alpha_table = KImage_AlphaTable[ca];
	register uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	register int y, x;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];

	for( y = 0; y < h; y++ ) {
		src_pixel += src_left_pad;
		dst_pixel += dst_left_pad;

		for( x = 0; x < w; x++, src_pixel += 4, dst_pixel += 4 ) {
			uint32	src_pix = *(uint32 *)src_pixel;
			uint32	dst_pix = *(uint32 *)dst_pixel;
			uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A( src_pix );
			if( a == 0 )
				continue;
			a = src_alpha_table[a];

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ color_alpha_table[IMG_GET_R(src_pix)] + cr ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ color_alpha_table[IMG_GET_G(src_pix)] + cg ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ color_alpha_table[IMG_GET_B(src_pix)] + cb ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*(uint32 *)dst_pixel = IMG_MAKE_RGB(r,g,b);
		}

		src_pixel += src_right_pad;
		dst_pixel += dst_right_pad;
	}
}

void
KImage_DrawPolygon_RGB(
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32 *dst_img = dst->image;
	uint32 dst_bpl = dst->bpl;
	uint32 *src_img = src->image;
	uint32 src_bpl = src->bpl;
	register int i, j;

	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		register uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ )
			*dst = *(uint32 *)((uint32)src_img + src_bpl * fp_fix2int(ty) + fp_fix2int(tx) * 4);
	}
}

void
KImage_DrawPolygon_RGBA(
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32 *dst_img = dst->image;
	uint32 dst_bpl = dst->bpl;
	uint32 *src_img = src->image;
	uint32 src_bpl = src->bpl;
	register int i, j;

	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		register uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			uint32	src_pix = *(uint32 *)((uint32)src_img + src_bpl * fp_fix2int(ty) + fp_fix2int(tx) * 4);
			uint32	dst_pix = *dst;
			uint32	r, g, b, a;
			uint8	*src_alpha_table, *dst_alpha_table;

			a = IMG_GET_A(src_pix);
			if( a == 0 )
				continue;
/*			else if( a == 255 ) {
				r = src_alpha_table[ IMG_GET_R(src_pix) ];
				g = src_alpha_table[ IMG_GET_G(src_pix) ];
				b = src_alpha_table[ IMG_GET_B(src_pix) ];
			} else {
*/				src_alpha_table = KImage_AlphaTable[a];
				dst_alpha_table = KImage_AlphaTable[255-a];
				r = src_alpha_table[ IMG_GET_R(src_pix) ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
				g = src_alpha_table[ IMG_GET_G(src_pix) ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
				b = src_alpha_table[ IMG_GET_B(src_pix) ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
//			}
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonAlpha_RGB(
	uint32	alpha,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32 *dst_img = dst->image;
	uint32 dst_bpl = dst->bpl;
	uint32 *src_img = src->image;
	uint32 src_bpl = src->bpl;
	uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	uint8	*dst_alpha_table = KImage_AlphaTable[255-alpha];
	int i, j;

	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			uint32	src_pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			uint32	dst_pix = *dst;
			uint32	r, g, b;

			r = src_alpha_table[ IMG_GET_R(src_pix) ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = src_alpha_table[ IMG_GET_G(src_pix) ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = src_alpha_table[ IMG_GET_B(src_pix) ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonAlpha_RGBA(
	uint32	alpha,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32 *dst_img = dst->image;
	uint32 dst_bpl = dst->bpl;
	uint32 *src_img = src->image;
	uint32 src_bpl = src->bpl;
	uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	int i, j;

	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			uint32	src_pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			uint32	dst_pix = *dst;
			uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A(src_pix);
			if( a == 0 )
				continue;
			a = src_alpha_table[a];

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ IMG_GET_R(src_pix) ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ IMG_GET_G(src_pix) ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ IMG_GET_B(src_pix) ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonColor_RGB(
	uint32	color,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32	*dst_img = dst->image;
	uint32	dst_bpl = dst->bpl;
	uint32	*src_img = src->image;
	uint32	src_bpl = src->bpl;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*alpha_table = KImage_AlphaTable[ca];
	register int i, j;

	cr = alpha_table[cr];
	cg = alpha_table[cg];
	cb = alpha_table[cb];
	alpha_table = KImage_AlphaTable[255-ca];
 
	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			register uint32	pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			register uint32	r, g, b;

			r = alpha_table[IMG_GET_R(pix)] + cr;
			g = alpha_table[IMG_GET_G(pix)] + cg;
			b = alpha_table[IMG_GET_B(pix)] + cb;
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonColor_RGBA(
	uint32	color,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32	*dst_img = dst->image;
	uint32	dst_bpl = dst->bpl;
	uint32	*src_img = src->image;
	uint32	src_bpl = src->bpl;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*src_alpha_table = KImage_AlphaTable[ca];
	register int i, j;

	cr = src_alpha_table[cr];
	cg = src_alpha_table[cg];
	cb = src_alpha_table[cb];
	src_alpha_table = KImage_AlphaTable[255-ca];
 
	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			register uint32	src_pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			register uint32	dst_pix = *dst;
			register uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A(src_pix);
			if( a == 0 )
				continue;

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ src_alpha_table[IMG_GET_R(src_pix)] + cr ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ src_alpha_table[IMG_GET_G(src_pix)] + cg ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ src_alpha_table[IMG_GET_B(src_pix)] + cb ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonColorAlpha_RGB(
	uint32	color,
	uint32	alpha,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32	*dst_img = dst->image;
	uint32	dst_bpl = dst->bpl;
	uint32	*src_img = src->image;
	uint32	src_bpl = src->bpl;
	register uint32	cr = IMG_GET_R( color );
	register uint32	cg = IMG_GET_G( color );
	register uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	register uint8	*alpha_table = KImage_AlphaTable[alpha];
	register uint8	*color_alpha_table = KImage_AlphaTable[ca];
	register int i, j;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];
 
	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		register uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			uint32	pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			uint32	r, g, b;

			r = alpha_table[ color_alpha_table[IMG_GET_R(pix)] + cr ];
			g = alpha_table[ color_alpha_table[IMG_GET_G(pix)] + cg ];
			b = alpha_table[ color_alpha_table[IMG_GET_B(pix)] + cb ];
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

void
KImage_DrawPolygonColorAlpha_RGBA(
	uint32	color,
	uint32	alpha,
	struct KEdgeBuffer *edge_buf,
	struct KImageData *src,
	struct KImageData *dst)
{
	struct KEdgeLineBuffer *lines = edge_buf->lines;
	uint32	*dst_img = dst->image;
	uint32	dst_bpl = dst->bpl;
	uint32	*src_img = src->image;
	uint32	src_bpl = src->bpl;
	uint32	cr = IMG_GET_R( color );
	uint32	cg = IMG_GET_G( color );
	uint32	cb = IMG_GET_B( color );
	uint32	ca = IMG_GET_A( color );
	uint8	*src_alpha_table = KImage_AlphaTable[alpha];
	uint8	*color_alpha_table = KImage_AlphaTable[ca];
	int i, j;

	cr = color_alpha_table[cr];
	cg = color_alpha_table[cg];
	cb = color_alpha_table[cb];
	color_alpha_table = KImage_AlphaTable[255-ca];
 
	for( i = 0; i < dst->height; i++ ) {
		register fixedpoint tx, ty, d_tx, d_ty;
		fixedpoint d_x;
		int min, max;
		register uint32 *dst;
	
		if( lines[i].min == SCBUF_MIN_INVALID )
			continue;

		tx = lines[i].min_tx;
		ty = lines[i].min_ty;
		d_x = lines[i].max - lines[i].min + 1;
		d_tx = fp_div( (lines[i].max_tx - tx) , d_x );
		d_ty = fp_div( (lines[i].max_ty - ty) , d_x );
		min = fp_fix2int( lines[i].min );
		max = fp_fix2int( lines[i].max );

		// Xï˚å¸ìhÇËÇ¬Ç‘Çµ
		dst = (uint32 *)((uint32)dst_img + dst_bpl * i + min * 4);
		for( j = min; j <  max; j++, tx += d_tx, ty += d_ty, dst++ ) {
			int ttx = fp_fix2int(tx);
			int tty = fp_fix2int(ty);
			uint32	src_pix = *(uint32 *)((uint32)src_img + src_bpl * tty + ttx * 4);
			uint32	dst_pix = *dst;
			uint32	r, g, b, a;
			uint8	*pix_alpha_table, *dst_alpha_table;

			a = IMG_GET_A(src_pix);
			if( a == 0 )
				continue;
			a = src_alpha_table[a];

			pix_alpha_table = KImage_AlphaTable[a];
			dst_alpha_table = KImage_AlphaTable[255-a];

			r = pix_alpha_table[ color_alpha_table[ IMG_GET_R(src_pix) ] + cr ] + dst_alpha_table[ IMG_GET_R(dst_pix) ];
			g = pix_alpha_table[ color_alpha_table[ IMG_GET_G(src_pix) ] + cg ] + dst_alpha_table[ IMG_GET_G(dst_pix) ];
			b = pix_alpha_table[ color_alpha_table[ IMG_GET_B(src_pix) ] + cb ] + dst_alpha_table[ IMG_GET_B(dst_pix) ];
			*dst = IMG_MAKE_RGB(r,g,b);
		}
	}
}

#define GLYPH_ALPHA_POWER (2)

void KImage_DrawGlyphGrayscale(
	uint32 dst_x,
	uint32 dst_y,
	uint32 src_w,
	uint32 src_h,
	uint32 rgb,
	uint8 *src_img,
	struct KImageData *dst_img)
{
	uint8 *src	= src_img;
	uint32 *dst = dst_img->image + dst_img->width * dst_y;
	uint8 *alpha_r = KImage_AlphaTable[IMG_GET_R(rgb)];
	uint8 *alpha_g = KImage_AlphaTable[IMG_GET_G(rgb)];
	uint8 *alpha_b = KImage_AlphaTable[IMG_GET_B(rgb)];
	int src_right_pad = ((src_w+3) & 0xfffc) - src_w;
	int dst_right_pad = dst_img->width - dst_x;
	uint32 sx, sy;

	for(sy = 0; sy < src_h; sy++) {
		dst = dst_img->image + dst_img->width * (dst_y+sy) + dst_x;
		for(sx = 0; sx < src_w; sx++, dst++) {
			uint32 val, dst_pixel, src_r, src_g, src_b;
			uint8 *dst_alpha_table;
			val = *src++ << GLYPH_ALPHA_POWER;
			if( val == 0 )
				continue;
			val--;
			dst_pixel = *dst;
			src_r = IMG_GET_R(dst_pixel);
			src_g = IMG_GET_G(dst_pixel);
			src_b = IMG_GET_B(dst_pixel);
			dst_alpha_table = KImage_AlphaTable[255-val];
			*dst = IMG_MAKE_RGB(
						dst_alpha_table[src_r] + alpha_r[val],
						dst_alpha_table[src_g] + alpha_g[val],
						dst_alpha_table[src_b] + alpha_b[val]);
		}
		src += src_right_pad;
		dst += dst_right_pad;
	}
}
