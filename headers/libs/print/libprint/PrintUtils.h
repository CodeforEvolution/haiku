/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Philippe Houdoin
 *		Simon Gauvin
 *		Michael Pfeiffer
 */
#ifndef _PRINT_UTILS_H
#define _PRINT_UTILS_H


#include <List.h>
#include <MessageFilter.h>
#include <String.h>
#include <Rect.h>


class BHandler;
class BMessage;
class BWindow;


#define BEGINS_CHAR(byte) ((byte & 0xc0) != 0x80)


BRect ScaleRect(const BRect& rect, float scale);


// Set or replace a value in a BMessage
void SetBool(BMessage* message, const char* name, bool value);
void SetFloat(BMessage* message, const char* name, float value);
void SetInt32(BMessage* message, const char* name, int32 value);
void SetString(BMessage* message, const char* name, const char* value);
void SetRect(BMessage* message, const char* name, const BRect& rect);
void SetString(BMessage* message, const char* name, const BString& value);
void AddFields(BMessage* to, const BMessage* from,
	const char* excludeList[] = NULL, const char* includeList[] = NULL,
	bool overwrite = true);


template <class T>
class TList {
public:

	virtual						~TList();

			void				MakeEmpty();
			int32				CountItems() const;

			T*					ItemAt(int32 index) const;
			void				AddItem(T* item);
			T*					RemoveItem(int index);

			T*					Items();
			void				SortItems(int (*comp)(const T**, const T**));

private:
			BList				fList;
	typedef int					(*sort_func)(const void*, const void*);
};


// TList
template<class T>
TList<T>::~TList()
{
	MakeEmpty();
}


template<class T>
void
TList<T>::MakeEmpty()
{
	const int32 n = CountItems();
	for (int i = 0; i < n; i++)
		delete ItemAt(i);

	fList.MakeEmpty();
}


template<class T>
int32
TList<T>::CountItems() const
{
	return fList.CountItems();
}


template<class T>
T*
TList<T>::ItemAt(int32 index) const
{
	return (T*)fList.ItemAt(index);
}


template<class T>
void
TList<T>::AddItem(T* item)
{
	fList.AddItem(item);
}


template<class T>
T*
TList<T>::RemoveItem(int index)
{
	return (T*)fList.RemoveItem(index);
}


template<class T>
T*
TList<T>::Items()
{
	return (T*)fList.Items();
}


template<class T>
void
TList<T>::SortItems(int (*comp)(const T**, const T**))
{
	sort_func sort = (sort_func)comp;
	fList.SortItems(sort);
}

#endif	// _PRINT_UTILS_H
