/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINTERS_H
#define _PRINTERS_H


#include <Application.h>
#include <Catalog.h>

#include "ScreenSettings.h"


#define PRINTERS_SIGNATURE	"application/x-vnd.Be-PRNT"


class PrintersApp : public BApplication {
public:
								PrintersApp();

			void				ReadyToRun();
			void				MessageReceived(BMessage* message);
			void				ArgvReceived(int32 argc, char** argv);
};

#endif /* _PRINTERS_H */
