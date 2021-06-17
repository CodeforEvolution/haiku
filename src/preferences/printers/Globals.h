/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _GLOBALS_H
#define _GLOBALS_H


#include <Messenger.h>
#include <String.h>


BString		ActivePrinterName();
status_t	GetPrinterServerMessenger(BMessenger& messenger);

#endif // _GLOBALS_H
