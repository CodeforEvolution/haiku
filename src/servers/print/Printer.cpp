/*
 * Copyright 2001-2021, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */


#include "Printer.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Application.h>
#include <Autolock.h>
#include <Message.h>
#include <NodeMonitor.h>
#include <StorageKit.h>
#include <String.h>
#include <SupportDefs.h>

#include "BeUtils.h"
#include "pr_server.h"
#include "PrintAddOnServer.h"
#include "PrintServerApp.h"


SpoolFolder::SpoolFolder(BLocker* locker, BLooper* looper,
	const BDirectory& spoolDir)
	:
	Folder(locker, looper, spoolDir)
{
}


// Notify print_server that there is a job file waiting for printing
void
SpoolFolder::Notify(Job* job, int kind)
{
	if ((kind == kJobAdded || kind == kJobAttrChanged)
		&& job->IsValid() && job->IsWaiting())
		be_app_messenger.SendMessage(PSRV_PRINT_SPOOLED_JOB);
}


// ---------------------------------------------------------------
BObjectList<Printer> Printer::sPrinters;


// ---------------------------------------------------------------
// Find [static]
//
// Searches the static object list for a printer object with the
// specified name.
//
// Parameters:
//    name - Printer definition name we're looking for.
//
// Returns:
//    Pointer to Printer object, or NULL if not found.
// ---------------------------------------------------------------
Printer*
Printer::Find(const BString& name)
{
	// Look in list to find printer definition
	for (int32 index = 0; index < sPrinters.CountItems(); index++) {
		if (name == sPrinters.ItemAt(index)->Name())
			return sPrinters.ItemAt(index);
	}

	return NULL;
}


Printer*
Printer::Find(node_ref* node)
{
	node_ref curNode;
	// Look in list to find printer definition
	for (int32 index = 0; index < sPrinters.CountItems(); index++) {
		Printer* printer = sPrinters.ItemAt(index);
		printer->_SpoolDir()->GetNodeRef(&curNode);
		if (curNode == *node)
			return printer;
	}

	// None found, so return NULL
	return NULL;
}


Printer*
Printer::At(int32 index)
{
	return sPrinters.ItemAt(index);
}


void
Printer::Remove(Printer* printer)
{
	sPrinters.RemoveItem(printer);
}


int32
Printer::CountPrinters()
{
	return sPrinters.CountItems();
}


// ---------------------------------------------------------------
// Printer [constructor]
//
// Initializes the printer object with data read from the
// attributes attached to the printer definition node.
//
// Parameters:
//    node - Printer definition node for this printer.
//
// Returns:
//    none.
// ---------------------------------------------------------------
Printer::Printer(const BDirectory* node, Resource* resource)
	:
	BHandler(B_EMPTY_STRING),
	fPrinter(gLock, be_app, *node),
	fResource(resource),
	fSinglePrintThread(resource->NeedsLocking()),
	fJob(NULL),
	fProcessing(0),
	fAbort(false)
{
	BString name;
		// Set our name to the name of the passed node
	if (_SpoolDir()->ReadAttrString(PSRV_PRINTER_ATTR_PRINTER_NAME, &name)
		== B_OK)
		SetName(name.String());

		// Add us to the global list of known printer definitions
	sPrinters.AddItem(this);

	_ResetJobStatus();
}


Printer::~Printer()
{
	((PrintServerApp*)be_app)->NotifyPrinterDeletion(this);
}


void
Printer::MessageReceived(BMessage* message)
{
	switch(message->what) {
		case B_GET_PROPERTY:
		case B_SET_PROPERTY:
		case B_CREATE_PROPERTY:
		case B_DELETE_PROPERTY:
		case B_COUNT_PROPERTIES:
		case B_EXECUTE_PROPERTY:
			HandleScriptingCommand(message);
			break;

		default:
			BHandler::MessageReceived(message);
	}
}


// Remove printer spooler directory
status_t
Printer::Remove()
{
	status_t result = B_OK;
	BPath path;

	result = find_directory(B_USER_PRINTERS_DIRECTORY, &path);
	if (result == B_OK) {
		path.Append(Name());
		result = rmdir(path.Path());
	}

	return result;
}


status_t
Printer::FindPathToDriver(const char* driverName, BPath* path)
{
	return PrintAddOnServer::FindPathToDriver(driverName, path);
}


// ---------------------------------------------------------------
// ConfigurePrinter
//
// Handles calling the printer addon's add_printer function.
//
// Parameters:
//    driverName - the name of the printer driver add-on
//    printerName - the name of the printer spool folder
//
// Returns:
//    B_OK if successful or errorcode otherwise.
// ---------------------------------------------------------------
status_t
Printer::ConfigurePrinter(const char* driverName, const char* printerName)
{
	PrintAddOnServer addOn(driverName);
	return addOn.AddPrinter(printerName);
}


