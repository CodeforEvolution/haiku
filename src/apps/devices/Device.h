/*
 * Copyright 2008-2009 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pieter Panman
 */
#ifndef DEVICE_H
#define DEVICE_H


#include <map>
#include <vector>

#include <String.h>
#include <StringItem.h>

extern "C" {
#include "dm_wrapper.h"
}


typedef enum {
	BUS_ISA = 1,
	BUS_PCI,
	BUS_SCSI,
	BUS_ACPI,
	BUS_USB,
	BUS_NONE
} BusType;


struct Attribute {
			Attribute(BString name, BString value)
				{ fName = name; fValue = value; }
	BString		fName;
	BString		fValue;

	const char* PRETTY_DEVICE_NAME = B_TRANSLATE("Device name");
	const char* PRETTY_DEVICE_MANUFACTURER = B_TRANSLATE("Manufacturer");
	const char* PRETTY_DEVICE_DRIVER_NAME = B_TRANSLATE("Driver name");
	const char* PRETTY_DEVICE_DRIVER_PATH = B_TRANSLATE("Driver path");
	const char* PRETTY_DEVICE_PATHS = B_TRANSLATE("Device paths");
};


typedef std::map<BString, BString>::const_iterator AttributeMapIterator;
typedef std::map<BString, BString> AttributeMap;
typedef std::pair<BString, BString> AttributePair;
typedef std::vector<Attribute> Attributes;


typedef enum {
	CAT_NONE,		// 0x00
	CAT_MASS,		// 0x01
	CAT_NETWORK,	// 0x02
	CAT_DISPLAY,	// 0x03
	CAT_MULTIMEDIA,	// 0x04
	CAT_MEMORY,		// 0x05
	CAT_BUS,		// 0x06
	CAT_COMM,		// 0x07
	CAT_GENERIC,	// 0x08
	CAT_INPUT,		// 0x09
	CAT_DOCK,		// 0x0A
	CAT_CPU,		// 0x0B
	CAT_SERIAL,		// 0x0C
	CAT_WIRELESS,	// 0x0D
	CAT_INTEL,		// 0x0E
	CAT_SATELLITE,	// 0x0F
	CAT_CRYPTO,		// 0x10
	CAT_SIGNAL,		// 0x11
	CAT_COMPUTER,	// 0x12
	CAT_ACPI		// 0x13
} Category;


extern const char* kCategoryString[];
extern const int kCategoryStringLength;


class Device : public BStringItem {
public:
							Device(const BString& deviceName,
								Device* physicalParent
								BusType busType = BUS_NONE
								Category category = CAT_NONE);
	virtual					~Device();

	virtual void			SetAttributes(const Attributes& attributes);
	virtual void 			SetAttribute(const BString& name, const BString& value);

	virtual BString			GetDeviceName() const;
	virtual BString			GetManufacturer() const;
	virtual BString			GetDriverName() const;

	virtual BString			GetDriverPath() const;
	virtual BStringList		GetDevicePaths() const;

			Category		GetCategory() const
								{ return fCategory; }
			Device*			GetPhysicalParent() const
								{ return fPhysicalParent; }
			BusType			GetBusType() const
								{ return fBusType; }

	virtual Attributes		GetBasicAttributes() const;
	virtual Attributes		GetBusAttributes() const;
	virtual Attributes		GetAllAttributes() const;

	virtual BString			GetBasicStrings() const;
	virtual BString			GetBusStrings() const;
	virtual BStringList		GetAllStrings() const;
	
	virtual BString			GetBusTabName() const;

	virtual Attribute		GetAttribute(const BString& name) const
								{ return Attribute(name, fAttributeMap[name]); }

protected:
	virtual void			InitFromAttributes();

protected:
			AttributeMap	fAttributeMap;

			Device*			fPhysicalParent;
			BusType			fBusType;
			Category		fCategory;

// 			BString			fDeviceName;
// 			BString			fManufacturerName;
// 			BString			fDriverName;

// 			BPath			fDriverModulePath;
// 			BObjectList<BPath>	fDevicePaths;

private:
			BString			_ConvertToHex(uint16 number);
			void			_CleanUpAttributes();
			void			_SyncWithAttributes();
};

#endif /* DEVICE_H */

