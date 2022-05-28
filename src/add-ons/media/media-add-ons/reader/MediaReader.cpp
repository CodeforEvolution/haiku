/*
 * Copyright 2002 Andrew Bachmann
 * Copyright 2022 Jacob Secunda
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "MediaReader.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <Buffer.h>
#include <Errors.h>
#include <MediaAddOn.h>
#include <MediaRoster.h>
#include <ParameterWeb.h>
#include <TimeSource.h>


//#define TRACE_MEDIA_READER
#ifdef TRACE_MEDIA_READER
	#define TRACE(args...)		dprintf(STDOUT_FILENO, "reader.media_addon: " args)
#else
	#define TRACE(args...)
#endif

#define TRACE_ALWAYS(args...)	dprintf(STDOUT_FILENO, "reader.media_addon: " args)
#define TRACE_ERROR(args...)	dprintf(STDERR_FILENO, "\33[33mreader.media_addon:\33[0m " args)
#define CALLED()				TRACE("CALLED %s\n", __PRETTY_FUNCTION__)


// A MediaReader is a node that implements FileInterface and BBufferProducer.
// It reads any file and produces one output, which is a multistream.
//
// See also: MediaReaderAddOn.cpp


MediaReader::MediaReader(size_t defaultChunkSize, float defaultBitRate, const flavor_info* info,
	BMessage* config, BMediaAddOn* addOn)
	:
	BMediaNode("MediaReader"),
	BBufferProducer(B_MEDIA_MULTISTREAM),
	AbstractFileInterfaceNode(defaultChunkSize, defaultBitRate, info, config, addOn),
	fOutputEnabled(true),
	fBufferGroup(NULL)
{
	CALLED();

	// don't overwrite available space, and be sure to terminate
	strlcpy(fOutput.name, "MediaReader Output", B_MEDIA_NAME_LENGTH);

	// Initialize the output
	fOutput.node = media_node::null;     // until registration
	fOutput.source = media_source::null; // until registration
	fOutput.destination = media_destination::null;
	GetFormat(&fOutput.format);
}


MediaReader::~MediaReader()
{
	CALLED();

	delete fBufferGroup;
	fBufferGroup = NULL;
}


// -------------------------------------------------------- //
// Implementation of BMediaNode
// -------------------------------------------------------- //
void
MediaReader::Preroll()
{
	CALLED();

	// FIXME: Performance opportunity
	BMediaNode::Preroll();
}


status_t
MediaReader::HandleMessage(int32 message, const void* data, size_t size)
{
	CALLED();

	status_t result = B_ERROR;

	switch (message) {
		// No special messages for now
		default:
			result = BBufferProducer::HandleMessage(message, data, size);
			if (result == B_OK)
				break;

			result = AbstractFileInterfaceNode::HandleMessage(message, data, size);
			break;
	}

	return result;
}


void
MediaReader::NodeRegistered()
{
	CALLED();

	// Now we can do this
	fOutput.node = Node();
	fOutput.source.id = 0;
	fOutput.source.port = fOutput.node.port;
		// Same as ControlPort();

	// Creates the parameter web and starts the looper thread
	AbstractFileInterfaceNode::NodeRegistered();
}


// -------------------------------------------------------- //
// Implementation of BFileInterface
// -------------------------------------------------------- //
status_t
MediaReader::SetRef(const entry_ref& file, bool create, bigtime_t* outTime)
{
	CALLED();

	status_t result = AbstractFileInterfaceNode::SetRefWithOpenMode(file, B_READ_ONLY, create,
		outTime);
	if (result != B_OK) {
		TRACE_ERROR("AbstractFileInterfaceNode::SetRefWithOpenMode() failed: %s\n",
			strerror(result));
		return result;
	}

	if (fOutput.destination == media_destination::null) {
		// Reset the format, and set the requirements imposed by this file
		GetFormat(&fOutput.format);
		AddRequirements(&fOutput.format);
		return B_OK;
	}

	// If we are connected we may have to re-negotiate the connection
	media_format format;
	GetFormat(&format);
	AddRequirements(&format);
	if (format_is_compatible(format, fOutput.format)) {
		TRACE("Compatible format detected! No re-negotiation necessary.\n");
		return B_OK;
	}

	result = ProposeFormatChange(&format, fOutput.destination);
	if (result == B_OK) {
		TRACE("Destination accepts proposed format change.\n");
		return ChangeFormat(fOutput.source, fOutput.destination, &format);
	}

	TRACE("Destination didn't accept the proposed format change?!\n");
	return result;

/*
	// Okay, the hard way requires we get the MediaRoster
	BMediaRoster* roster = BMediaRoster::Roster(&result);
	if (roster == NULL)
		return B_MEDIA_SYSTEM_FAILURE;

	if (result != B_OK)
		return result;

	// Before disconnect, one should always stop the nodes. (As the BeBook says.)
	// Requires run_state cast since the return type on RunState() is wrong. [int32]
	run_state ourRunState = run_state(RunState());
	if (ourRunState == BMediaEventLooper::B_STARTED)
		Stop(0, true); // Stop us right now

	// We also stop the destination if it is running!
	media_node_id destinationNodeId = -1;
	destinationNodeId = roster->NodeIdFor(fOutput.destination.port);
	if (destinationNodeId < B_NO_ERROR)
		return B_MEDIA_SYSTEM_FAILURE;

	media_node* destinationNode = NULL;
	status = roster->GetNodeFor(destinationNodeId, destinationNode);
	if (status != B_OK)
		return status;

	//
	status = roster->StopNode(destinationNode, 0, true);
	if (status != B_OK) {
		return status;

	// We should disconnect right now!
	media_destination outputDestination = fOutput.destination;
	result = roster->Disconnect(fOutput.source, fOutput.destination);
	if (result != B_OK)
		return status;

	// If that went okay, we'll try reconnecting.
	media_output connectOutput;
	media_input connectInput;
	result = roster->Connect(fOutput.source, outputDestination, &format, &connectOutput,
		&connectInput);
	if (result != B_OK)
		return result;

	// Now restart if necessary
	if (ourRunState == BMediaEventLooper::B_STARTED)
		Start(0);


	return status;
*/
}


