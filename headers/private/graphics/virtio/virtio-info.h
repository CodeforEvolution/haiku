/*
 * Copyright 2020 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */
#ifndef VIRTIO_INFO_H
#define VIRTIO_INFO_H


#include <Drivers.h>
#include <Accelerant.h>
#include <PCI.h>

#include <edid.h>


struct virtio_shared_info {
	int32			type;
	area_id			mode_list_area;		// area containing display mode list
	uint32			mode_count;
	display_mode	current_mode;
	uint32			bytes_per_row;

	area_id			frame_buffer_area;	// area of frame buffer
	uint8*			frame_buffer;
		// pointer to frame buffer (visible by all apps!)
	uint8*			physical_frame_buffer;

	uint32			vesa_mode_offset;
	uint32			vesa_mode_count;

	edid1_info		edid_info;
	bool			has_edid;
	uint32			dpms_capabilities;
};

//----------------- ioctl() interface ----------------

// list ioctls
enum {
	VIRTIO_GET_PRIVATE_DATA = B_DEVICE_OP_CODES_END + 1,
};

#endif	/* VIRTIO_INFO_H */
