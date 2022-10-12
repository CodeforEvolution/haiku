// Andrew Bachmann, 2002
//
// MediaDemultiplexerAddOn is an add-on
// that can make instances of MediaDemultiplexerNode
//
// MediaDemultiplexerNode handles a file and a multistream


#include "MediaDemultiplexerAddOn.h"

#include <Errors.h>
#include <MediaAddOn.h>
#include <MediaDefs.h>

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "MediaDemultiplexerNode.h"


//#define TRACE_MEDIA_DEMULTIPLEXER_ADDON
#ifdef TRACE_MEDIA_DEMULTIPLEXER_ADDON
	#define TRACE(args...)		dprintf(STDOUT_FILENO, "demultiplexer.media_addon: " args)
#else
	#define TRACE(args...)
#endif

#define TRACE_ALWAYS(args...)	dprintf(STDOUT_FILENO, "demultiplexer.media_addon: " args)
#define TRACE_ERROR(args...)	dprintf(STDERR_FILENO, "\33[33mdemultiplexer.media_addon:\33[0m " args)
#define CALLED()				TRACE("CALLED %s\n", __PRETTY_FUNCTION__)


// instantiation function
extern "C" _EXPORT BMediaAddOn* make_media_addon(image_id image)
{
	return new MediaDemultiplexerAddOn(image);
}

// -------------------------------------------------------- //
// ctor/dtor
// -------------------------------------------------------- //

MediaDemultiplexerAddOn::~MediaDemultiplexerAddOn()
{
}


MediaDemultiplexerAddOn::MediaDemultiplexerAddOn(image_id image)
	:
	BMediaAddOn(image),
	fRefCount(0)
{
	CALLED();
}

// -------------------------------------------------------- //
// BMediaAddOn impl
// -------------------------------------------------------- //

status_t
MediaDemultiplexerAddOn::InitCheck(const char** out_failure_text)
{
	CALLED();
	return B_OK;
}


int32
MediaDemultiplexerAddOn::CountFlavors()
{
	CALLED();
	return 1;
}


status_t
MediaDemultiplexerAddOn::GetFlavorAt(int32 index, const flavor_info** out_info)
{
	CALLED();

	if (index != 0)
		return B_BAD_INDEX;

	flavor_info* infos = new flavor_info[1];
	if (infos == NULL)
		return B_NO_MEMORY;

	MediaDemultiplexerNode::GetFlavor(&infos[0], index);
	*out_info = infos;

	return B_OK;
}

BMediaNode*
MediaDemultiplexerAddOn::InstantiateNodeFor(const flavor_info* info, BMessage* config,
	status_t* out_error)
{
	CALLED();

	MediaDemultiplexerNode* node = new MediaDemultiplexerNode(info, config, this);
	if (node == NULL) {
		*out_error = B_NO_MEMORY;
		ERROR("Couldn't construct a MediaDemultiplexerNode!");
	} else
		*out_error = node->InitCheck();

	return node;
}

status_t
MediaDemultiplexerAddOn::GetConfigurationFor(BMediaNode* your_node, BMessage* into_message)
{
	CALLED();

	MediaDemultiplexerNode* node = dynamic_cast<MediaDemultiplexerNode*>(your_node);
	if (node == NULL)
		return B_BAD_TYPE;

	return node->GetConfigurationFor(into_message);
}

bool
MediaDemultiplexerAddOn::WantsAutoStart()
{
	CALLED();
	return false;
}

status_t
MediaDemultiplexerAddOn::AutoStart(int in_count, BMediaNode** out_node, int32* out_internal_id,
	bool* out_has_more)
{
	CALLED();
	return B_OK;
}

// -------------------------------------------------------- //
// main
// -------------------------------------------------------- //

int main(int argc, char* argv[])
{
	CALLED();
}

// -------------------------------------------------------- //
// stuffing
// -------------------------------------------------------- //

status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_0(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_1(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_2(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_3(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_4(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_5(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_6(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_7(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_8(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_9(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_10(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_11(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_12(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_13(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_14(void*) {};
status_t MediaDemultiplexerAddOn::_Reserved_MediaDemultiplexerAddOn_15(void*) {};
