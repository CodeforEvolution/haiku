/*
 * Copyright 2007 Oliver Ruiz Dorantes, oliver.ruiz.dorantes_at_gmail.com
 * Copyright 2022-2024 Jacob Secunda, secundaja@gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 *
 */

/*
 * Copyright (c) Maksim Yevmenkin <m_evmenkin@yahoo.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 */


/**************************************************************************
 **************************************************************************
 **                   Common defines and types (L2CAP)
 **************************************************************************
 **************************************************************************/
#ifndef _L2CAP_DEFINITIONS
#define _L2CAP_DEFINITIONS


#include <bluetooth/bluetooth.h>

#define HZ	1000000 // us per second TODO: move somewhere more generic
#define bluetooth_l2cap_ertx_timeout (60 * HZ)
#define bluetooth_l2cap_rtx_timeout  (300 * HZ)

/*
 * Channel IDs are assigned relative to the instance of L2CAP node, i.e.
 * relative to the unit. So the total number of channels that unit can have
 * open at the same time is 0xffff - 0x0040 = 0xffbf (65471). This number
 * does not depend on number of connections.
 */

#define L2CAP_NULL_CID						0x0000	/* Null channel ID */
#define L2CAP_SIGNALING_CID					0x0001	/* Signaling channel ID */
#define L2CAP_CONNECTIONLESS_CID			0x0002	/* Connectionless channel ID */
/* 0x0003 Reserved */
#define L2CAP_ATTRIBUTE_PROTOCOL_CID		0x0004	/* Attribute protocol channel ID (LE-U) */
#define L2CAP_LE_SIGNALING_CID				0x0005	/* LE Signaling channel ID (LE-U) */
#define L2CAP_SECURITY_MANAGER_PROTOCOL_CID	0x0006	/* Security Manager protocol (LE-U) */
#define L2CAP_BR_EDR_SECURITY_MANAGER_CID	0x0007	/* BR/EDR Security Manager channel ID */
/* 0x0008 - 0x0019 Reserved */
#define L2CAP_FIRST_ASSIGNED_NUMBER			0x0020	/* First assigned number (LE-U) */
#define L2CAP_LAST_ASSIGNED_NUMBER			0x003E	/* Last assigned number (LE-U) */
/* 0x003F Reserved */
#define L2CAP_FIRST_CID						0x0040	/* First dynamically alloc channel ID */
#define L2CAP_LAST_LE_CID					0x007F	/* Last LE-U dynamic alloc channel ID */
#define L2CAP_LAST_CID						0xFFFF	/* Last dynamically alloc channel ID */


/*
 * L2CAP signaling command ident's are assigned relative to the connection,
 * because there is only one signaling channel (cid == 0x01) for every
 * connection. So up to 254 (0xff - 0x01) L2CAP commands can be pending at the
 * same time for the same connection.
 */
#define L2CAP_NULL_IDENT		0x00	/* DO NOT USE THIS IDENT */
#define L2CAP_FIRST_IDENT		0x01	/* dynamically alloc. (start) */
#define L2CAP_LAST_IDENT		0xFF	/* dynamically alloc. (end) */


/* L2CAP MTU */
#define L2CAP_MTU_MINIMUM		48
#define L2CAP_MTU_DEFAULT		672
#define L2CAP_MTU_MAXIMUM		0xffff

/* L2CAP flush and link timeouts */
#define L2CAP_FLUSH_TIMO_DEFAULT	0xffff /* always retransmit */
#define L2CAP_LINK_TIMO_DEFAULT		0xffff

/* L2CAP Command Reject reasons */
#define L2CAP_REJ_NOT_UNDERSTOOD	0x0000
#define L2CAP_REJ_MTU_EXCEEDED		0x0001
#define L2CAP_REJ_INVALID_CID		0x0002
/* 0x0003 - 0xffff - reserved for future use */

