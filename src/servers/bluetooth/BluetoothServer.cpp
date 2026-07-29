/*
 * Copyright 2007-2009 Oliver Ruiz Dorantes, oliver.ruiz.dorantes_at_gmail.com
 * Copyright 2008 Mika Lindqvist, monni1995_at_gmail.com
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * All rights reserved. Distributed under the terms of the MIT License.
 * 
 * Authors:
 * 		Jacob Secunda, secundaja@gmail.com
 */


#include "BluetoothServer.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include <syslog.h>
#include <unistd.h>

#include <Entry.h>
#include <Deskbar.h>
#include <Directory.h>
#include <Message.h>
#include <Path.h>
#include <Roster.h>
#include <String.h>
#include <TypeConstants.h>
#include <Window.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/HCI/btHCI_command.h>
#include <bluetooth/L2CAP/btL2CAP.h>
#include <bluetoothserver_p.h>

#include "Debug.h"
#include "DeskbarReplicant.h"
#include "LocalDeviceImpl.h"


const size_t kSDPClientBufferSize = 29;


status_t
DispatchEvent(struct hci_event_header* header, int32 code, size_t size)
{
	// We only handle events
	if (GET_PORTCODE_TYPE(code) != BT_EVENT) {
		TRACE_BT("BluetoothServer: Wrong type frame code\n");
		return B_OK;
	}

	BluetoothServer* server = dynamic_cast<BluetoothServer*>(be_app);
	if (server == NULL)
		return B_ERROR;

	// Fetch the LocalDevice who belongs this event
	LocalDeviceImpl* localDevice = server->LocateLocalDeviceImpl(GET_PORTCODE_HID(code));
	if (localDevice == NULL) {
		TRACE_BT("BluetoothServer: LocalDevice could not be fetched\n");
		return B_ERROR;
	}

	localDevice->HandleEvent(header);

	return B_OK;
}


BluetoothServer::BluetoothServer()
	:
	BApplication(BLUETOOTH_SIGNATURE),
	fLocalDevicesList(5),
	fEventListener(NULL),
	fDeviceManager(NULL),
	fSDPThreadID(-1),
	fIsShuttingDown(false)
{
	fDeviceManager = new DeviceManager();
	fLocalDevicesList.MakeEmpty();

	fEventListener = new BluetoothPortListener(BT_USERLAND_PORT_NAME,
		static_cast<BluetoothPortListener::port_listener_func>(&DispatchEvent));
}


bool
BluetoothServer::QuitRequested()
{
	fLocalDevicesList.MakeEmpty(true);

	_RemoveDeskbarIcon();

	// Stop the SDP server thread
	fIsShuttingDown = true;

	status_t threadReturnStatus;
	wait_for_thread(fSDPThreadID, &threadReturnStatus);
	TRACE_BT("BluetoothServer server thread exited with: %s\n", strerror(threadReturnStatus));

	delete fEventListener;
	TRACE_BT("Shutting down bluetooth_server.\n");

	return BApplication::QuitRequested();
}


void
BluetoothServer::ArgvReceived(int32 argc, char **argv)
{
	if (argc > 1) {
		if (strcmp(argv[1], "--finish") == 0)
			PostMessage(B_QUIT_REQUESTED);
	}
}


void
BluetoothServer::ReadyToRun()
{
	fDeviceManager->StartMonitoringDevice("bluetooth/h2");
	fDeviceManager->StartMonitoringDevice("bluetooth/h3");
	fDeviceManager->StartMonitoringDevice("bluetooth/h4");
	fDeviceManager->StartMonitoringDevice("bluetooth/h5");

	if (fEventListener->Launch() != B_OK)
		TRACE_BT("General: Bluetooth event listener failed\n");
	else
		TRACE_BT("General: Bluetooth event listener Ready\n");

	_InstallDeskbarIcon();

	// Spawn the SDP server thread
	fSDPThreadID = spawn_thread(SDPServerThread, "SDP server thread", B_NORMAL_PRIORITY, this);

#define _USE_FAKE_SDP_SERVER
#ifdef _USE_FAKE_SDP_SERVER
	if (fSDPThreadID <= 0 || resume_thread(fSDPThreadID) != B_OK) {
		TRACE_BT("BluetoothServer: Failed launching the SDP server thread\n");
	}
#endif
}


