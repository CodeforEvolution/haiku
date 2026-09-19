/*
* Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */
#ifndef USB_ATTACHED_SCSI_REQUEST_H
#define USB_ATTACHED_SCSI_REQUEST_H


#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bus/SCSI.h>
#include <condition_variable.h>
#include <lock.h>
#include <scsi_cmds.h>
#include <util/DoublyLinkedList.h>


class USBAttachedSCSIRequest : public DoublyLinkedListLinkImpl<USBAttachedSCSIRequest> {
public:
							USBAttachedSCSIRequest();
							~USBAttachedSCSIRequest();
	status_t				InitCheck() const { return fStatus; }

	void					Reset();
	status_t				PrepareData();
	void					Complete(uint8 status);

	bigtime_t				GetTimeout() const { return fTimeout; }
	void					SetTimeout(bigtime_t timeout) { fTimeout = timeout; }

	void					SetCCB(scsi_ccb* ccb) { fCCB = ccb; }
	scsi_ccb*				GetCCB() { return fCCB; }

	void					AddWaiter(ConditionVariableEntry* entry);
	void					Notify();

private:

private:
	mutex					fLock;
	status_t				fStatus;
	bigtime_t				fTimeout;
	scsi_ccb*				fCCB;

	area_id					fBounceArea;
	void*					fBounceBuffer;
	phys_addr_t				fBounceBufferPhys;
	bool					fBounceBufferInUse;

	ConditionVariable		fConditionVariable;
};
typedef DoublyLinkedList<USBAttachedSCSIRequest> USBAttachedSCSIRequestList;


#endif// USB_ATTACHED_SCSI_REQUEST_H
