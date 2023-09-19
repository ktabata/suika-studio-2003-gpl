/*****************************************************************************
 image_load.cpp
 イメージクラス・ロード

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/graphic.h"
#include "klib/io.h"
#include "klib/debug.h"

bool KImage::Load(KString color_file, KString mask_file)
{
	KFileInputStream fis(color_file);
	if(!LoadBmp(fis, false))
		return false;

	bool b = true;
	is_alpha_enabled = false;
	if(mask_file.Bytes()) {
		is_alpha_enabled = true;
		if(mask_file.Equals("AUTO"))
			DoAutoMask();
		else {
			KFileInputStream f(mask_file);
			b = LoadBmp(f, true);
		}
	}
	return b;
}

bool KImage::LoadBmp(KInputStream& is, bool is_mask)
{
	bool	b = false;
	int		bpp, colors;
	RGBA32 *palette = NULL;

	// ヘッダを読み込む
	if(ReadBmpHeader(is, width, height, bpp, colors)) {

		// イメージのバッファを確保する
		if(image == NULL)
			if(!Create(width, height))
				return false;

		// １，４，８ビットＢＭＰならパレットをロードする
		if(bpp != 24) {
			palette = new RGBA32[256];
			if(colors == 0)
				colors = IntPow(2, bpp);
			if(!ReadBmpPalette(is, colors, palette)) {
				delete[] palette;
				return false;
			}
		}

		// マスク読み込みの場合パレットをグレースケールに変換する
		if(bpp != 24 && is_mask)
			ConvertBmpPaletteToGrayscale(bpp, palette);
		// ピクセル列を読み込む
		if(ReadBmpPixels(is, bpp, palette, is_mask))
			b = true;
	}

	if(palette != NULL)
		delete[] palette;
	return b;
}

int KImage::IntPow(int x, int y)
{
	int ret = x;
	for( ; y > 1; y-- )
		ret *= x;
	return ret;
}

bool KImage::ReadBmpHeader(KInputStream& is, int& w, int& h, int& bpp, int& colors)
{
	uint16	u2;
	uint32	u4[13];
	const int WIDTH_INDEX = 4, HEIGHT_INDEX	= 5,
			  BPP_INDEX = 6, COMPRESS_INDEX = 7,
			  COLORS_INDEX = 11;

	// 'BM' シグネチャを読み込んで確認する
	if(is.InputBytes(&u2, 2)) {
		if(LSB_FIRST_OF_4(u2) == ('B' | ('M'<<8))) {
			// 残りのヘッダ５２バイトを一度に読み込む
			if(is.InputBytes(u4, 52)) {
				w = LSB_FIRST_OF_4( u4[WIDTH_INDEX] );
				h = LSB_FIRST_OF_4( u4[HEIGHT_INDEX] );
				bpp = LSB_FIRST_OF_4( u4[BPP_INDEX] ) >> 16;
				colors = LSB_FIRST_OF_4( u4[COLORS_INDEX] );

				// 色ビット数を確認する
				if(bpp == 1 || bpp == 4 || bpp == 8 || bpp == 24) {
					// 圧縮無しであることを確認する
					if( u4[COMPRESS_INDEX] == 0 )
						return true;
				}
			}
		}
	}
	return false;
}

bool KImage::ReadBmpPalette(KInputStream& is, int colors, RGBA32 *pal)
{
	uint32	u4;
	for(int i = 0; i < colors; i++) {
		if(is.InputBytes(&u4, 4))
			*pal++ = LSB_FIRST_OF_4(u4);
		else
			return false;
	}
	return true;
}

void KImage::ConvertBmpPaletteToGrayscale(int bpp, RGBA32 *pal)
{
	// 参考: 輝度Y = 0.3R + 0.59G + 0.11B
	int colors = IntPow(2, bpp);
	for(int i = 0; i < colors; i++) {
		uint32	p = *pal;
		float	b = (float)IMG_GET_R(p);
		float	g = (float)IMG_GET_G(p);
		float	r = (float)IMG_GET_B(p);
		uint8	y = (uint8)(0.3f * r + 0.59f * g + 0.11f * b);
		*pal++ = IMG_MAKE_A(y);
	}
}

bool KImage::ReadBmpPixels(KInputStream& is, int bpp, RGBA32 *pal, bool is_mask)
{
	bool	b = true;
	int		read_bpl = CalcBmpBpl(width, bpp);
	uint8	*line_buf = new uint8[read_bpl];

	for(int i = height-1; i >= 0; i--) {
		if(!is.InputBytes(line_buf, read_bpl)) {
			b = false;
			break;
		}
		if(is_mask)
			SetMaskImageLineFromBmp(line_buf, i, bpp, pal);
		else
			SetColorImageLineFromBmp(line_buf, i, bpp, pal);
	}
	return true;
}

int KImage::CalcBmpBpl(int w, int bpp)
{
	switch(bpp) {
	case 1:
		return ((w+31) & 0xffffffe0) / 8;
	case 4:
		return ((w+7) & 0xfffffff8) / 2;
	case 8:
		return (w+3) & 0xfffffffc;
	case 24:
		return (w*3+3) & 0xfffffffc;
	default:
		 return -1;
	}
}

void KImage::SetColorImageLineFromBmp(uint8 *line_buf, int y, int bpp, RGBA32 *pal)
{
	uint8	*src = line_buf;
	uint32	*dst = image + width * y;
	int x;

	switch(bpp) {
	case 1:
		for(x = 0; x < width; x++) {
			int i = x % 8;
			*dst++ = pal[(i==7) ? (*src++ & 1) : ( (*src>>(7-i)) & 1)];
		}
		break;
	case 4:
		for(x = 0; x < width; x++)
			*dst++ = pal[(x%2) ? (*src++ & 0x0f) : (*src >> 4)];
		break;
	case 8:
		for(x = 0; x < width; x++)
			*dst++ = pal[*src++];
		break;
	case 24:
		for(x = 0; x < width; x++) {
			uint32 b = *src++;
			uint32 g = *src++;
			uint32 r = *src++;
			*dst++ = IMG_MAKE_RGB(r, g, b);
		}
		break;
	}
}

void KImage::SetMaskImageLineFromBmp(uint8 *line_buf, int y, int bpp, RGBA32 *pal)
{
	uint8	*src = line_buf;
	uint32	*dst = image + width * y;
	int x;

	switch(bpp) {
	case 1:
		for(x = 0; x < width; x++) {
			int i = x % 8;
			*dst++ |= pal[(i==7) ? (*src++ & 1) : ( (*src>>(7-i)) & 1)];
		}
		break;
	case 4:
		for(x = 0; x < width; x++)
			*dst++ |= pal[(x%2) ? (*src++ & 0x0f) : (*src >> 4)];
		break;
	case 8:
		for(x = 0; x < width; x++)
			*dst++ |= pal[*src++];
		break;
	case 24:
		for(x = 0; x < width; x++) {
			uint32 b = *src++;
			uint32 g = *src++;
			uint32 r = *src++;
			uint32 a = (uint32)(0.3f * r + 0.59f * g + 0.11f * b);
			*dst++ |= IMG_MAKE_A(a);
		}
		break;
	}
}

void KImage::DoAutoMask()
{
	uint32 trans = *image & IMG_MAKE_RGB(0xff, 0xff, 0xff);

	for(int y = 0; y < height; y++) {
		uint32 *dst = image + width * y;
		for(int x = 0; x < width; x++) {
			uint32 tmp = *dst & IMG_MAKE_RGB(0xff, 0xff, 0xff);
			if(tmp == trans)
				*dst++ = tmp;
			else
				*dst++ = tmp | IMG_MAKE_A(0xff);
		}
	}
}
