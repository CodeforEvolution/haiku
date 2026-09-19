/*
* Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef USB_UAS_H
#define USB_UAS_H

#include <SupportDefs.h>

// TODO: List references

// Endpoints
// For all endpoints
// - Bulk
// - Max Packet Size: 1 x 1024 bytes
// - Interval: 0
// -
// Endpoint 1: Command Pipe (0x01)
// - EP 1 OUT
// - Endpoint Address: 0x01
// - Max Burst: 0
// Endpoint 2: Data-in pipe (0x03)
// - EP 2 IN
// - Endpoint Address: 0x82
// - Max Burst: 3
// - Max Streams: 4
// Endpoint 3: Data-out pipe (0x04)
// - EP 3 OUT
// - Endpoint Address: 0x03
// - Max Burst: 15
// - Max Streams: 4
// Endpoint 4: Status pipe (0x02)
// - EP 4 IN
// - Endpoint Address: 0x84
// - Max Burst: 3
// - Max Streams: 4

// Mass Storage Specific Pipe Usage Descriptor
#define USB_PIPE_USAGE_DESCRIPTOR	0x24

typedef struct {
	uint8_t length;
	uint8_t descriptor_type; // USB_PIPE_USAGE_DESCRIPTOR
	uint8_t pipe_id;
	uint8_t _reserved1;
} usb_uas_pipe_usage_descriptor;

// Information Unit Identifiers
enum {
	// 0x00 - Reserved...But maybe not...
	USB_COMMAND_IU			= 0x01,
	// 0x02 - Reserved
	USB_SENSE_IU			= 0x03,
	USB_RESPONSE_IU			= 0x04,
	USB_TASK_MANAGEMENT_IU	= 0x05,
	USB_READ_READY_IU		= 0x06,
	USB_WRITE_READY_IU		= 0x07,
	// 0x08..0xFF - Reserved
};

// Task Attributes
enum {
	SIMPLE = 0b000,
	HEAD_OF_QUEUE = 0b001,
	ORDERED = 0b010,
	// 0b011 - Reserved
	ACA = 0b100
	// 0b101..0b111 - Reserved
};

// Response Codes
enum {
	TASK_MANAGEMENT_FUNCTION_COMPLETE = 0x00,
	// 0x01 - Reserved
	INVALID_INFORMATION_UNIT = 0x02,
	// 0x03 - Reserved
	TASK_MANAGEMENT_FUNCTION_NOT_SUPPORTED = 0x04,
	TASK_MANAGEMENT_FUNCTION_FAILED = 0x05,
	// 0x06..0x07 - Reserved
	TASK_MANAGEMENT_FUNCTION_SUCCEEDED = 0x08,
	INCORRECT_LOGICAL_UNIT_NUMBER = 0x09,
	OVERLAPPED_TAG_ATTEMPTED = 0x0A,
	// 0x0B..0xFF - Reserved
};

// Task Management Functions
enum {
	// 0x00 - Reserved
	ABORT_TASK = 0x01,
	ABORT_TASK_SET = 0x02,
	// 0x03 - Reserved
	CLEAR_TASK_SET = 0x04,
	// 0x05..0x07 - Reserved
	LOGICAL_UNIT_RESET = 0x08,
	// 0x09..0x0F - Reserved
	I_T_NEXUS_RESET = 0x10,
	// 0x11..0x3F - Reserved
	CLEAR_ACA = 0x40,
	// 0x41..0x7F - Reserved
	QUERY_TASK = 0x80,
	QUERY_TASK_SET = 0x81,
	QUERY_UNIT_ATTENTION = 0x82,
	// 0x83..0xFF - Reserved
};

// Command IU...?
typedef struct {
	uint8_t id; // USB_COMMAND_IU
	uint8_t _reserved1;
	uint16_t tag;
	uint8_t _reserved2 : 1;
	uint8_t command_priority : 4;
	uint8_t task_attribute : 3; // One of "Task Attributes"
	uint8_t _reserved3;
	uint8_t additional_cdb_length;
	uint8_t _reserved4;
	uint16_t lun_address_mode: 2;
	uint16_t lun: 14;
	uint8_t _reserved5[6];
	uint8_t cdb[0]; // At least 16 bytes; Uses 'additional_cdb_length'
} _PACKED usb_uas_command_iu;

// Sense IU...?
typedef struct {
	uint8_t id; // USB_SENSE_IU
	uint8_t _reserved1;
	uint16_t tag;
	uint16_t status_qualifier;
	uint8_t status;
	uint8_t _reserved2[7]; // 7 bytes??? Really...
	uint16_t length;
	uint8_t sense_data[0]; // "length" bytes
} _PACKED usb_uas_sense_iu;

// TODO: Response IU???
typedef struct {
	uint8_t id; // USB_RESPONSE_IU
	uint8_t _reserved1;
	uint16_t tag;
	uint16_t additional_response_information;
	uint8_t response_code; // One of "Response Codes"
} _PACKED usb_uas_response_iu;

// TODO: Task Management IU???
typedef struct {
	uint8_t id; // USB_TASK_MANAGEMENT_IU
	uint8_t _reserved1;
	uint16_t tag;
	uint8_t task_management_function; // One of "Task Management Functions"
	uint8_t _reserved2;
	uint16_t managed_task_tag;
	uint16_t lun;
} _PACKED usb_uas_task_management_iu;

// TODO: Read/Write Ready IU???
typedef struct {
	uint8_t id; // USB_{READ | WRITE}_READY_IU
	uint8_t _reserved1;
	uint16_t tag;
} _PACKED usb_uas_ready_iu;

#endif // USB_UAS_H
