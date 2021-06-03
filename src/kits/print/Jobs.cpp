/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "Jobs.h"

#include <stdlib.h>
#include <string.h>

#include <Application.h>
#include <fs_attr.h>
#include <Node.h>
#include <NodeInfo.h>
#include <NodeMonitor.h>

#include "pr_server.h"


// Implementation of Job
Job::Job(const BEntry& job, Folder* folder)
	:
	fFolder(folder),
	fTime(-1),
	fStatus(kUnknown),
	fValid(false),
	fPrinter(NULL)
{
	// store light weight versions of BEntry and BNode
	job.GetRef(&fEntry);
	job.GetNodeRef(&fNode);

	fValid = IsValidJobFile();

	BNode node(&job);
	if (node.InitCheck() != B_OK)
		return;

	BString status;
		// Read status attribute
	if (node.ReadAttrString(PSRV_SPOOL_ATTR_STATUS, &status) != B_OK)
		status = "";

    UpdateStatus(status.String());

    // Now get file name and creation time from file name
    fTime = 0;
    BEntry entry(job);
    char name[B_FILE_NAME_LENGTH];

    if (entry.InitCheck() != B_OK || entry.GetName(name) != B_OK)
		return;

	fName = name;

	// Search for last '@' in file name
	char* p = NULL
	char* c = name;
	while ((c = strchr(c, '@')) != NULL) {
		p = c;
		c++;
	}

	// Get time from file name
	if (p != NULL)
		fTime = atoi(p + 1);
}


// Conversion from string representation of status to JobStatus constant
void
Job::UpdateStatus(const char* status)
{
	if (strcmp(status, PSRV_JOB_STATUS_WAITING) == 0)
		fStatus = kWaiting;
	else if (strcmp(status, PSRV_JOB_STATUS_PROCESSING) == 0)
		fStatus = kProcessing;
	else if (strcmp(status, PSRV_JOB_STATUS_FAILED) == 0)
		fStatus = kFailed;
	else if (strcmp(status, PSRV_JOB_STATUS_COMPLETED) == 0)
		fStatus = kCompleted;
	else
		fStatus = kUnknown;
}


// Write to status attribute of node
void
Job::UpdateStatusAttribute(const char* status)
{
	BNode node(&fEntry);
	if (node.InitCheck() == B_OK)
		node.WriteAttrString(PSRV_SPOOL_ATTR_STATUS, status);

		// node.WriteAttr(PSRV_SPOOL_ATTR_STATUS, B_STRING_TYPE, 0, status, strlen(status)+1);
}


bool
Job::HasAttribute(BNode* node, const char* name)
{
	attr_info info;
	return node->GetAttrInfo(name, &info) == B_OK;
}


bool
Job::IsValidJobFile()
{
	BNode node(&fEntry);
	if (node.InitCheck() != B_OK)
		return false;

	BNodeInfo info(&node);
	char mimeType[B_MIME_TYPE_LENGTH];

	// Is the job a spool file?
	if (info.InitCheck() == B_OK
		&& info.GetType(mimeType) == B_OK
		&& strcmp(mimeType, PSRV_SPOOL_FILETYPE) == 0)
		&& HasAttribute(&node, PSRV_SPOOL_ATTR_MIMETYPE)
		&& HasAttribute(&node, PSRV_SPOOL_ATTR_PAGECOUNT)
		&& HasAttribute(&node, PSRV_SPOOL_ATTR_DESCRIPTION)
		&& HasAttribute(&node, PSRV_SPOOL_ATTR_PRINTER)
		&& HasAttribute(&node, PSRV_SPOOL_ATTR_STATUS)
		return true;

	return false;
}


