/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 */


#include "Printer.h"

#include <AppDefs.h>
#include <Catalog.h>
#include <Locale.h>
#include <Message.h>
#include <Messenger.h>
#include <PropertyInfo.h>

#include "pr_server.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Printer Scripting"


static property_info prop_list[] = {
	{ "Name", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Get name of printer") },
	{ "TransportAddon", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK(
			"Get name of the transport add-on used for this printer") },
	{ "TransportConfig", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Get the transport configuration for this printer") },
	{ "PrinterAddon", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK(
			"Get name of the printer add-on used for this printer") },
	{ "Comments", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		B_TRANSLATE_MARK("Get comments about this printer") },

	{ NULL } // terminate list
};


void
Printer::HandleScriptingCommand(BMessage* message)
{
	status_t error = B_ERROR;
	BString propName;
	BString result;
	BMessage spec;
	int32 index;

	if (message->GetCurrentSpecifier(&index, &spec) == B_OK
		&& spec.FindString("property", &propName) == B_OK) {
		switch (message->what) {
			case B_GET_PROPERTY:
			{
				if (propName == "Name")
					error = _SpoolDir()->ReadAttrString(
						PSRV_PRINTER_ATTR_PRINTER_NAME, &result);
				else if (propName == "TransportAddon")
					error = _SpoolDir()->ReadAttrString(
						PSRV_PRINTER_ATTR_TRANSPORT, &result);
				else if (propName == "TransportConfig")
					error = _SpoolDir()->ReadAttrString(
						PSRV_PRINTER_ATTR_TRANSPORT_ADDRESS, &result);
				else if (propName == "PrinterAddon")
					error = _SpoolDir()->ReadAttrString(
						PSRV_PRINTER_ATTR_DRIVER_NAME, &result);
				else if (propName == "Comments")
					error = _SpoolDir()->ReadAttrString(
						PSRV_PRINTER_ATTR_COMMENTS, &result);
				else {
					// If unknown scripting request, let super class handle it
					BHandler::MessageReceived(message);
					break;
				}

				BMessage reply(B_REPLY);
				reply.AddString("result", result);
				reply.AddInt32("error", error);
				message->SendReply(&reply);

				break;
			}
		}
	} else {
		// If GetSpecifier failed
		if (index == -1) {
			BMessage reply(B_REPLY);
			reply.AddMessenger("result", BMessenger(this));
			reply.AddInt32("error", B_OK);
			message->SendReply(&reply);
		}
	}
}


BHandler*
Printer::ResolveSpecifier(BMessage* message, int32 index, BMessage* spec,
	int32 form, const char* prop)
{
	BPropertyInfo prop_info(prop_list);
	BHandler* result = this;

	int32 matchIndex = prop_info.FindMatch(message, 0, spec, form, prop);
	switch (matchIndex) {
		case B_ERROR:
			result = BHandler::ResolveSpecifier(message, index, spec, form,
				prop);
			break;
	}

	return result;
}


status_t
Printer::GetSupportedSuites(BMessage* message)
{
	message->AddString("suites", "suite/vnd.Haiku-printer");

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