void
BluetoothServer::MessageReceived(BMessage* message)
{
	BMessage reply;
	status_t status = B_WOULD_BLOCK; // mark somehow to do not reply anything

	switch (message->what) {
		case BT_MSG_ADD_DEVICE:
		{
			BString deviceName;
			message->FindString("name", &deviceName);

			TRACE_BT("BluetoothServer: Requested LocalDevice %s\n", deviceName.String());
			BPath path(deviceName.String());

			LocalDeviceImpl* localDeviceImpl = LocalDeviceImpl::CreateTransportAccessor(&path);
			if (localDeviceImpl->GetID() >= 0) {
				fLocalDevicesList.AddItem(localDeviceImpl);
				TRACE_BT("LocalDevice %s id=%" B_PRId32 " added\n", deviceName.String(),
					localDeviceImpl->GetID());
			} else
				TRACE_BT("BluetoothServer: Adding LocalDevice hci id invalid\n");

			status = B_WOULD_BLOCK;
			/* TODO: This should be by user request only! */
			localDeviceImpl->Launch();
			break;
		}

		case BT_MSG_REMOVE_DEVICE:
		{
			LocalDeviceImpl* localDeviceImpl = LocateDelegateFromMessage(message);
			if (localDeviceImpl != NULL) {
				fLocalDevicesList.RemoveItem(localDeviceImpl);
				delete localDeviceImpl;
			}
			break;
		}

		case BT_MSG_COUNT_LOCAL_DEVICES:
			status = HandleLocalDevicesCount(message, &reply);
			break;

		case BT_MSG_ACQUIRE_LOCAL_DEVICE:
			status = HandleAcquireLocalDevice(message, &reply);
			break;

		case BT_MSG_HANDLE_SIMPLE_REQUEST:
			status = HandleSimpleRequest(message, &reply);
			break;

		case BT_MSG_GET_PROPERTY:
			status = HandleGetProperty(message, &reply);
			break;

		default:
			BApplication::MessageReceived(message);
			break;
	}

	// Can we reply right now?
	// TODO: Review this condition
	if (status != B_WOULD_BLOCK) {
		reply.AddInt32("status", status);
		message->SendReply(&reply);
//		printf("Sending reply message for->\n");
//		message->PrintToStream();
	}
}


#pragma mark -


LocalDeviceImpl*
BluetoothServer::LocateDelegateFromMessage(BMessage* message)
{
	LocalDeviceImpl* localDeviceImpl = NULL;
	hci_id hid;
	if (message->FindInt32("hci_id", &hid) == B_OK) {
		// Try to find out when a ID was specified
		for (int32 index = 0; index < fLocalDevicesList.CountItems(); index++) {
			localDeviceImpl = fLocalDevicesList.ItemAt(index);
			if (localDeviceImpl->GetID() == hid)
				break;
		}
	}

	return localDeviceImpl;

}


LocalDeviceImpl*
BluetoothServer::LocateLocalDeviceImpl(hci_id hid)
{
	// Try to find out when a ID was specified
	for (int32 index = 0; index < fLocalDevicesList.CountItems(); index++) {
		LocalDeviceImpl* localDeviceImpl = fLocalDevicesList.ItemAt(index);
		if (localDeviceImpl->GetID() == hid)
			return localDeviceImpl;
	}

	return NULL;
}


#pragma - Messages reply


status_t
BluetoothServer::HandleLocalDevicesCount(BMessage* message, BMessage* reply)
{
	TRACE_BT("BluetoothServer: count requested\n");

	return reply->AddInt32("count", fLocalDevicesList.CountItems());
}