// Set status of object and optionally write to attribute of node
void
Job::SetStatus(JobStatus status, bool writeToNode)
{
	fStatus = status;

	if (!writeToNode)
		return;

	switch (status) {
		case kWaiting:
			UpdateStatusAttribute(PSRV_JOB_STATUS_WAITING);
			break;

		case kProcessing:
			UpdateStatusAttribute(PSRV_JOB_STATUS_PROCESSING);
			break;

		case kFailed:
			UpdateStatusAttribute(PSRV_JOB_STATUS_FAILED);
			break;

		case kCompleted:
			UpdateStatusAttribute(PSRV_JOB_STATUS_COMPLETED);
			break;

		default:
			break;
	}
}


// Synchronize file attribute with member variable
void
Job::UpdateAttribute()
{
	fValid = fValid || IsValidJobFile();

	BNode node(&fEntry);
	BString status;
	if (node.InitCheck() == B_OK
		&& node.ReadAttrString(PSRV_SPOOL_ATTR_STATUS, &status) == B_OK)
		UpdateStatus(status.String());
}

void
Job::Remove()
{
	BEntry entry(&fEntry);
	if (entry.InitCheck() == B_OK)
		entry.Remove();
}


// Implementation of Folder

// BObjectList CompareFunction
int
Folder::AscendingByTime(const Job* a, const Job* b)
{
	return a->Time() - b->Time();
}


bool
Folder::AddJob(BEntry& entry, bool notify)
{
	Job* job = new Job(entry, this);
	if (job == NULL)
		return false;

	if (job->InitCheck() == B_OK) {
		fJobs.AddItem(job);
		if (notify)
			Notify(job, kJobAdded);

		return true;
	}

	job->Release();

	return false;
}


// Simplified assumption that ino_t identifies job file
// will probabely not work in all cases with link to a file on another volume???
Job*
Folder::Find(node_ref* node)
{
	if (node == NULL)
		return NULL;

	for (int32 index = 0; index < fJobs.CountItems(); index++) {
		Job* job = fJobs.ItemAt(index);
		if (job != NULL && job->NodeRef() == *node)
			return job;
	}

	return NULL;
}


void
Folder::EntryCreated(node_ref* node, entry_ref* entry)
{
	BEntry job(entry);
	if (job.InitCheck() == B_OK && Lock()) {
		if (AddJob(job))
			fJobs.SortItems(AscendingByTime);

		Unlock();
	}
}


void
Folder::EntryRemoved(node_ref* node)
{
	Job* job = Find(node);
	if (job != NULL && Lock()) {
		fJobs.RemoveItem(job);
		Notify(job, kJobRemoved);
		job->Release();
		Unlock();
	}
}


void
Folder::AttributeChanged(node_ref* node)
{
	Job* job = Find(node);
	if (job != NULL && Lock()) {
		job->UpdateAttribute();
		Notify(job, kJobAttrChanged);
		Unlock();
	}
}


// Initial setup of job list
void
Folder::SetupJobList()
{
	if (inherited::Folder()->InitCheck() != B_OK)
		return;

	inherited::Folder()->Rewind();

	BEntry entry;
	while (inherited::Folder()->GetNextEntry(&entry) == B_OK)
		AddJob(entry, false);

	fJobs.SortItems(AscendingByTime);
}


Folder::Folder(BLocker* locker, BLooper* looper, const BDirectory& spoolDir)
	:
	FolderWatcher(looper, spoolDir, true),
	fLocker(locker),
	fJobs()
{
	SetListener(this);

	if (Lock()) {
		SetupJobList();
		Unlock();
	}
}


Folder::~Folder()
{
	if (!Lock())
		return;

	// Release jobs
	for (int32 index = 0; index < fJobs.CountItems(); index++) {
		Job* job = fJobs.ItemAt(index);
		if (job != NULL)
			job->Release();
	}

	Unlock();
}


Job*
Folder::GetNextJob()
{
	for (int32 index = 0; index < fJobs.CountItems(); index++) {
		Job* job = fJobs.ItemAt(index);
		if (job != NULL && job->IsValid() && job->IsWaiting()) {
			job->Acquire();
			return job;
		}
	}

	return NULL;
}

