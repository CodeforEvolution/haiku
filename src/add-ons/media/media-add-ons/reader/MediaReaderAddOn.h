/*
 * Copyright 2002 Andrew Bachmann
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _MEDIA_READER_ADD_ON_H
#define _MEDIA_READER_ADD_ON_H


#include <MediaAddOn.h>
#include <MediaDefs.h>

#include "AbstractFileInterfaceAddOn.h"


// A MediaReaderAddOn is an add-on that can make MediaReader nodes.
// MediaReader nodes read a file into a multistream


class MediaReaderAddOn : public AbstractFileInterfaceAddOn {
public:
	explicit					MediaReaderAddOn(image_id image);
	virtual						~MediaReaderAddOn();


/**************************/
/* Begin inherited from BMediaAddOn */

public:
	virtual	status_t			GetFlavorAt(int32 index, const flavor_info** outInfo);
	virtual	BMediaNode*			InstantiateNodeFor(const flavor_info* info, BMessage* config,
									status_t* outError);
	virtual	status_t			GetConfigurationFor(BMediaNode* yourNode, BMessage* intoMessage);

								/* Only implement if you have a B_FILE_INTERFACE node */
	virtual	status_t			GetFileFormatList(int32 flavorId,
														// For this node flavor (if it matters)
													media_file_format* outWritableFormats,
														// Don't write here if NULL
													int32 inWriteItems,
														// Slot count in outWritableFormats
													int32* outWriteItems,
														// Set this to actual # available,
														// even if bigger than in count
													media_file_format* outReadableFormats,
														// Don't write here if NULL
													int32 inReadItems,
														// Slot count in outReadableFormats
													int32* outReadItems,
														// Set this to actual # available,
														// even if bigger than in count
													void* _reserved
														// Ignore until further notice
													);

protected:
								//	Like SniffType(), but for the specific kind(s).
	virtual	status_t			SniffTypeKind(const BMimeType& type, uint64 inKinds,
									float* outQuality, int32* outInternalId, void* _reserved);

/* end from BMediaAddOn */
/************************/

private:
								/* Private/Unimplemented */
								MediaReaderAddOn(const MediaReaderAddOn& clone);
								MediaReaderAddOn& operator= (const MediaReaderAddOn& clone);

	/* Mmmh, stuffing! */
	virtual	status_t			_Reserved_MediaReaderAddOn_0(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_1(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_2(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_3(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_4(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_5(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_6(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_7(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_8(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_9(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_10(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_11(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_12(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_13(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_14(void*);
	virtual	status_t			_Reserved_MediaReaderAddOn_15(void*);

			uint32				_reserved_media_reader_add_on_[16];

};

#if BUILDING_MEDIA_READER__ADD_ON
extern "C" _EXPORT BMediaAddOn* make_media_reader_add_on(image_id you);
#endif

#endif /* _MEDIA_READER_ADD_ON_H */
