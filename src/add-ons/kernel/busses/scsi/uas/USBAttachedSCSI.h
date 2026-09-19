/*
* Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */
#ifndef USB_ATTACHED_SCSI_H
#define USB_ATTACHED_SCSI_H


#include <AutoDeleter.h>
#include <AutoDeleterDrivers.h>
#include <bus/SCSI.h>
#include <condition_variable.h>
#include <lock.h>
#include <scsi_cmds.h>
#include <util/AutoLock.h>

#include "USBAttachedSCSIRequest.h"


//#define TRACE_USB_ATTACHED_SCSI
#ifdef TRACE_USB_ATTACHED_SCSI
#	define TRACE(x...) dprintf("\33[94musb_attached_scsi:\33[0m " x)
#else
#	define TRACE(x...) ;
#endif
#define TRACE_ALWAYS(x...)	dprintf("\33[94musb_attached_scsi:\33[0m " x)
#define ERROR(x...)			dprintf("\33[94musb_attached_scsi:\33[0m " x)
#define CALLED(x...)		TRACE("CALLED %s\n", __PRETTY_FUNCTION__)


#define USB_ATTACHED_SCSI_ID_GENERATOR			"usb_attached_scsi/id"
#define USB_ATTACHED_SCSI_ID_ITEM				"usb_attached_scsi/id"
#define USB_ATTACHED_SCSI_DRIVER_MODULE_NAME	"busses/scsi/usb_attached_scsi/driver_v1"
#define USB_ATTACHED_SCSI_SIM_MODULE_NAME		"busses/scsi/usb_attached_scsi/sim/driver_v1"
#define USB_ATTACHED_SCSI_SIM_PRETTY_NAME		"USB Attached SCSI Interface"

#define USB_ATTACHED_SCSI_MAX_REQUESTS 10


extern device_manager_info* gDeviceManager;
extern scsi_for_sim_interface* gSCSI;


// holds common information about an attached device (pointed to by luns)
typedef struct disk_device_s {
					disk_device_s();
					~disk_device_s();

	int32			number;
	device_node*	node;

	usb_device		device;
	bool			removed;
	uint32			open_count;
	recursive_lock	io_lock;
	mutex			lock;

	// device state
	usb_pipe		status_pipe;
	usb_pipe		data_in_pipe;
	usb_pipe		command_pipe;
	usb_pipe		data_out_pipe;

	uint8			interface;
	uint32			current_tag;
	uint8			sync_support;
	bool			tur_supported;

	// used to store callback information
	sem_id			notify;
	status_t		status;
	size_t			actual_length;

	// logical units of this device
	uint8			lun_count;
	device_lun**	luns;
} disk_device;


// represents a logical unit on the pointed to device - this gets published
struct device_lun_s {
	disk_device*	device;
	char			name[32];
	uint8			logical_unit_number;

	IOScheduler*	io_scheduler;
	bool			should_sync;

	// device information through read capacity/inquiry
	bool			media_present;
	bool			media_changed;
	uint64			block_count;
	uint32			block_size;
	uint32			physical_block_size;
	uint8			device_type;
	bool			removable;
	bool			write_protected;

	char			vendor_name[8];
	char			product_name[16];
	char			product_revision[4];
};


class USBAttachedSCSI {
public:
							USBAttachedSCSI(device_node* node);
							~USBAttachedSCSI();

	status_t				InitCheck() const { return fStatus; }
	scsi_bus				GetBus() const { return fBus; }
	void					SetBus(scsi_bus bus) { fBus = bus; }

	status_t				StartIO(scsi_ccb* ccb);
	uchar					PathInquiry(scsi_path_inquiry* inquiry_data);
	uchar					ResetBus();
	uchar					ResetDevice(uchar targetID, uchar targetLUN);

private:
	uint8					_GetSenseLength() const;
	uint32					_GetMessageLengthDelta() const;
	static	void			_CallbackHandler(void* data);
	void					_Callback();
	static	void			_RescanDPCHandler(void* data);
	void					_RescanBus();
	status_t				_SendRequest(USBAttachedSCSIRequest* request, bool wait);
	USBAttachedSCSIRequest*	_GetRequest();
	void					_ReturnRequest(USBAttachedSCSIRequest* request);
	void					_CompleteIO(uint64 transactionID, hv_scsi_msg* message);
	status_t				_BeginInit();
	status_t				_NegotiateProtocol();
	status_t				_QueryProperties();
	status_t				_EndInit();

private:
	device_node*			fNode;
	scsi_bus				fBus;
	scsi_dpc_cookie			fBusDPC;
	status_t				fStatus;

	uint8					fTargetID;
	uint8					fMaxDevices;
	uint16					fVersion;
	uint16					fMaxSubChannels;
	uint32					fMaxTransferBytes;

	USBAttachedSCSIRequestList	fFreeRequests;
	USBAttachedSCSIRequestList	fActiveRequests;
	mutex					fRequestLock;
};


#endif // USB_ATTACHED_SCSI_H