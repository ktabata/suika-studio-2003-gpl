/*****************************************************************************
 io.h
 入出力ストリーム

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_IO_H_
#define _KLIB_IO_H_

#include <stdio.h>
#include "klib/config.h"
#include "klib/string.h"

#define TYPEDSTREAM_INT			(1)
#define TYPEDSTREAM_STRING		(2)
#define TYPEDSTREAM_ENCSTRING	(3)
#define TYPEDSTREAM_UNDEFINED	(4)
#define TYPEDSTREAM_KEY			(311)
#define TYPEDSTREAM_LEN_MAX		(1024*1024*5)

/*************************************************
  KInputStream
*************************************************/

class KInputStream {
public:
	virtual int	InputBytes(void *, int) = 0;
};

/*************************************************
  KOutputStream
*************************************************/

class KOutputStream {
public:
	virtual int	OutputBytes(void *, int) = 0;
};

/*************************************************
  KTypedInputStream
*************************************************/

class KTypedInputStream : public KInputStream {
public:
	KTypedInputStream(KInputStream *);

	bool	GetInt(uint32& result);
	bool	GetString(KString& result);
	bool	GetValue(KIntStrValue& result);

	int	InputBytes(void *p, int size);

private:
	KInputStream	*in_stream;

	bool GetType(int& type);
	bool GetIntData(uint32& result);
	bool GetStringLength(int& len);
	bool GetStringData(KString& s, bool dec);
};

/*************************************************
  KTypedOutputStream
*************************************************/

class KTypedOutputStream : public KOutputStream  {
public:
	KTypedOutputStream(KOutputStream *os);

	bool	PutInt(uint32 data);
	bool	PutString(KString data);
	bool	PutValue(KIntStrValue data);

	int	OutputBytes(void *p, int size);

private:
	KOutputStream	*out_stream;
	bool	PutUndefined();
};

/*************************************************
  KFileInputStream
*************************************************/

class KFileInputStream : public KInputStream {
public:
	KFileInputStream();
	KFileInputStream(KString);
	~KFileInputStream();

	bool	Open(KString);
	bool	IsOpen();
	bool	Close();
	int		Tell();
	bool	Seek(int);

	int		InputBytes(void *, int);

private:
	FILE	*fp;
};

/*************************************************
  KFileOutputStream
*************************************************/

class KFileOutputStream : public KOutputStream  {
public:
	KFileOutputStream();
	KFileOutputStream(KString);
	~KFileOutputStream();

	bool	Create(KString);
	bool	IsOpen();
	bool	Close();

	int		OutputBytes(void *, int);

private:
	FILE	*fp;
};

#endif
