/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _FOLDER_WATCHER_H
#define _FOLDER_WATCHER_H


#include <Directory.h>
#include <Entry.h>
#include <Handler.h>
#include <Looper.h>
#include <StorageDefs.h>
#include <String.h>


class FolderListener {
public:
	virtual ~FolderListener() {};
		// entry created or moved into folder
	virtual void EntryCreated(node_ref* node, entry_ref* entry) {};
		// entry removed from folder (or moved to another folder)
	virtual void EntryRemoved(node_ref* node) {};
		// attribute of an entry has changed
	virtual void AttributeChanged(node_ref* node) {};
};


// Watches creation, deletion of files in a directory, and optionally watches
// attribute changes of files in the directory.
class FolderWatcher : public BHandler {
	typedef BHandler inherited;

public:
	FolderWatcher(BLooper* looper, const BDirectory& folder,
		bool watchAttrChanges = false);
		// Start node watching (optionally watch attribute changes)
	virtual ~FolderWatcher();
		// Stop node watching

	void MessageReceived(BMessage* msg);

	BDirectory* Folder() { return &fFolder; }
		// The directory

	void SetListener(FolderListener* listener);
		// Set listener that is notified of changes in the directory

	// Start/stop watching of attribute changes
	status_t StartAttrWatching(node_ref* node);
	status_t StopAttrWatching(node_ref* node);

private:
	bool BuildEntryRef(BMessage* msg, const char* dirName, entry_ref* entry);
	bool BuildNodeRef(BMessage* msg, node_ref* node);

	void HandleCreatedEntry(BMessage* msg, const char* dirName);
	void HandleRemovedEntry(BMessage* msg);
	void HandleChangedAttr(BMessage* msg);

private:
	BDirectory fFolder;
	FolderListener* fListener;
	bool fWatchAttrChanges;
};

#endif
