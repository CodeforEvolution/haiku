/*
 * Copyright 2002 Andrew Bachmann
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "MediaReaderAddOn.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <Errors.h>
#include <MediaRoster.h>
#include <Mime.h>
#include <Node.h>
#include <StorageDefs.h>

#include "MediaReader.h"


//#define TRACE_MEDIA_READER_ADDON
#ifdef TRACE_MEDIA_READER_ADDON
	#define TRACE(args...)		dprintf(STDOUT_FILENO, "reader.media_addon: " args)
#else
	#define TRACE(args...)
#endif

#define TRACE_ALWAYS(args...)	dprintf(STDOUT_FILENO, "reader.media_addon: " args)
#define TRACE_ERROR(args...)	dprintf(STDERR_FILENO, "\33[33mreader.media_addon:\33[0m " args)
#define CALLED()				TRACE("CALLED %s\n", __PRETTY_FUNCTION__)


// A MediaReaderAddOn is an add-on
// that can make MediaReader nodes
//
// MediaReader nodes read a file into a multistream


// instantiation function
extern "C" _EXPORT BMediaAddOn*
make_media_addon(image_id image) {
	return new MediaReaderAddOn(image);
}


MediaReaderAddOn::MediaReaderAddOn(image_id image)
	:
	AbstractFileInterfaceAddOn(image)
{
	CALLED();
}


MediaReaderAddOn::~MediaReaderAddOn()
{
	CALLED();
}



// -------------------------------------------------------- //
// BMediaAddOn implementation
// -------------------------------------------------------- //
status_t
MediaReaderAddOn::GetFlavorAt(int32 index, const flavor_info** outInfo)
{
	CALLED();

	if (index != 0)
		return B_BAD_INDEX;

	flavor_info* infos = new flavor_info[1];
	if (infos == NULL)
		return B_NO_MEMORY;

	MediaReader::GetFlavor(&infos[0], index);
	(*outInfo) = infos;

	return B_OK;
}


BMediaNode*
MediaReaderAddOn::InstantiateNodeFor(const flavor_info* info, BMessage* config, status_t* outError)
{
	CALLED();

	// TODO: Read from add-on's attributes
	const size_t kDefaultChunkSize = 8192;
		// 8192 bytes = 8 kilobytes = 2048 kilobits/millisec = 256000 kilobytes/sec
	const float kDefaultBitRate = 2048;

	MediaReader* node = new MediaReader(kDefaultChunkSize, kDefaultBitRate, info, config, this);
	if (node == NULL) {
		*outError = B_NO_MEMORY;
	} else
		*outError = node->InitCheck();

	return node;
}


status_t
MediaReaderAddOn::GetConfigurationFor(BMediaNode* yourNode, BMessage* intoMessage)
{
	CALLED();

	if (yourNode == NULL || intoMessage == NULL)
		return B_BAD_VALUE;

	MediaReader* node = dynamic_cast<MediaReader*>(yourNode);
	if (node == NULL)
		return B_BAD_TYPE;

	return node->GetConfigurationFor(intoMessage);
}


// -------------------------------------------------------- //
// BMediaAddOn implementation for B_FILE_INTERFACE nodes
// -------------------------------------------------------- //

status_t
MediaReaderAddOn::GetFileFormatList(int32 flavorId, media_file_format* outWritableFormats,
	int32 inWriteItems, int32* outWriteItems, media_file_format* outReadableFormats,
	int32 inReadItems, int32* outReadItems, void* _reserved)
{
	CALLED();

	if (flavorId != 0) {
		// This is a sanity check for now.
		return B_BAD_INDEX;
	}

	if (outReadableFormats == NULL)
		return B_BAD_VALUE;

	// Don't go off the end!
	if (inReadItems > 0)
		MediaReader::GetFileFormat(&outReadableFormats[0]);

	return B_OK;
}


status_t
MediaReaderAddOn::SniffTypeKind(const BMimeType& type, uint64 inKinds, float* outQuality,
	int32* outInternalId, void* _reserved)
{
	CALLED();
	return AbstractFileInterfaceAddOn::SniffTypeIOKind(type, inKinds, B_BUFFER_PRODUCER, outQuality,
		outInternalId, _reserved);
}


// -------------------------------------------------------- //
// Mmmh! Stuffing...
// -------------------------------------------------------- //

status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_0(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_1(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_2(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_3(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_4(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_5(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_6(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_7(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_8(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_9(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_10(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_11(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_12(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_13(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_14(void*) { return B_ERROR; };
status_t	MediaReaderAddOn::_Reserved_MediaReaderAddOn_15(void*) { return B_ERROR; };
