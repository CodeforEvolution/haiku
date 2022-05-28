/*
 * Copyright 2002 Andrew Bachmann
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _MEDIA_READER_H
#define _MEDIA_READER_H


#include <BufferGroup.h>
#include <BufferProducer.h>
#include <Controllable.h>
#include <Entry.h>
#include <File.h>
#include <FileInterface.h>
#include <MediaDefs.h>
#include <MediaEventLooper.h>
#include <MediaNode.h>

#include "AbstractFileInterfaceNode.h"


// A MediaReader is a node that
// implements FileInterface and BBufferProducer.
// It reads any file and produces one output,
// which is a multistream.  It has a rather
// unique interpretation of time.  Time is
// distance in the file.  So the duration is the
// file length. (in bytes)


class MediaReader : public BBufferProducer, public AbstractFileInterfaceNode {
public:
	explicit					MediaReader(size_t defaultChunkSize = 8192,
												// chunk size = 8 KB
											float defaultBitRate = 800000,
												// bit rate = 100.000 KB/sec = 5.85 MB/minute
											const flavor_info* info = NULL,
												// buffer period = 80 milliseconds
											BMessage* config = NULL,
											BMediaAddOn* addOn = NULL);

protected:
	virtual						~MediaReader();


/**************************************************************************************************/
/* Begin inherited from BMediaNode */
public:
	virtual	status_t			HandleMessage(int32 message, const void* data, size_t size);

protected:
		/* These don't return errors; instead, they use the global error condition reporter. */
		/* A node is required to have a queue of at least one pending command (plus TimeWarp) */
		/* and is recommended to allow for at least one pending command of each type. */
		/* Allowing an arbitrary number of outstanding commands might be nice, but apps */
		/* cannot depend on that happening. */
	virtual	void				NodeRegistered();
	virtual	void				Preroll();

/* End inherited from BMediaNode */
/**************************************************************************************************/


/**************************************************************************************************/
/* Begin inherited from BFileInterface */
protected:
	//using						AbstractFileInterfaceNode::SetRef;

	virtual	status_t			SetRef(const entry_ref& file, bool create, bigtime_t* outTime);


/* End inherited from BFileInterface */
/**************************************************************************************************/


// Provided for BMediaReaderAddOn
public:
								// outMimeType should be 256 bytes
	static	status_t			StaticSniffRef(const entry_ref& file, char* outMimeType,
									float* outQuality);


/**************************************************************************************************/
/* Begin inherited from BBufferProducer */
protected:
	virtual	status_t			FormatSuggestionRequested(media_type type, int32 quality,
									media_format* format);
	virtual	status_t			FormatProposal(const media_source& output, media_format* format);

	/* If the format isn't good, put a good format into *ioFormat and return error. */
	/* If format has wildcard, specialize to what you can do (and change).			*/
	/* If you can change the format, return B_OK.									*/
	/* The request comes from your destination sychronously, so you cannot ask it	*/
	/* whether it likes it; You should assume it will since it asked.				*/
	virtual	status_t			FormatChangeRequested(const media_source& source,
														const media_destination& destination,
														media_format* ioFormat,
														int32* _deprecated_);

	virtual	status_t			GetNextOutput(int32* cookie, media_output* outOutput);
									// cookie starts as 0
	virtual	status_t			DisposeOutputCookie(int32 cookie);

	/* SetBufferGroup(): You should either pass on the group to your upstream guy,				*/
	/* or delete your current group and hang on to this group. Deleting the previous			*/
	/* group (unless you passed it on with the reclaim flag set to false) is very important!	*/
	/* If you don't you will 1) leak memory, and 2) block someone who may want to reclaim the	*/
	/* buffers living in that group.															*/
	virtual	status_t			SetBufferGroup(const media_source& forSource, BBufferGroup* group);

	/* Format of clipping is (as int16-s): <from line> <npairs> <startclip> <endclip>.		*/
	/* Repeat for each line where the clipping is different from the previous line.			*/
	/* If <npairs> is negative, use the data from line -<npairs> (there are 0 pairs after	*/
	/* a negative <npairs>. Yes, we only support 32k*32k frame buffers for clipping.		*/
	/* Any non-0 field of 'display' means that that field changed, and if you don't support */
	/* that change, you should return an error and ignore the request. Note that the buffer */
	/* offset values do not have wildcards; 0 (or -1, or whatever) are real values and must */
	/* be adhered to. */
	virtual	status_t			VideoClippingChanged(const media_source & forSource,
									int16 numShorts, int16* clipData,
									const media_video_display_info& display, int32* _deprecated_);

	/* Iterates over all outputs and maxes the latency found */
	virtual	status_t			GetLatency(bigtime_t* outLatency);

	virtual	status_t			PrepareToConnect(const media_source& what,
									const media_destination& where, media_format* format,
									media_source* outSource, char* outName);
	virtual	void				Connect(status_t error, const media_source& source,
									const media_destination& destination,
									const media_format& format, char* ioName);
	virtual	void				Disconnect(const media_source& what,
									const media_destination& where);
	virtual	void				LateNoticeReceived(const media_source& what, bigtime_t howMuch,
									bigtime_t performanceTime);
	virtual	void				EnableOutput(const media_source& what, bool enabled,
									int32* _deprecated_ = NULL);
	virtual	status_t			SetPlayRate(int32 numerator, int32 denominator);

	virtual	void				AdditionalBufferRequested(const media_source& source,
									media_buffer_id prevBuffer, bigtime_t prevTime,
									const media_seek_tag* prevTag = NULL);

	virtual	void				LatencyChanged(const media_source& source,
									const media_destination& destination, bigtime_t newLatency,
									uint32 flags);