// ---------------------------------------------------------------
// ConfigurePage
//
// Handles calling the printer addon's config_page function.
//
// Parameters:
//    settings - Page settings to display. The contents of this
//               message will be replaced with the new settings
//               if the function returns success.
//
// Returns:
//    B_OK if successful or errorcode otherwise.
// ---------------------------------------------------------------
status_t
Printer::ConfigurePage(BMessage& settings)
{
	BString driver;
	status_t result = _GetDriverName(&driver);
	if (result != B_OK)
		return result;

	PrintAddOnServer addOn(driver.String());
	result = addOn.ConfigPage(_SpoolDir(), &settings);
	if (result == B_OK)
		_AddCurrentPrinter(settings);

	return result;
}


// ---------------------------------------------------------------
// ConfigureJob
//
// Handles calling the printer addon's config_job function.
//
// Parameters:
//    settings - Job settings to display. The contents of this
//               message will be replaced with the new settings
//               if the function returns success.
//
// Returns:
//    B_OK if successful or errorcode otherwise.
// ---------------------------------------------------------------
status_t
Printer::ConfigureJob(BMessage& settings)
{
	BString driver;
	status_t result = _GetDriverName(&driver);
	if (result != B_OK)
		return result;

	PrintAddOnServer addOn(driver.String());
	result = addOn.ConfigJob(_SpoolDir(), &settings);
	if (result == B_OK)
		_AddCurrentPrinter(settings);

	return result;
}


// ---------------------------------------------------------------
// HandleSpooledJobs
//
// Print spooled jobs in a new thread.
// ---------------------------------------------------------------
void
Printer::HandleSpooledJob()
{
	BAutolock lock(gLock);
	if (lock.IsLocked()
		&& (!fSinglePrintThread || fProcessing == 0) && _FindSpooledJob())
		_StartPrintThread();
}


// ---------------------------------------------------------------
// GetDefaultSettings
//
// Retrieve the default configuration message from printer add-on
//
// Parameters:
//   settings, output paramter.
//
// Returns:
//    B_OK if successful or errorcode otherwise.
// ---------------------------------------------------------------
status_t
Printer::GetDefaultSettings(BMessage& settings)
{
	BString driver;
	status_t result = _GetDriverName(&driver);
	if (result != B_OK)
		return result;

	PrintAddOnServer addOn(driver.String());
	result = addOn.DefaultSettings(_SpoolDir(), &settings);
	if (result == B_OK)
		_AddCurrentPrinter(settings);

	return result;
}


void
Printer::AbortPrintThread()
{
	fAbort = true;
}


status_t
Printer::_GetDriverName(BString* name)
{
	return _SpoolDir()->ReadAttrString(PSRV_PRINTER_ATTR_DRIVER_NAME, name);
}


// ---------------------------------------------------------------
// AddCurrentPrinter
//
// Add printer name to message.
//
// Parameters:
//    message - message.
// ---------------------------------------------------------------
void
Printer::_AddCurrentPrinter(BMessage& message)
{
	BString name;
	GetName(name);

	message.RemoveName(PSRV_FIELD_CURRENT_PRINTER);
	message.AddString(PSRV_FIELD_CURRENT_PRINTER, name.String());
}


// ---------------------------------------------------------------
// GetName
//
// Get the name from spool directory.
//
// Parameters:
//    name - the name of the printer.
// ---------------------------------------------------------------
void
Printer::GetName(BString& name)
{
	if (_SpoolDir()->ReadAttrString(PSRV_PRINTER_ATTR_PRINTER_NAME, &name)
		!= B_OK)
		name = "Unknown Printer";
}


// ---------------------------------------------------------------
// ResetJobStatus
//
// Reset status of "processing" jobs to "waiting" at print_server start.
// ---------------------------------------------------------------
void
Printer::_ResetJobStatus()
{
	if (fPrinter.Lock()) {
		const int32 jobCount = fPrinter.CountJobs();
		for (int32 i = 0; i < jobCount; i++) {
			Job* job = fPrinter.JobAt(i);
			if (job->Status() == kProcessing)
				job->SetStatus(kWaiting);
		}
		fPrinter.Unlock();
	}
}


// ---------------------------------------------------------------
// HasCurrentPrinter
//
// Try to read the printer name from job file.
//
// Parameters:
//    name - the printer name.
//
// Returns:
//    true if successful.
// ---------------------------------------------------------------
bool
Printer::_HasCurrentPrinter(BString& name)
{
	BMessage settings;
		// Read settings from spool file and get printer name
	BFile jobFile(&fJob->EntryRef(), B_READ_WRITE);
	return jobFile.InitCheck() == B_OK
		&& jobFile.Seek(sizeof(print_file_header), SEEK_SET) ==
			sizeof(print_file_header)
		&& settings.Unflatten(&jobFile) == B_OK
		&& settings.FindString(PSRV_FIELD_CURRENT_PRINTER, &name) == B_OK;
}


