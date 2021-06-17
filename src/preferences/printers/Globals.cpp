/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "Globals.h"

#include <stdio.h>

#include <Roster.h>

#include "pr_server.h"


BString
ActivePrinterName()
{
	BMessenger messenger;
	if (GetPrinterServerMessenger(messenger) != B_OK)
		return BString();

	BMessage getNameOfActivePrinter(B_GET_PROPERTY);
	getNameOfActivePrinter.AddSpecifier("ActivePrinter");

	BMessage reply;
	messenger.SendMessage(&getNameOfActivePrinter, &reply);

	BString activePrinterName;
	reply.FindString("result", &activePrinterName);

	return activePrinterName;
}


status_t
GetPrinterServerMessenger(BMessenger& messenger)
{
	messenger = BMessenger(PSRV_SIGNATURE_TYPE);
	return messenger.IsValid() ? B_OK : B_ERROR;
}
