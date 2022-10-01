// AbstractFileInterfaceNode.h
//
// Andrew Bachmann, 2002
//
// The AbstractFileInterfaceNode class implements
// the common functionality between MediaReader
// and MediaWriter.

#ifndef _ABSTRACT_FILE_INTERFACE_NODE_H
#define _ABSTRACT_FILE_INTERFACE_NODE_H


#include <BufferGroup.h>
#include <Controllable.h>
#include <Entry.h>
#include <File.h>
#include <FileInterface.h>
#include <MediaAddOn.h>
#include <MediaDefs.h>
#include <MediaNode.h>
#include <MediaEventLooper.h>
#include <String.h>


class AbstractFileInterfaceNode : public BFileInterface, public BControllable,
	public BMediaEventLooper {
protected:
	virtual ~AbstractFileInterfaceNode();
		// NOTE: Call Release() to destroy a node.

public:
	static status_t StaticSniffRef(const entry_ref& file, char* out_mime_type, float* out_quality);

	virtual	status_t InitCheck() const;

	virtual	status_t SetRefWithOpenMode(const entry_ref& file, uint32 openMode, bool create,
						bigtime_t* out_time);

	virtual	status_t GetConfigurationFor(BMessage* into_message);

protected:
	explicit AbstractFileInterfaceNode(
				size_t defaultChunkSize = 8192,	// chunk size = 8 KB
				float defaultBitRate = 800000,	// bit rate = 100.000 KB/sec = 5.85 MB/minute
				const flavor_info* info = NULL,	// buffer period = 80 milliseconds
				BMessage* config = NULL,
				BMediaAddOn* addOn = NULL);

	virtual	BParameterWeb* MakeParameterWeb();


/**************/
/* BMediaNode */
/**************/

public:
	virtual	BMediaAddOn* AddOn(int32* internalID) const;

	virtual	status_t HandleMessage(int32 message, const void* data, size_t size);

	virtual	status_t GetNodeAttributes(media_node_attribute* outAttributes, size_t inMaxCount);

	virtual	status_t AddTimer(bigtime_t at_performance_time, int32 cookie);

protected:
	// These don't return errors; instead, they use the global error condition reporter.
	virtual	void Start(bigtime_t performance_time);
	virtual	void Stop(bigtime_t performance_time, bool immediate);
	virtual	void Seek(bigtime_t media_time, bigtime_t performance_time);
	virtual	void SetRunMode(run_mode mode);
	virtual	void TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time);
	virtual	void Preroll();
	virtual	void SetTimeSource(BTimeSource* time_source);

	virtual	status_t RequestCompleted(const media_request_info& info);

	virtual	status_t DeleteHook(BMediaNode* node);

	virtual	void NodeRegistered();

/***********************/
/* end from BMediaNode */
/***********************/


/******************/
/* BFileInterface */
/******************/

protected:
	virtual	status_t GetNextFileFormat(int32* cookie, media_file_format* out_format);
	virtual	void DisposeFileFormatCookie(int32 cookie);

	virtual	status_t GetDuration(bigtime_t* out_time);

	virtual	status_t SniffRef(const entry_ref& file, char* out_mime_type, float* out_quality);
	virtual	status_t SetRef(const entry_ref& file, bool create, bigtime_t* out_time) = 0;
	virtual	status_t GetRef(entry_ref* out_ref, char* out_mime_type);

/***************************/
/* end from BFileInterface */
/***************************/


/****************************/
/* start from BControllable */

public:
		/* These are alternate methods of accomplishing the same thing as */
		/* connecting to control information source/destinations would. */
	virtual	status_t GetParameterValue(int32 id, bigtime_t* last_change, void* value,
						size_t* ioSize);
	virtual	void SetParameterValue(int32 id, bigtime_t when, const void* value, size_t size);
	virtual	status_t StartControlPanel(BMessenger* out_messenger);

/* end from BControllable */
/**************************/

public:
		// these three are related:
		// DEFAULT_CHUNK_SIZE = (DEFAULT_BIT_RATE * 1024) * (DEFAULT_BUFFER_PERIOD / 8000000)
		static const int32 DEFAULT_CHUNK_SIZE_PARAM;    // in bytes
		static const int32 DEFAULT_BIT_RATE_PARAM;      // in 1000 * kilobits/sec
		static const int32 DEFAULT_BUFFER_PERIOD_PARAM; // milliseconds

private:
		size_t fDefaultChunkSizeParam;
		bigtime_t fDefaultChunkSizeParamChangeTime;
		float fDefaultBitRateParam;
		bigtime_t fDefaultBitRateParamChangeTime;
		int32 fDefaultBufferPeriodParam;
		bigtime_t fDefaultBufferPeriodParamChangeTime;

		// This is used to figure out which parameter to compute
		// when enforcing the above constraint relating the three params
		int32 fLastUpdatedParameter;
		int32 fLeastRecentlyUpdatedParameter;

/********************************/
/* start from BMediaEventLooper */
protected:
	/* you must override to handle your events! */
	/* you should not call HandleEvent directly */
	virtual void		HandleEvent(const media_timed_event* event, bigtime_t lateness,
							bool realTimeEvent = false);

	/* override to clean up custom events you have added to your queue */
	virtual void		CleanUpEvent(const media_timed_event* event);

	/* called from Offline mode to determine the current time of the node */
	/* update your internal information whenever it changes */
	virtual	bigtime_t	OfflineTime();

	/* override only if you know what you are doing! */
	/* otherwise much badness could occur */
	/* the actual control loop function: */
	/* 	waits for messages, Pops events off the queue and calls DispatchEvent */
	virtual void		ControlLoop();
/* end from BMediaEventLooper */
/******************************/

protected:
	virtual status_t HandleStart(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false);
	virtual status_t HandleSeek(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false);
	virtual status_t HandleWarp(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false);
	virtual status_t HandleStop(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false);
	virtual status_t HandleBuffer(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false) = 0;
	virtual status_t HandleDataStatus(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false) = 0;
	virtual status_t HandleParameter(const media_timed_event* event, bigtime_t lateness,
						bool realTimeEvent = false);

public:
	static void GetFlavor(flavor_info* outInfo, int32 id);
	static void GetFormat(media_format* outFormat);
	static void GetFileFormat(media_file_format* outFileFormat);

protected:
	virtual status_t AddRequirements(media_format* format);
	virtual status_t ResolveWildcards(media_format* format);

// accessors

	inline BFile* GetCurrentFile() { return fCurrentFile; }

private:
		AbstractFileInterfaceNode(	/* private unimplemented */
				const AbstractFileInterfaceNode& clone);
		AbstractFileInterfaceNode& operator=(
				const AbstractFileInterfaceNode& clone);

		status_t fInitCheckStatus;

		BMediaAddOn* fAddOn;

		BFile* fCurrentFile;
		entry_ref fCurrentRef;
		BString* fCurrentMimeType;

	/* Mmmh, stuffing! */
	virtual		status_t _Reserved_AbstractFileInterfaceNode_0(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_1(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_2(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_3(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_4(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_5(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_6(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_7(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_8(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_9(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_10(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_11(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_12(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_13(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_14(void*);
	virtual		status_t _Reserved_AbstractFileInterfaceNode_15(void*);

		uint32 _reserved_abstract_file_interface_node_[16];

};

#endif /* _ABSTRACT_FILE_INTERFACE_NODE_H */
