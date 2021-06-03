/*
 * Copyright 2010 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PrintAddOnServerProtocol.h"


const char* kPrintAddOnServerApplicationSignature =
	"application/x-vnd.haiku-print-addon-server";

const char* kPrintAddOnServerStatusAttribute = "status";
const char* kPrinterDriverAttribute = "driver";
const char* kPrinterNameAttribute = "name";
const char* kPrinterFolderAttribute = "folder";
const char* kPrintJobFileAttribute = "job";
const char* kPrintSettingsAttribute = "settings";