// ---------------------------------------------------------------
// MoveJob
//
// Try to move job to another printer folder.
//
// Parameters:
//    name - the printer folder name.
//
// Returns:
//    true if successful.
// ---------------------------------------------------------------
bool
Printer::_MoveJob(const BString& name)
{
	BPath file(&fJob->EntryRef());
	BPath path;

	file.GetParent(&path);
	path.Append("..");
	path.Append(name.String());
	BDirectory dir(path.Path());

	BEntry entry(&fJob->EntryRef());
		// Try to move job file to proper directory
	return entry.MoveTo(&dir) == B_OK;
}


// ---------------------------------------------------------------
// FindSpooledJob
//
// Looks if there is a job waiting to be processed and moves
// jobs to the proper printer folder.
//
// Note:
//       Our implementation of BPrintJob moves jobs to the
//       proper printer folder.
//
//
// Returns:
//    true if there is a job present in fJob.
// ---------------------------------------------------------------
bool
Printer::_FindSpooledJob()
{
	BString name2;
	GetName(name2);
	do {
		fJob = fPrinter.GetNextJob();
		if (fJob != NULL) {
			BString name;
			if (_HasCurrentPrinter(name) && name != name2 && _MoveJob(name)) {
					// Job in wrong printer folder moved to apropriate one
				fJob->SetStatus(kUnknown, false);
					// So that fPrinter.GetNextJob skips it
				fJob->Release();
			} else {
					// Job found
				fJob->SetPrinter(this);
				return true;
			}
		}
	} while (fJob != NULL);

	return false;
}


// ---------------------------------------------------------------
// PrintSpooledJob
//
// Loads the printer add-on and calls its take_job function with
// the spool file as argument.
//
// Parameters:
//    spoolFile - the path to the spool file.
//
// Returns:
//    B_OK if successful.
// ---------------------------------------------------------------
status_t
Printer::_PrintSpooledJob(const char* spoolFile)
{
	BString driver;
	status_t result = _GetDriverName(&driver);
	if (result != B_OK)
		return result;

	PrintAddOnServer addOn(driver.String());
	return addOn.TakeJob(spoolFile, _SpoolDir());
}


// ---------------------------------------------------------------
// PrintThread
//
// Loads the printer add-on and calls its take_job function with
// the spool file as argument.
//
// Parameters:
//    job - the spool job.
// ---------------------------------------------------------------
void
Printer::_PrintThread(Job* job)
{
	fResource->Lock();
		// Wait until resource is available
	bool failed = true;

	if (!fAbort) {
		// Can we continue?
		BPath path;
		bool canOpenFile;
		{
			BEntry entry(&job->EntryRef());
			path.SetTo(&entry);
			BFile jobFile(path.Path(), B_READ_WRITE);
			canOpenFile = jobFile.InitCheck() == B_OK;
		}

		if (canOpenFile && _PrintSpooledJob(path.Path()) == B_OK) {
			// Tell the printer to print the spooled job
			job->Remove();
				// Remove spool file if printing was successful.
			failed = false;
		}
	}
		// Set status of spooled job on error
	if (failed)
		job->SetStatus(kFailed);

	fResource->Unlock();
	job->Release();
	atomic_add(&fProcessing, -1);
	Release();

	be_app_messenger.SendMessage(PSRV_PRINT_SPOOLED_JOB);
		// Notify print_server to process next spooled job
}


// ---------------------------------------------------------------
// print_thread
//
// Print thread entry, calls PrintThread with spool job.
//
// Parameters:
//    data - spool job.
//
// Returns:
//    0 always.
// ---------------------------------------------------------------
status_t
Printer::_print_thread(void* data)
{
	Job* job = static_cast<Job*>(data);
	job->GetPrinter()->_PrintThread(job);

	return 0;
}


// ---------------------------------------------------------------
// StartPrintThread
//
// Sets the status of the current spool job to "processing" and
// starts the print_thread.
// ---------------------------------------------------------------
void
Printer::_StartPrintThread()
{
	Acquire();
	thread_id tid = spawn_thread(_print_thread, "print", B_NORMAL_PRIORITY,
		(void*)fJob);
	if (tid > 0) {
		fJob->SetStatus(kProcessing);
		atomic_add(&fProcessing, 1);
		resume_thread(tid);
	} else {
		fJob->Release();
		Release();
	}
}

