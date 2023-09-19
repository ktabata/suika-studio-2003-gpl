/*****************************************************************************
 string.h
 ������N���X

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
    KString���ێ����镶����o�b�t�@�B���t�@�����X�J�E���g�Ŏ��g�̃C���X�^���X
  ���K�x�[�W�R���N�g����B
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
    �������ێ�����B������o�b�t�@�ւ̎Q�Ƃ�ێ�����̂ŁA�R�s�[�ɂ��I�[
  �o�[�w�b�h�͏��Ȃ��B�Q�Ƃ��g�����̓I�u�W�F�N�g���R�s�[����悤�ɂ���B
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
    �������Unicode�ŕێ�����B�X�N���v�g�����p�����A���݂͖������B
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
    ������������̒l��ێ�����B�X�N���v�g�����Ȃǂŗp����B���̃N���X�̃C��
  �X�^���X�́A�ŏ��̑���Ō^�����肵�A�ȍ~�̑���ňႤ�^�̒l���������Ă��A
  �ŏ��̌^�ɕϊ����ĕێ�����B
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

