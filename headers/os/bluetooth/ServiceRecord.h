/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SERVICE_RECORD_H
#define SERVICE_RECORD_H

#include <map>

#include <SupportDefs.h>

#include <bluetooth/ServiceAttribute.h>


// Bluetooth Core Specification Version 5.4 | Vol 3, Part B, Section 2.2: Service records

typedef uint32 service_id;
	// Service records are uniquely identified with 32 bit ids (pg 1178)


const service_id SDP_SERVER_RECORD_HANDLE = 0x00000000;

typedef std::map<attribute_id, ServiceAttribute*> AttributeMap;

class ServiceRecord {
public:
									ServiceRecord(const AttributeMap* attributes,
										const BluetoothDevice* hostDevice);
	virtual							~ServiceRecord();
	
			const BluetoothDevice&	HostDevice() const;
			const AttributeMap&		Attributes() const;
			const AttributeMap&		SortedAttributes() const;
	
			service_id				ID() const { return fID; }

private:
									ServiceRecord(); // Delete

private:
			service_id				fID;
			BluetoothDevice*		fHostDevice;
			AttributeMap			fAttributeMap;
};

#endif // SERVICE_RECORD_H
