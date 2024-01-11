/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include <bluetooth/ServiceAttribute.h>



ServiceAttribute::ServiceAttribute(attribute_id id, const DataElement* value)
	:
	fID(id),
	fValue(NULL),
	fInitStatus(B_NO_INIT)
{
	if (value == NULL || value->Value() == NULL) {
		fInitStatus = B_BAD_VALUE;
		return;
	}
	
	fValue = value->Value();
	fInitStatus = B_OK;
}


ServiceAttribute::ServiceAttribute(attribute_id id, const BVariant* elementValue)
	:
	fID(id),
	fValue(NULL),
	fInitStatus(B_NO_INIT)
{
	if (elementValue == NULL) {
		fInitStatus = B_BAD_VALUE;
		return;
	}
	
	fValue = elementValue;
	fInitStatus = B_OK;
}


ServiceAttribute::~ServiceAttribute()
{
	// delete fValue;
}
