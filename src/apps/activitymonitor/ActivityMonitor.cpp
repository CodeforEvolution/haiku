/*
 * Copyright 2008-2019 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Dörfler, axeld@pinc-software.de
 *		Jacob Secunda
 */


#include "ActivityMonitor.h"

#include <stdlib.h>

#include <Application.h>
#include <Catalog.h>

#include <AboutWindow.h>

#include "ActivityWindow.h"

const char* kAppName = B_TRANSLATE_SYSTEM_NAME("ActivityMonitor");
const char* kSignature = "application/x-vnd.Haiku-ActivityMonitor";


ActivityMonitor::ActivityMonitor()
	:
	BApplication(kSignature)
{
	fWindow = new ActivityWindow();
}


ActivityMonitor::~ActivityMonitor()
{
}


void
ActivityMonitor::ReadyToRun()
{
	fWindow->Show();
}


void
ActivityMonitor::RefsReceived(BMessage* message)
{
	fWindow->PostMessage(message);
}


void
ActivityMonitor::MessageReceived(BMessage* message)
{
	BApplication::MessageReceived(message);
}


void
ActivityMonitor::AboutRequested()
{
	const char* authors[] = {
		"Axel Dörfler",
		"Jacob Secunda",
		NULL
	};

	BAboutWindow* window = new BAboutWindow(kAppName, kSignature);
	window->AddCopyright(2008, "Haiku, Inc.");
	window->AddDescription(B_TRANSLATE("A system resource monitor."));
	window->AddAuthors(authors);
	window->Show();
}


//	#pragma mark -


int
main(int /*argc*/, char** /*argv*/)
{
	ActivityMonitor app;
	app.Run();

	return 0;
}