/* Protocol/Service Multiplexer (PSM) values */
#define L2CAP_PSM_ANY		0x0000	/* Any/Invalid PSM */
#define L2CAP_PSM_SDP		0x0001	/* Service Discovery Protocol */
#define L2CAP_PSM_RFCOMM	0x0003	/* RFCOMM protocol */
#define L2CAP_PSM_TCP		0x0005	/* Telephony Control Protocol */
#define L2CAP_PSM_TCS		0x0007	/* TCS cordless */
#define L2CAP_PSM_BNEP		0x000F	/* BNEP */
#define L2CAP_PSM_HID_CTRL	0x0011	/* HID Control */
#define L2CAP_PSM_HID_INT	0x0013	/* HID Interrupt */
#define L2CAP_PSM_UPnP		0x0015	/* UPnP (ESDP) */
#define L2CAP_PSM_AVCTP		0x0017	/* AVCTP */
#define L2CAP_PSM_AVDTP		0x0019	/* AVDTP */
/*  < 0x1000 - reserved for future use */
/*  0x1001 < x < 0xFFFF dynamically assigned */

/* L2CAP Connection response (L2CAP_CONNECTION_RSP) result codes */
enum l2cap_connection_rsp_result {
	L2CAP_CONN_SUCCESS = 0x0000,
	L2CAP_CONN_PENDING = 0x0001,
	L2CAP_CONN_REFUSED_PSM_NOT_SUPPORTED = 0x0002,
	L2CAP_CONN_REFUSED_SECURITY_BLOCK = 0x0003,
	L2CAP_CONN_REFUSED_NO_RESOURCES = 0x0004,
	// 0x0005 - Reserved for future use
	L2CAP_CONN_REFUSED_INVALID_SOURCE_CID = 0x0006,
	L2CAP_CONN_REFUSED_SOURCE_CID_ALREADY_ALLOCATED = 0x0007
	/* 0x0008 - 0xFFFF - reserved for future use */
};

/* L2CAP Connection response (L2CAP_CONNECTION_RSP) status codes */
enum l2cap_connection_rsp_status {
	L2CAP_NO_INFO = 0x0000,
	L2CAP_AUTH_PENDING = 0x0001,
	L2CAP_AUTZ_PENDING = 0x0002
	/* 0x0003 - 0xFFFF - reserved for future use */
};

/* L2CAP Configuration response (L2CAP_CONFIGURATION_RSP) result codes */
enum l2cap_cfg_rsp_result {
	L2CAP_CFG_SUCCESS = 0x0000,
	L2CAP_CFG_FAIL_UNACCEPTABLE_PARAMS = 0x0001,
	L2CAP_CFG_FAIL_REJECTED = 0x0002,
	L2CAP_CFG_FAIL_UNKNOWN_OPTION = 0x0003,
	L2CAP_CFG_PENDING = 0x0004,
	L2CAP_CFG_FAIL_FLOW_SPEC_REJECTED = 0x0005
	/* 0x0006 - 0xFFFF - reserved for future use */
};

/* L2CAP Configuration options */
#define L2CAP_OPT_CFLAG_BIT		0x0001
#define L2CAP_OPT_CFLAG(flags)		((flags) & L2CAP_OPT_CFLAG_BIT)
#define L2CAP_OPT_HINT_BIT		0x80
#define L2CAP_OPT_HINT(type)		((type) & L2CAP_OPT_HINT_BIT)
#define L2CAP_OPT_HINT_MASK		0x7f
#define L2CAP_OPT_MTU			0x01
#define L2CAP_OPT_MTU_SIZE		sizeof(uint16)
#define L2CAP_OPT_FLUSH_TIMO		0x02
#define L2CAP_OPT_FLUSH_TIMO_SIZE	sizeof(uint16)
#define L2CAP_OPT_QOS			0x03
#define L2CAP_OPT_QOS_SIZE		sizeof(l2cap_flow_t)
/* 0x4 - 0xff - reserved for future use */

#define	L2CAP_CFG_IN	(1 << 0)	/* incoming path done */
#define	L2CAP_CFG_OUT	(1 << 1)	/* outgoing path done */
#define	L2CAP_CFG_BOTH  (L2CAP_CFG_IN | L2CAP_CFG_OUT)
#define	L2CAP_CFG_IN_SENT	(1 << 2)	/* L2CAP ConfigReq sent */
#define	L2CAP_CFG_OUT_SENT	(1 << 3)	/* ---/--- */

/* L2CAP Information request (L2CAP_INFORMATION_REQ) info type codes */
enum l2cap_information_request_info_type {
	L2CAP_CONNLESS_MTU = 0x0001,
	L2CAP_EXTENDED_FEATURES_SUPPORTED = 0x0002,
	L2CAP_FIXED_CHAN_SUPPORT_OVER_BR_EDR = 0x0003
	/* 0x0004 - 0xFFFF - reserved for future use */
};