// -------------------------------------------------------- //
// Implemention of BBufferProducer
// -------------------------------------------------------- //

// They are asking us to make the first offering.
// So, we get a fresh format and then add requirements based on the current file. (If any)
status_t
MediaReader::FormatSuggestionRequested(media_type type, int32 quality, media_format* format)
{
	CALLED();

	if ((type != B_MEDIA_MULTISTREAM) && (type != B_MEDIA_UNKNOWN_TYPE))
		return B_MEDIA_BAD_FORMAT;

	GetFormat(format);
	AddRequirements(format);

	return B_OK;
}


// They made an offer to us.  We should make sure that the offer is
// acceptable, and then we can add any requirements we have on top of
// that.  We leave wildcards for anything that we don't care about.
status_t
MediaReader::FormatProposal(const media_source& output_source, media_format* format)
{
	CALLED();

	// We only have one output so that better be it.
	if (fOutput.source != output_source)
		return B_MEDIA_BAD_SOURCE;

	//PRINT_FORMAT("Proposed format:", format);
	//media_format* myFormat = GetFormat();
	//PRINT_FORMAT("My format:", myFormat);

	media_format myFormat;
	GetFormat(&myFormat);
	if (!format_is_compatible(*format, myFormat))
		return B_MEDIA_BAD_FORMAT;

	AddRequirements(format);
	return B_OK;
}


// Presumably we have already agreed with them that this format is okay.
// But just in case, we check the offer. (and complain if it is invalid)
// Then as the last thing we do, we get rid of any remaining wildcards.
status_t
MediaReader::FormatChangeRequested(const media_source& source, const media_destination& destination,
	media_format* ioFormat, int32* _deprecated_)
{
	CALLED();

	if (fOutput.source != source)
		return B_MEDIA_BAD_SOURCE;

	status_t result = FormatProposal(source, ioFormat);
	if (result != B_OK) {
		TRACE_ERROR("FormatProposal() failed: %s\n", strerror(result));
		GetFormat(ioFormat);
		return result;
	}

	return ResolveWildcards(ioFormat);
}