/* End inherited from BBufferProducer */
/****************************/


/*********************/
/* BMediaEventLooper */
/*********************/

protected:
	virtual	status_t			HandleBuffer(const media_timed_event* event, bigtime_t lateness,
									bool realTimeEvent = false);

	virtual	status_t			HandleDataStatus(const media_timed_event* event, bigtime_t lateness,
									bool realTimeEvent = false);


/***************/
/* MediaReader */
/***************/

public:
	static	status_t			GetFlavor(flavor_info* outInfo, int32 id);
	static	void				GetFormat(media_format* outFormat);
	static	void				GetFileFormat(media_file_format* outFileFormat);

protected:
	virtual	status_t			GetFilledBuffer(BBuffer** outBuffer);
	virtual	status_t			FillFileBuffer(BBuffer* buffer);

private:
								/* Private/Unimplemented */
								MediaReader(const MediaReader& clone);
								MediaReader& operator= (const MediaReader& clone);

private:
			media_output		fOutput;
			bool				fOutputEnabled;

			BBufferGroup*		fBufferGroup;
			bigtime_t			fDownstreamLatency;
			bigtime_t			fInternalLatency;
									// This is computed from the real (negotiated) chunk size and
									// bit rate, not the defaults that are in the parameters.
			bigtime_t			fBufferPeriod;

	/* Mmmh, stuffing! */
	virtual	status_t			_Reserved_MediaReader_0(void*);
	virtual	status_t			_Reserved_MediaReader_1(void*);
	virtual	status_t			_Reserved_MediaReader_2(void*);
	virtual	status_t			_Reserved_MediaReader_3(void*);
	virtual	status_t			_Reserved_MediaReader_4(void*);
	virtual	status_t			_Reserved_MediaReader_5(void*);
	virtual	status_t			_Reserved_MediaReader_6(void*);
	virtual	status_t			_Reserved_MediaReader_7(void*);
	virtual	status_t			_Reserved_MediaReader_8(void*);
	virtual	status_t			_Reserved_MediaReader_9(void*);
	virtual	status_t			_Reserved_MediaReader_10(void*);
	virtual	status_t			_Reserved_MediaReader_11(void*);
	virtual	status_t			_Reserved_MediaReader_12(void*);
	virtual	status_t			_Reserved_MediaReader_13(void*);
	virtual	status_t			_Reserved_MediaReader_14(void*);
	virtual	status_t			_Reserved_MediaReader_15(void*);

			uint32				_reserved_media_reader_[16];
};

#endif /* _MEDIA_READER_H */
