// MediaOutputInfo.cpp
//
// Andrew Bachmann, 2002
//
// A class to encapsulate and manipulate
// all the information for a particular
// output of a media node.


#include "MediaOutputInfo.h"

#include <stdio.h>
#include <string.h>

#include <BufferGroup.h>
#include <BufferProducer.h>
#include <MediaDefs.h>

//#include "misc.h"


MediaOutputInfo::MediaOutputInfo(BBufferProducer* node, const char* name)
	:
	fProducer(node),
	fOutputEnabled(true),
	fBufferGroup(NULL),
	fBufferSize(0),
	fDownstreamLatency(0),
	fBufferPeriod(1)
{
	CALLED();

	strlcpy(fOutput.name, name, B_MEDIA_NAME_LENGTH);

	// Initialize the output
	fOutput.node = media_node::null;
	fOutput.source = media_source::null;
	fOutput.destination = media_destination::null;
}


MediaOutputInfo::~MediaOutputInfo()
{
	CALLED();

	delete fBufferGroup;
	fBufferGroup = NULL;
}


status_t
MediaOutputInfo::SetBufferGroup(BBufferGroup* group)
{
	CALLED();

	if (group == NULL)
		return B_BAD_VALUE;

	if (fBufferGroup != NULL) {
		if (fBufferGroup == group)
			return B_OK;

		delete fBufferGroup;
		fBufferGroup = NULL;
	}

	fBufferGroup = group;
}


// They made an offer to us.  We should make sure that the offer is
// acceptable, and then we can add any requirements we have on top of
// that.  We leave wildcards for anything that we don't care about.
status_t
MediaOutputInfo::FormatProposal(media_format* format)
{
	CALLED();

	if (!format_is_compatible(*format, generalFormat))
		return B_MEDIA_BAD_FORMAT;

	// FIXME: Test because we don't trust them!
	format->SpecializeTo(&wildcardedFormat);
	return B_OK;
}


// Presumably we have already agreed with them that this format is
// okay. But just in case, we check the offer. We will also complain if it
// is invalid. Then, as the last thing we do, we get rid of any
// remaining wilcards.
status_t
MediaOutputInfo::FormatChangeRequested(const media_destination& destination,
	media_format* io_format)
{
	CALLED();

	status_t result = FormatProposal(io_format);
	if (result != B_OK) {
		*io_format = fGeneralFormat;
		return result;
	}

	io_format->SpecializeTo(&fFullySpecifiedFormat);
	return B_OK;
}


status_t
MediaOutputInfo::PrepareToConnect(const media_destination& where, media_format* format,
	media_source* out_source, char* out_name)
{
	CALLED();

	if (fOutput.destination != media_destination::null)
		return B_MEDIA_ALREADY_CONNECTED;

	status_t result = FormatChangeRequested(where,format);
	if (result != B_OK) {
		ERROR("\tFailed to perform a format change while preparing to connect: %s\n",
			strerror(result));
		return result;
	}

	*out_source = fOutput.source;
	fOutput.destination = where;
	strlcpy(out_name, output.name, B_MEDIA_NAME_LENGTH);

	return B_OK;
}


status_t
MediaOutputInfo::Connect(const media_destination& destination, const media_format& format,
	char* io_name, bigtime_t _downstreamLatency)
{
	CALLED();

	fOutput.destination = destination;
	fOutput.format = format;
	strlcpy(io_name, fOutput.name, B_MEDIA_NAME_LENGTH);

	fDownstreamLatency = _downstreamLatency;
		// This must be set before creating a buffer group!
	status_t result = CreateBufferGroup();
		// Also initializes the buffer period.
	if (result != B_OK) {
		fOutput.destination = media_destination::null;
		fOutput.format = generalFormat;
		return result;
	}

	return B_OK;
}


status_t
MediaOutputInfo::Disconnect()
{
	CALLED();

	fOutput.destination = media_destination::null;
	fOutput.format = fGeneralFormat;

	delete fBufferGroup;
	fBufferGroup = NULL;
}


status_t
MediaOutputInfo::EnableOutput(bool enabled)
{
	CALLED();

	fOutputEnabled = enabled;
	return B_OK;
}


status_t
MediaOutputInfo::AdditionalBufferRequested(media_buffer_id prev_buffer, bigtime_t prev_time,
	const media_seek_tag* prev_tag)
{
	CALLED();

	// TODO: Implement me!
	return B_OK;
}


