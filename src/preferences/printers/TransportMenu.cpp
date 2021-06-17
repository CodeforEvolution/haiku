/*
 * Copyright 2002-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 *		Philippe Houdoin
 */


#include "TransportMenu.h"

#include <Catalog.h>
#include <MenuItem.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TransportMenu"


TransportMenu::TransportMenu(const char* title, uint32 what,
	const BMessenger& messenger, const BString& transportName)
	:
	BMenu(title),
	fWhat(what),
	fMessenger(messenger),
	fTransportName(transportName)
{
}


bool
TransportMenu::AddDynamicItem(add_state state)
{
	if (state != B_INITIAL_ADD)
		return false;

	BMenuItem* item = RemoveItem((int32)0);
	while (item != NULL) {
		delete item;
		item = RemoveItem((int32)0);
	}

	BMessage message;
	message.MakeEmpty();
	message.what = B_GET_PROPERTY;
	message.AddSpecifier("Ports");
	BMessage reply;
	if (fMessenger.SendMessage(&message, &reply) != B_OK)
		return false;

	BString portId;
	BString portName;
	if (reply.FindString("port_id", &portId) != B_OK) {
		// Show error message in submenu
		BMessage* portMessage = new BMessage(fWhat);
		AddItem(new BMenuItem(
			B_TRANSLATE("No printer found!"), portMessage));
		return false;
	}

	// Add ports to submenu
	for (int32 index = 0; reply.FindString("port_id", index, &portId) == B_OK;
		index++) {
		if (reply.FindString("port_name", index, &portName) != B_OK
			|| portName.IsEmpty())
			portName = portId;

		// Create menu item in submenu for port
		BMessage* portMessage = new BMessage(fWhat);
		portMessage->AddString("name", fTransportName);
		portMessage->AddString("path", portId);
		AddItem(new BMenuItem(portName.String(), portMessage));
	}

	return false;
}
