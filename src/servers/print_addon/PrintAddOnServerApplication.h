/*
 * Copyright 2010-2021 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINT_ADD_ON_SERVER_H
#define _PRINT_ADD_ON_SERVER_H


#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <Message.h>
#include <SupportDefs.h>

#include <PrintAddOnServerProtocol.h>


class PrintAddOnServerApplication : public BApplication {
public:
								PrintAddOnServerApplication(
									const char* signature);

			void				MessageReceived(BMessage* message);

private:
			void				_AddPrinter(BMessage* message);
			status_t			_AddPrinter(const char* driver,
									const char* spoolFolderName);

			void				_ConfigPage(BMessage* message);
			status_t			_ConfigPage(const char* driver,
									BDirectory* spoolFolder,
									BMessage* settings);

			void				_ConfigJob(BMessage* message);
			status_t			_ConfigJob(const char* driver,
									BDirectory* spoolFolder,
									BMessage* settings);

			void				_DefaultSettings(BMessage* message);
			status_t			_DefaultSettings(const char* driver,
									BDirectory* spoolFolder,
									BMessage* settings);

			void				_TakeJob(BMessage* message);
			status_t			_TakeJob(const char* driver,
									const char* spoolFile,
									BDirectory* spoolFolder);

			void				_SendReply(BMessage* message, status_t status);
			void				_SendReply(BMessage* message, BMessage* reply);
};

#endif /* _PRINT_ADD_ON_SERVER_H */
