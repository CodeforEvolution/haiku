/*
 * Copyright 2023, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _BLUETOOTH_ADDRESS_H
#define _BLUETOOTH_ADDRESS_H


#include <Flattenable.h>
#include <SupportDefs.h>



typedef struct {
	union {
		struct {
			uint8[3] lowerAddrPart;
			uint8 upperAddrPart;
			uint16 nonSignificantAddrPart;
		};

		uint8[6] address;
	};
} _PACKED bd_addr;





class BBluetoothDeviceAddress : public BFlattenable {
public:
								BBluetoothDeviceAddress();
								BBluetoothDeviceAddress(uint8[3] lowerAddrPart, uint8 upperAddrPart, uint16 nonSignificantAddrPart);
								BBluetoothDeviceAddress(uint8[6] address);
								BBluetoothDeviceAddress(const BBluetoothDeviceAddress& other);
	virtual						~BBluetoothDeviceAddress();

				status_t		InitCheck() const;

				void			Unset();

				status_t		SetTo(uint8[3] lowerAddrPart, uint8 upperAddrPart, uint16 nonSignificantAddrPart);
				status_t		SetTo(uint8[6] address);
				status_t		SetTo(const BBluetoothDeviceAddress& other);

				status_t		SetTo();

				BString			ToString();




private:
				status_t		fStatus;
				bd_addr			fBluetoothDeviceAddress;
};


#endif // _BLUETOOTH_ADDRESS_H
