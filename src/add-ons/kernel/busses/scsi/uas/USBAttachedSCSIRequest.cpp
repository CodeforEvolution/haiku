/*
* Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */


#include "USBAttachedSCSI.h"
#include "USBAttachedSCSIRequest.h"

#include <vm/vm.h>


USBAttachedSCSIRequest::USBAttachedSCSIRequest()
	:
	fStatus(B_NO_INIT),
	fBounceArea(0),
	fBounceBuffer(NULL),
	fBounceBufferInUse(false),
{
	fConditionVariable.Init(this, "hyperv_scsi request");

	fBounceArea = create_area("hyperv_scsi buffer", &fBounceBuffer, B_ANY_KERNEL_ADDRESS,
		HV_SCSI_MAX_BUFFER_SIZE, B_CONTIGUOUS, B_KERNEL_READ_AREA | B_KERNEL_WRITE_AREA);
	if (fBounceArea < B_OK) {
		fStatus = fBounceArea;
		return;
	}

	physical_entry entry;
	fStatus = get_memory_map(fBounceBuffer, HV_SCSI_MAX_BUFFER_SIZE, &entry, 1);
	if (fStatus != B_OK)
		return;
	fBounceBufferPhys = entry.address;

	fStatus = B_OK;
	Reset();
}


USBAttachedSCSIRequest::~USBAttachedSCSIRequest()
{
	delete_area(fBounceArea);
}


void
USBAttachedSCSIRequest::Reset()
{
	bzero(&fMessage, sizeof(fMessage));
	SetTimeout(HV_SCSI_TIMEOUT_US);

	fCCB = NULL;
	fBounceBufferInUse = false;
}


status_t
USBAttachedSCSIRequest::PrepareData()
{
	if (fCCB == NULL)
		return B_BAD_VALUE;

	uint32 flags = fCCB->flags & SCSI_DIR_MASK;
	if (flags != SCSI_DIR_IN && flags != SCSI_DIR_OUT)
		return B_BAD_VALUE;

	if (fCCB->sg_count == 0)
		return B_BAD_VALUE;

	if (fCCB->sg_count < 2)
		return _FillGPARange(fCCB->sg_list, fCCB->sg_count, fCCB->data_length);

	// Hyper-V supports multiple discontiguous buffers, but the SCSI controller device does not
	// Bounce buffer will be used if there are any non-page sized entries, or holes
	fBounceBufferInUse = false;
	for (uint16 i = 0; i < fCCB->sg_count; i++) {
		phys_addr_t offset = fCCB->sg_list[i].address & HV_PAGE_MASK;
		phys_size_t size = fCCB->sg_list[i].size;

		if (i == 0) {
			// Ensure first entry does not leave a hole between it and the next entry
			if (offset + size != HV_PAGE_SIZE) {
				fBounceBufferInUse = true;
				break;
			}
		} else if (i == fCCB->sg_count - 1) {
			// Ensure last entry is page-aligned
			if (offset != 0) {
				fBounceBufferInUse = true;
				break;
			}
		} else {
			// Ensure all other entries are page-aligned and an entire page
			if (offset != 0 || size != HV_PAGE_SIZE) {
				fBounceBufferInUse = true;
				break;
			}
		}
	}

	if (!fBounceBufferInUse)
		return _FillGPARange(fCCB->sg_list, fCCB->sg_count, fCCB->data_length);

	if (fCCB->data_length > HV_SCSI_MAX_BUFFER_SIZE)
		return B_NO_MEMORY;

	TRACE("Bounce buffer used\n");

	// Copy data to bounce buffer if a write was requested
	if ((fCCB->flags & SCSI_DIR_MASK) == SCSI_DIR_OUT) {
		uint8* bounceBufferPtr = static_cast<uint8*>(fBounceBuffer);
		for (uint32 i = 0; i < fCCB->sg_count; i++) {
			vm_memcpy_from_physical(bounceBufferPtr, fCCB->sg_list[i].address,
				fCCB->sg_list[i].size, false);
			bounceBufferPtr += fCCB->sg_list[i].size;
		}
	}

	physical_entry entry;
	entry.address = fBounceBufferPhys;
	entry.size = fCCB->data_length;
	return _FillGPARange(&entry, 1, fCCB->data_length);
}


void
USBAttachedSCSIRequest::Complete(uint8 status)
{
	TRACE("Complete request %p status 0x%X\n", this, status);

	if (fCCB != NULL) {
		fCCB->subsys_status = status;
		if (fMessage.header.type == HV_SCSI_MSGTYPE_COMPLETE_IO) {
			if (fCCB->data_length == 0)
				fCCB->data_resid = 0;
			else
				fCCB->data_resid = fCCB->data_length - fMessage.request.data_length;

			TRACE("  msg status 0x%X SRB status 0x%X SCSI status 0x%X data resid %u\n",
				fMessage.header.status, fMessage.request.srb_status, fMessage.request.scsi_status,
				fCCB->data_resid);

			// Copy data from bounce buffer if used
			if (fBounceBufferInUse) {
				uint8* bounceBufferPtr = static_cast<uint8*>(fBounceBuffer);
				for (uint32 i = 0; i < fCCB->sg_count; i++) {
					vm_memcpy_to_physical(fCCB->sg_list[i].address, bounceBufferPtr,
						fCCB->sg_list[i].size, false);
					bounceBufferPtr += fCCB->sg_list[i].size;
				}
			}

			fCCB->subsys_status = fMessage.request.srb_status;
			fCCB->device_status = fMessage.request.scsi_status;
			if (fMessage.request.scsi_status == SCSI_STATUS_CHECK_CONDITION
					&& (fMessage.request.srb_status & SCSI_AUTOSNS_VALID) != 0
					&& (fCCB->flags & SCSI_DIS_AUTOSENSE) == 0) {
				uint32 senseLength = min_c(sizeof(fCCB->sense),
					fMessage.request.sense_info_length);
				TRACE("  sense length 0x%X\n", senseLength);

				memcpy(fCCB->sense, fMessage.request.sense, senseLength);
				fCCB->sense_resid = sizeof(fCCB->sense) - senseLength;
			}
		}

		gSCSI->finished(fCCB, 1);
	}
}


void
USBAttachedSCSIRequest::SetMessageData(hv_scsi_msg* message)
{
	memcpy(&fMessage, message, sizeof(fMessage));
}


void
USBAttachedSCSIRequest::AddWaiter(ConditionVariableEntry* entry)
{
	fConditionVariable.Add(entry);
}


void
USBAttachedSCSIRequest::Notify()
{
	fConditionVariable.NotifyAll();
}
