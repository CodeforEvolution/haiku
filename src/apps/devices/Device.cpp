/*
 * Copyright 2008-2009 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pieter Panman
 */


#include "Device.h"

#include <iostream>

#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Device"

// This list comes from the pciid list, except for the last ones
const char* kCategoryString[] = {
	B_TRANSLATE("Unclassified device"), 				// 0x00
	B_TRANSLATE("Mass storage controller"),				// 0x01
	B_TRANSLATE("Network controller"),					// 0x02
	B_TRANSLATE("Display controller"), 					// 0x03
	B_TRANSLATE("Multimedia controller"), 				// 0x04
	B_TRANSLATE("Memory controller"),  					// 0x05
	B_TRANSLATE("Bridge"), 								// 0x06
	B_TRANSLATE("Communication controller"),  			// 0x07
	B_TRANSLATE("Generic system peripheral"),  			// 0x08
	B_TRANSLATE("Input device controller"),  			// 0x09
	B_TRANSLATE("Docking station"),  					// 0x0a
	B_TRANSLATE("Processor"),  							// 0x0b
	B_TRANSLATE("Serial bus controller"),  				// 0x0c
	B_TRANSLATE("Wireless controller"),  				// 0x0d
	B_TRANSLATE("Intelligent controller"),  			// 0x0e
	B_TRANSLATE("Satellite communications controller"), // 0x0f
	B_TRANSLATE("Encryption controller"),  				// 0x10
	B_TRANSLATE("Signal processing controller"),		// 0x11
	B_TRANSLATE("Computer"),							// 0x12 (added later)
	B_TRANSLATE("ACPI controller")						// 0x13 (added later)
};
const int kCategoryStringLength	= B_COUNT_OF(kCategoryString);

// This list is only used to translate device properties
B_TRANSLATE_MARK_VOID("Unknown");
B_TRANSLATE_MARK_VOID("None");
B_TRANSLATE_MARK_VOID("Device");
B_TRANSLATE_MARK_VOID("Computer");

B_TRANSLATE_MARK_VOID("ACPI bus");
B_TRANSLATE_MARK_VOID("PCI bus");
B_TRANSLATE_MARK_VOID("ISA bus");
B_TRANSLATE_MARK_VOID("SCSI bus");
B_TRANSLATE_MARK_VOID("USB bus");

B_TRANSLATE_MARK_VOID("Unknown ACPI device");
B_TRANSLATE_MARK_VOID("Unknown PCI device");
B_TRANSLATE_MARK_VOID("Unknown ISA device");
B_TRANSLATE_MARK_VOID("Unknown SCSI device node");
B_TRANSLATE_MARK_VOID("Unknown USB device");
B_TRANSLATE_MARK_VOID("Unknown device");

// This list is only used to translate pretty device attribute names
B_TRANSLATE_MARK_VOID("Device name")
B_TRANSLATE_MARK_VOID("Manufacturer");
B_TRANSLATE_MARK_VOID("Driver name");
B_TRANSLATE_MARK_VOID("Driver path");
B_TRANSLATE_MARK_VOID("Device paths");


Device::Device(const BString& deviceName, Device* physicalParent, BusType busType,
	Category category)
	:
	BStringItem(deviceName.String()),
	fDeviceName(deviceName)
	fPhysicalParent(physicalParent),
	fBusType(busType),
	fCategory(category)
// 	fManufacturerName(),
// 	fDriverName(),
// 	fDriverPath(),
// 	fDevicePaths()
{
	SetAttribute(Attribute::PRETTY_DEVICE_NAME, deviceName);

// 	_CleanUpAttributes();
}


Device::~Device()
{

}


BString
Device::GetName()
{
	return fAttributeMap[Attribute::PRETTY_DEVICE_NAME];
}


BString
Device::GetManufacturer()
{
	return fAttributeMap[Attribute::PRETTY_MANUFACTURER_NAME];
}


BString
Device::GetDriverName()
{
	return fAttributeMap[Attribute::PRETTY_DEVICE_DRIVER_NAME];
}


BPath
Device::GetDriverPath()
{
	return fDriverPath;
}


BObjectList<BPath>
Device::GetDevicePaths()
{
	return fDevicePaths;
}


void
Device::SetAttributes(const Attributes& attributes)
{
	fAttributeMap.insert(attributes);
}


void
Device::SetAttribute(const BString& name, const BString& value)
{
	if (name == Attribute::PRETTY_DEVICE_NAME)) {
		SetText(value);
		fDeviceName = value;
	} else if (name == Attribute::PRETTY_MANUFACTURER_NAME)
		fManufacturerName = value;
	else if (name == Attribute::PRETTY_DEVICE_DRIVER_NAME)
		fDriverName = value;
	else if (name == Attribute::PRETTY_DEVICE_DRIVER_PATH)
		fDriverPath.SetTo(value);
	else if (name == Attribute::PRETTY_DEVICE_PATHS) {
		if (!fDevicesPaths.IsEmpty())
			fDevicePaths.MakeEmpty();

		fDevicePaths.AddItem(BPath(value));
	} else {
		fAttributeMap[name] = value;
	}
}


