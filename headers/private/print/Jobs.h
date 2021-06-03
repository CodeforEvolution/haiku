/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _JOBS_H
#define _JOBS_H


#include <Directory.h>
#include <Entry.h>
#include <Handler.h>
#include <Locker.h>
#include <ObjectList.h>
#include <String.h>
#include <StorageDefs.h>

#include "BeUtils.h"
#include "FolderWatcher.h"


enum JobStatus {  // Job file's status
	kWaiting,     // To be processed
	kProcessing,  // Processed by a printer add-on
	kFailed,      // Failed to process the job file
	kCompleted,   // Successfully processed
	kUnknown,     // Other
};

class Printer;
class Folder;

// Job file in printer folder
class Job : public Object {
public:
	Job(const BEntry& entry, Folder* folder);

	status_t InitCheck() const        { return fTime >= 0 ? B_OK : B_ERROR; }

	// accessors
	const BString& Name() const       { return fName; }
	JobStatus Status() const          { return fStatus; }
	bool IsValid() const              { return fValid; }
	long Time() const                 { return fTime; }
	const node_ref& NodeRef() const   { return fNode; }
	const entry_ref& EntryRef() const { return fEntry; }
	bool IsWaiting() const            { return fStatus == kWaiting; }
	Printer* GetPrinter() const       { return fPrinter; }

	// modifiers
	void SetPrinter(Printer* p) { fPrinter = p; }
	void SetStatus(JobStatus status, bool writeToNode = true);
	void UpdateAttribute();
	void Remove();

private:
	void UpdateStatus(const char* status);
	void UpdateStatusAttribute(const char* status);
	bool HasAttribute(BNode* node, const char* name);
	bool IsValidJobFile();

private:
	Folder* fFolder;      // Handler that watches the node of the job file
	BString fName;        // Name of the job file
	long fTime;           // Time encoded in the file name
	node_ref fNode;       // Node of the job file
	entry_ref fEntry;     // Entry of the job file
	JobStatus fStatus;    // Status of the job file
	bool fValid;          // Is the job file valid?
	Printer* fPrinter;    // Printer that is processing this job
};


// Printer folder watches creation, deletion, and attribute changes of job files
// and notifies print_server if a job is waiting for processing
class Folder : public FolderWatcher, public FolderListener {
	typedef FolderWatcher inherited;

public:
	Folder(BLocker* fLocker, BLooper* looper, const BDirectory& spoolDir);
	~Folder();

	BDirectory* GetSpoolDir() { return inherited::Folder(); }

	BLocker* Locker() const { return fLocker; }
	bool Lock() const       { return fLocker != NULL ? fLocker->Lock() : true; }
	void Unlock() const     { if (fLocker) fLocker->Unlock(); }

	int32 CountJobs() const { return fJobs.CountItems(); }
	Job* JobAt(int32 index) const { return fJobs.ItemAt(index); }

		// Caller is responsible to set the status of the job appropriately
		// and to release the object when done
	Job* GetNextJob();

protected:
	enum print_job_notification {
		kJobAdded,
		kJobRemoved,
		kJobAttrChanged,
	};

	virtual void Notify(Job* job, print_job_notification kind) = 0;

private:
	static int AscendingByTime(const Job* a, const Job* b);

	bool AddJob(BEntry& entry, bool notify = true);
	Job* Find(node_ref* node);

	// FolderListener
	void EntryCreated(node_ref* node, entry_ref* entry);
	void EntryRemoved(node_ref* node);
	void AttributeChanged(node_ref* node);

	void SetupJobList();

private:
	BLocker* fLocker;
	BObjectList<Job> fJobs;
};
#endif