/* cookie starts as 0. */
status_t
MediaReader::GetNextOutput(int32* cookie, media_output* outOutput)
{
	CALLED();

	if (*cookie != 0)
		return B_ERROR;

	// Make sure they won't get the same output again.
	*cookie = 1;
	*outOutput = fOutput;

	return B_OK;
}


status_t
MediaReader::DisposeOutputCookie(int32 cookie)
{
	CALLED();

	// Nothing to do since our cookies are just integers.
	return B_OK;
}


status_t
MediaReader::SetBufferGroup(const media_source& forSource, BBufferGroup* group)
{
	CALLED();

	if (fOutput.source != forSource) {
		// We only have one output so that better be it.
		return B_MEDIA_BAD_SOURCE;
	}

	if (fBufferGroup != NULL) {
		if (fBufferGroup == group)
			return B_OK; // time saver
		delete fBufferGroup;
	}

	if (group != NULL)
		fBufferGroup = group;
	else {
		// Let's take advantage of this opportunity to recalculate our downstream latency and ensure
		// that it is up to date
		media_node_id id;
		FindLatencyFor(fOutput.destination, &fDownstreamLatency, &id);

		// Buffer period gets initialized in Connect() because that is the first time we get the
		// real values for chunk size and bit rate, which are used to compute buffer period.
		//
		// NOTE:
		// You can still make a buffer group before connecting (why?), but we don't make it.
		// You make it yourself and pass it here. Not sure why anybody would want to do that since
		// they need a connection anyway...
		if (fBufferPeriod <= 0)
			return B_NO_INIT;

		int32 count = int32(fDownstreamLatency / fBufferPeriod) + 2;

		TRACE("Downstream latency = %" B_PRIdBIGTIME ", buffer period = %" B_PRIdBIGTIME \
			", buffer count = %" B_PRId32 "\n", fDownstreamLatency, fBufferPeriod, count);


		// Allocate the buffers
		fBufferGroup = new BBufferGroup(fOutput.format.u.multistream.max_chunk_size, count);
		if (fBufferGroup == NULL)
			return B_NO_MEMORY;

		status_t result = fBufferGroup->InitCheck();
		if (result != B_OK) {
			TRACE_ERROR("BBufferGroup->InitCheck() failed: %s\n", strerror(result));
			return result;
		}
	}

	return B_OK;
}


/* Format of clipping is (as int16-s): <from line> <npairs> <startclip> <endclip>. */
/* Repeat for each line where the clipping is different from the previous line. */
/* If <npairs> is negative, use the data from line -<npairs> (there are 0 pairs after */
/* a negative <npairs>. Yes, we only support 32k*32k frame buffers for clipping. */
/* Any non-0 field of 'display' means that that field changed, and if you don't support */
/* that change, you should return an error and ignore the request. Note that the buffer */
/* offset values do not have wildcards; 0 (or -1, or whatever) are real values and must */
/* be adhered to. */
status_t
MediaReader::VideoClippingChanged(const media_source& forSource, int16 numShorts, int16* clipData,
	const media_video_display_info& display, int32* _deprecated_)
{
	CALLED();

	return BBufferProducer::VideoClippingChanged(forSource, numShorts, clipData, display,
		_deprecated_);
}


status_t
MediaReader::GetLatency(bigtime_t* outLatency)
{
	CALLED();

	*outLatency = EventLatency() + SchedulingLatency();
	return B_OK;
}


status_t
MediaReader::PrepareToConnect(const media_source& what, const media_destination& where,
	media_format* format, media_source* outSource, char* outName)
{
	CALLED();

	if (fOutput.source != what)
		return B_MEDIA_BAD_SOURCE;

	if (fOutput.destination != media_destination::null)
		return B_MEDIA_ALREADY_CONNECTED;

	status_t result = FormatChangeRequested(fOutput.source, where, format, 0);
	if (result != B_OK) {
		TRACE_ERROR("MediaReader::FormatChangeRequested failed: %s\n", strerror(result));
		return result;
	}

	// last check for wildcards and general validity
	if (format->type != B_MEDIA_MULTISTREAM)
		return B_MEDIA_BAD_FORMAT;

	*outSource = fOutput.source;
	fOutput.destination = where;

	strlcpy(outName, fOutput.name, B_MEDIA_NAME_LENGTH);

	return ResolveWildcards(format);
}


