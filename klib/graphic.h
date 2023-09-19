/*****************************************************************************
 graphic.h
 グラフィックライブラリ

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_GRAPHIC_H_
#define _KLIB_GRAPHIC_H_

#include "klib/string.h"
#include "klib/io.h"
#include "klib/debug.h"

extern "C" {
#include "klib/image_ext_c.h"
}

/******************************************
 KImage
  RGBA(32bpp)の汎用イメージ操作ライブラリ
******************************************/

class KImage : protected KImageData {
public:
	KImage();
	~KImage();
	bool	Create(int w, int h);
	void	EnableAlpha(bool);
	bool	Load(KString bmp_fname, KString alpha_fname);
	void	DoAutoMask();
	void	ReleaseAll();

	int		GetWidth();
	int		GetHeight();
	RGBA32	*GetBuffer();
	bool	IsAlphaEnabled();

	void	Clear();
	void	White();
	void	Fill(RGBA32 rgb);
	void	DrawImage(KImage * src, int dst_x, int dst_y, uint8 alpha = 255,
				RGBA32 color = IMG_MAKE_RGBA(0,0,0,0),
				int src_x = -1, int src_y = -1, int w = -1, int h = -1 );
	void	DrawPolygon(KImage *src, KEdgeBuffer *edge_buf, 
				uint8 alpha = 255, RGBA32 color = IMG_MAKE_RGBA(0,0,0,0));

private:
	// DrawImage() helper
	bool	Clip(KImage *src, int& dst_x, int& dst_y, int& src_x,
				int& src_y, int& w, int& h);

	// BMPロード
	bool LoadBmp(KInputStream&, bool);
	static int  IntPow(int, int);
	static bool ReadBmpHeader(KInputStream&, int&, int&, int&, int&);
	static bool ReadBmpPalette(KInputStream&, int, RGBA32 *);
	static void ConvertBmpPaletteToGrayscale(int, RGBA32 *);
	static int  CalcBmpBpl(int, int);
	bool ReadBmpPixels(KInputStream&, int, uint32 *, bool);
	void SetColorImageLineFromBmp(uint8 *, int, int, RGBA32 *);
	void SetMaskImageLineFromBmp(uint8 *, int, int, RGBA32 *);

// 以下，プラットフォーム依存

#ifdef	_WIN32
public:
	bool	CreateWin32();
	void	ReleaseAllWin32();
	HDC		GetDC() { return hDC; }
	void	DrawGlyphGrayscale(uint32 dst_x, uint32 dst_y, uint32 src_w,
				uint32 src_h, RGBA32 rgb, uint8 *src_img);
	static KImage *LoadResourceBitmap(LPCSTR lpszResource);
private:
	HBITMAP	hBitmap;
	HDC		hDC;
#endif
};

/*********************************************************
 KGraphicContext
  一般化された描画対象へのアクセス手段を提供するクラス
*********************************************************/

class KGraphicContext {
public:
	KGraphicContext(KImage *image);	// KImageを描画対象とする
	~KGraphicContext();

	bool	SelectFont(int w, int h, const char *name = NULL);
	void	SetColor(RGBA32 rgb);
	void	EnableAntiAliasing(bool is_enabled);

	int		GetTextWidth(const char *str, int len = -1);

	void	FillRect(KRECT& rect);
	void	DrawText(int x, int y, const char *str, int len = -1);
	void	DrawImage(KImage *src, int dx, int dy, int w, int h,
						int sx, int sy);

private:
	KImage	*image;
	bool	is_aa_enabled;
	RGBA32	rgb;

// 以下，プラットフォーム依存

#ifdef _WIN32
public:
	KGraphicContext(HDC hDC);		// HDCを描画対象とする
	KGraphicContext(HWND hWnd);		// ウィンドウのクライアント領域を描画対象とする

private:
	HWND	hWnd;
	HDC		hDC;
	HFONT	hFont, hFontOld;
	int		ascent;
	uint8	*outline_buffer;
	unsigned int	last_code;
	GLYPHMETRICS	last_glyph;

	void	SetDefVal();
	void	InitFont();
	void	ReleaseFont();
	void	DrawOutlineChar(int x, int y, unsigned int code, int& w, int& h);
#endif
};

/**********************************
 KEdgeScanner
  ポリゴン描画用のエッジスキャナ
**********************************/

class KEdgeScanner : public KEdgeBuffer {
public:
	struct Point {
		fixedpoint x, y, tx, ty;
	};

	KEdgeScanner(int);
	~KEdgeScanner();

	void Clear();
	void ScanEdge(Point, Point);
};

#endif
