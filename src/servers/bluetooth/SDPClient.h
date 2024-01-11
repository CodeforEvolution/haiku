/*
 * Copyright 2024, My Name <my@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _H
#define _H


#include <SupportDefs.h>

class ServiceRecordRoster {
public:
			ServiceRecordRoster();
	virtual ~ServiceRecordRoster();

private:
	BObjectList<ServiceRecord> fRecordList;
};


class SDPClient {
public:
	// Owned by callee
	static BObjectList<ServiceRecord>* GetServicesForAddress(bdaddr_t address);
	
private:

};

#endif // _H