void
MediaReader::Connect(status_t error, const media_source& source,
	const media_destination& destination, const media_format& format, char* ioName)
{
	CALLED();

	if (error != B_OK) {
		TRACE_ERROR("An error occured before the connection process could commence?? Error: %s\n",
			strerror(error));
		fOutput.destination = media_destination::null;
		GetFormat(&fOutput.format);
		return;
	}

	if (fOutput.source != source) {
		fOutput.destination = media_destination::null;
		GetFormat(&fOutput.format);
		return;
	}

	// Record the agreed upon values
	fOutput.destination = destination;
	fOutput.format = format;
	strlcpy(ioName, fOutput.name, B_MEDIA_NAME_LENGTH);

	// Determine our downstream latency
	media_node_id id;
	FindLatencyFor(fOutput.destination, &fDownstreamLatency, &id);

	// Compute the buffer period, which must be done before SetBufferGroup().
	fBufferPeriod = bigtime_t(1000u * 8000000u / 1024u
		* fOutput.format.u.multistream.max_chunk_size / fOutput.format.u.multistream.max_bit_rate);


	TRACE("Max_chunk_size = %" B_PRIu32 ", max_bit_rate = %f, fBufferPeriod = %" B_PRIdBIGTIME \
		"\n", fOutput.format.u.multistream.max_chunk_size,
		fOutput.format.u.multistream.max_bit_rate, fBufferPeriod);


	// Setup the buffers if they aren't setup yet.
	if (fBufferGroup == NULL) {
		status_t result = SetBufferGroup(fOutput.source, 0);
		if (result != B_OK) {
			TRACE_ERROR("MediaReader::SetBufferGroup() failed: %s\n", strerror(result));
			fOutput.destination = media_destination::null;
			GetFormat(&fOutput.format);
			return;
		}
	}

	SetBufferDuration(fBufferPeriod);

	uint8* data = new uint8[fOutput.format.u.multistream.max_chunk_size];

	if (data != NULL && GetCurrentFile() != NULL) {
		bigtime_t start, end;
		BBuffer* buffer = NULL;
		ssize_t bytesRead = 0;

		// Timed section of code starts here
		{
			start = TimeSource()->RealTime();
			// First, we try to use a real BBuffer.
			buffer = fBufferGroup->RequestBuffer(fOutput.format.u.multistream.max_chunk_size,
				fBufferPeriod);
			if (buffer != NULL)
				FillFileBuffer(buffer);
			else {
				// Didn't get a real BBuffer, try simulation by just a read from the disk.
				bytesRead = GetCurrentFile()->Read(data,
					fOutput.format.u.multistream.max_chunk_size);
			}
			end = TimeSource()->RealTime();
		}

		delete[] data;
		data = NULL;

		if (buffer != NULL) {
			bytesRead = buffer->SizeUsed();
			buffer->Recycle();
		}

		GetCurrentFile()->Seek(-bytesRead, SEEK_CUR);
			// Put it back where we found it.

		fInternalLatency = end - start;

		TRACE("Internal latency from disk read = %" B_PRIdBIGTIME "\n", fInternalLatency);
	} else {
		fInternalLatency = 100;
			// Well, let's just guess...
		TRACE("Internal latency guessed = %" B_PRIdBIGTIME "\n", fInternalLatency);
	}

	SetEventLatency(fDownstreamLatency + fInternalLatency);

	// TODO: Anything else we need to accomplish?
}


void
MediaReader::Disconnect(const media_source& what, const media_destination& where)
{
	CALLED();

	if (fOutput.destination != where) {
		TRACE_ERROR("B_MEDIA_BAD_DESTINATION\n");
		return;
	}

	if (fOutput.source != what) {
		TRACE_ERROR("B_MEDIA_BAD_SOURCE\n");
		return;
	}

	fOutput.destination = media_destination::null;
	GetFormat(&fOutput.format);

	delete fBufferGroup;
	fBufferGroup = NULL;
}


