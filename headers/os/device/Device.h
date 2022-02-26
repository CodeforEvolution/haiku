/*
 * Copyright 2022 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _DEVICE_H
#define _DEVICE_H


class BDevice : public BFile {
public:
	virtual						~BDevice();

protected:
								BDevice(const entry_ref* deviceEntry);
								BDevice(const char* devicePath);

private:

};


#endif /* _DEVICE_H */
