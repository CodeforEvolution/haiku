/*
 * Copyright 2009-2019 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Dörfler, axeld@pinc-software.de
 *		Jacob Secunda
 */


#include "CharacterMap.h"

#include <stdlib.h>

#include <Application.h>
#include <Catalog.h>

#include <AboutWindow.h>

#include "CharacterWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "CharacterMap"

const char* kAppName = B_TRANSLATE_SYSTEM_NAME("CharacterMap");
const char* kSignature = "application/x-vnd.Haiku-CharacterMap";


CharacterMap::CharacterMap()
	:
	BApplication(kSignature)
{
}


CharacterMap::~CharacterMap()
{
}


void
CharacterMap::ReadyToRun()
{
	fWindow = new CharacterWindow();
	fWindow->Show();
}


void
CharacterMap::RefsReceived(BMessage* message)
{
	fWindow->PostMessage(message);
}


void
CharacterMap::MessageReceived(BMessage* message)
{
	BApplication::MessageReceived(message);
}


void
CharacterMap::AboutRequested()
{
	const char* authors[] = {
		"Axel Dörfler",
		"Jacob Secunda",
		NULL
	};

	BAboutWindow* window = new BAboutWindow(kAppName, kSignature);
	window->AddCopyright(2009, "Haiku, Inc.");
	window->AddDescription(B_TRANSLATE("A viewer for UTF-8 characters "
									   "within fonts."));
	window->AddAuthors(authors);
	window->Show();
}


//	#pragma mark -


int
main(int /*argc*/, char** /*argv*/)
{
	CharacterMap app;
	app.Run();

	return 0;
}
