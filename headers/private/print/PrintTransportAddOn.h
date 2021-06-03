/*
 * Copyright 2003-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINT_TRANSPORT_ADD_ON_H
#define _PRINT_TRANSPORT_ADD_ON_H


#include <DataIO.h>
#include <Directory.h>
#include <image.h>
#include <Message.h>


#define B_TRANSPORT_LIST_PORTS_SYMBOL	"list_transport_ports"
#define B_TRANSPORT_FEATURES_SYMBOL		"transport_features"

// Bit values for 'transport_features'
enum {
	B_TRANSPORT_IS_HOTPLUG	= 1,
	B_TRANSPORT_IS_NETWORK	= 2,
};

// To be implemented by the transport add-on
extern "C" BDataIO* instantiate_transport(BDirectory* printer,
						BMessage* message);
extern "C" status_t list_transport_ports(BMessage* message);

#endif
