/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Philippe Houdoin
 *		Simon Gauvin
 *		Michael Pfeiffer
 */


#include "PrintUtils.h"

#include <Message.h>
#include <Window.h>


BRect
ScaleRect(const BRect& rect, float scale)
{
	BRect scaleRect(rect);

	scaleRect.left *= scale;
	scaleRect.right *= scale;
	scaleRect.top *= scale;
	scaleRect.bottom *= scale;

	return scaleRect;
}


void
SetBool(BMessage* message, const char* name, bool value)
{
	if (message->HasBool(name))
		message->ReplaceBool(name, value);
	else
		message->AddBool(name, value);
}


void
SetFloat(BMessage* message, const char* name, float value)
{
	if (message->HasFloat(name))
		message->ReplaceFloat(name, value);
	else
		message->AddFloat(name, value);
}


void
SetInt32(BMessage* message, const char* name, int32 value)
{
	if (message->HasInt32(name))
		message->ReplaceInt32(name, value);
	else
		message->AddInt32(name, value);
}


void
SetString(BMessage* message, const char* name, const char* value)
{
	if (message->HasString(name, 0))
		message->ReplaceString(name, value);
	else
		message->AddString(name, value);
}


void
SetRect(BMessage* message, const char* name, const BRect& rect)
{
	if (message->HasRect(name))
		message->ReplaceRect(name, rect);
	else
		message->AddRect(name, rect);
}


void
SetString(BMessage* message, const char* name, const BString& value)
{
	SetString(message, name, value.String());
}


static
bool InList(const char* list[], const char* name)
{
	for (int i = 0; list[i] != NULL; ++i) {
		if (strcmp(list[i], name) == 0)
			return true;
	}

	return false;
}


void
AddFields(BMessage* to, const BMessage* from, const char* excludeList[],
	const char* includeList[], bool overwrite)
{
	if (to == from)
		return;

	char* name;
	type_code type;
	int32 count;
	for (int32 i = 0; from->GetInfo(B_ANY_TYPE, i, &name, &type, &count)
		== B_OK; ++i) {
		if (excludeList && InList(excludeList, name))
			continue;

		if (includeList && !InList(includeList, name))
			continue;

		ssize_t size;
		const void* data;
		if (!overwrite && to->FindData(name, type, 0, &data, &size) == B_OK)
			continue;

		// replace existing data
		to->RemoveName(name);

		for (int32 j = 0; j < count; ++j) {
			if (from->FindData(name, type, j, &data, &size) == B_OK) {
				if (type == B_STRING_TYPE)
					to->AddString(name, (const char*)data);
				else if (type == B_MESSAGE_TYPE) {
					BMessage message;
					from->FindMessage(name, j, &message);
					to->AddMessage(name, &message);
				} else
					to->AddData(name, type, data, size);
			}
		}
	}
}