void
MediaReader::LateNoticeReceived(const media_source& what, bigtime_t howMuch,
	bigtime_t performanceTime)
{
	CALLED();

	if (what == fOutput.source) {
		switch (RunMode()) {
			case B_OFFLINE:
			case B_RECORDING:
			{
			    // Nothing to do...
				break;
			}

			case B_INCREASE_LATENCY:
			{
				fInternalLatency += howMuch;
				SetEventLatency(fDownstreamLatency + fInternalLatency);
				break;
			}

			case B_DECREASE_PRECISION:
			{
				// TODO: Shorten our buffer period!
				// We could opt to just not wait, but we should gradually shorten the period so we
				// don't starve others. Also, we need to make sure we are catching up!
				// We may have some sort of time goal for how long it takes us to catch up, as well.
				break;
			}

			case B_DROP_DATA:
			{
				// Okay you asked for it! We'll skip ahead in the file by dropping 1 buffer.
				if (GetCurrentFile() == NULL)
					TRACE("MediaReader::LateNoticeReceived called without a GetCurrentFile().\n");
				else
					GetCurrentFile()->Seek(fOutput.format.u.multistream.max_chunk_size, SEEK_CUR);

				break;
			}

			default:
			{
				// HUH!? There aren't any more run modes.
				TRACE_ERROR("MediaReader::LateNoticeReceived() with unexpected run mode: %i\n",
					RunMode());
				break;
			}
		}
	}
}


void
MediaReader::EnableOutput(const media_source& what, bool enabled, int32* _deprecated_)
{
	CALLED();

	if (fOutput.source != what) {
		TRACE_ERROR("MediaReader::EnableOutput() failed: %s\n", strerror(B_MEDIA_BAD_SOURCE));
		return;
	}

	fOutputEnabled = enabled;
}


status_t
MediaReader::SetPlayRate(int32 numerator, int32 denominator)
{
	CALLED();

	// TODO: Do something more intelligent here!
	return BBufferProducer::SetPlayRate(numerator, denominator);
}


void
MediaReader::AdditionalBufferRequested(const media_source& source, media_buffer_id prevBuffer,
	bigtime_t prevTime, const media_seek_tag* prevTag)
{
	CALLED();

	if (fOutput.source != source)
		return;

	BBuffer* buffer = NULL;
	status_t result = GetFilledBuffer(&buffer);
	if (result != B_OK) {
		TRACE_ERROR("GetFilledBuffer() in MediaReader::AdditionalBufferRequested() failed with: " \
			"%s\n", strerror(result));
		return; // We can't send the buffer.
	}

	SendBuffer(buffer, fOutput.source, fOutput.destination);
}


void
MediaReader::LatencyChanged(const media_source& source, const media_destination& destination,
	bigtime_t newLatency, uint32 flags)
{
	CALLED();

	if ((fOutput.source == source) && (fOutput.destination == destination)) {
		fDownstreamLatency = newLatency;
		SetEventLatency(fDownstreamLatency + fInternalLatency);
	}

	// FIXME: We may have to recompute the number of buffers that we are using.
	// See SetBufferGroup()
}


// -------------------------------------------------------- //
// Implementation for BMediaEventLooper
// -------------------------------------------------------- //

status_t
MediaReader::HandleBuffer(const media_timed_event* event, bigtime_t lateness, bool realTimeEvent)
{
	CALLED();

	if (fOutput.destination == media_destination::null)
		return B_MEDIA_NOT_CONNECTED;

	status_t result = B_ERROR;

	BBuffer* buffer = fBufferGroup->RequestBuffer(fOutput.format.u.multistream.max_chunk_size,
		fBufferPeriod);
	if (buffer != NULL) {
	    result = FillFileBuffer(buffer);
	    if (result != B_OK) {
			TRACE_ERROR("MediaReader::HandleEvent got an error from FillFileBuffer(): %s\n",
				strerror(result));
			buffer->Recycle();
		} else {
			if (fOutputEnabled) {
				result = SendBuffer(buffer, fOutput.source, fOutput.destination);
				if (result != B_OK) {
					TRACE_ERROR("MediaReader::HandleEvent got an error from SendBuffer(): %s\n",
						strerror(result));
					buffer->Recycle();
				}
			} else
				buffer->Recycle();
		}
	}

	bigtime_t nextEventTime = event->event_time + fBufferPeriod;
	media_timed_event nextBufferEvent(nextEventTime, BTimedEventQueue::B_HANDLE_BUFFER);
	EventQueue()->AddEvent(nextBufferEvent);

	return result;
}