/* L2CAP Information response (L2CAP_INFORMATION_RSP) result codes */
enum l2cap_information_response_result {
	L2CAP_SUCCESS = 0x0000,
	L2CAP_NOT_SUPPORTED = 0x0001
	/* 0x0002 - 0xffff - reserved for future use */
};

/* L2CAP flow (QoS) */
typedef struct {
	uint8	flags;             /* reserved for future use */
	uint8	service_type;      /* service type */
	uint32	token_rate;        /* bytes per second */
	uint32	token_bucket_size; /* bytes */
	uint32	peak_bandwidth;    /* bytes per second */
	uint32	latency;           /* microseconds */
	uint32	delay_variation;   /* microseconds */
} _PACKED l2cap_flow_t;


/**************************************************************************
 **************************************************************************
 **                 Link level defines, headers and types				 **
 **************************************************************************
 **************************************************************************/

/* Basic L2CAP header (Vol 3 - Sec 3.1) */
// - Header for Basic information frames (B-frames)
// - Commences header for all other frame types too
typedef struct {
	uint16	pduLength;	/* Protocol data unit length (2 octets) */
	uint16	destCID;	/* Destination channel endpoint ID (2 octets) */
} _PACKED l2cap_basic_header_t;


/* L2CAP Connectionless Traffic (Vol 3 - Sec 3.2) */
// - Header for Group frames (G-frames)
// - Only if destination channel ID is L2CAP_CONNECTIONLESS_CID
/*
typedef struct l2cap_clt_header : l2cap_basic_header {
	uint16	psm;
		// Protocol/Service Multiplexer (minimum of 2 octets)
} _PACKED l2cap_clt_header_t;
*/

#define L2CAP_CLT_MTU_MAXIMUM (L2CAP_MTU_MAXIMUM - sizeof(l2cap_clt_header_t))


/* L2CAP Control Field (Vol 3 - Sec 3.3.2) */

enum l2cap_frame_type {
	L2CAP_INFORMATION_FRAME = 0x0,
	L2CAP_SUPERVISORY_FRAME = 0x1
};

enum l2cap_retransmission_state {
	L2CAP_NORMAL_RETRANSMISSION = 0x0,
	L2CAP_POSTPONE_RETRANSMISSION = 0x1
};

enum l2cap_segementation_flags {
	L2CAP_UNSEGMENTED_SDU = 0x0,
	L2CAP_START_SDU = 0x1,
	L2CAP_END_SDU = 0x2,
	L2CAP_CONTINUE_SDU = 0x3
};

enum l2cap_supervisory_function {
	L2CAP_RECEIVER_READY = 0x0,
	L2CAP_REJECT = 0x1,
	L2CAP_RECEIVER_NOT_READY = 0x2,
	L2CAP_SELECTIVE_REJECT = 0x3
};


// L2CAP Standard/Enhanced Control Field for I-frames & S-frames
/*
typedef struct {
	uint8 frame_type : 1;
		// 0: Information (I-frame), 1: Supervisory (S-frame)

	union {
		// I-frame
		uint8 send_sequence : 6;

		// S-frame
		struct {
			uint8 _reserved : 1;
			uint8 supervisory_function : 2;
			uint8 _reserved : 3;
		} _PACKED;
	};

	union {
		// Standard field
		uint8 retransmission_disable_bit : 1;
			// 0: Normal Operation
			// 1: Receiver side requests sender to postpone retransmission of I-frames.

		// Enhanced field
		uint8 final_bit : 1;
	};

	uint8 receive_sequence : 6;

	union {
		uint8 segmentation_flags : 2;	// (SAR)
			// I-frame
		uint8 _reserved : 2;
			// S-frame
	};
} _PACKED l2cap_control_field_t;
*/


// L2CAP Extended Control Field for I-frames & S-frames
/*
typedef struct {
	uint8 frame_type			: 1;
		// 0: Information (I-Frame), 1: Supervisory (S-Frame)

	uint8 final_bit			: 1
	uint16 receive_sequence	: 14;

	union {
		// I-frame
		struct {
			uint8 segmentation_flags : 2;	// (SAR)
			uint16 send_sequence : 14;
		} _PACKED;

		// S-frame
		struct {
			uint8	supervisory_function : 2;
			uint8	poll : 1;
			uint16	_reserved : 13;
		} _PACKED;
} _PACKED l2cap_extended_control_field_t;
*/


