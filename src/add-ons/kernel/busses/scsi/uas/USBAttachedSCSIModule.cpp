/*
* Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */


#include "USBAttachedSCSI.h"

#include <USB3.h>

#include "../../../../../../headers/os/drivers/device_manager.h"
#include "../../../../../../headers/os/drivers/USB3.h"
#include "../../../../../../headers/os/drivers/bus/USB.h"
#include "../../../../../../headers/os/drivers/usb/USB_massbulk.h"
#include "../../../../../../headers/os/support/SupportDefs.h"
#include "../../../drivers/audio/usb/Driver.h"


device_manager_info* gDeviceManager;
scsi_for_sim_interface* gSCSI;
usb_module_info* gUSB;



static status_t
usb_attached_scsi_init_bus(device_node* node, void** _driverCookie)
{
	CALLED();

	USBAttachedSCSI* scsi = new(std::nothrow) USBAttachedSCSI(node);
	if (scsi == NULL)
		return B_NO_MEMORY;

	status_t status = scsi->InitCheck();
	if (status != B_OK) {
		delete scsi;
		return status;
	}

	*_driverCookie = scsi;
	return B_OK;
}


static void
usb_attached_scsi_uninit_bus(void* driverCookie)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(driverCookie);
	delete scsi;
}


static void
usb_attached_scsi_set_scsi_bus(scsi_sim_cookie cookie, scsi_bus bus)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(cookie);
	scsi->SetBus(bus);
}


static void
usb_attached_scsi_scsi_io(scsi_sim_cookie cookie, scsi_ccb* ccb)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(cookie);
	if (scsi->StartIO(ccb) == B_BUSY)
		gSCSI->requeue(ccb, true);
}


static uchar
usb_attached_scsi_abort(scsi_sim_cookie cookie, scsi_ccb* ccb_to_abort)
{
	CALLED();
	return SCSI_REQ_CMP;
}


static uchar
usb_attached_scsi_reset_device(scsi_sim_cookie cookie, uchar target_id, uchar target_lun)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(cookie);
	return scsi->ResetDevice(target_id, target_lun);
}


static uchar
usb_attached_scsi_term_io(scsi_sim_cookie cookie, scsi_ccb* ccb_to_terminate)
{
	CALLED();
	return SCSI_REQ_CMP;
}


static uchar
usb_attached_scsi_path_inquiry(scsi_sim_cookie cookie, scsi_path_inquiry* inquiry_data)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(cookie);
	return scsi->PathInquiry(inquiry_data);
}


static uchar
usb_attached_scsi_scan_bus(scsi_sim_cookie cookie)
{
	// Function is called prior to a full bus scan
	CALLED();
	return SCSI_REQ_CMP;
}


static uchar
usb_attached_scsi_reset_bus(scsi_sim_cookie cookie)
{
	CALLED();
	USBAttachedSCSI* scsi = reinterpret_cast<USBAttachedSCSI*>(cookie);
	return scsi->ResetBus();
}


static void
usb_attached_scsi_get_restrictions(scsi_sim_cookie cookie, uchar target_id, bool* is_atapi,
	bool* no_autosense, uint32* max_blocks)
{
	CALLED();

	*is_atapi = true; // TODO: Atapi?
	*no_autosense = false; // TODO: Autosense?
	*max_blocks = 1; // TODO: Max block count?
}


static status_t
usb_attached_scsi_ioctl(scsi_sim_cookie, uint8 targetID, uint32 op, void* buffer, size_t length)
{
	CALLED();
	return B_DEV_INVALID_IOCTL;
}


static float
usb_attached_scsi_supports_device(device_node* parent)
{
	CALLED();

	// Is the incoming device a child of the USB bus?
	const char* bus;
	if (gDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false))
		return -1;
	if (strcmp(bus, "usb") != 0)
		return 0.0f;

	usb_device device;
	if (gDeviceManager->get_attr_uint32(parent, USB_DEVICE_ID_ITEM, &device, true))
		return -1;

	const usb_configuration_info* configuration = gUSB->get_configuration(device);
	if (configuration == NULL)
		return -1;

	static usb_support_descriptor sSupportedDevices[] = {
		{ USB_MASS_STORAGE_DEVICE_CLASS, 0x06 /* SCSI */, 0x62 /* UASP */, 0, 0 },
	};

	for (size_t index = 0; index < configuration->interface_count; index++) {
		usb_interface_info* interface = configuration->interface[index].active;
		if (interface == NULL)
			continue;

		for (size_t index = 0; index < B_COUNT_OF(sSupportedDevices); index++) {
			if (interface->descr->interface_class != sSupportedDevices[index].dev_class)
				continue;
			if (interface->descr->interface_subclass != sSupportedDevices[index].dev_subclass)
				continue;
			if (interface->descr->interface_protocol != sSupportedDevices[index].dev_protocol)
				continue;
		}

		TRACE("USB Attached SCSI device found!\n");
	}
	;
	return 0.0f;
}


