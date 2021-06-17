/*
 * Copyright 2010-2021 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINT_ADD_ON_SERVER_H
#define _PRINT_ADD_ON_SERVER_H


#include <Directory.h>
#include <Entry.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <String.h>
#include <SupportDefs.h>


class PrintAddOnServer {
public:
								PrintAddOnServer(const char* driver);
	virtual						~PrintAddOnServer();

			status_t			AddPrinter(const char* spoolFolderName);
			status_t			ConfigPage(BDirectory* spoolFolder,
									BMessage* settings);
			status_t			ConfigJob(BDirectory* spoolFolder,
									BMessage* settings);
			status_t			DefaultSettings(BDirectory* spoolFolder,
									BMessage* settings);
			status_t			TakeJob(const char* spoolFile,
									BDirectory* spoolFolder);

	static	status_t			FindPathToDriver(const char* driver,
									BPath* path);

private:
			const char*			_Driver() const;

			status_t			_Launch(BMessenger& messenger);
			bool				_IsLaunched();
			void				_Quit();

			void				_AddDirectory(BMessage& message,
									const char* name, BDirectory* directory);
			void				_AddEntryRef(BMessage& message,
									const char* name,
									const entry_ref* entryRef);

			status_t			_SendRequest(BMessage& request,
									BMessage& reply);
			status_t			_GetResult(BMessage& reply);
			status_t			_GetResultAndUpdateSettings(BMessage& reply,
									BMessage* settings);

private:
			BString				fDriver;
			status_t			fLaunchStatus;
			BMessenger			fMessenger;
};

#endif /* _PRINT_ADD_ON_SERVER_H */
