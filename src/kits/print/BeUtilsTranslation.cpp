/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */


#include "BeUtils.h"

#include <Application.h>
#include <Bitmap.h>
#include <Messenger.h>
#include <Resources.h>
#include <Roster.h>
#include <String.h>


BBitmap*
LoadBitmap(const char* name, uint32 type_code)
{
	if (name == NULL)
		return NULL;

	if (type_code == B_TRANSLATOR_BITMAP)
		return BTranslationUtils::GetBitmap(type_code, name);

	BResources* resources = BApplication::AppResources();
	if (resources != NULL) {
		BMessage message;
		size_t length;

		const void* bits = resources->LoadResource(type_code, name, &length);
		if (bits != NULL && message.Unflatten((char*)bits) == B_OK)
			return (BBitmap*)BBitmap::Instantiate(&message);
	}

	return NULL;
}


BPicture*
BitmapToPicture(BView* view, BBitmap* bitmap)
{
	if (view != NULL && bitmap != NULL) {
		view->BeginPicture(new BPicture());
		view->DrawBitmap(bitmap);

		return view->EndPicture();
	}

	return NULL;
}


BPicture*
BitmapToGrayedPicture(BView* view, BBitmap* bitmap)
{
	if (view != NULL && bitmap != NULL) {
		BRect rect(bitmap->Bounds());

		view->BeginPicture(new BPicture());
		view->DrawBitmap(bitmap);
		view->SetHighColor(255, 255, 255, 128);
		view->SetDrawingMode(B_OP_ALPHA);
		view->FillRect(rect);

		return view->EndPicture();
	}

	return NULL;
}