status_t
BluetoothServer::HandleAcquireLocalDevice(BMessage* message, BMessage* reply)
{
	static int32 lastIndex = 0;

	LocalDeviceImpl* localDeviceImpl = NULL;
	hci_id hid;
	bdaddr_t bdaddr;	
	ssize_t size;	
	if (message->FindInt32("hci_id", &hid) == B_OK)	{
		TRACE_BT("BluetoothServer: GetLocalDevice requested with id\n");
		localDeviceImpl = LocateDelegateFromMessage(message);
	} else if (message->FindData("bdaddr", B_ANY_TYPE, reinterpret_cast<const void**>(&bdaddr),
		&size) == B_OK) {

		// Try to find out when the user specified the address
		TRACE_BT("BluetoothServer: GetLocalDevice requested with bdaddr\n");
		for (lastIndex = 0; lastIndex < fLocalDevicesList.CountItems(); lastIndex++) {
			// TODO: Only possible if the property is available!
			// bdaddr_t local;
			// lDeviceImpl = fLocalDevicesList.ItemAt(lastIndex);
			// if ((lDeviceImpl->GetAddress(&local, message) == B_OK)
			// 	&& bacmp(&local, &bdaddr)) {
			// 	break;
			// }
		}
	} else {
		// Careless, any device not performing operations will be fine.
		TRACE_BT("BluetoothServer: GetLocalDevice plain request\n");
		// From last assigned till end...
		for (int32 index = lastIndex + 1; index < fLocalDevicesList.CountItems(); index++) {
			localDeviceImpl = fLocalDevicesList.ItemAt(index);
			if (localDeviceImpl != NULL && localDeviceImpl->Available()) {
				printf("Requested local device %" B_PRId32 "\n", localDeviceImpl->GetID());
				TRACE_BT("BluetoothServer: Device available: %" B_PRId32 "\n",
					localDeviceImpl->GetID());
				lastIndex = index;
				break;
			}
		}

		// from starting till last assigned if not yet found
		if (localDeviceImpl == NULL) {
			for (int32 index = 0; index <= lastIndex ; index ++) {
				localDeviceImpl = fLocalDevicesList.ItemAt(index);
				if (localDeviceImpl != NULL && localDeviceImpl->Available()) {
					printf("Requested local device %" B_PRId32 "\n",
						localDeviceImpl->GetID());
					TRACE_BT("BluetoothServer: Device available: %" B_PRId32 "\n",
						localDeviceImpl->GetID());
					lastIndex = index;
					break;
				}
			}
		}
	}

	if (lastIndex <= fLocalDevicesList.CountItems() && localDeviceImpl != NULL
		&& localDeviceImpl->Available()) {
		hid = localDeviceImpl->GetID();
		localDeviceImpl->Acquire();

		TRACE_BT("BluetoothServer: Device acquired %" B_PRId32 "\n", hid);
		return reply->AddInt32("hci_id", hid);
	}

	return B_ERROR;

}


status_t
BluetoothServer::HandleSimpleRequest(BMessage* message, BMessage* reply)
{
	LocalDeviceImpl* localDeviceImpl = LocateDelegateFromMessage(message);
	if (localDeviceImpl == NULL)
		return B_ERROR;

	// Find out if there is a property being requested
	const char* propertyRequested;	
	if (message->FindString("property", &propertyRequested) == B_OK
		&& localDeviceImpl->IsPropertyAvailable(propertyRequested)) {
		// Check if the property has been already retrieved and if so, dump everything...
		reply->AddMessage("properties", localDeviceImpl->GetPropertiesMessage());
		return B_OK;
	}

	// We are going to need to issue the command...
	if (localDeviceImpl->ProcessSimpleRequest(DetachCurrentMessage()) == B_OK)
		return B_WOULD_BLOCK;

	localDeviceImpl->Unregister();
	return B_ERROR;
}


status_t
BluetoothServer::HandleGetProperty(BMessage* message, BMessage* reply)
{
	// User side will look for the reply in a result field and will
	// not care about status fields, therefore we return OK in all cases
	LocalDeviceImpl* localDeviceImpl = LocateDelegateFromMessage(message);
	if (localDeviceImpl == NULL)
		return B_ERROR;

	// Find out if there is a property being requested...
	const char* propertyRequested;
	if (message->FindString("property", &propertyRequested) == B_OK) {
		TRACE_BT("BluetoothServer: Searching %s property...\n", propertyRequested);

		// Check if the property has been already retrieved
		if (localDeviceImpl->IsPropertyAvailable(propertyRequested)) {
			// 1 byte requests
			if (strcmp(propertyRequested, "hci_version") == 0
				|| strcmp(propertyRequested, "lmp_version") == 0
				|| strcmp(propertyRequested, "sco_mtu") == 0) {

				uint8 result = localDeviceImpl->GetPropertiesMessage()->
					FindInt8(propertyRequested);
				reply->AddInt32("result", result);

			// 2 bytes requests
			} else if (strcmp(propertyRequested, "hci_revision") == 0
				|| strcmp(propertyRequested, "lmp_subversion") == 0
				|| strcmp(propertyRequested, "manufacturer") == 0
				|| strcmp(propertyRequested, "acl_mtu") == 0
				|| strcmp(propertyRequested, "acl_max_pkt") == 0
				|| strcmp(propertyRequested, "sco_max_pkt") == 0
				|| strcmp(propertyRequested, "packet_type") == 0 ) {

				uint16 result = localDeviceImpl->GetPropertiesMessage()->
					FindInt16(propertyRequested);
				reply->AddInt32("result", result);

			// 1 bit requests
			} else if (strcmp(propertyRequested, "role_switch_capable") == 0
					|| strcmp(propertyRequested, "encrypt_capable") == 0) {

				bool result = localDeviceImpl->GetPropertiesMessage()->FindBool(propertyRequested);
				reply->AddInt32("result", result);

			} else
				TRACE_BT("BluetoothServer: Property %s could not be satisfied\n", propertyRequested);
		}
	}

	return B_OK;
}


