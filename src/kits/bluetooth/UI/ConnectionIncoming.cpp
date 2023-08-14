/*
 * Copyright 2007-2008 Oliver Ruiz Dorantes, oliver.ruiz.dorantes_at_gmail.com
 * Copyright 2021, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Tri-Edge AI <triedgeai@gmail.com>
 */


#include <ConnectionIncoming.h>

#include <LayoutBuilder.h>

#include <ConnectionView.h>

namespace Bluetooth {

ConnectionIncoming::ConnectionIncoming(bdaddr_t address)
	:
	BWindow(BRect(), "Incoming Connection..",
		B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	SetPulseRate(1 * 1000 * 1000);
		// 1 second
	fView = new ConnectionView("<unknown_device>",
		bdaddrUtils::ToString(address));

	BLayoutBuilder::Group<>(this)
		.Add(fView)
	.End();

	UpdateSizeLimits();
	CenterOnScreen();
}


ConnectionIncoming::ConnectionIncoming(RemoteDevice* rDevice)
	:
	BWindow(BRect(), "Incoming Connection",
		B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	SetPulseRate(1 * 1000 * 1000);
		// 1 second

	if (rDevice != NULL) {
		fView = new ConnectionView(rDevice->GetFriendlyName(),
					bdaddrUtils::ToString(rDevice->GetBluetoothAddress()));
	} else {
		fView = new ConnectionView("<unknown_device>",
					bdaddrUtils::ToString(bdaddrUtils::NullAddress()));
	}

	BLayoutBuilder::Group<>(this)
		.Add(fView)
	.End();

	UpdateSizeLimits();
	CenterOnScreen();
}


ConnectionIncoming::~ConnectionIncoming()
{
}


void
ConnectionIncoming::MessageReceived(BMessage* message)
{
}


bool
ConnectionIncoming::QuitRequested()
{
	return BWindow::QuitRequested();
}


} /* end namespace Bluetooth */
