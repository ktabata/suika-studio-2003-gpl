/*****************************************************************************
 array.h
 動的配列テンプレート

 Suika Studio (core)
 (C) Copyright 2002, 2003, TABATA Keiichi / LuxionSoft.
*****************************************************************************/

#ifndef _KLIB_ARRAY_H_
#define _KLIB_ARRAY_H_

#include "klib/config.h"
#include "klib/debug.h"

template<class T> class KArray {
public:
	KArray();
	KArray(KArray<T>& src);
	~KArray();
     
	int		GetItemsCount() const;
	void	InsertAt(int index, const T& val);
	void	DeleteAt(int index);
	void	DeleteAll();
	void	SetItemsCount(int size);	// 配列の要素数を拡張する（縮小はできない）
	T&	At(int index);
	const T& At(int index) const;
	T& operator[](int index);
	const T& operator[](int index) const;

private:
	T		*array;
	int		items_count;

	void	Copy(T *dst, T *src, int cnt);
};

template <class T>
KArray<T>::KArray()
{
	array = NULL;
	items_count = 0;
}

template <class T>
KArray<T>::~KArray()
{
}

template <class T>
inline int KArray<T>::GetItemsCount() const
{
	return items_count;
}

template <class T>
void KArray<T>::InsertAt(int index, const T& value)
{
	K_ASSERT(index >= 0 && index <= items_count);

	T *new_array = new T[items_count+1];
	if(items_count != 0) {
		if(index != 0)
			Copy(new_array, array, index);
		if(index != items_count)
			Copy(&new_array[index+1], &array[index], items_count-index);
		delete[] array;
	}
	array = new_array;
	array[index] = value;
	items_count++;
}

template <class T>
void KArray<T>::DeleteAt(int index)
{
	K_ASSERT(index >= 0 && index < items_count);

	T *new_array = new T[items_count-1];
	if(items_count != 0) {
		if(index != 0)
			Copy(new_array, array, index);
		if(index != items_count)
			Copy(&new_array[index], &array[index+1], items_count-index-1);
		delete[] array;
	}
	array = new_array;
	items_count--;
}

template <class T>
void KArray<T>::DeleteAll()
{
	delete[] array;
	array = NULL;
	items_count = 0;
}

template <class T>
void KArray<T>::SetItemsCount(int items)
{
	if(items == 0 && items_count == 0) {
		DeleteAll();
		return;
	}

	T *new_array = new T[items];
	if(items_count > 0) {
		Copy(new_array, array, (items<=items_count) ? items : items_count );
		delete[] array;
	}
	array = new_array;
	items_count = items;
}

template <class T>
T& KArray<T>::operator[](int index)
{
	K_ASSERT(index >= 0 && index < items_count);
	return array[index];
}

template <class T>
const T& KArray<T>::operator[](int index) const 
{
	K_ASSERT(index >= 0 && index < items_count);
	return array[index];
}

template <class T>
T& KArray<T>::At(int index)
{
	K_ASSERT(index >= 0 && index < items_count);
	return array[index];
}

template <class T>
const T& KArray<T>::At(int index) const
{
	K_ASSERT(index >= 0 && index < items_count);
	return array[index];
}

template <class T>
void KArray<T>::Copy(T *dst, T *src, int cnt)
{
	for(int i = 0; i < cnt; i++)
		*dst++ = *src++;
}

#endif
