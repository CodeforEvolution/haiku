/*
 * Copyright 2010-2021 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PrintAddOnServerApplication.h"

#include <String.h>

#include <PrinterDriverAddOn.h>


PrintAddOnServerApplication::PrintAddOnServerApplication(const char* signature)
	:
	BApplication(signature)
{

}


void
PrintAddOnServerApplication::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMessageAddPrinter:
			_AddPrinter(message);
			break;

		case kMessageConfigPage:
			_ConfigPage(message);
			break;

		case kMessageConfigJob:
			_ConfigJob(message);
			break;

		case kMessageDefaultSettings:
			_DefaultSettings(message);
			break;

		case kMessageTakeJob:
			_TakeJob(message);
			break;

		default:
			BApplication::MessageReceived(message);
	}
}


void
PrintAddOnServerApplication::_AddPrinter(BMessage* message)
{
	BString driver;
	BString name;
	if (message->FindString(kPrinterDriverAttribute, &driver) != B_OK
			|| message->FindString(kPrinterNameAttribute, &name) != B_OK) {
		_SendReply(message, B_BAD_VALUE);
		return;
	}

	status_t status = _AddPrinter(driver.String(), name.String());
	_SendReply(message, status);
}


status_t
PrintAddOnServerApplication::_AddPrinter(const char* driver,
	const char* spoolFolderName)
{
	PrinterDriverAddOn addOn(driver);
	return addOn.AddPrinter(spoolFolderName);
}


void
PrintAddOnServerApplication::_ConfigPage(BMessage* message)
{
	BString driver;
	BString folder;
	BMessage settings;
	if (message->FindString(kPrinterDriverAttribute, &driver) != B_OK
		|| message->FindString(kPrinterFolderAttribute, &folder) != B_OK
		|| message->FindMessage(kPrintSettingsAttribute, &settings) != B_OK) {
		_SendReply(message, B_BAD_VALUE);
		return;
	}

	BDirectory spoolFolder(folder.String());
	status_t status = _ConfigPage(driver.String(), &spoolFolder, &settings);

	if (status != B_OK)
		_SendReply(message, status);
	else {
		BMessage reply(B_REPLY);
		reply.AddMessage(kPrintSettingsAttribute, &settings);
		_SendReply(message, &reply);
	}
}


status_t
PrintAddOnServerApplication::_ConfigPage(const char* driver,
	BDirectory* spoolFolder, BMessage* settings)
{
	PrinterDriverAddOn addOn(driver);
	return addOn.ConfigPage(spoolFolder, settings);
}


void
PrintAddOnServerApplication::_ConfigJob(BMessage* message)
{
	BString driver;
	BString folder;
	BMessage settings;
	if (message->FindString(kPrinterDriverAttribute, &driver) != B_OK
		|| message->FindString(kPrinterFolderAttribute, &folder) != B_OK
		|| message->FindMessage(kPrintSettingsAttribute, &settings) != B_OK) {
		_SendReply(message, B_BAD_VALUE);
		return;
	}

	BDirectory spoolFolder(folder.String());
	status_t status = _ConfigJob(driver.String(), &spoolFolder, &settings);

	if (status != B_OK)
		_SendReply(message, status);
	else {
		BMessage reply(B_REPLY);
		reply.AddMessage(kPrintSettingsAttribute, &settings);
		_SendReply(message, &reply);
	}
}


status_t
PrintAddOnServerApplication::_ConfigJob(const char* driver,
	BDirectory* spoolFolder, BMessage* settings)
{
	PrinterDriverAddOn addOn(driver);
	return addOn.ConfigJob(spoolFolder, settings);
}


void
PrintAddOnServerApplication::_DefaultSettings(BMessage* message)
{
	BString driver;
	BString folder;
	if (message->FindString(kPrinterDriverAttribute, &driver) != B_OK
		|| message->FindString(kPrinterFolderAttribute, &folder) != B_OK) {
		_SendReply(message, B_BAD_VALUE);
		return;
	}

	BMessage settings;
	BDirectory spoolFolder(folder.String());
	status_t status = _DefaultSettings(driver.String(), &spoolFolder,
		&settings);

	if (status != B_OK)
		_SendReply(message, status);
	else {
		BMessage reply(B_REPLY);
		reply.AddMessage(kPrintSettingsAttribute, &settings);
		_SendReply(message, &reply);
	}
}


status_t
PrintAddOnServerApplication::_DefaultSettings(const char* driver,
	BDirectory* spoolFolder, BMessage* settings)
{
	PrinterDriverAddOn addOn(driver);
	return addOn.DefaultSettings(spoolFolder, settings);
}


void
PrintAddOnServerApplication::_TakeJob(BMessage* message)
{
	BString driver;
	BString folder;
	BString jobFile;
	if (message->FindString(kPrinterDriverAttribute, &driver) != B_OK
		|| message->FindString(kPrinterFolderAttribute, &folder) != B_OK
		|| message->FindString(kPrintJobFileAttribute, &jobFile) != B_OK) {
		_SendReply(message, B_BAD_VALUE);
		return;
	}

	BDirectory spoolFolder(folder.String());
	status_t status = _TakeJob(driver.String(), jobFile.String(),
		&spoolFolder);
	_SendReply(message, status);
}


status_t
PrintAddOnServerApplication::_TakeJob(const char* driver, const char* spoolFile,
	BDirectory* spoolFolder)
{
	PrinterDriverAddOn addOn(driver);
	return addOn.TakeJob(spoolFile, spoolFolder);
}


void
PrintAddOnServerApplication::_SendReply(BMessage* message, status_t status)
{
	BMessage reply(B_REPLY);
	reply.AddInt32(kPrintAddOnServerStatusAttribute, status);
	message->SendReply(&reply);
}


void
PrintAddOnServerApplication::_SendReply(BMessage* message, BMessage* reply)
{
	reply->AddInt32(kPrintAddOnServerStatusAttribute, B_OK);
	message->SendReply(reply);
}


int
main(int argc, char* argv[])
{
	PrintAddOnServerApplication application(
		kPrintAddOnServerApplicationSignature);
	application.Run();
}
