/*****************************************************************************
 image_init.cpp
 イメージ

 Suika Studio (core/Win32)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/graphic.h"
#include "klib/debug.h"

#define RELEASE(p)	if(p!=NULL){delete p;p=NULL;}

KImage::KImage()
{
	width		= 0;
	height		= 0;
	image		= NULL;
	bpl			= 0;
	is_alpha_enabled = false;

#ifdef	_WIN32
	hBitmap = NULL;
	hDC = NULL;
#endif
}

bool KImage::Create(int w, int h)
{
	K_ASSERT(image == NULL);

	width = w;
	height = h;
	bpl = w * 4;

#ifdef	_WIN32

	return CreateWin32();

#else

	K_ASSERT(false);	// for debug

	image = new RGBA32[bpl * height];
	return true;

#endif
}

void KImage::ReleaseAll()
{
#ifdef	_WIN32

	ReleaseAllWin32();

#else

	K_ASSERT(false);	// for debug

	RELEASE(image);

#endif
}

KImage::~KImage()
{
	ReleaseAll();
}

int	KImage::GetWidth()
{
	return width;
}

int KImage::GetHeight() 
{
	return height;
}

RGBA32 *KImage::GetBuffer()
{
	return image;
}

void KImage::EnableAlpha( bool b )
{
	is_alpha_enabled = b;
}

bool KImage::IsAlphaEnabled()
{
	return is_alpha_enabled ? true : false;
}

/****************************************************************************
  Win32
****************************************************************************/

#define RELEASE_GDI_OBJ(func, p)	if(p!=NULL){func(p);p=NULL;}

bool KImage::CreateWin32()
{
	// HBITMAPハンドルを作成する
	BITMAPINFO	bi;
	::memset (&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth		= width;
	bi.bmiHeader.biHeight		= -height;	// 負数ならトップダウン
	bi.bmiHeader.biPlanes		= 1;
	bi.bmiHeader.biBitCount		= 32;
	bi.bmiHeader.biCompression	= BI_RGB;

	hDC = ::CreateCompatibleDC (NULL);
	if(hDC == NULL)
		return false;
	hBitmap = ::CreateDIBSection (hDC, &bi, DIB_RGB_COLORS, (VOID **)&image, NULL, 0);
	if((hBitmap == NULL) || (image == NULL)) {
		::DeleteDC (hDC);
		return false;
	}
	::SelectObject (hDC, hBitmap);
	return true;
}

void KImage::ReleaseAllWin32()
{
	RELEASE_GDI_OBJ(::DeleteDC, hDC);
	RELEASE_GDI_OBJ(::DeleteObject, hBitmap);
	image = NULL;
}

KImage *KImage::LoadResourceBitmap(LPCSTR lpszResource)
{
	// イメージをロードする
	CBitmap	bitmap;
	if(!bitmap.LoadBitmap(lpszResource))
		return NULL;

	// イメージの幅，高さ等の情報を取得する
	BITMAP	bm;
	BOOL b = bitmap.GetObject(sizeof(bm), &bm);
	ASSERT(b);

	// KImageのインスタンスを作成する
	KImage *res = new KImage();
	if(!res->Create(bm.bmWidth, bm.bmHeight)) {
		delete res;
		return NULL;
	}

	// ロードしたイメージをアイコンイメージにコピーする
	HBITMAP hBitmap = (HBITMAP)bitmap;
	HDC		hBitmapDC = ::CreateCompatibleDC (NULL);
	ASSERT(hBitmapDC != NULL);
	::SelectObject (hBitmapDC, hBitmap);
	::BitBlt(res->hDC, 0, 0, bm.bmWidth, bm.bmHeight,
		hBitmapDC, 0, 0, SRCCOPY);
	::DeleteDC(hBitmapDC);
	return res;
}
