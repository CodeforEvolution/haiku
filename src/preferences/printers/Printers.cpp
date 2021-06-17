/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "Printers.h"

#include <Locale.h>

#include "Messages.h"
#include "pr_server.h"
#include "PrintersWindow.h"
#include "ScreenSettings.h"


int
main()
{
	PrintersApp app;
	app.Run();
	return 0;
}


PrintersApp::PrintersApp()
	:
	BApplication(PRINTERS_SIGNATURE)
{
}


void
PrintersApp::ReadyToRun()
{
	PrintersWindow* window = new PrintersWindow(new ScreenSettings());
	window->Show();
}


void
PrintersApp::MessageReceived(BMessage* message)
{
	if (message->what == B_PRINTER_CHANGED
		|| message->what == PRINTERS_ADD_PRINTER) {
		// Broadcast message
		uint32 what = message->what;
		if (what == PRINTERS_ADD_PRINTER)
			what = kMsgAddPrinter;

		BWindow* window = NULL;
		for (int32 index = 0; (window = WindowAt(index)) != NULL; index++) {
			BMessenger messenger(NULL, window);
			messenger.SendMessage(what);
		}
	} else
		BApplication::MessageReceived(message);
}


void
PrintersApp::ArgvReceived(int32 argc, char** argv)
{
	for (int index = 1; index < argc; index++) {
		// TODO: show a pre-filled add printer dialog here
	}
}