Attributes
Device::GetBasicAttributes()
{
	Attributes attributes;
	attributes.push_back(Attribute(Attribute::PRETTY_DEVICE_NAME, GetName()));
	attributes.push_back(Attribute(Attribute::PRETTY_DEVICE_MANUFACTURER_NAME, GetManufacturerName()));
	attributes.push_back(Attribute(Attribute::PRETTY_DEVICE_DRIVER_NAME, GetManufacturer()));
	attributes.push_back(Attribute(Attribute::PRETTY_DEVICE_DRIVER_PATH, GetDriverPath().Path());
	attributes.push_back(Attribute(Attribute::PRETTY_DEVICE_PATHS, GetDevicePaths().Get(0).Path());

	return attributes;
}


Attributes
Device::GetBusAttributes()
{
	Attributes attributes;
	attributes.push_back(Attribute(B_TRANS"None", ""));

	return attributes;
}


Attributes
Device::GetAllAttributes()
{
	Attributes attributes(fAttributeMap.begin(), fAttributeMap.end());

//	AttributeMapIterator iter;
// 	for (iter = fAttributeMap.begin(); iter != fAttributeMap.end(); iter++) {
// 		attributes.push_back(Attribute(iter->first, iter->second));
// 	}

	return attributes;
}


BStringList
Device::GetBasicStrings()
{
	BStringList basicStrings;
	basicStrings.Add(ATTRIBUTE::PRETTY_DEVICE_NAME + " : " + GetDeviceName());
	basicStrings.Add(ATTRIBUTE::PRETTY_DEVICE_MANUFACTURER + " : " + GetManufacturer());
	basicStrings.Add(ATTRIBUTE::PRETTY_DEVICE_PATHS + " : " + GetDevicePaths().ItemAt(0));
	basicStrings.Add(ATTRIBUTE::PRETTY_DRIVER_NAME + " : " + GetName());
	basicStrings.Add(ATTRIBUTE::PRETTY_DRIVER_PATH + " : " + GetName());



	BString str(B_TRANSLATE("Device name\t\t\t\t: %Name%\n"
							"Manufacturer\t\t\t: %Manufacturer%\n"
							"Driver name\t\t\t\t: %DriverName%\n"
							"Driver path\t\t\t\t: %DriverPath%\n"
							"Device paths\t: %DevicePaths%"));

	str.ReplaceFirst("%Name%", GetName());
	str.ReplaceFirst("%Manufacturer%", GetManufacturer());
	str.ReplaceFirst("%DriverName%", GetDriverName());
	str.ReplaceFirst("%DriverPath%", GetDriverPath());
	str.ReplaceFirst("%DevicePaths%", GetDevPathsPublished());

	return str;
}


BStringList
Device::GetBusStrings()
{
	BStringList list;
	list.Add(B_TRANSLATE("None"));

	return list;
}


BString
Device::GetBusTabName()
{
	return B_TRANSLATE("Bus Information");
}


BStringList
Device::GetAllStrings()
{
	BStringList list;

	AttributeMapIterator iter;
	for (iter = fAttributeMap.begin(); iter != fAttributeMap.end(); iter++)
		list.Add(BString(iter->first + " : " + iter->second));

	return list;
}


void
Device::InitFromAttributes()
{
	_SyncWithAttributes();
}


// BString
// Device::_ConvertToHex(uint16 number)
// {
// 	std::stringstream ss;
// 	ss.flags(std::ios::hex | std::ios::showbase);
// 	ss << number;
// 	return BString(ss.str().c_str());
// }
//
//
// void
// Device::_CleanUpAttributes()
// {
// 	// Looks better in Hex, so rewrite
// 	fAttributeMap[B_DEVICE_TYPE] = _ConvertToHex(atoi(fAttributeMap[B_DEVICE_TYPE]));
// 	fAttributeMap[B_DEVICE_SUB_TYPE] = _ConvertToHex(atoi(fAttributeMap[B_DEVICE_SUB_TYPE]));
// 	fAttributeMap[B_DEVICE_INTERFACE] = _ConvertToHex(atoi(fAttributeMap[B_DEVICE_INTERFACE]));
// 	fAttributeMap[B_DEVICE_VENDOR_ID] = _ConvertToHex(atoi(fAttributeMap[B_DEVICE_VENDOR_ID]));
// 	fAttributeMap[B_DEVICE_ID] = _ConvertToHex(atoi(fAttributeMap[B_DEVICE_ID]));
// }


void
Device::_SyncWithAttributes()
{
	fDeviceName = fAttributeMap[B_DEVICE_NAME];
	fManufacturerName = fAttributeMap[B_DEVICE_MANUFACTURER];
	fDriverName = fAttributeMap[B_DEVICE_MODULE_NAME];

	BString driverPath = fAttributeMap[B_DEVICE_MODULE_PATH];
	if (!driverPath.IsEmpty())
		fDriverPath.SetTo(driverPath.String());

	BString devicePath = fAttributeMap[B_DEVICE_PATH];
	if (!devicePath.IsEmpty()) {
		fDevicePaths.MakeEmpty();
		fDevicePaths.AddItem(BPath(devicePath.String()));
	}

}
