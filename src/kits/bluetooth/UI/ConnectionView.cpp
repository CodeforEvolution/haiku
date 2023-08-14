/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Tri-Edge AI <triedgeai@gmail.com>
 */

#include <ConnectionView.h>

#include <LayoutBuilder.h>

#include <BluetoothIconView.h>


namespace Bluetooth {

ConnectionView::ConnectionView(BString device, BString address)
	:
	BView("ConnectionView", B_WILL_DRAW | B_PULSE_NEEDED)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	fIcon = new BluetoothIconView();

	fMessageStr = "A new connection is incoming...";

	fMessageText = new BStringView("MessageText", fMessageStr);
	fMessageText->SetAlignment(B_ALIGN_LEFT);

	fDeviceLabel = new BStringView("DeviceLabel", "Device Name:");
	fDeviceLabel->SetFont(be_bold_font);

	fDeviceText = new BStringView("DeviceText", device);
	fDeviceText->SetAlignment(B_ALIGN_RIGHT);

	fAddressLabel = new BStringView("AddressLabel", "MAC Address:");
	fAddressLabel->SetFont(be_bold_font);

	fAddressText = new BStringView("AddressText", address);
	fAddressText->SetAlignment(B_ALIGN_RIGHT);

	BLayoutBuilder::Group<>(this, B_HORIZONTAL)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddGroup(B_VERTICAL, 8)
			.Add(fIcon)
		.End()
		.AddGroup(B_VERTICAL)
			.Add(fMessageText)
			.AddGlue()
			.AddGroup(B_HORIZONTAL, 10)
				.Add(fDeviceLabel)
				.AddGlue()
				.Add(fDeviceText)
			.End()
			.AddGroup(B_HORIZONTAL, 10)
				.Add(fAddressLabel)
				.AddGlue()
				.Add(fAddressText)
			.End()
			.AddGlue()
		.End()
		.AddGlue()
	.End();
}


void
ConnectionView::Pulse()
{
	static uint8 pulses = 0;

	pulses++;

	if (pulses >= 5) {
		Window()->PostMessage(B_QUIT_REQUESTED);
	} else {
		fMessageStr += ".";
		fMessageText->SetText(fMessageStr);
	}
}

}