static status_t
usb_attached_scsi_register_device(device_node* parent)
{
	CALLED();

	device_attr attributes[] = {
		{ B_DEVICE_PRETTY_NAME, B_STRING_TYPE,
			{ .string = "USB Attached SCSI Controller" }},

		{ SCSI_DEVICE_MAX_TARGET_COUNT, B_UINT32_TYPE,
			{ .ui32 = 1 }}, // TODO
		{ SCSI_DEVICE_MAX_LUN_COUNT, B_UINT32_TYPE,
			{ .ui32 = 1 }},

		{ B_DMA_ALIGNMENT, B_UINT32_TYPE,
			{ .ui32 = 1 }}, // Word alignment
		{ B_DMA_MAX_SEGMENT_BLOCKS, B_UINT32_TYPE,
			{ .ui32 = 4096 }}, // TODO: Page Size
		{ B_DMA_MAX_SEGMENT_COUNT, B_UINT32_TYPE,
			{ .ui32 = 1 }}, // TODO: Max buffer segments

		{ NULL }
	};

	return gDeviceManager->register_node(parent, USB_ATTACHED_SCSI_DRIVER_MODULE_NAME, attributes, NULL,
		NULL);
}


static status_t
usb_attached_scsi_init_driver(device_node* node, void** _driverCookie)
{
	CALLED();
	*_driverCookie = node;
	return B_OK;
}


static status_t
usb_attached_scsi_register_child_devices(void* driverCookie)
{
	CALLED();
	device_node* node = reinterpret_cast<device_node*>(driverCookie);

	int32 id = gDeviceManager->create_id(USB_ATTACHED_SCSI_ID_GENERATOR);
	if (id < 0)
		return id;

	device_attr attributes[] = {
		{ B_DEVICE_FIXED_CHILD, B_STRING_TYPE,
			{ .string = SCSI_FOR_SIM_MODULE_NAME }},
		{ B_DEVICE_PRETTY_NAME, B_STRING_TYPE,
			{ .string = USB_ATTACHED_SCSI_SIM_PRETTY_NAME }},

		{ SCSI_DESCRIPTION_CONTROLLER_NAME, B_STRING_TYPE,
			{ .string = USB_ATTACHED_SCSI_DRIVER_MODULE_NAME }},

		{ USB_ATTACHED_SCSI_ID_ITEM, B_UINT32_TYPE,
			{ .ui32 = static_cast<uint32>(id) }},

		{ NULL }
	};

	status_t status = gDeviceManager->register_node(node, USB_ATTACHED_SCSI_SIM_MODULE_NAME, attributes,
		NULL, NULL);
	if (status != B_OK)
		gDeviceManager->free_id(USB_ATTACHED_SCSI_ID_GENERATOR, id);

	return status;
}


static scsi_sim_interface sUSBAttachedSCSISimInterface = {
	{
		{
			USB_ATTACHED_SCSI_SIM_MODULE_NAME,
			0,
			NULL
		},

		NULL,	// supports device
		NULL,	// register device
		usb_attached_scsi_init_bus,
		usb_attached_scsi_uninit_bus,
		NULL,	// register child devices
		NULL,	// rescan child devices
		NULL,	// device removed
		NULL,	// suspend
		NULL	// resume
	},

	usb_attached_scsi_set_scsi_bus,
	usb_attached_scsi_scsi_io,
	usb_attached_scsi_abort,
	usb_attached_scsi_reset_device,
	usb_attached_scsi_term_io,
	usb_attached_scsi_path_inquiry,
	usb_attached_scsi_scan_bus,
	usb_attached_scsi_reset_bus,
	usb_attached_scsi_get_restrictions,
	usb_attached_scsi_ioctl
};


static driver_module_info sUSBAttachedSCSIModule = {
	{
		USB_ATTACHED_SCSI_DRIVER_MODULE_NAME,
		0,
		NULL
	},

	usb_attached_scsi_supports_device,
	usb_attached_scsi_register_device,
	usb_attached_scsi_init_driver,
	NULL,	// uninit driver
	usb_attached_scsi_register_child_devices,
	NULL,	// rescan child devices
	NULL,	// device removed
	NULL,	// suspend
	NULL	// resume
};


module_dependency module_dependencies[] = {
	{ B_DEVICE_MANAGER_MODULE_NAME, (module_info**)&gDeviceManager },
	{ SCSI_FOR_SIM_MODULE_NAME, (module_info**)&gSCSI },
	{ B_USB_MODULE_NAME, (module_info**)&gUSB },
	{}
};


module_info *modules[] = {
	(module_info*)&sUSBAttachedSCSIModule,
	(module_info*)&sUSBAttachedSCSISimInterface,
	NULL
};
