/*
 * Copyright 2003-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PrintTransportAddOn.h"


// We don't support multiple instances of the same transport add-on
static BDataIO* gTransport = NULL;


extern "C" _EXPORT BDataIO*
init_transport(BMessage* message)
{
	if (message == NULL || gTransport != NULL)
		return NULL;

	const char* spool_path = message->FindString("printer_file");

	if (spool_path != NULL && *spool_path != '\0') {
		BDirectory printer(spool_path);

		if (printer.InitCheck() == B_OK) {
			gTransport = instantiate_transport(&printer, message);
			return gTransport;
		}
	}

	return NULL;
}


extern "C" _EXPORT void
exit_transport()
{
	if (gTransport != NULL) {
		delete gTransport;
		gTransport = NULL;
	}
}
