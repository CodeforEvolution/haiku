/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Marcus Overhagen
 *		Axel Dörfler, axeld@pinc-software.de
 *		Jacob Secunda
 */


#include <MediaTheme.h>

#include <string.h>

#include <Autolock.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <image.h>
#include <Locker.h>
#include <Path.h>
#include <StringView.h>

#include "DefaultMediaTheme.h"
#include "MediaDebug.h"


static BLocker sLock("BMediaTheme preferred theme lock");
BMediaTheme* BMediaTheme::sDefaultTheme;


BMediaTheme::~BMediaTheme()
{
	CALLED();

	free(fName);
	free(fInfo);
}


const char*
BMediaTheme::Name()
{
	return fName;
}


const char*
BMediaTheme::Info()
{
	return fInfo;
}


int32
BMediaTheme::ID()
{
	return fID;
}


bool
BMediaTheme::GetRef(entry_ref* ref)
{
	if (fIsAddOn == false || ref == NULL)
		return false;

	*ref = fAddOnRef;
	return true;
}


BView*
BMediaTheme::ViewFor(BParameterWeb* web, const BRect* hintRect,
	BMediaTheme* usingTheme)
{
	CALLED();

	BMediaTheme* finalTheme = NULL;

	// Use the preferred theme if none was specified
	if (usingTheme != NULL)
		finalTheme = usingTheme;
	else
		finalTheme = PreferredTheme();

	// Oh no...
	if (finalTheme == NULL)
		return NULL;

	return finalTheme->MakeViewFor(web, hintRect);
}


status_t
BMediaTheme::SetPreferredTheme(BMediaTheme* newPreferredTheme)
{
	CALLED();

	status_t result = B_ERROR;

	BAutolock locker(sLock);
	if (!locker.IsLocked())
		return result;

	// We have a request to utilize the default theme and we're already using
	// it...nothing to do for us!
	if (newPreferredTheme == NULL
		&& dynamic_cast<BPrivate::DefaultMediaTheme*>(sDefaultTheme) != NULL)
		return B_OK;

	// This method takes possession of the BMediaTheme passed, even
	// if it fails, so it has to delete it.
	delete sDefaultTheme;

	// If newPreferredTheme is NULL, we use the builtin Haiku theme
	if (newPreferredTheme == NULL)
		sDefaultTheme = new BPrivate::DefaultMediaTheme();
	else
		sDefaultTheme = newPreferredTheme;

	// Construct a BMessage that will store the preferred theme's information.
	BMessage settings;

	if (newPreferredTheme == NULL)
		settings.AddBool("haiku:system_theme", true);
	else {
		settings.AddBool("haiku:system_theme", false);
		settings.AddInt32("haiku:theme_id", sDefaultTheme->ID());

		entry_ref themeRef;
		if (sDefaultTheme->GetRef(&themeRef) != false)
			settings.AddRef("haiku:theme_ref", &themeRef);
	}

	// Find and open the file that will store the info about the the set
	// preferred theme.
	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) != B_OK)
		settingsPath.SetTo("/boot/home/config/settings");
	settingsPath.Append("Media/MediaTheme_settings");

	BFile settingsFile = BFile(settingsPath.Path(), B_WRITE_ONLY |
		B_CREATE_FILE | B_ERASE_FILE);
	result = settingsFile.InitCheck();
	if (result != B_OK)
		return result;

	result = settings.Flatten(&settingsFile);

	return result;
}


BMediaTheme*
BMediaTheme::PreferredTheme()
{
	CALLED();

	status_t result = B_ERROR;

	BAutolock locker(sLock);
	if (!locker.IsLocked())
		return NULL;

	if (sDefaultTheme != NULL)
		return sDefaultTheme;

	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) != B_OK)
		settingsPath.SetTo("/boot/home/config/settings");
	settingsPath.Append("Media/MediaTheme_settings");

	BFile settingsFile = BFile(settingsPath.Path(), B_READ_ONLY);
	result = settingsFile.InitCheck();
	if (result == B_OK) {
		BMessage settings;

		result = settings.Unflatten(&settingsFile);
		if (result == B_OK) {
			bool isSystemTheme = false;
			settings.GetBool("haiku:system_theme", isSystemTheme, true);

			if (isSystemTheme == false) {
				int32 themeID = -1;
				settings.FindInt32("haiku:theme_id", themeID);

				entry_ref themeRef;
				settings.FindRef("haiku:theme_ref", &themeRef);
				BEntry themeEntry = BEntry(&themeRef);

				if (themeEntry.InitCheck() == B_OK) {
					BPath themePath;
					themeEntry.GetPath(&themePath);

					image_id theme = load_add_on(themePath.Path());

					BMediaTheme* (*make_theme)(int32, image_id);
					result = get_image_symbol(theme, "make_theme",
						B_SYMBOL_TYPE_TEXT, (void**)&make_theme);
					if (result == B_OK)
						sDefaultTheme = make_theme(themeID, theme);

					unload_add_on(theme);
				}
			}
		}
	}

	// It's time for the system theme to shine!
	if (sDefaultTheme == NULL)
		sDefaultTheme = new BPrivate::DefaultMediaTheme();

	return sDefaultTheme;
}


