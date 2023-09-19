/*****************************************************************************
 string.cpp
 文字列クラス

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "klib/string.h"
#include "klib/debug.h"

#define FIXED_LEN	(4096)

/****************************************************************************
  KStringBuffer
****************************************************************************/

KStringBuffer::KStringBuffer()
{
	ref_count = 0;
	bytes = 0;
	buf = new char[1];
	*buf = '\0';
}

KStringBuffer::~KStringBuffer()
{
	delete[] buf;
}

KStringBuffer::KStringBuffer(const char *str)
{
	ref_count = 0;
	bytes = strlen(str);
	buf = new char[bytes+1];
	strcpy(buf, str);
}

KStringBuffer::KStringBuffer(const char *str, int cnt)
{
	ref_count = 0;
	bytes = cnt;
	buf = new char[cnt+1];
	memcpy(buf, str, cnt);
	buf[cnt] = '\0';
}

void KStringBuffer::Attach()
{
	ref_count++;
}

void KStringBuffer::Detach()
{
	ref_count--;
	if(ref_count <= 0)
		delete this;
}

const char *KStringBuffer::GetPtr()
{
	return buf;
}

int KStringBuffer::GetBytes()
{
	return bytes;
}

/****************************************************************************
  KString
****************************************************************************/

KString::KString()
{
	buf = new KStringBuffer();
	buf->Attach();
}

KString::~KString()
{
	buf->Detach();
}


KString::KString(const KString& s)
{
	buf = s.buf;
	buf->Attach();
}

KString::KString(const char *str)
{
	buf = new KStringBuffer(str);
	buf->Attach();
}

KString::KString(const char *str, int n)
{
	buf = new KStringBuffer(str, n);
	buf->Attach();
}

KString::KString(int i)
{
	char *tmp = new char [FIXED_LEN];
	sprintf(tmp, "%d", i);

	KString s(tmp);
	delete[] tmp;

	buf = s.buf;
	buf->Attach();
}

KString::KString(const KIntStrValue& val)
{
	buf = val.GetString().buf;
	buf->Attach();
}

KString& KString::operator=(const KString& s)
{
	// str=str のような代入を防ぐ
	if(buf != s.buf) {
		buf->Detach();
		buf = s.buf;
		buf->Attach();
	}
	return *this;
}

KString KString::operator+(const KString& s)
{
	int a_bytes = buf->GetBytes();
	int b_bytes = s.buf->GetBytes();
	int res_bytes = a_bytes + b_bytes;

	char *tmp = new char [res_bytes + 1];
	memcpy(tmp, buf->GetPtr(), a_bytes);
	memcpy(tmp + a_bytes, s.buf->GetPtr(), b_bytes);

	KString res(tmp, res_bytes);
	delete[] tmp;

	return res;
}

KString& KString::operator+=( const KString& s )
{
	int a_bytes = buf->GetBytes();
	int b_bytes = s.buf->GetBytes();
	int res_bytes = a_bytes + b_bytes;

	char *tmp = new char [res_bytes + 1];
	memcpy(tmp, buf->GetPtr(), a_bytes);
	memcpy(tmp + a_bytes, s.buf->GetPtr(), b_bytes);

	KString res(tmp, res_bytes);
	delete[] tmp;

	buf->Detach();
	buf = res.buf;
	buf->Attach();

	return *this;
}

KString::operator const char *() const
{
	return buf->GetPtr();
}

int KString::Bytes() const
{
	return buf->GetBytes();
}

signed int KString::ToInt() const
{
	return atoi(buf->GetPtr());
}

char KString::ByteAt(int n) const
{
	assert(n < buf->GetBytes());
	return *(buf->GetPtr() + n);
}

void KString::CopyTo(char *d, int buflen) const
{
	assert(buflen > 0);

	const char *s = buf->GetPtr();
	int bytes = buf->GetBytes();

	buflen--;
	for(int i = 0; i < bytes && i < buflen; i++)
		*d++ = *s++;
	*d = '\0';
}

bool KString::Equals(const KString& s) const
{
	return (strcmp(buf->GetPtr(), s.buf->GetPtr())==0);
}

int KString::Cmp(const KString& s) const
{
	return strcmp(buf->GetPtr(), s.buf->GetPtr());
}

bool KString::IsEmpty() const
{
	return (buf->GetBytes() == 0);
}

KString KString::sprintf(const char *format, ...)
{
	char *tmp = new char [FIXED_LEN];

	va_list list;
	va_start(list, format);
	vsprintf(tmp, format, list);
	va_end(list);

	KString res(tmp);
	delete[] tmp;
	return res;
}

/****************************************************************************
  KStringTokenizer
****************************************************************************/

KStringTokenizer::KStringTokenizer(KString s)
{
	// ワイド文字列(Unicode文字列)を生成する
	chars = mbstowcs(NULL, s, s.Bytes());
	str = new wchar_t [chars+1];
	mbstowcs(str, s, chars+1);
}

int KStringTokenizer::Chars()
{
	return chars;
}

wchar_t KStringTokenizer::CharAt(int n)
{
	assert(n < chars);
	return str[n];
}

/****************************************************************************
  KIntStrValue
****************************************************************************/

KIntStrValue::KIntStrValue()
{
	type = ISV_TYPE_UNDEFINED;
}

KIntStrValue::KIntStrValue(int value)
{
	type = ISV_TYPE_INT;
	int_val = value;
}

KIntStrValue::KIntStrValue(KString value)
{
	type = ISV_TYPE_STRING;
	str_val = value;
}

KIntStrValue::KIntStrValue(const KIntStrValue& value)
{
	type = value.type;
	if(type == ISV_TYPE_INT)
		int_val = value.int_val;
	else if(type == ISV_TYPE_STRING)
		str_val = value.str_val;
}

int KIntStrValue::GetType() const
{
	K_ASSERT(type == ISV_TYPE_INT || type == ISV_TYPE_STRING ||
			 type == ISV_TYPE_UNDEFINED);
	return type;
}

int KIntStrValue::GetInt() const
{
	K_ASSERT(type != ISV_TYPE_UNDEFINED);
	if(type == ISV_TYPE_INT)
		return int_val;
	return str_val.ToInt();
}

KString KIntStrValue::GetString() const
{
	K_ASSERT(type != ISV_TYPE_UNDEFINED);
	if(type == ISV_TYPE_STRING)
		return str_val;
	return (KString)int_val;
}


KIntStrValue::operator int() const
{
	return GetInt();
}

KIntStrValue::operator KString() const
{
	return GetString();
}

KIntStrValue& KIntStrValue::operator=(int value)
{
	switch(type) {
	case ISV_TYPE_INT:
		int_val = value;
		break;
	case ISV_TYPE_STRING:
		str_val = (KString) value;
		break;
	default:
		type = ISV_TYPE_INT;
		int_val = value;
		break;
	}
	return *this;
}

KIntStrValue& KIntStrValue::operator=(const KString& value)
{
	switch(type) {
	case ISV_TYPE_INT:
		int_val = value.ToInt();
		break;
	case ISV_TYPE_STRING:
		str_val = value;
		break;
	default:
		type = ISV_TYPE_STRING;
		str_val = value;
		break;
	}
	return *this;
}
