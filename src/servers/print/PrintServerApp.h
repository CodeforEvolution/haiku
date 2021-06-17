/*
 * Copyright 2001-2021, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */
#ifndef _PRINT_SERVER_APP_H
#define _PRINT_SERVER_APP_H


#include <Bitmap.h>
#include <Catalog.h>
#include <OS.h>
#include <Server.h>
#include <String.h>

#include "FolderWatcher.h"
#include "ResourceManager.h"
#include "Settings.h"


class Printer;
class Transport;


// The global BLocker for synchronisation.
extern BLocker* gLock;


// The print_server application.
class PrintServerApp : public BServer, public FolderListener {
public:
								PrintServerApp(status_t* error);
								~PrintServerApp();

			void				Acquire();
			void				Release();

	virtual	bool				QuitRequested();
	virtual	void				MessageReceived(BMessage* message);
			void				NotifyPrinterDeletion(Printer* printer);

	// Scripting support, see PrintServerApp.Scripting.cpp
	virtual	status_t			GetSupportedSuites(BMessage* message);
			void				HandleScriptingCommand(BMessage* message);
			Printer*			GetPrinterFromSpecifier(BMessage* message);
			Transport*			GetTransportFromSpecifier(BMessage* message);
	virtual	BHandler*			ResolveSpecifier(BMessage* message, int32 index,
									BMessage* specifier, int32 form,
									const char* property);

private:
			bool				_OpenSettings(BFile& file, const char* name,
									bool forReading);
			void				_LoadSettings();
			void				_SaveSettings();

			status_t			_SetupPrinterList();

			void				_HandleSpooledJobs();

			status_t			_SelectPrinter(const char* printerName);
			status_t			_CreatePrinter(const char* printerName,
									const char* driverName,
									const char* connection,
									const char* transportName,
									const char* transportPath);

			void				_RegisterPrinter(BDirectory* node);
			void				_UnregisterPrinter(Printer* printer);

	// FolderListener
			void				_EntryCreated(node_ref* node, entry_ref* entry);
			void				_EntryRemoved(node_ref* node);
			void				_AttributeChanged(node_ref* node);

			status_t			_StoreDefaultPrinter();
			status_t			_RetrieveDefaultPrinter();

			status_t			_FindPrinterNode(const char* name, BNode& node);

	// "Classic" BeOS R5 support, see PrintServerApp.R5.cpp
	static	status_t			_async_thread(void* data);
			void				_AsyncHandleMessage(BMessage* message);
			void				_Handle_BeOSR5_Message(BMessage* message);

private:
			ResourceManager		fResourceManager;
			Printer*			fDefaultPrinter;
			size_t				fIconSize;
			uint8*				fSelectedIcon;
			int32				fReferences;
			sem_id				fHasReferences;
			Settings*			fSettings;
			bool				fUseConfigWindow;
			FolderWatcher*		fFolder;
};

#endif	/* _PRINT_SERVER_APP_H */
