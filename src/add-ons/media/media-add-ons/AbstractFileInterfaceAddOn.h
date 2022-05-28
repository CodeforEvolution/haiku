/*
 * Copyright 2002 Andrew Bachmann
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _ABSTRACT_FILE_INTERFACE_ADD_ON_H
#define _ABSTRACT_FILE_INTERFACE_ADD_ON_H


#include <MediaAddOn.h>
#include <MediaDefs.h>


// AbstractFileInterfaceAddOn is an add-on
// that can make instances of AbstractFileInterfaceNode
//
// AbstractFileInterfaceNode handles a file and a multistream


class AbstractFileInterfaceAddOn : public BMediaAddOn {
public:
	explicit					AbstractFileInterfaceAddOn(image_id image);
	virtual						~AbstractFileInterfaceAddOn();

public:
	virtual	status_t			InitCheck(const char** _failureText);
	virtual	int32				CountFlavors();
	virtual	status_t			GetFlavorAt(int32 index, const flavor_info** _info);
	virtual	BMediaNode*			InstantiateNodeFor(const flavor_info* info, BMessage* config,
									status_t* _error) = 0;
	virtual	status_t			GetConfigurationFor(BMediaNode* yourNode, BMessage* intoMessage);
	virtual	bool				WantsAutoStart();
	virtual	status_t			AutoStart(int index, BMediaNode** _node, int32* _internalId,
									bool* _hasMore);

	// NOTE: Only implement if you have a B_FILE_INTERFACE node
	virtual	status_t			SniffRef(const entry_ref& file, BMimeType* ioMimeType,
									float* _quality, int32* _internalID);

	// NOTE: This is broken if you deal with producers and consumers both.
	// Implement SniffTypeKind instead. If you implement SniffTypeKind, this
	// doesn't get called.
	virtual	status_t			SniffType(const BMimeType& type, float* _quality,
									int32* _internalID);

	virtual	status_t			GetFileFormatList(int32 forNodeFlavorID,
									media_file_format* _writableFormats, int32 writableFormatsCount,
									int32* _writableFormatsTotalCount,
									media_file_format* _readableFormats, int32 readableFormatsCount,
									int32* _readableFormatsTotalCount, void* _reserved);

	virtual	status_t SniffTypeKind(				//	Like SniffType, but for the specific kind(s)
					const BMimeType & type,
					uint64 in_kinds,
					float * out_quality,
					int32 * out_internal_id,
					void * _reserved) = 0;
	virtual	status_t SniffTypeKind(				//	Like SniffType, but for the specific kind(s)
					const BMimeType & type,
					uint64 in_kinds,
					uint64 io_kind,
					float * out_quality,
					int32 * out_internal_id,
					void * _reserved);

	// NOTE: Like SniffType, but for the specific kind(s)
	virtual	status_t			SniffTypeKind(const BMimeType& type, uint64 kinds, float* _quality,
									int32* _internalID, void* _reserved);

private:
	/* Private/Unimplemented */
								AbstractFileInterfaceAddOn(const AbstractFileInterfaceAddOn& clone);
			AbstractFileInterfaceAddOn&	operator=(const AbstractFileInterfaceAddOn& clone);

			int32	fRefCount;

	/* Mmmh, stuffing! */
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_0(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_1(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_2(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_3(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_4(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_5(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_6(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_7(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_8(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_9(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_10(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_11(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_12(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_13(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_14(void*);
	virtual	status_t			_Reserved_AbstractFileInterfaceAddOn_15(void*);

			uint32				_reserved_abstract_file_interface_add_on_[16];

};


#if BUILDING_ABSTRACT_FILE_INTERFACE__ADD_ON
extern "C" _EXPORT BMediaAddOn* make_abstract_file_interface_add_on(image_id you);
#endif


#endif /* _ABSTRACT_FILE_INTERFACE_ADD_ON_H */