/* L2CAP Segmented Data Unit (SDU) length field  (Vol 3 - Sec 3.3.3) */
// - Only used in I-frames where SAR = L2CAP_START_SDU (0x1);
typedef struct {
	uint16 sdu_length;
} _PACKED l2cap_sdu_length_field_t;


/* L2CAP Frame Check Sequence (FCS) field (Vol 3 - Sec 3.3.5) */
typedef struct {
	uint16 frame_check_sequence;
} _PACKED l2cap_fcs_field_t;


/***********************************************/
/* Volume 3 - Sec 4 - Signaling Packet Formats */
/***********************************************/

/* Sec 4.0 - L2CAP command header */
typedef struct {
	uint8	code;		// A signaling command code
	uint8	identifier;	// Matches responses with requests
	uint16	dataLength;	// Size of data field following this header
} _PACKED l2cap_command_header_t;

// Table 4.2 - Signaling command codes
enum l2cap_signaling_command_code {
	L2CAP_COMMAND_REJECT_RSP = 0x01,
	L2CAP_CONNECTION_REQ = 0x02,
	L2CAP_CONNECTION_RSP = 0x03,
	L2CAP_CONFIGURATION_REQ = 0x04,
	L2CAP_CONFIGURATION_RSP = 0x05,
	L2CAP_DISCONNECTION_REQ = 0x06,
	L2CAP_DISCONNECTION_RSP = 0x07,
	L2CAP_ECHO_REQ = 0x08,
	L2CAP_ECHO_RSP = 0x09,
	L2CAP_INFORMATION_REQ = 0x0A,
	L2CAP_INFORMATION_RSP = 0x0B,
	/* 0x0C to 0x11 - Reserved (Previously used) */
	L2CAP_CONNECTION_PARAMETER_UPDATE_REQ = 0x12,
	L2CAP_CONNECTION_PARAMETER_UPDATE_RSP = 0x13,
	L2CAP_LE_CREDIT_BASED_CONNECTION_REQ = 0x14,
	L2CAP_LE_CREDIT_BASED_CONNECTION_RSP = 0x15,
	L2CAP_FLOW_CONTROL_CREDIT_IND = 0x16,
	L2CAP_CREDIT_BASED_CONNECTION_REQ = 0x17,
	L2CAP_CREDIT_BASED_CONNECTION_RSP = 0x18,
	L2CAP_CREDIT_BASED_RECONFIGURE_REQ = 0x19,
	L2CAP_CREDIT_BASED_RECONFIGURE_RSP = 0x1A
	/* Other codes - Reserved */
};


/* Sec 4.1 - L2CAP Command Reject */
typedef struct {
	uint16 reason; // Reason to reject command
	uint8* data; // Reason data (optional, depends on reason)
} _PACKED l2cap_command_reject_response_packet_t;

// Reject reason codes
enum l2cap_reject_reason_code {
	L2CAP_REJECT_NOT_UNDERSTOOD = 0x0000,
	L2CAP_REJECT_SIGNALING_MTU_EXCEEDED = 0x0001,
	L2CAP_REJECT_INVALID_CID = 0x0002
	/* Other codes - Reserved */
};

// Reason data
typedef union {
 	// L2CAP_REJECT_SIGNALING_MTU_EXCEEDED
	struct {
		uint16 mtu;	// Actual signaling MTU
	} _PACKED mtu;

	// L2CAP_REJECT_INVALID_CID
	struct {
		uint16 sourceCID;	// Local channel ID
		uint16 destCID;	// Remote channel ID
	} _PACKED cid;
} l2cap_command_reject_data_u;


/* Sec 4.2 - L2CAP Connection Request */
typedef struct {
	uint16 psm; // Protocol/Service Multiplexor
	uint16 sourceCID; // Source channel ID
} _PACKED l2cap_connection_request_packet_t;


/* Sec 4.3 - L2CAP Connection Response */
typedef struct {
	uint16 destCID;	// Destination channel ID
	uint16 sourceCID;	// Source channel ID
	uint16 result; // 0x00 - success
	uint16 status; // More info if result != 0x00
} _PACKED l2cap_connection_response_packet_t;

