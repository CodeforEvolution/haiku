/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include <bluetooth/DataElement.h>

#include <private/shared/AutoDeleter.h>


DataElement::DataElement()
	:
	fElementType(NIL),
	fElementSize(SIZE_1),
	fValue(NULL),
	fInitStatus(B_NO_INIT)
{
}


DataElement(const void* data, size_t dataSize)
	:
	fElementType(NIL),
	fElementSize(SIZE_1),
	fValue(NULL),
	fInitStatus(B_NO_INIT)
{
	if (data != NULL && dataSize > 0) {
		BMemoryIO memoryIO(data, dataSize);
		fInitStatus = _InitCommon(memoryIO);
		return;
	}

	fInitStatus = B_BAD_VALUE;
}

DataElement(BMemoryIO& memoryIO)
	:
	fElementType(NIL),
	fElementSize(SIZE_1),
	fValue(NULL),
	fInitStatus(B_NO_INIT)
{
	fInitStatus = _InitCommon(memoryIO);
}


DataElement::~DataElement()
{
	delete fValue;
}


status_t
DataElement::InitCheck()
{
	return fInitStatus;
}


DataElementType
DataElement::ElementType()
{
	return fElementType;
}


DataElementSize
DataElement::ElementSize()
{
	return fElementSize;
}


const BVariant*
DataElement::Value()
{
	return fValue;
}


size_t
DataElement::_SizeForSizeDescriptor(DataElementSize elementSize, BMemoryIO& memoryIO)
{
	if (elementSize == SIZE_1)
		return (header.type == NIL) ? 0 : 1;
	else if (elementSize == SIZE_2)
		return 2;
	else if (elementSize == SIZE_4)
		return 4;
	else if (elementSize == SIZE_8)
		return 8;
	else if (elementSize == SIZE_16)
		return 16;

	size_t sizeOfValueSize = 0;
	if (elementSize == VAR_SIZE_8)
		sizeOfValueSize = 8;
	else if (elementSize == VAR_SIZE_16)
		sizeOfValueSize = 16;
	else if (elementSize == VAR_SIZE_32)
		sizeOfValueSize = 32;
	else
		debugger("Unexpected element size descriptor!");

	size_t valueSize = 0;
	if (sizeOfValueSize != 0) {
		ssize_t amountRead = memoryIO.Read(valueSize, sizeOfValueSize);
		if (amountRead != sizeOfValueSize)
			debugger("Failed to read size of value from data element!");
	}

	return valueSize;
}


type_code
DataElement::_ElementTypeToTypeCode(DataElementType elementType, size_t valueSize)
{
	switch (elementType) {
		case NIL:
		{
			if (valueSize != 0) debugger("Invalid byte size for NIL element type!");
			return B_POINTER_TYPE;
		}

		case UNSIGNED_INT:
		case SIGNED_INT:
		{
			if (valueSize == 1) {
				return (elementType == UNSIGNED_INT) ? B_UINT8_TYPE : B_INT8_TYPE;
			else if (valueSize == 2) {
				return (elementType == UNSIGNED_INT) ? B_UINT16_TYPE : B_INT16_TYPE;
			else if (valueSize == 4) {
				return (elementType == UNSIGNED_INT) ? B_UINT32_TYPE : B_INT32_TYPE;
			else if (valueSize == 8) {
				return (elementType == UNSIGNED_INT) ? B_UINT64_TYPE : B_INT64_TYPE;
			else if (valueSize == 16) {
				return B_POINTER_TYPE;
					// This is what other platforms do rather than using a 128-bit integer type
			} else
				debugger("Invalid byte size for integer data element!");
		}

		case STRING:
		case URL:
		{
			// TODO: Should we check if the URL and string are of the proper size?
			return B_STRING_TYPE;
		}

		case BOOLEAN:
		{
			if (valueSize != 1) debugger("Invalid byte size for boolean data element!");
			return B_BOOL_TYPE;
		}

		case DATA_ELEMENT_SEQUENCE:
		case DATA_ELEMENT_ALTERNATIVE:
		{
			// TODO: Should we check if the sequence of data elements are of the proper size?
			return B_POINTER_TYPE;
		}

		default:
			// Should be unreachable!
			debugger("Unknown DataElementType!");
	}
}


status_t
DataElement::_InitCommon(BMemoryIO& memoryIO)
{
	DataElementHeader header;
	size_t amountRead = memoryIO.Read(&header, sizeof(DataElementHeader));
	if (amountRead != sizeof(DataElementHeader))
		return B_BAD_DATA;

	size_t valueSize = _SizeForSizeDescriptor(header.size, memoryIO);
	MemoryDeleter valueData(malloc(valueSize));
	if (!valueData.IsSet())
		return B_NO_MEMORY;

	amountRead = memoryIO.Read(valueData.Get(), valueSize);
	if (amountRead != valueSize)
		return B_BAD_DATA;

	ObjectDeleter valueVariant(new BVariant());
	if (!valueVariant.IsSet())
		return B_NO_MEMORY;

	status_t result = valueVariant->SetToTypedData(valueData.Detach(),
		_ElementTypeToTypeCode(header.type, valueSize));
	if (result != B_OK)
		return result;

	fElementType = header.type;
	fElementSize = header.size;
	fValue = valueVariant.Detach();

	return B_OK;
}


