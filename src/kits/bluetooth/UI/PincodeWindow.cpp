/*
 * Copyright 2007-2008 Oliver Ruiz Dorantes, oliver.ruiz.dorantes_at_gmail.com
 * Copyright 2021, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Tri-Edge AI <triedgeai@gmail.com>
 */


#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

#include <String.h>
#include <Message.h>
#include <Application.h>

#include <Button.h>
#include <LayoutBuilder.h>
#include <InterfaceDefs.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <TextControl.h>

#include <bluetooth/RemoteDevice.h>
#include <bluetooth/LocalDevice.h>
#include <bluetooth/bdaddrUtils.h>
#include <bluetooth/bluetooth_error.h>

#include <bluetooth/HCI/btHCI_command.h>
#include <bluetooth/HCI/btHCI_event.h>

#include <PincodeWindow.h>
#include <bluetoothserver_p.h>
#include <CommandManager.h>


#define BD_ADDR_LABEL "BD_ADDR: "


static const uint32 skMessageAcceptButton = 'acCp';
static const uint32 skMessageCancelButton = 'mVch';


namespace Bluetooth
{

PincodeWindow::PincodeWindow(bdaddr_t address, hci_id hid)
	:
	BWindow(BRect(), "PIN Code Request",
		B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS),
		fBdaddr(address),
		fHid(hid)
{
	InitUI();

	// TODO: Get more info about device" ote name/features/encry/auth... etc
	SetBDaddr(bdaddrUtils::ToString(fBdaddr));

	UpdateSizeLimits();
	CenterOnScreen();
}


PincodeWindow::PincodeWindow(RemoteDevice* rDevice)
	:
	BWindow(BRect(), "PIN Code Request",
		B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fBdaddr(rDevice->GetBluetoothAddress()),
	fHid(rDevice->GetLocalDeviceOwner()->ID())
{
	InitUI();

	// TODO: Get more info about device" ote name/features/encry/auth... etc
	SetBDaddr(bdaddrUtils::ToString(fBdaddr));

	UpdateSizeLimits();
	CenterOnScreen();
}


void
PincodeWindow::InitUI()
{
	fIcon = new BluetoothIconView();

	fMessage = new BStringView("fMessage", "Input the PIN code to pair with");
	fMessage2 = new BStringView("fMessage2", "the following Bluetooth device.");

	fDeviceLabel = new BStringView("fDeviceLabel","Device Name: ");
	fDeviceLabel->SetFont(be_bold_font);

	fDeviceText = new BStringView("fDeviceText", "<unknown_device>");

	fAddressLabel = new BStringView("fAddressLabel", "MAC Address: ");
	fAddressLabel->SetFont(be_bold_font);

	fAddressText = new BStringView("fAddressText", "<mac_address>");

	fPincodeText = new BTextControl("fPINCode", "PIN Code:", "0000", NULL);
	fPincodeText->TextView()->SetMaxBytes(16 * sizeof(fPincodeText->Text()[0]));
	fPincodeText->MakeFocus();

	fAcceptButton = new BButton("fAcceptButton", "Pair",
		new BMessage(skMessageAcceptButton));

	fCancelButton = new BButton("fCancelButton", "Cancel",
		new BMessage(skMessageCancelButton));

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddGroup(B_HORIZONTAL)
			.AddGroup(B_HORIZONTAL, 8)
				.Add(fIcon)
			.End()
			.AddGroup(B_VERTICAL)
				.Add(fMessage)
				.Add(fMessage2)
				.AddGlue()
			.End()
		.End()
		.AddGroup(B_HORIZONTAL)
			.Add(fDeviceLabel)
			.AddGlue()
			.Add(fDeviceText)
		.End()
		.AddGroup(B_HORIZONTAL)
			.Add(fAddressLabel)
			.AddGlue()
			.Add(fAddressText)
		.End()
		.AddGlue()
		.Add(fPincodeText)
		.AddGlue()
		.AddGroup(B_HORIZONTAL, 10)
			.AddGlue()
			.Add(fCancelButton)
			.Add(fAcceptButton)
		.End()
	.End();
}


void
PincodeWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case skMessageAcceptButton:
		{
			BMessage request(BT_MSG_HANDLE_SIMPLE_REQUEST);
			BMessage reply;
			size_t size;
			int8 bt_status = BT_ERROR;

			void* command = buildPinCodeRequestReply(fBdaddr,
				strlen(fPincodeText->Text()),
				(char*)fPincodeText->Text(), &size);

			if (command == NULL) {
				break;
			}

			request.AddInt32("hci_id", fHid);
			request.AddData("raw command", B_ANY_TYPE, command, size);
			request.AddInt16("eventExpected",  HCI_EVENT_CMD_COMPLETE);
			request.AddInt16("opcodeExpected", PACK_OPCODE(OGF_LINK_CONTROL,
				OCF_PIN_CODE_REPLY));

			// we reside in the server
			if (be_app_messenger.SendMessage(&request, &reply) == B_OK) {
				if (reply.FindInt8("status", &bt_status ) == B_OK) {
					PostMessage(B_QUIT_REQUESTED);
				}
				// TODO: something failed here
			}
			break;
		}

		case skMessageCancelButton:
		{
			BMessage request(BT_MSG_HANDLE_SIMPLE_REQUEST);
			BMessage reply;
			size_t size;
			int8 bt_status = BT_ERROR;

			void* command = buildPinCodeRequestNegativeReply(fBdaddr, &size);

			if (command == NULL) {
				break;
			}

			request.AddInt32("hci_id", fHid);
			request.AddData("raw command", B_ANY_TYPE, command, size);
			request.AddInt16("eventExpected",  HCI_EVENT_CMD_COMPLETE);
			request.AddInt16("opcodeExpected", PACK_OPCODE(OGF_LINK_CONTROL,
				OCF_PIN_CODE_NEG_REPLY));

			if (be_app_messenger.SendMessage(&request, &reply) == B_OK) {
				if (reply.FindInt8("status", &bt_status ) == B_OK ) {
					PostMessage(B_QUIT_REQUESTED);
				}
				// TODO: something failed here
			}
			break;
		}

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}


bool
PincodeWindow::QuitRequested()
{
	return BWindow::QuitRequested();
}


void
PincodeWindow::SetBDaddr(BString address)
{
	fAddressText->SetText(address);
}

} /* end namespace Bluetooth */
