/*
 * Copyright 2004-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 *		Philippe Houdoin
 */


#include "PrintTransport.h"

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <String.h>


// Implementation of class PrintTransport
PrintTransport::PrintTransport()
	:
	fDataIO(NULL),
	fAddOnID(-1),
	fExitProc(NULL)
{
}


PrintTransport::~PrintTransport()
{
	if (fExitProc != NULL) {
		(*fExitProc)();
		fExitProc = NULL;
	}

	if (fAddOnID >= 0) {
		unload_add_on(fAddOnID);
		fAddOnID = -1;
	}
}


status_t
PrintTransport::Open(BNode* printerFolder)
{
	// Already opened?
	if (fDataIO != NULL)
		return B_ERROR;

	// retrieve transport add-on name from printer folder attribute
	BString transportName;
	if (printerFolder->ReadAttrString("transport", &transportName) != B_OK)
		return B_ERROR;

	const directory_which paths[] = {
		B_USER_NONPACKAGED_ADDONS_DIRECTORY,
		B_USER_ADDONS_DIRECTORY,
		B_SYSTEM_NONPACKAGED_ADDONS_DIRECTORY,
		B_SYSTEM_ADDONS_DIRECTORY,
	};

	BPath path;
	for (uint32 i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
		if (find_directory(paths[i], &path) != B_OK)
			continue;

		path.Append("Print/transport");
		path.Append(transportName.String());

		fAddOnID = load_add_on(path.Path());
		if (fAddOnID >= 0)
			break;
	}

	// Failed to load transport add-on
	if (fAddOnID < 0)
		return B_ERROR;

	// Get init & exit proc
	BDataIO* (*initProc)(BMessage*);
	get_image_symbol(fAddOnID, "init_transport", B_SYMBOL_TYPE_TEXT,
		(void **) &initProc);
	get_image_symbol(fAddOnID, "exit_transport", B_SYMBOL_TYPE_TEXT,
		(void **) &fExitProc);


	// Transport add-on doesn't have the proper interface
	if (initProc == NULL || fExitProc == NULL)
		return B_ERROR;

	// Now, initialize the transport add-on
	node_ref ref;
	BDirectory dir;

	printerFolder->GetNodeRef(&ref);
	dir.SetTo(&ref);

	if (path.SetTo(&dir, NULL) != B_OK)
		return B_ERROR;

	// Request BDataIO object from transport add-on
	BMessage input('TRIN');
	input.AddString("printer_file", path.Path());
	fDataIO = (*initProc)(&input);

	return B_OK;
}


BDataIO*
PrintTransport::GetDataIO()
{
	return fDataIO;
}


bool
PrintTransport::IsPrintToFileCanceled() const
{
	// The BeOS "Print To File" transport add-on returns a non-NULL BDataIO*
	// even after user filepanel cancellation!
	BFile* file = dynamic_cast<BFile*>(fDataIO);
	return fDataIO == NULL || (file != NULL && file->InitCheck() != B_OK);
}