BBitmap*
BMediaTheme::BackgroundBitmapFor(bg_kind bg)
{
	UNIMPLEMENTED();

	// TODO: Check BeOS Behaviour?
	// BeOS calls both BTranslationUtils::GetBitmap and constructs a BBitmap
	// in this method but doesn't return any unique bitmap for any bg_kind...

	BBitmap* bgBitmap = NULL;

	switch (bg) {
		case B_CONTROL_BG:
		case B_HILITE_BG:
		case B_PRESENTATION_BG:
		case B_EDIT_BG:
		case B_SETTINGS_BG:
		case B_GENERAL_BG:
		default:
			break;
	}

	return bgBitmap;
}


rgb_color
BMediaTheme::BackgroundColorFor(bg_kind bg)
{
	// TODO: Check BeOS Behaviour? Any second opinions here?
	// BeOS uses BScreen's ColorForIndex for some reason in this method...

	color_which which = B_NO_COLOR;

	switch (bg) {
		case B_CONTROL_BG:
			which = B_CONTROL_BACKGROUND_COLOR;
			break;
		case B_HILITE_BG:
			which = B_CONTROL_HIGHLIGHT_COLOR;
			break;
		case B_PRESENTATION_BG:
		case B_EDIT_BG:
			which = B_DOCUMENT_BACKGROUND_COLOR;
			break;
		case B_SETTINGS_BG:
		case B_GENERAL_BG:
		default:
			which = B_PANEL_BACKGROUND_COLOR;
			break;
	}

	return ui_color(which);
}


rgb_color
BMediaTheme::ForegroundColorFor(fg_kind fg)
{
	// TODO: Check BeOS Behaviour? Any second opinions here?
	// BeOS uses BScreen's ColorForIndex for some reason in this method...

	color_which which = B_NO_COLOR;

	switch (fg) {
		case B_CONTROL_FG:
			which = B_CONTROL_TEXT_COLOR;
			break;
		case B_HILITE_FG:
			which = B_CONTROL_HIGHLIGHT_COLOR;
			break;
		case B_PRESENTATION_FG:
		case B_EDIT_FG:
			which = B_DOCUMENT_TEXT_COLOR;
			break;
		case B_SETTINGS_FG:
		case B_GENERAL_FG:
		default:
			which = B_PANEL_TEXT_COLOR;
			break;
	}

	return ui_color(which);
}


//! protected BMediaTheme
BMediaTheme::BMediaTheme(const char* name, const char* info,
	const entry_ref* ref, int32 id)
	:
	fID(id)
{
	fName = name != NULL ? strdup(name) : strdup(B_EMPTY_STRING);
	fInfo = info != NULL ? strdup(info) : strdup(B_EMPTY_STRING);

	if (ref != NULL) {
		fIsAddOn = true;
		fAddOnRef = *ref;
	} else
		fIsAddOn = false;
}


BControl*
BMediaTheme::MakeFallbackViewFor(BParameter* parameter)
{
	if (parameter == NULL)
		return NULL;

	return BPrivate::DefaultMediaTheme::MakeViewFor(parameter);
}


/*
private unimplemented
BMediaTheme::BMediaTheme()
BMediaTheme::BMediaTheme(const BMediaTheme& clone)
BMediaTheme& BMediaTheme::operator=(const BMediaTheme& clone)
*/

status_t BMediaTheme::_Reserved_ControlTheme_0(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_1(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_2(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_3(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_4(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_5(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_6(void*) { return B_ERROR; }
status_t BMediaTheme::_Reserved_ControlTheme_7(void*) { return B_ERROR; }