status_t
MediaOutputInfo::CreateBufferGroup()
{
	CALLED();

	fBufferPeriod = ComputeBufferPeriod();

	if (fBufferGroup != NULL)
		return B_OK;

	int32 count = int32(fDownstreamLatency / fBufferPeriod) + 2;
	TRACE("\tDownstream latency = %lld, Buffer period = %lld, Buffer count = %i\n",
		fDownstreamLatency, fBufferPeriod, count);

	// allocate the buffers
	fBufferGroup = new BBufferGroup(ComputeBufferSize(), count);
	if (fBufferGroup == NULL)
		return B_NO_MEMORY;

	status_t result = fBufferGroup->InitCheck();
	if (result != B_OK) {
		ERROR("\tBufferGroup initialization failed: %s\n", strerror(result));

		delete fBufferGroup;
		fBufferGroup = NULL;

		return result;
	}

	return B_OK;
}


uint32
MediaOutputInfo::ComputeBufferSize()
{
	CALLED();
	return ComputeBufferSize(fOutput.format);
}


// Returns result in # of bytes.
uint32
MediaOutputInfo::ComputeBufferSize(const media_format& format)
{
	CALLED();

	uint64 bufferSize = 1024;
		// Default to 1024 bytes

	switch (format.type) {
		case B_MEDIA_MULTISTREAM:
			bufferSize = format.u.multistream.max_chunk_size;
			break;

		case B_MEDIA_ENCODED_VIDEO:
			bufferSize = format.u.encoded_video.frame_size;
			break;

		case B_MEDIA_RAW_VIDEO:
		{
			if (format.u.raw_video.interlace == 0) {
				// Okay, you have no fields... So, you need no space, right? :)
				bufferSize = 0;
			} else {
				// This is the size of a *field*, not a frame
				bufferSize = (format.u.raw_video.display.bytes_per_row *
					format.u.raw_video.display.line_count) / format.u.raw_video.interlace;
			}

			break;
		}

		case B_MEDIA_ENCODED_AUDIO:
			bufferSize = format.u.encoded_audio.frame_size;
			break;

		case B_MEDIA_RAW_AUDIO:
			bufferSize = format.u.raw_audio.buffer_size;
			break;

		default:
			break;
	}

	if (bufferSize > INT_MAX)
		bufferSize = INT_MAX;

	return int32(bufferSize);
}


bigtime_t
MediaOutputInfo::ComputeBufferPeriod()
{
	CALLED();
	return ComputeBufferPeriod(fOutput.format);
}


// Returns result in # of microseconds.
bigtime_t
MediaOutputInfo::ComputeBufferPeriod(const media_format& format)
{
	CALLED();

	bigtime_t bufferPeriod = 25000;
		// Default of 25 milliseconds

	switch (format.type) {
		case B_MEDIA_MULTISTREAM:
		{
			// Given a buffer size of 8192 bytes
			// and a bitrate of 1024 kilobits/millisecond (= 128 bytes/millisecond):
			// We need to produce a buffer every 64 milliseconds (= every 64000 microseconds)
			bufferPeriod = bigtime_t(1000.0 * 8.0 * ComputeBufferSize(format)
				/ format.u.multistream.max_bit_rate);
			break;
		}

		case B_MEDIA_ENCODED_VIDEO:
		{
			bufferPeriod = bigtime_t(1000.0 * 8.0 * ComputeBufferSize(format)
				/ format.u.encoded_video.max_bit_rate);
			break;
		}

		case B_MEDIA_ENCODED_AUDIO:
		{
			bufferPeriod = bigtime_t(1000.0 * 8.0 * ComputeBufferSize(format)
				/ format.u.encoded_audio.bit_rate);
			break;
		}

		case B_MEDIA_RAW_VIDEO:
		{
			// Given a field rate of 50.00 fields per second, (PAL)
			// we need to generate a field/buffer
			// every 1/50 of a second = 20000 microseconds.
			bufferPeriod = bigtime_t(1000000.0 / format.u.raw_video.field_rate);
			break;
		}

		case B_MEDIA_RAW_AUDIO:
		{
			// Given a sample size of 4 bytes [B_AUDIO_INT]
			// and a channel count of 2 and a buffer_size
			// of 256 bytes and a frame_rate of 44100 Hertz (1/sec)
			// 1 frame = 1 sample/channel.
			// comes to ??
			// this is a guess:
			bufferPeriod = bigtime_t(1000000.0 * ComputeBufferSize(format)
				/ (format.u.raw_audio.format & media_raw_audio_format::B_AUDIO_SIZE_MASK)
				/ format.u.raw_audio.channel_count / format.u.raw_audio.frame_rate);
			break;
		}

		default:
			break;
	}

	return bufferPeriod;
}
