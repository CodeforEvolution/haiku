/*
 * Copyright 2008-09, Oliver Ruiz Dorantes, <oliver.ruiz.dorantes_at_gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "BluetoothDeviceView.h"
#include <bluetooth/bdaddrUtils.h>

#include <bluetooth/LocalDevice.h>
#include <bluetooth/HCI/btHCI_command.h>


#include <Bitmap.h>
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <TextView.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Device View"

BluetoothDeviceView::BluetoothDeviceView(BluetoothDevice* bDevice)
	:
	BView("BluetoothDeviceView", B_WILL_DRAW),
	fDevice(bDevice)
{
	fName = new BStringView("name", "");
	fName->SetFont(be_bold_font);
	fName->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

	fBdaddr = new BStringView("bdaddr",
		bdaddrUtils::ToString(bdaddrUtils::NullAddress()));
	fBdaddr->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

	fClassService = new BStringView("ServiceClass",
		B_TRANSLATE("Service classes: "));
	fClassService->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));

	fClass = new BStringView("class", "- / -");
	fClass->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

	fHCIVersionProperties = new BStringView("hci", "");
	fHCIVersionProperties->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));
	fLMPVersionProperties = new BStringView("lmp", "");
	fLMPVersionProperties->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));
	fManufacturerProperties = new BStringView("manufacturer", "");
	fManufacturerProperties->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));
	fACLBuffersProperties = new BStringView("buffers acl", "");
	fACLBuffersProperties->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));
	fSCOBuffersProperties = new BStringView("buffers sco", "");
	fSCOBuffersProperties->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,
		B_ALIGN_MIDDLE));

	fIcon = new BView(BRect(0, 0, 32 - 1, 32 - 1), "Icon", B_FOLLOW_ALL,
		B_WILL_DRAW);
	fIcon->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	SetBluetoothDevice(bDevice);

	BLayoutBuilder::Group<>(this, B_HORIZONTAL, 5)
		.SetInsets(10)
		.Add(fIcon)
		.AddGroup(B_VERTICAL, 0)
			.SetInsets(5)
			.Add(fName)
			.Add(fBdaddr)
			.Add(fClass)
			.Add(fClassService)
			.Add(fHCIVersionProperties)
			.Add(fLMPVersionProperties)
			.Add(fManufacturerProperties)
			.Add(fACLBuffersProperties)
			.Add(fSCOBuffersProperties)
		.End()
		.AddGlue()
	.End();

}


BluetoothDeviceView::~BluetoothDeviceView()
{
}


void
BluetoothDeviceView::SetBluetoothDevice(BluetoothDevice* bDevice)
{
	if (bDevice == NULL)
		return;

	SetName(bDevice->GetFriendlyName().String());

	fName->SetText(bDevice->GetFriendlyName().String());
	fBdaddr->SetText(bdaddrUtils::ToString(bDevice->GetBluetoothAddress()));

	BString tempString(B_TRANSLATE("Service classes: "));
	bDevice->GetDeviceClass().GetServiceClass(tempString);
	fClassService->SetText(tempString.String());

	tempString = "";
	bDevice->GetDeviceClass().GetMajorDeviceClass(tempString);
	tempString << " / ";
	bDevice->GetDeviceClass().GetMinorDeviceClass(tempString);
	fClass->SetText(tempString.String());

	bDevice->GetDeviceClass().Draw(fIcon, BPoint(Bounds().left, Bounds().top));

	uint32 value;

	tempString = "";
	if (bDevice->GetProperty("hci_version", &value) == B_OK)
		tempString << "HCI ver: " << BluetoothHciVersion(value);
	if (bDevice->GetProperty("hci_revision", &value) == B_OK)
		tempString << " HCI rev: " << value ;

	fHCIVersionProperties->SetText(tempString.String());

	tempString = "";
	if (bDevice->GetProperty("lmp_version", &value) == B_OK)
		tempString << "LMP ver: " << BluetoothLmpVersion(value);
	if (bDevice->GetProperty("lmp_subversion", &value) == B_OK)
		tempString << " LMP subver: " << value;
	fLMPVersionProperties->SetText(tempString.String());

	tempString = "";
	if (bDevice->GetProperty("manufacturer", &value) == B_OK)
		tempString << B_TRANSLATE("Manufacturer: ")
		   	<< BluetoothManufacturer(value);
	fManufacturerProperties->SetText(tempString.String());

	tempString = "";
	if (bDevice->GetProperty("acl_mtu", &value) == B_OK)
		tempString << "ACL mtu: " << value;
	if (bDevice->GetProperty("acl_max_pkt", &value) == B_OK)
		tempString << B_TRANSLATE(" packets: ") << value;
	fACLBuffersProperties->SetText(tempString.String());

	tempString = "";
	if (bDevice->GetProperty("sco_mtu", &value) == B_OK)
		tempString << "SCO mtu: " << value;
	if (bDevice->GetProperty("sco_max_pkt", &value) == B_OK)
		tempString << B_TRANSLATE(" packets: ") << value;
	fSCOBuffersProperties->SetText(tempString.String());
}


void
BluetoothDeviceView::SetTarget(BHandler* target)
{
}


void
BluetoothDeviceView::MessageReceived(BMessage* message)
{
	// The default
	BView::MessageReceived(message);
}


void
BluetoothDeviceView::SetEnabled(bool value)
{
}