status_t
MediaReader::HandleDataStatus(const media_timed_event* event, bigtime_t lateness,
	bool realTimeEvent)
{
	CALLED();

	return SendDataStatus(event->data, fOutput.destination, event->event_time);
}


// -------------------------------------------------------- //
// MediaReader specific functions
// -------------------------------------------------------- //

status_t
MediaReader::GetFlavor(flavor_info* outInfo, int32 id)
{
	CALLED();

	if (outInfo == NULL)
		return B_BAD_VALUE;

	AbstractFileInterfaceNode::GetFlavor(outInfo, id);

	outInfo->name = strdup("Media Reader");
	outInfo->info = strdup("The Haiku Media Reader reads a file and produces a multistream.");
	outInfo->kinds |= B_BUFFER_PRODUCER;
	outInfo->out_format_count = 1;
		// Only 1 output

	media_format* formats = new media_format[outInfo->out_format_count];
	if (formats == NULL)
		return B_NO_MEMORY;

	GetFormat(&formats[0]);
	outInfo->out_formats = formats;

	return B_OK;
}


void
MediaReader::GetFormat(media_format* outFormat)
{
	CALLED();

	AbstractFileInterfaceNode::GetFormat(outFormat);
}


void
MediaReader::GetFileFormat(media_file_format* outFileFormat)
{
	CALLED();

	AbstractFileInterfaceNode::GetFileFormat(outFileFormat);
	outFileFormat->capabilities |= media_file_format::B_READABLE;
}


status_t
MediaReader::GetFilledBuffer(BBuffer** outBuffer)
{
	CALLED();

	BBuffer* buffer = fBufferGroup->RequestBuffer(fOutput.format.u.multistream.max_chunk_size, -1);
	if (buffer == NULL) {
		// TODO: Add a new buffer and get it!
		TRACE_ERROR("MediaReader::GetFilledBuffer() needs a new buffer!\n");
		return B_ERROR; // Don't send the buffer.
	}

	status_t result = FillFileBuffer(buffer);
	*outBuffer = buffer;

	return result;
}


status_t
MediaReader::FillFileBuffer(BBuffer* buffer)
{
	CALLED();

	if (GetCurrentFile() == NULL) {
		TRACE_ERROR("MediaReader::FillFileBuffer(): There's no file currently opened!\n");
		return B_NO_INIT;
	}

	TRACE("Passed in BBuffer stats: %" B_PRIuSIZE " buffer bytes used, %" B_PRIuSIZE \
			" buffer bytes available.\n", buffer->SizeUsed(), buffer->SizeAvailable());

	off_t position = GetCurrentFile()->Position();
	ssize_t bytesRead = GetCurrentFile()->Read(buffer->Data(), buffer->SizeAvailable());
	if (bytesRead < 0) {
		// Some sort of file related error occured...
		TRACE_ERROR("Failed to read from the file: %s\n", strerror(bytesRead));
		return bytesRead;
	}

	TRACE("Post file read stats: %" B_PRIdSSIZE " file bytes read at position %" B_PRIdOFF ".\n",
		bytesRead, position);

	buffer->SetSizeUsed(bytesRead);

	media_header* header = buffer->Header();
	header->type = B_MEDIA_MULTISTREAM;
	header->size_used = bytesRead;
	header->file_pos = position;
	header->orig_size = bytesRead;
	header->time_source = TimeSource()->ID();
	header->start_time = TimeSource()->Now();

	// TODO: Nothing more to say?
	return B_OK;
}


// -------------------------------------------------------- //
// Mmmh, stuffing! Though is this needed???
// (Allows us to add new methods to this class without breaking ABI compatibility.)
// -------------------------------------------------------- //
status_t MediaReader::_Reserved_MediaReader_0(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_1(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_2(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_3(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_4(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_5(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_6(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_7(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_8(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_9(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_10(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_11(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_12(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_13(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_14(void*) { return B_ERROR; }
status_t MediaReader::_Reserved_MediaReader_15(void*) { return B_ERROR; }
