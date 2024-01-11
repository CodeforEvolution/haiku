/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef DATA_ELEMENT_H
#define DATA_ELEMENT_H


#include <SupportDefs.h>

#include <private/shared/Variant.h>


// Bluetooth Core Specification Version 5.4 | Vol 3, Part B, Section 3.1: Data Element

// Data Element is compromised of two fields: Header Field & Data Field
// - Header field is compromised of two parts: Type Descriptor & Size Descriptor
// - Data field is: Byte sequence with size specified by Size Descriptor

// Multiple-byte fields should be transferred in Network Byte Order (Section 1.5.1)

// Type Descriptor (5 bits): Section 3.2
enum DataElementType {
	NIL = 0,
	UNSIGNED_INT = 1,
	SIGNED_INT = 2,
	UUID = 3,
	STRING = 4,
	BOOLEAN = 5,
	DATA_ELEMENT_SEQUENCE = 6,
	DATA_ELEMENT_ALTERNATIVE = 7,
	URL = 8
}

// Size Descriptor (3 bits): Section 3.3
enum DataElementSize {
	SIZE_1 = 0,
		// This corresponds to 0 bytes with a data element type of NIL
	SIZE_2 = 1,
	SIZE_4 = 2,
	SIZE_8 = 3,
	SIZE_16 = 4,
	VAR_SIZE_8 = 5,
	VAR_SIZE_16 = 6,
	VAR_SIZE_32 = 7
}


struct DataElementHeader {
	DataElementType type : 5;
	DataElementSize size : 3;
}
// The DataElementHeader is followed by (sometimes) a size, and then the actual data!


class DataElement {
public:
							DataElement(DataElementHeader header, const void* value,
								size_t valueSize);
							DataElement(const void* rawDataElement, size_t rawDataElementSize);
							DataElement(BMemoryIO& rawDataElement);

	virtual					~DataElement();

			status_t		InitCheck() const;

			DataElementType	ElementType() const;
			DataElementSize	ElementSize() const;

			const BVariant*	Value() const;

private:
			size_t			_SizeForSizeDescriptor(DataElementSize elementSize,
								BMemoryIO& memoryIO);
			type_code		_ElementTypeToTypeCode(DataElementType elementType, size_t valueSize);

			status_t		_InitCommon(BMemoryIO& memoryIO);

private:
			DataElementType	fElementType;
			DataElementSize	fElementSize;

			BVariant*		fValue;

			status_t		fInitStatus;
};


#endif // DATA_ELEMENT_H