// Table 4.6 - Result values for L2CAP Connection Respones
enum l2cap_connection_response_result {
	L2CAP_CONNECTION_SUCCESSFUL = 0x0000,
	L2CAP_CONNECTION_PENDING = 0x0001,
	L2CAP_CONNECTION_REFUSED_PSM_NOT_SUPPORTED = 0x0002,
	L2CAP_CONNECTION_REFUSED_SECURITY_BLOCK = 0x0003,
	L2CAP_CONNECTION_REFUSED_NO_RESOURCES_AVAIL = 0x0004,
	/* 0x0005 - Reserved */
	L2CAP_CONNECTION_REFUSED_INVALID_SOURCE_CID = 0x0006,
	L2CAP_CONNECTION_REFUSED_SOURCE_CID_ALREADY_ALLOC = 0x0007
	/* Other - Reserved */
};

// Table 4.7:
enum l2cap_connection_response_status {
	L2CAP_NO_FURTHER_INFO_AVAIL = 0x0000,
	L2CAP_AUTHENTICATION_PENDING = 0x0001,
	L2CAP_AUTHORIZATION_PENDING = 0x0002,
	/* Other - Reserved */
};


/* Sec 4.4 - L2CAP Configuration Request */
typedef struct {
	uint16	destCID;	// Destination channel ID
	uint16	flags;		// Flags
	uint8	options[];	// Options
} _PACKED l2cap_configuration_request_packet_t;


/* L2CAP Configuration Response */
typedef struct {
	uint16	sourceCID;   // source channel ID
	uint16	flags;  // flags
	uint16	result; // 0x00 - success
	uint8	options[]; // Options
} _PACKED l2cap_configuration_response_packet_t;

// L2CAP Configuration Option
typedef struct {
	uint8 type;
	uint8 length;
	uint8* value; // Option value (depends on type)
} _PACKED l2cap_configuration_option_t;

// L2CAP Configuration Option Value
typedef union {
	uint16 mtu;			/* L2CAP_OPT_MTU */
	uint16 flush_timo;	/* L2CAP_OPT_FLUSH_TIMO */
	l2cap_flow_t flow;	/* L2CAP_OPT_QOS */
} l2cap_configuration_option_value_t;


/* Sec 4.6 - L2CAP Disconnection Request */
typedef struct {
	uint16 destCID; /* destination channel ID */
	uint16 sourceCID; /* source channel ID */
} _PACKED l2cap_disconnection_request_packet_t;


/* Sec 4.7 - L2CAP Disconnection Response */
typedef l2cap_disconnection_request_packet_t l2cap_disconnection_response_packet_t;


/* Sec 4.8 - L2CAP Echo Request */
/* No command parameters, only optional data */


/* Sec 4.9 - L2CAP Echo Response */
#define L2CAP_MAX_ECHO_SIZE \
	(L2CAP_MTU_MAXIMUM - sizeof(l2cap_cmd_header_t))
/* No command parameters, only optional data */


/* Sec 4.10 - L2CAP Information Request */
typedef struct {
	uint16	type; // Requested information type
} _PACKED l2cap_info_request_packet_t;


/* Sec 4.11 - L2CAP Information Response */
typedef struct {
	uint16 type;	// Requested information type
	uint16 result;	// 0x00 - success
	uint8* info;	// Info data (depends on type)
		// L2CAP_CONNLESS_MTU - 2 bytes connectionless MTU
} _PACKED l2cap_info_response_packet_t;

#define IS_SIGNAL_REQ(code) ((code & 1) == 0)
#define IS_SIGNAL_RSP(code) ((code & 1) == 1)

typedef union {
 	/* L2CAP_CONNLESS_MTU */
	struct {
		uint16 mtu;
	} _PACKED mtu;
} l2cap_info_response_data_u;


/* Sec 4.20 - L2CAP Connection Parameter Update Request */
typedef struct {
	uint16 intervalMin;
	uint16 intervalMax;
	uint16 latency;
	uint16 timeout;
} _PACKED l2cap_connection_parameter_update_request_t;


/* Sec 4.21 - L2CAP Connection Parameter Update Response */
typedef struct {
	uint16 result; // 0x0000 - All connections successful
} _PACKED l2cap_connection_parameter_update_response_t;


#endif