#pragma mark -


status_t
BluetoothServer::SDPServerThread(void* data)
{
	const BluetoothServer* server = static_cast<BluetoothServer*>(data);

	// Set up the SDP socket
	TRACE_BT("SDP: SDP server thread up...\n");
	int socketServer = socket(PF_BLUETOOTH, SOCK_STREAM, BLUETOOTH_PROTO_L2CAP);
	if (socketServer < 0) {
		TRACE_BT("SDP: Could not create server socket ...\n");
		return socketServer;
	}

	// Bind socket to port 0x1001 of the first available Bluetooth adapter
	struct sockaddr_l2cap l2capAddress = { 0 };
	l2capAddress.l2cap_family = AF_BLUETOOTH;
	l2capAddress.l2cap_bdaddr = BDADDR_ANY;
	l2capAddress.l2cap_psm = B_HOST_TO_LENDIAN_INT16(1);
	l2capAddress.l2cap_len = sizeof(struct sockaddr_l2cap);

	status_t result = bind(socketServer, reinterpret_cast<struct sockaddr*>(&l2capAddress),
		sizeof(struct sockaddr_l2cap));
	if (result < 0) {
		TRACE_BT("SDP: Could not bind server socket (%s)...\n", strerror(result));
		return result;
	}

	// setsockopt(sock, SOL_L2CAP, SO_L2CAP_OMTU, &omtu, len );
	// getsockopt(sock, SOL_L2CAP, SO_L2CAP_IMTU, &omtu, &len );

	// Listen for up to 10 connections
	result = listen(socketServer, 10);
	if (result != B_OK) {
		TRACE_BT("SDP: Could not listen server socket (%s)...\n", strerror(result));
		return result;
	}

	char buffer[512] = "";
	while (!server->fIsShuttingDown) {
		TRACE_BT("SDP: Waiting connection for socket (%s)...\n", strerror(result));

		size_t length = sizeof(struct sockaddr_l2cap);
		int client = accept(socketServer, reinterpret_cast<struct sockaddr*>(&l2capAddress),
			reinterpret_cast<socklen_t*>(&length));

		TRACE_BT("SDP: Incomming connection... %d\n", client);

		ssize_t receivedSize;
		do {
			receivedSize = recv(client, buffer, kSDPClientBufferSize, 0);
			if (receivedSize < 0)
				TRACE_BT("SDP: Error reading client socket\n");
			else {
				TRACE_BT("SDP: Received from SDP client: %ld:\n", receivedSize);
				for (ssize_t i = 0; i < receivedSize ; i++)
					TRACE_BT("SDP: %x:", buffer[i]);

				TRACE_BT("\n");
			}
		} while (receivedSize >= 0);

		snooze(5000000);
		TRACE_BT("SDP: Waiting for next connection...\n");
	}

	// Close the socket
	close(socketServer);

	return B_NO_ERROR;
}


void
BluetoothServer::ShowWindow(BWindow* window)
{
	window->Lock();
	if (window->IsHidden())
		window->Show();
	else
		window->Activate();
	window->Unlock();
}


void
BluetoothServer::_InstallDeskbarIcon()
{
	app_info appInfo;
	be_app->GetAppInfo(&appInfo);

	BDeskbar deskbar;
	if (deskbar.HasItem(kDeskbarItemName))
		_RemoveDeskbarIcon();

	status_t result = deskbar.AddItem(&appInfo.ref);
	if (result != B_OK)
		TRACE_BT("Failed adding deskbar icon: %" B_PRId32 "\n", result);
}


void
BluetoothServer::_RemoveDeskbarIcon()
{
	BDeskbar deskbar;
	status_t result = deskbar.RemoveItem(kDeskbarItemName);
	if (result != B_OK)
		TRACE_BT("Failed removing Deskbar icon: %" B_PRId32 ": \n", result);
}


#pragma mark -


int
main(int /*argc*/, char** /*argv*/)
{
	BluetoothServer* bluetoothServer = new BluetoothServer;
	bluetoothServer->Run();

	delete bluetoothServer;

	return 0;
}

