/*****************************************************************************
 string.h
 文字列クラス

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_STRING_H_
#define _KLIB_STRING_H_

#include <wchar.h>

class KIntStrValue;

/*****************************************************************************
 KStringBuffer
 ============================================================================
    KStringが保持する文字列バッファ。リファレンスカウントで自身のインスタンス
  をガベージコレクトする。
*****************************************************************************/

class KStringBuffer {
	friend class KString;
protected:
	KStringBuffer();
	~KStringBuffer();
	KStringBuffer(const char *);
	KStringBuffer(const char *, int);
	void Attach();
	void Detach();
	const char *GetPtr();
	int GetBytes();

private:
	char *buf;
	int bytes;
	int ref_count;
};

/*****************************************************************************
 KString
 ============================================================================
    文字列を保持する。文字列バッファへの参照を保持するので、コピーによるオー
  バーヘッドは少ない。参照を使うよりはオブジェクトをコピーするようにする。
*****************************************************************************/

class KString {
public:
	KString();
	KString(const char *);
	KString(const char *, int);
	KString(const KString&);
	KString(int);
	KString(const KIntStrValue&);
	~KString();

	KString& operator=(const KString&);
	KString operator+(const KString&);
	KString& operator+=(const KString&);
	operator const char *() const;

	int		Bytes() const;
	int		ToInt() const;
	char	ByteAt(int) const;
	void	CopyTo(char *, int) const;
	bool	Equals(const KString&) const;
	int		Cmp(const KString&) const;
	bool	IsEmpty() const;
	static KString sprintf(const char *, ...);

private:
	KStringBuffer	*buf;
};

/*****************************************************************************
 KStringTokenizer
 ============================================================================
    文字列をUnicodeで保持する。スクリプト処理用だが、現在は未実装。
*****************************************************************************/

class KStringTokenizer {
public:
	KStringTokenizer(KString);

	int		Chars();
	wchar_t	CharAt(int);

private:
	wchar_t	*str;
	int	chars;
};

/*****************************************************************************
 KIntStrValue
 ============================================================================
    整数か文字列の値を保持する。スクリプト処理などで用いる。このクラスのイン
  スタンスは、最初の代入で型を決定し、以降の代入で違う型の値が代入されても、
  最初の型に変換して保持する。
*****************************************************************************/

#define ISV_TYPE_UNDEFINED	(0)
#define ISV_TYPE_INT		(1)
#define ISV_TYPE_STRING		(2)

class KIntStrValue {
public:
	KIntStrValue();
	KIntStrValue(int value);
	KIntStrValue(KString value);
	KIntStrValue(const KIntStrValue& value);

	int	GetType() const;
	operator int() const;
	operator KString() const;
	int GetInt() const;
	KString GetString() const;

	KIntStrValue& operator=(int value);
	KIntStrValue& operator=(const KString& value);

private:
	int		type;
	int		int_val;
	KString	str_val;
};

#endif

