/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */
#ifndef _BE_UTILS_H
#define _BE_UTILS_H


// Several utilities for writing applications for the BeOS. These are small
// very specific functions, but generally useful (could be here because of a
// lack in the APIs, or just sheer lazyness :))


#include <FindDirectory.h>
#include <Path.h>
#include <Picture.h>
#include <PictureButton.h>
#include <SupportDefs.h>
#include <TranslationUtils.h>
#include <TranslatorFormats.h>


status_t TestForAddonExistence(const char* name, directory_which which,
							   const char* section, BPath& outPath);

// Reference counted object
class Object {
public:
	// After construction reference count is 1
	Object() : fRefCount(1) { }
	// dtor should be private, but ie. ObjectList requires a public dtor!
	virtual ~Object() { };

	// thread-safe as long as thread that calls Acquire has already
	// a reference to the object
	void Acquire() {
		atomic_add(&fRefCount, 1);
	}

	bool Release() {
		if (atomic_add(&fRefCount, -1) == 1) {
			delete this;
			return true;
		} else
			return false;
	}

private:
	int32 fRefCount;
};

// Automatically send a reply to sender on destruction of object
// and delete sender
class AutoReply {
public:
	AutoReply(BMessage* sender, uint32 what);
	~AutoReply();

	void SetReply(BMessage* message) { fReply = *message; }

private:
	BMessage* fSender;
	BMessage  fReply;
};

// mimetype from sender
bool MimeTypeForSender(BMessage* sender, BString& mime);
// load bitmap from application resources
BBitmap* LoadBitmap(const char* name, uint32 type_code = B_TRANSLATOR_BITMAP);
// convert bitmap to picture; view must be attached to a window!
// returns NULL if bitmap is NULL
BPicture *BitmapToPicture(BView* view, BBitmap *bitmap);
BPicture *BitmapToGrayedPicture(BView* view, BBitmap *bitmap);

#endif
