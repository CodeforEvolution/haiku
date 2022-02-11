/*
 * Copyright 2002-2022 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Doerfler
 */
#ifndef _CONFIG_MANAGER_DRIVER_H
#define _CONFIG_MANAGER_DRIVER_H


/*
 * Definitions for the /dev/misc/config driver which provides access to the
 * config_manager via calls to ioctl().
 */


#define B_CM_DEVICE_NAME "misc/config"

/* IOCTLs */
#define B_CM_GET_NEXT_DEVICE_INFO 						'GNDI'
#define B_CM_GET_DEVICE_INFO_FOR						'GDIF'
#define B_CM_GET_SIZE_OF_CURRENT_CONFIGURATION_FOR		'GSCC'
#define B_CM_GET_CURRENT_CONFIGURATION_FOR				'GCCF'
#define B_CM_GET_SIZE_OF_POSSIBLE_CONFIGURATIONS_FOR	'GSPC'
#define B_CM_GET_POSSIBLE_CONFIGURATIONS_FOR			'GPCF'

#define B_CM_COUNT_RESOURCE_DESCRIPTORS_OF_TYPE			'CRDT'
#define B_CM_GET_NTH_RESOURCE_DESCRIPTOR_OF_TYPE		'GNRD'


struct cm_ioctl_data {
	uint32		magic;	// The ioctl itself!
	bus_type	bus;
	uint64		cookie;
	void*		config;
	uint32		n;
	uint32		type;
	void*		data;
	uint32		data_len;
};

#endif	/* _CONFIG_MANAGER_DRIVER_H */
