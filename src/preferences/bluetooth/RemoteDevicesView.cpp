/*
 * Copyright 2008-2009, Oliver Ruiz Dorantes, <oliver.ruiz.dorantes@gmail.com>
 * Copyright 2021, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Fredrik Modéen <fredrik_at_modeen.se>
 */

#include <stdio.h>

#include <Alert.h>
#include <Catalog.h>
#include <Messenger.h>

#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>

#include <LayoutBuilder.h>
#include <SpaceLayoutItem.h>

#include <PincodeWindow.h>
#include <bluetooth/RemoteDevice.h>

#include "BluetoothWindow.h"
#include "defs.h"
#include "DeviceListItem.h"
#include "InquiryPanel.h"
#include "RemoteDevicesView.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Remote devices"

static const uint32 kMsgAddDevices = 'ddDv';
static const uint32 kMsgRemoveDevice = 'rmDv';
static const uint32 kMsgPairDevice = 'trDv';
static const uint32 kMsgDisconnectDevice = 'dsDv';
//static const uint32 kMsgBlockDevice = 'blDv';
//static const uint32 kMsgRefreshDevices = 'rfDv';

using namespace Bluetooth;

RemoteDevicesView::RemoteDevicesView(const char* name)
	:
	BView(name, B_WILL_DRAW)
{
	fAddButton = new BButton("add", B_TRANSLATE("Add" B_UTF8_ELLIPSIS),
		new BMessage(kMsgAddDevices));

	fRemoveButton = new BButton("remove", B_TRANSLATE("Remove"),
		new BMessage(kMsgRemoveDevice));

	fPairButton = new BButton("pair", B_TRANSLATE("Pair" B_UTF8_ELLIPSIS),
		new BMessage(kMsgPairDevice));

	fDisconnectButton = new BButton("disconnect", B_TRANSLATE("Disconnect"),
		new BMessage(kMsgDisconnectDevice));
/*
	fBlockButton = new BButton("block", B_TRANSLATE("As blocked"),
		new BMessage(kMsgBlockDevice));

	fAvailButton = new BButton("check", B_TRANSLATE("Refresh" B_UTF8_ELLIPSIS),
		new BMessage(kMsgRefreshDevices));
*/
	// Set up device list
	fDeviceList = new BListView("DeviceList", B_SINGLE_SELECTION_LIST);
	fDeviceList->SetSelectionMessage(new BMessage(kMsgDeviceListSelection));
	fDeviceList->SetInvocationMessage(new BMessage(kMsgDeviceListInvocation));

	fScrollView = new BScrollView("ScrollView", fDeviceList, 0, false, true);

	BLayoutBuilder::Group<>(this, B_HORIZONTAL, 10)
		.SetInsets(5)
		.Add(fScrollView)
		//.Add(BSpaceLayoutItem::CreateHorizontalStrut(5))
		.AddGroup(B_VERTICAL)
			.SetInsets(0, 15, 0, 15)
			.Add(fAddButton)
			.Add(fRemoveButton)
			.AddGlue()
//			.Add(fAvailButton)
	//		.AddGlue()
			.Add(fPairButton)
			.Add(fDisconnectButton)
//			.Add(fBlockButton)
			.AddGlue()
		.End()
	.End();

	fDeviceList->SetSelectionMessage(NULL);
}


RemoteDevicesView::~RemoteDevicesView()
{

}


void
RemoteDevicesView::AttachedToWindow()
{
	fDeviceList->SetTarget(this);
	fAddButton->SetTarget(this);
	fRemoveButton->SetTarget(this);
	fPairButton->SetTarget(this);
	fDisconnectButton->SetTarget(this);
//	fBlockButton->SetTarget(this);
//	fAvailButton->SetTarget(this);

	LoadSettings();
	fDeviceList->Select(0);
}


void
RemoteDevicesView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgAddDevices:
		{
			InquiryPanel* inquiryPanel = new InquiryPanel(gActiveLocalDevice);
			if (Window() != NULL) {
				Window()->AddToSubset(inquiryPanel);
				inquiryPanel->CenterIn(Window()->Frame());
			} else
				inquiryPanel->CenterOnScreen();

			inquiryPanel->Show();
			break;
		}

		case kMsgRemoveDevice:
			fDeviceList->RemoveItem(fDeviceList->CurrentSelection(0));
			break;

		case kMsgAddToRemoteList:
		{
			BListItem* device = NULL;
			if (message->FindPointer("device", (void**)&device) != B_OK)
				break;

			fDeviceList->AddItem(device);
			fDeviceList->Invalidate();
			break;
		}

		case kMsgPairDevice:
		{
			DeviceListItem* device = static_cast<DeviceListItem*>(
				fDeviceList->ItemAt(fDeviceList->CurrentSelection(0)));
			if (device == NULL)
				break;

			RemoteDevice* remote = dynamic_cast<RemoteDevice*>(device->Device());
			if (remote == NULL)
				break;

			remote->Authenticate();

			break;
		}

		case kMsgDisconnectDevice:
		{
			DeviceListItem* device = static_cast<DeviceListItem*>(
				fDeviceList->ItemAt(fDeviceList->CurrentSelection(0)));
			if (device == NULL)
				break;

			RemoteDevice* remote = dynamic_cast<RemoteDevice*>(device->Device());
			if (remote == NULL)
				break;

			remote->Disconnect();

			break;
		}

		default:
			BView::MessageReceived(message);
			break;
	}
}


void
RemoteDevicesView::LoadSettings()
{

}


bool
RemoteDevicesView::IsDefaultable()
{
	return true;
}

