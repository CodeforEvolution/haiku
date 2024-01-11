/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include <bluetooth/ServiceRecord.h>


ServiceRecord::ServiceRecord()
	:
	fID(0),
	f
{

}


ServiceRecord::~ServiceRecord()
{
	delete fAttributeList;
}
	

ServiceAttribute*
ServiceRecord::GetAttributeAt(uint32 index)
{

}


ServiceAttribute*
ServiceRecord::GetAttributeWithID(attribute_id id)
{

}