/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SERVICE_ATTRIBUTE_H
#define SERVICE_ATTRIBUTE_H


#include <SupportDefs.h>


// Bluetooth Core Specification Version 5.4 | Vol 3, Part B, Section 2.3: Service attributes

typedef uint16 attribute_id;
	// Section 2.3.1: Attribute ID

class ServiceAttribute {
public:
								ServiceAttribute(attribute_id id, DataElement* value);
								ServiceAttribute(attribute_id id, BVariant* elementValue);
	virtual						~ServiceAttribute();

			status_t			InitCheck() const { return fInitStatus; }

			attribute_id		ID() const { return fID; }
			const BVariant*		Value() const { return fValue; }

private:
			attribute_id		fID;
			BVariant*			fValue;
			
			status_t			fInitStatus;
};


// Section 5.1: Universal Service Attributes	
enum UniversalAttributeID {
	SERVICE_RECORD_HANDLE				= 0x0000, // Value: uint32
	SERVICE_CLASS_ID_LIST				= 0x0001, // Value: Data Element Sequence
	SERVICE_RECORD_STATE				= 0x0002, // Value: uint32
	SERVICE_ID							= 0x0003, // Value: UUID
	PROTOCOL_DESCRIPTOR_LIST			= 0x0004, // Value: Data Element Sequence/Alternative
	ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST	= 0x000D, // Value: Data Element Sequence
	BROWSE_GROUP_LIST					= 0x0005, // Value: Data Element Sequence
	LANGUAGE_BASE_ATTRIBUTE_ID_LIST		= 0x0006, // Value: Data Element Sequence
	SERVICE_INFO_TIME_TO_LIVE			= 0x0007, // Value: uint32
	SERVICE_AVAILABILITY				= 0x0008, // Value: uint8
	BLUETOOTH_PROFILE_DESCRIPTOR_LIST	= 0x0009, // Value: Data Element Sequence
	DOCUMENTATION_URL					= 0x000A, // Value: URL
	CLIENT_EXECUTABLE_URL				= 0x000B, // Value: URL
	ICON_URL							= 0x000C, // Value: URL
	// Section 5.1.18: 0x000E to 0x00FF are reserved for future use
	
	// Offsets into LANGUAGE_BASE_ATTRIBUTE_ID_LIST
	SERVICE_NAME						= 0x0000, // Value: String
	SERVICE_DESCRIPTION					= 0x0001, // Value: String
	PROVIDER_NAME						= 0x0002, // Value: String
}


// Section 5.2: Service Class Attributes: Service Discovery Server
enum ServiceDiscoveryServerAttributeID {
	SERVICE_RECORD_HANDLE				= 0x0000, // Value: int32 = 0x00000000
	SERVICE_CLASS_ID_LIST				= 0x0001, // Value: UUID
	VERSION_NUMBER_LIST					= 0x0200, // Value: Data Element Sequence
	SERVICE_DATABASE_STATE				= 0x0201, // Value: uint32
	// Section 5.2.5: 0x0202 to 0x02FF are reserved for future use
}


// Section 5.3: Service Class Attributes: Browse Group Descriptor
enum BrowseGroupDescriptorAttributeID {
	SERVICE_CLASS_ID_LIST				= 0x0001, // Value: UUID
	GROUP_ID							= 0x0200, // Value: UUID
	// Section 5.3.3: 0x0201 to 0x02FF are reserved for future use
}


#endif // SERVICE_ATTRIBUTE_H
