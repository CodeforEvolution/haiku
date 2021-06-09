/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "Transport.h"

#include <DataIO.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <image.h>
#include <Message.h>
#include <Path.h>

#include "DbgMsg.h"
#include "PrinterData.h"


TransportException::TransportException(const BString& what)
	:
	fWhat(what)
{
}


const char*
TransportException::What() const
{
	return fWhat.String();
}


Transport::Transport(const PrinterData* printerData)
	:
	fImage(-1),
	fInitTransport(NULL),
	fExitTransport(NULL),
	fDataStream(NULL),
	fAbort(false)
{
	const directory_which paths[] = {
		B_USER_NONPACKAGED_ADDONS_DIRECTORY,
		B_USER_ADDONS_DIRECTORY,
		B_SYSTEM_NONPACKAGED_ADDONS_DIRECTORY,
		B_BEOS_ADDONS_DIRECTORY,
	};

	BPath path;
	for (uint32 i = 0; i < B_COUNT_OF(paths); ++i) {
		if (find_directory(paths[i], &path) != B_OK)
			continue;

		path.Append("Print/transport");
		path.Append(printerData->GetTransport().String());

		DBGMSG(("load_add_on: %s\n", path.Path()));
		fImage = load_add_on(path.Path());
		if (fImage >= 0)
			break;
	}

	if (fImage < 0) {
		SetLastError("cannot load a transport add-on");
		return;
	}

	DBGMSG(("image id = %d\n", (int)fImage));

	get_image_symbol(fImage, "init_transport", B_SYMBOL_TYPE_TEXT,
		(void**)&fInitTransport);
	get_image_symbol(fImage, "exit_transport", B_SYMBOL_TYPE_TEXT,
		(void**)&fExitTransport);

	if (fInitTransport == NULL) {
		SetLastError("get_image_symbol failed.");
		DBGMSG(("init_transport is NULL\n"));
	}

	if (fExitTransport == NULL) {
		SetLastError("get_image_symbol failed.");
		DBGMSG(("exit_transport is NULL\n"));
	}

	if (fInitTransport != NULL) {
		BString spoolPath;
		printerData->GetPath(spoolPath);
		BMessage* message = new BMessage('TRIN');
		message->AddString("printer_file", spoolPath.String());
		fDataStream = (*fInitTransport)(message);
		delete message;
		if (fDataStream == NULL)
			SetLastError("init_transport failed.");
	}
}


Transport::~Transport()
{
	if (fExitTransport != NULL)
		(*fExitTransport)();

	if (fImage >= 0)
		unload_add_on(fImage);
}


bool
Transport::CheckAbort() const
{
	return fDataStream == NULL;
}


const
BString& Transport::LastError() const
{
	return fLastErrorString;
}


bool
Transport::IsPrintToFileCanceled() const
{
	// The BeOS "Print To File" transport add-on returns a non-NULL BDataIO*
	// even after user filepanel cancellation!
	BFile* file = dynamic_cast<BFile*>(fDataStream);
	return file != NULL && file->InitCheck() != B_OK;
}


void
Transport::SetLastError(const char* error)
{
	fLastErrorString = error;
	fAbort = true;
}


void
Transport::Write(const void* buffer, size_t size)
{
	if (fDataStream != NULL) {
		if (size == (size_t)fDataStream->Write(buffer, size))
			return;

		SetLastError("BDataIO::Write failed.");
	}

	throw TransportException(LastError());
}


void
Transport::Read(void* buffer, size_t size)
{
	if (fDataStream != NULL) {
		if (size == (size_t)fDataStream->Read(buffer, size))
			return;

		SetLastError("BDataIO::Read failed.");
	}

	throw TransportException(LastError());
}
