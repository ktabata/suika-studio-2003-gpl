/*****************************************************************************
 io.cpp
 入出力ストリーム

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include "klib/io.h"

#define DECODE(c)		((char)((int)c+TYPEDSTREAM_KEY))
#define ENCODE(c)		((char)((int)c-TYPEDSTREAM_KEY))

/***************************************
 KTypedInputStream
***************************************/

KTypedInputStream::KTypedInputStream(KInputStream *is)
{
	in_stream = is;
}

bool KTypedInputStream::GetInt(uint32& result)
{
	// データ型を取得して整数型かどうかチェックする
	int type;
	if(!GetType(type))
		return false;
	if(type != TYPEDSTREAM_INT)
		return false;

	return GetIntData(result);
}

bool KTypedInputStream::GetString(KString& result)
{
	// データ型を取得して文字列型かどうかチェックする
	int type;
	if(!GetType(type))
		return false;
	if(type != TYPEDSTREAM_STRING && type != TYPEDSTREAM_ENCSTRING)
		return false;

	return GetStringData(result, (type==TYPEDSTREAM_ENCSTRING));
}

bool KTypedInputStream::GetValue(KIntStrValue& result) {
	int type;
	if(!GetType(type))
		return false;

	if(type == TYPEDSTREAM_INT) {
		uint32 val;
		if(!GetIntData(val))
			return false;
		result = (int)val;
	} else if(type == TYPEDSTREAM_STRING || type == TYPEDSTREAM_ENCSTRING) {
		KString val;
		if(!GetStringData(val, (type==TYPEDSTREAM_ENCSTRING)))
			return false;
		result = val;
	} else if(type != TYPEDSTREAM_UNDEFINED)
		return false;
	return true;
}

bool KTypedInputStream::GetType(int& type)
{
	uint8 b;
	if(in_stream->InputBytes(&b, 1) == 1) {
		type = b;
		return true;
	}
	return false;
}

bool KTypedInputStream::GetIntData(uint32& result)
{
	uint32 n;
	if(in_stream->InputBytes(&n, 4) != 4)
		return false;
	result = LSB_FIRST_OF_4(n);
	return true;
}

bool KTypedInputStream::GetStringLength(int& len)
{
	uint32 n;
	if(in_stream->InputBytes(&n, 4) == 4) {
		if(n <= TYPEDSTREAM_LEN_MAX) {
			len = n;
			return true;
		}
	}
	return false;
}

bool KTypedInputStream::GetStringData(KString& s, bool decode)
{
	// 文字列のバイト数を取得する
	int len;
	if(!GetStringLength(len))
		return false;

	// 文字列の長さが０バイトなら
	if(len ==0) {
		s = "";
		return true;
	}

	// 読み込む
	char *buf = new char[len];
	if(in_stream->InputBytes(buf, len) != len) {
		delete[] buf;
		return false;
	}

	// ENCSTRINGならデコードする
	if(decode) {
		char *ptr = buf;
		for(int i = 0; i < len; i++)
			*ptr++ = DECODE(*ptr);
	}

	s = KString(buf, len);
	delete[] buf;
	return true;
}

int	KTypedInputStream::InputBytes(void *p, int size)
{
	return in_stream->InputBytes(p, size);
}

/***************************************
 KTypedOutputStream
***************************************/

KTypedOutputStream::KTypedOutputStream(KOutputStream *os)
{
	out_stream = os;
}

bool KTypedOutputStream::PutInt(uint32 n)
{
	n = TO_LSB_FIRST_4(n);

	// データ型を出力する
	uint8 type = TYPEDSTREAM_INT;
	if(out_stream->OutputBytes(&type, 1) == 1) {
		// データを出力する
		if(out_stream->OutputBytes(&n, 4) == 4)
			return true;
	}
	return false;
}

bool KTypedOutputStream::PutString(KString s)
{
	// データ型を出力する
	uint8 type = TYPEDSTREAM_STRING;
	if(out_stream->OutputBytes(&type, 1) == 1) {
		// 文字列のバイト数を出力する
		uint32 len = s.Bytes();
		if(out_stream->OutputBytes(&len, 4) == 4) {
			// データを出力する
			const char *buf = s;
			if(out_stream->OutputBytes((void *)buf, len) == (int)len)
				return true;
		}
	}
	return false;
}

bool KTypedOutputStream::PutUndefined()
{
	uint8 type = TYPEDSTREAM_UNDEFINED;
	return (out_stream->OutputBytes(&type, 1) == 1);
}

bool KTypedOutputStream::PutValue(KIntStrValue val)
{
	switch(val.GetType()) {
	case ISV_TYPE_INT:
		return PutInt( (uint32)( (int)val ) );
	case ISV_TYPE_STRING:
		return PutString( (KString) val);
	case ISV_TYPE_UNDEFINED:
		return PutUndefined();
	}
	return false;
}

int	KTypedOutputStream::OutputBytes(void *p, int size)
{
	return out_stream->OutputBytes(p, size);
}

/***************************************
 KFileInputStream
***************************************/

KFileInputStream::KFileInputStream()
{
	fp = NULL;
}

KFileInputStream::KFileInputStream(KString fname)
{
	fp = NULL;
	Open(fname);
}

KFileInputStream::~KFileInputStream()
{
	if(IsOpen())
		Close();
}

bool KFileInputStream::Open(KString fname)
{
	if(IsOpen())
		return false;
	if(!fname.IsEmpty())
		fp = ::fopen(fname, "rb");
	return IsOpen();
}

bool KFileInputStream::IsOpen()
{
	return (fp != NULL);
}

bool KFileInputStream::Close()
{
	if(fp == NULL)
		return false;

	int res = ::fclose(fp);
	fp = NULL;
	return (res != EOF);
}

int KFileInputStream::Tell()
{
	return ::ftell(fp);
}

bool KFileInputStream::Seek( int ofs )
{
	return (::fseek(fp, ofs, SEEK_SET) == 0);
}

int KFileInputStream::InputBytes( void *p, int size )
{
	if(fp == NULL)
		return false;
	return ::fread(p, 1, size, fp);
}

/***************************************
 KFileOutputStream
***************************************/

KFileOutputStream::KFileOutputStream()
{
	fp = NULL;
}

KFileOutputStream::KFileOutputStream(KString fname)
{
	fp = NULL;
	Create(fname);
}

KFileOutputStream::~KFileOutputStream()
{
	if(IsOpen())
		Close();
}

bool KFileOutputStream::Create(KString fname)
{
	if(IsOpen())
		Close();
	if(!fname.IsEmpty())
		fp = fopen(fname, "wb");
	return IsOpen();
}

bool KFileOutputStream::IsOpen()
{
	return (fp != NULL);
}

bool KFileOutputStream::Close()
{
	if(fp == NULL)
		return false;

	int res = ::fclose(fp);
	fp = NULL;
	return (res != EOF);
}

int KFileOutputStream::OutputBytes(void *p, int size)
{
	if(fp == NULL)
		return false;
	return ::fwrite(p, 1, size, fp);
}
