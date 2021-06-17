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

			status_t			InitCheck() const
									{ return fTime >= 0 ? B_OK : B_ERROR; }

	// Accessors
	const	BString&			Name() const
									{ return fName; }
			JobStatus			Status() const
									{ return fStatus; }
			bool				IsValid() const
									{ return fValid; }
			bigtime_t			Time() const
									{ return fTime; }
	const	node_ref&			NodeRef() const
									{ return fNode; }
	const	entry_ref&			EntryRef() const
									{ return fEntry; }
			bool				IsWaiting() const
									{ return fStatus == kWaiting; }
			Printer*			GetPrinter() const
									{ return fPrinter; }

	// Modifiers
			void				SetPrinter(Printer* printer)
									{ fPrinter = printer; }
			void				SetStatus(JobStatus status,
									bool writeToNode = true);
			void				UpdateAttribute();
			void				Remove();

private:
			void				_UpdateStatus(const char* status);
			void				_UpdateStatusAttribute(const char* status);
			bool				_HasAttribute(BNode* node, const char* name);
			bool				_IsValidJobFile();

private:
			Folder*				fFolder;
				// Handler that watches the node of the job file
			BString				fName;
				// Name of the job file
			bigtime_t			fTime;
				// Time encoded in the file name
			node_ref			fNode;
				// Node of the job file
			entry_ref			fEntry;
				// Entry of the job file
			JobStatus			fStatus;
				// Status of the job file
			bool				fValid;
				// Is the job file valid?
			Printer*			fPrinter;
				// Printer that is processing this job
};


// Printer folder watches creation, deletion, and attribute changes of job files
// and notifies print_server if a job is waiting for processing
class Folder : public FolderWatcher, public FolderListener {
public:
								Folder(BLocker* fLocker, BLooper* looper,
									const BDirectory& spoolDir);
								~Folder();

			BDirectory*			GetSpoolDir()
									{ return FolderWatcher::Folder(); }

			BLocker*			Locker() const
									{ return fLocker; }
			bool				Lock() const
									{ return fLocker != NULL ?
										fLocker->Lock() : true; }
			void				Unlock() const
									{ if (fLocker != NULL) fLocker->Unlock(); }

			int32				CountJobs() const
									{ return fJobs.CountItems(); }
			Job*				JobAt(int32 index) const
									{ return fJobs.ItemAt(index); }

								// Caller is responsible to set the status of
								// the job appropriately and to release the
								// object when done
			Job*				GetNextJob();

protected:
			enum {
				kJobAdded,
				kJobRemoved,
				kJobAttrChanged,
			};

	virtual	void				Notify(Job* job, int kind) = 0;

private:
	static	int					_AscendingByTime(const Job* a, const Job* b);

			bool				_AddJob(BEntry& entry, bool notify = true);
			Job*				_Find(node_ref* node);

	// FolderListener
			void				_EntryCreated(node_ref* node, entry_ref* entry);
			void				_EntryRemoved(node_ref* node);
			void				_AttributeChanged(node_ref* node);

			void				_SetupJobList();

private:
			BLocker*			fLocker;
			BObjectList<Job>	fJobs;
};

#endif /* _JOBS_H */
