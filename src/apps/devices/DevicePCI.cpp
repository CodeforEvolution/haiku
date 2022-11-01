/*
 * Copyright 2008-2009 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pieter Panman
 */


#include "DevicePCI.h"

#include <sstream>
#include <stdlib.h>

#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "DevicePCI"

extern "C" {
#include "dm_wrapper.h"
#include "pcihdr.h"
#include "pci-utils.h"
}


const char* ATTRIBUTE_CLASS_INFO = B_TRANSLATE("Class info");



DevicePCI::DevicePCI(Device* parent)
	:
	Device(parent),
	fClassBaseId(0),
	fClassSubId(0),
	fClassApiId(0),
	fVendorId(0),
	fDeviceId(0),
	fSubsystemVendorId(0),
	fSubSystemId(0)
{
}


DevicePCI::~DevicePCI()
{
}


BString ToHex(uint16 num)
{
	std::stringstream ss;
	ss.flags(std::ios::hex | std::ios::showbase);
	ss << num;
	return BString(ss.str().c_str());
}


void
DevicePCI::InitFromAttributes()
{
	// Process the attributes
	fClassBaseId = atoi(fAttributeMap[B_DEVICE_TYPE].String());
	fClassSubId = atoi(fAttributeMap[B_DEVICE_SUB_TYPE].String());
	fClassApiId = atoi(fAttributeMap[B_DEVICE_INTERFACE].String());
	fVendorId = atoi(fAttributeMap[B_DEVICE_VENDOR_ID].String());
	fDeviceId = atoi(fAttributeMap[B_DEVICE_ID].String());

	// Looks better in Hex, so rewrite
	fAttributeMap[B_DEVICE_TYPE] = ToHex(fClassBaseId);
	fAttributeMap[B_DEVICE_SUB_TYPE] = ToHex(fClassSubId);
	fAttributeMap[B_DEVICE_INTERFACE] = ToHex(fClassApiId);
	fAttributeMap[B_DEVICE_VENDOR_ID] = ToHex(fVendorId);
	fAttributeMap[B_DEVICE_ID] = ToHex(fDeviceId);

	// Fetch the class info
	char classInfo[128];
	get_class_info(fClassBaseId, fClassSubId, fClassApiId, classInfo,
		sizeof(classInfo));
	
	// Fetch the manufacturer's name
	BString manufacturerName;
	const char *venShort;
	const char *venFull;
	get_vendor_info(fVendorId, &venShort, &venFull);
	if (!venShort && !venFull) {
		manufacturerName << B_TRANSLATE("Unknown");
	} else if (venShort && venFull) {
		manufacturerName << venFull << "(" << venShort << ")";
	} else {
		manufacturerName << (venShort ? venShort : venFull);
	}
	
	// Fetch the device's name
	BString deviceName;
	const char *devShort;
	const char *devFull;
	get_device_info(fVendorId, fDeviceId, fSubsystemVendorId, fSubSystemId,
		&devShort, &devFull);
	if (!devShort && !devFull) {
		deviceName << B_TRANSLATE("Unknown");
	} else if (devShort && devFull) {
		deviceName << devFull << "(" << devShort << ")";
	} else {
		deviceName << (devShort ? devShort : devFull);
	}
	
	SetAttribute(B_TRANSLATE("Device name"), deviceName);
	SetAttribute(B_TRANSLATE("Manufacturer"), manufacturerName);


	SetAttribute(ATTRIBUTE_CLASS_INFO, classInfo);
	fCategory = (Category)fClassBaseId;
	BString outlineName;
	outlineName << manufacturerName << " " << deviceName;
	SetText(outlineName.String());
}


Attributes
DevicePCI::GetBusAttributes()
{
	Attributes attributes;
	attributes.push_back(GetAttribute(B_DEVICE_TYPE));
	attributes.push_back(GetAttribute(B_DEVICE_SUB_TYPE));
	attributes.push_back(GetAttribute(B_DEVICE_INTERFACE));
	attributes.push_back(GetAttribute(B_DEVICE_VENDOR_ID));
	attributes.push_back(GetAttribute(B_DEVICE_ID));
	return attributes;
}


BString
DevicePCI::GetBusStrings()
{
	BString busStrings(B_TRANSLATE("Class Info:\t\t\t\t: %classInfo%"));
	busStrings.ReplaceFirst("%classInfo%", fAttributeMap[ATTRIBUTE_CLASS_INFO]);
	return busStrings;
}


BString
DevicePCI::GetBusTabName()
{
	return B_TRANSLATE("PCI Information");
}

