/*****************************************************************************
 graphiccontext.cpp
 グラフィックコンテキスト
  一般化された描画対象へのアクセス手段を提供します。

 Suika Studio (core/Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/graphic.h"
#include <mbstring.h>

#define RELEASE(p)			if(p!=NULL){delete p;p=NULL;}
#define RELEASE_ARRAY(p)	if(p!=NULL){delete[] p;p=NULL;}
#define DELETE_OBJ(func, p)	if(p!=NULL){func(p);p=NULL;}

KGraphicContext::KGraphicContext(KImage *image)
{
	K_ASSERT(image != NULL);

	SetDefVal();
	this->image = image;
	hDC = image->GetDC();

	K_ASSERT(hDC != NULL);
}

KGraphicContext::KGraphicContext(HDC hDC)
{
	K_ASSERT(hDC != NULL);

	SetDefVal();
	this->hDC = hDC;
}

KGraphicContext::KGraphicContext(HWND hWnd)
{
	K_ASSERT(hWnd != NULL);

	SetDefVal();
	this->hWnd = hWnd;
	hDC = ::GetDC(hWnd);
}

KGraphicContext::~KGraphicContext()
{
	if(hFont != NULL) {
		::SelectObject(hDC, hFontOld);
		hFont = NULL;
	}
	DELETE_OBJ(::DeleteObject, hFont);
	RELEASE_ARRAY(outline_buffer);
	if(hWnd != NULL) {
		::ReleaseDC(hWnd, hDC);
		hWnd = NULL;
		hDC = NULL;
	}
}

void KGraphicContext::SetDefVal()
{
	image = NULL;
	rgb = IMG_MAKE_RGB(0, 0, 0);
	is_aa_enabled = false;
	hDC = NULL;
	hWnd = NULL;
	hFont = NULL;
	outline_buffer = NULL;
}

bool KGraphicContext::SelectFont(int w, int h, const char *font_name /* = NULL*/)
{
	if(font_name == NULL)
		font_name = "ＭＳ ゴシック";

	// フォントオブジェクトを作成する
	hFont = ::CreateFont(h, w, 0, 0, 400, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, FF_MODERN | FIXED_PITCH, font_name);
	if(hFont == NULL)
		return false;

	hFontOld = (HFONT)::SelectObject(hDC, hFont);
	::SetBkMode(hDC, TRANSPARENT);

	TEXTMETRIC tm;
	::GetTextMetrics(hDC, &tm);
	ascent = tm.tmAscent;

	return true;
}

void KGraphicContext::SetColor(RGBA32 rgb)
{
	::SetTextColor(hDC,
		IMG_GET_R(rgb) | (IMG_GET_G(rgb)<<8) | (IMG_GET_B(rgb)<<16));
	this->rgb = rgb;
}

void KGraphicContext::EnableAntiAliasing(bool is_enabled)
{
	is_aa_enabled = is_enabled;
}

int KGraphicContext::GetTextWidth(const char *str, int len/* = -1 */)
{
	SIZE size;
	K_ASSERT(str != NULL);
	if(len == -1)
		len = strlen(str);
	if(!::GetTextExtentPoint32(hDC, str, len, &size))
		return -1;
	return size.cx;
}

void KGraphicContext::FillRect(KRECT& rect)
{
	RECT rc;
	rc.left = rect.left;
	rc.top = rect.top;
	rc.right = rect.right;
	rc.bottom = rect.bottom;

	HBRUSH hBrush = ::CreateSolidBrush( (COLORREF) (IMG_GET_R(rgb) |
						(IMG_GET_G(rgb)<<8) | (IMG_GET_B(rgb)<<16) ) );
	::FillRect(hDC, &rc, hBrush);
	::DeleteObject(hBrush);
}

void KGraphicContext::DrawText(int x, int y, const char *str, int len /* = -1 */)
{
	if(len == -1)
		len = strlen(str);

	// アンチエイリアシング無効か？
	// （KImageへのレンダリングではない場合も）
	if(!is_aa_enabled || image==NULL) {
		::TextOut(hDC, x, y, str, len);
		return;
	}

	// アンチエイリアシング有効
	unsigned char *ptr = (unsigned char *)str;
	unsigned int code;
	int px = x, py = y;
	while(code = ::_mbsnextc(ptr)) {
		int w, h;
		DrawOutlineChar(px, py, code, w, h);
		px += w;
		ptr = ::_mbsinc(ptr);
	}
}

void KGraphicContext::DrawImage(
	KImage *src,
	int dx,
	int dy,
	int w,
	int h,
	int sx,
	int sy)
{
	::BitBlt(hDC, dx, dy, w, h, src->GetDC(), sx, sy, SRCCOPY);
}

void KGraphicContext::DrawOutlineChar(
	int x,
	int y,
	unsigned int code,
	int& w,
	int& h)
{
	// 前回描画した文字違う文字コードか？
	if(code != last_code)
	{
		MAT2	mat2 = {{0,1},{0,0},{0,0},{0,1}};
		int bufsize = ::GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP,
							&last_glyph, 0, NULL, &mat2);
		assert(bufsize != GDI_ERROR);
		w = last_glyph.gmCellIncX;
		if(bufsize == 0)	// スペース文字など、描画の必要がない文字の場合
			return;

		if(outline_buffer != NULL) {
			delete[] outline_buffer;	// 前回描画分のキャッシュを削除する
			outline_buffer = NULL;
		}
		outline_buffer = new uint8[bufsize];
		::GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP, &last_glyph,
							bufsize, outline_buffer, &mat2);
		last_code = code;
	}
	else
		w = last_glyph.gmCellIncX;	// 前回描画分のキャッシュから取得する

	int shift_x = last_glyph.gmptGlyphOrigin.x;
	int	shift_y = ascent - last_glyph.gmptGlyphOrigin.y;
	image->DrawGlyphGrayscale(x+shift_x, y+shift_y,
		last_glyph.gmBlackBoxX, last_glyph.gmBlackBoxY, rgb, outline_buffer);
}