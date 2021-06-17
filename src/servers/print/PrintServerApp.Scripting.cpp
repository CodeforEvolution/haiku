/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 */


#include "PrintServerApp.h"

#include <stdio.h>

#include <Catalog.h>
#include <Locale.h>
#include <PropertyInfo.h>

#include "Printer.h"
#include "Transport.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PrintServerApp Scripting"


static property_info prop_list[] = {
	{ "ActivePrinter", { B_GET_PROPERTY, B_SET_PROPERTY },
		{ B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Retrieve or select the active printer") },
	{ "Printer", { B_GET_PROPERTY }, { B_INDEX_SPECIFIER, B_NAME_SPECIFIER,
		B_REVERSE_INDEX_SPECIFIER },
		B_TRANSLATE_MARK("Retrieve a specific printer") },
	{ "Printer", { B_CREATE_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Create a new printer") },
	{ "Printer", { B_DELETE_PROPERTY }, { B_INDEX_SPECIFIER, B_NAME_SPECIFIER,
		B_REVERSE_INDEX_SPECIFIER },
		B_TRANSLATE_MARK("Delete a specific printer") },
	{ "Printers", { B_COUNT_PROPERTIES }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Return the number of available printers") },
	{ "Transport", { B_GET_PROPERTY }, { B_INDEX_SPECIFIER, B_NAME_SPECIFIER,
		B_REVERSE_INDEX_SPECIFIER },
		B_TRANSLATE_MARK("Retrieve a specific transport") },
	{ "Transports", { B_COUNT_PROPERTIES }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Return the number of available transports") },
	{ "UseConfigWindow", { B_GET_PROPERTY, B_SET_PROPERTY },
		{ B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Show configuration window") },

	{ NULL } // terminate list
};


void
PrintServerApp::HandleScriptingCommand(BMessage* message)
{
	BString propName;
	BMessage spec;
	int32 index;

	if (message->GetCurrentSpecifier(&index, &spec) == B_OK
		&& spec.FindString("property", &propName) == B_OK) {
		switch (message->what) {
			case B_GET_PROPERTY:
			{
				if (propName == "ActivePrinter") {
					BMessage reply(B_REPLY);
					reply.AddString("result", fDefaultPrinter
						? fDefaultPrinter->Name() : "");
					reply.AddInt32("error", B_OK);
					message->SendReply(&reply);
				} else if (propName == "UseConfigWindow") {
					BMessage reply(B_REPLY);
					reply.AddString("result", fUseConfigWindow
						? "true" : "false");
					reply.AddInt32("error", B_OK);
					message->SendReply(&reply);
				}

				break;
			}

			case B_SET_PROPERTY:
			{
				if (propName == "ActivePrinter") {
					BString newActivePrinter;
					if (message->FindString("data", &newActivePrinter) ==
						B_OK) {
						BMessage reply(B_REPLY);
						reply.AddInt32("error",
							_SelectPrinter(newActivePrinter.String()));
						message->SendReply(&reply);
					}
				} else if (propName == "UseConfigWindow") {
					bool useConfigWindow;
					if (message->FindBool("data", &useConfigWindow) == B_OK) {
						fUseConfigWindow = useConfigWindow;
						BMessage reply(B_REPLY);
						reply.AddInt32("error", fUseConfigWindow);
						message->SendReply(&reply);
					}
				}

				break;
			}

			case B_CREATE_PROPERTY:
			{
				if (propName == "Printer") {
					BString name;
					BString driver;
					BString transport;
					BString config;

					if (message->FindString("name", &name) == B_OK
						&& message->FindString("driver", &driver) == B_OK
						&& message->FindString("transport", &transport) == B_OK
						&& message->FindString("config", &config) == B_OK) {
						BMessage reply(B_REPLY);
						reply.AddInt32("error", _CreatePrinter(name.String(),
							driver.String(), "Local", transport.String(),
							config.String()));
						message->SendReply(&reply);
					}
				}

				break;
			}

			case B_DELETE_PROPERTY:
			{
				Printer* printer = GetPrinterFromSpecifier(&spec);
				status_t result = B_BAD_VALUE;

				if (printer != NULL)
					result=printer->Remove();

				BMessage reply(B_REPLY);
				reply.AddInt32("error", result);
				message->SendReply(&reply);

				break;
			}

			case B_COUNT_PROPERTIES:
			{
				if (propName == "Printers") {
					BMessage reply(B_REPLY);
					reply.AddInt32("result", Printer::CountPrinters());
					reply.AddInt32("error", B_OK);
					message->SendReply(&reply);
				} else if (propName == "Transports") {
					BMessage reply(B_REPLY);
					reply.AddInt32("result", Transport::CountTransports());
					reply.AddInt32("error", B_OK);
					message->SendReply(&reply);
				}

				break;
			}
		}
	}
}


Printer*
PrintServerApp::GetPrinterFromSpecifier(BMessage* message)
{
	switch (message->what) {
		case B_NAME_SPECIFIER:
		{
			BString name;
			if (message->FindString("name", &name) == B_OK)
				return Printer::Find(name.String());

			break;
		}

		case B_INDEX_SPECIFIER:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK)
				return Printer::At(index);

			break;
		}

		case B_REVERSE_INDEX_SPECIFIER:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK)
				return Printer::At(Printer::CountPrinters() - index);

			break;
		}
	}

	return NULL;
}


Transport*
PrintServerApp::GetTransportFromSpecifier(BMessage* message)
{
	switch (message->what) {
		case B_NAME_SPECIFIER:
		{
			BString name;
			if (message->FindString("name", &name) == B_OK)
				return Transport::Find(name);

			break;
		}

		case B_INDEX_SPECIFIER:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK)
				return Transport::At(index);

			break;
		}

		case B_REVERSE_INDEX_SPECIFIER:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK)
				return Transport::At(Transport::CountTransports() - index);

			break;
		}
	}

	return NULL;
}


BHandler*
PrintServerApp::ResolveSpecifier(BMessage* message, int32 index, BMessage* spec,
	int32 form, const char* prop)
{
	BPropertyInfo prop_info(prop_list);
	BHandler* result = NULL;

	int32 matchIndex = prop_info.FindMatch(message, 0, spec, form, prop);
	switch (matchIndex) {
		case B_ERROR:
			result = BHandler::ResolveSpecifier(message, index, spec, form,
				prop);
			break;

		case 1:
		{
			// GET Printer [arg]
			result = GetPrinterFromSpecifier(spec);
			if (result == NULL) {
				BMessage reply(B_REPLY);
				reply.AddInt32("error", B_BAD_INDEX);
				message->SendReply(&reply);
			} else
				message->PopSpecifier();

			break;
		}

		case 5:
		{
			// GET Transport [arg]
			result = GetTransportFromSpecifier(spec);
			if (result == NULL) {
				BMessage reply(B_REPLY);
				reply.AddInt32("error", B_BAD_INDEX);
				message->SendReply(&reply);
			} else
				message->PopSpecifier();

			break;
		}

		default:
			result = this;
	}

	return result;
}


status_t
PrintServerApp::GetSupportedSuites(BMessage* message)
{
	message->AddString("suites", "suite/vnd.Haiku-print_server");

	static bool localized = false;
	if (!localized) {
		localized = true;
		for (int i = 0; prop_list[i].name != NULL; i++)
			prop_list[i].usage = B_TRANSLATE_NOCOLLECT(prop_list[i].usage);
	}

	BPropertyInfo prop_info(prop_list);
	message->AddFlat("messages", &prop_info);

	return BHandler::GetSupportedSuites(message);
}
