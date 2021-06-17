/*
 * Copyright 2001-2021, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */
#ifndef _PRINTER_H
#define _PRINTER_H


#include <Directory.h>
#include <Handler.h>
#include <image.h>
#include <Locker.h>
#include <ObjectList.h>
#include <PrintJob.h>
#include <String.h>

#include "BeUtils.h"
#include "Jobs.h"
#include "ResourceManager.h"


class Printer;


class SpoolFolder : public Folder {
public:
								SpoolFolder(BLocker* locker, BLooper* looper,
									const BDirectory& spoolDir);

protected:
			void				Notify(Job* job, int kind);
};


/*****************************************************************************/
// Printer
//
// This class represents one printer definition. It is manages all actions &
// data related to that printer.
/*****************************************************************************/
class Printer : public BHandler, public Object
{
public:
								Printer(const BDirectory* node,
									Resource* resource);
	virtual						~Printer();

	virtual	void				MessageReceived(BMessage* message);
	virtual	status_t			GetSupportedSuites(BMessage* message);
	virtual	BHandler*			ResolveSpecifier(BMessage* message, int32 index,
									BMessage* spec, int32 form,
									const char* prop);

			// Static helper functions
	static	Printer*			Find(const BString& name);
	static	Printer*			Find(node_ref* node);
	static	Printer*			At(int32 index);
	static	void				Remove(Printer* printer);
	static	int32				CountPrinters();

			status_t			Remove();
	static	status_t			FindPathToDriver(const char* driver,
									BPath* path);
	static	status_t			ConfigurePrinter(const char* driverName,
									const char* printerName);
			status_t			ConfigureJob(BMessage& ioSettings);
			status_t			ConfigurePage(BMessage& ioSettings);
			status_t			GetDefaultSettings(BMessage& configuration);

			// Try to start processing of next spooled job
			void				HandleSpooledJob();

			// Abort print_thread without processing spooled job
			void				AbortPrintThread();

			// Scripting support, see Printer.Scripting.cpp
			void				HandleScriptingCommand(BMessage* message);

			void				GetName(BString& name);
			Resource*			GetResource() { return fResource; }

private:
			status_t			_GetDriverName(BString* name);
			void				_AddCurrentPrinter(BMessage& message);

			BDirectory*			_SpoolDir() { return fPrinter.GetSpoolDir(); }

			void				_ResetJobStatus();
			bool				_HasCurrentPrinter(BString& name);
			bool				_MoveJob(const BString& name);

			// Get next spooled job if any
			bool				_FindSpooledJob();
			status_t			_PrintSpooledJob(const char* spoolFile);
			void				_PrintThread(Job* job);

	static	status_t			_print_thread(void* data);
			void				_StartPrintThread();

private:
			// The printer spooling directory
			SpoolFolder			fPrinter;
			// The resource required for processing a print job
			Resource*			fResource;
			// Is printer add-on allowed to process multiple print job at once
			bool				fSinglePrintThread;
			// The next job to process
			Job*				fJob;
			// The current nmber of processing threads
			int32				fProcessing;
			// Stop processing
			bool				fAbort;
	static	BObjectList<Printer> sPrinters;
};

#endif /* _PRINTER_H */
