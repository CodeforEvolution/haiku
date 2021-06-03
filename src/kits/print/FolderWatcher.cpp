/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "FolderWatcher.h"

#include <stdio.h>

#include <fs_attr.h>
#include <Node.h>
#include <NodeInfo.h>
#include <NodeMonitor.h>


// Implementation of FolderWatcher
FolderWatcher::FolderWatcher(BLooper* looper, const BDirectory& folder,
	bool watchAttrChanges)
	:
	fFolder(folder),
	fListener(NULL),
	fWatchAttrChanges(watchAttrChanges)
{
	// Add this handler to the application for node monitoring
	if (looper->Lock()) {
		looper->AddHandler(this);
		looper->Unlock();
	}

	// Start attribute change watching for existing files
	if (watchAttrChanges) {
		BEntry entry;
		node_ref node;
		while (fFolder.GetNextEntry(&entry) == B_OK
			   && entry.GetNodeRef(&node) == B_OK)
			StartAttrWatching(&node);
	}

	// Start watching the spooler directory
	node_ref ref;
	fFolder.GetNodeRef(&ref);
	watch_node(&ref, B_WATCH_DIRECTORY, this);
}


FolderWatcher::~FolderWatcher()
{
	// Stop node watching for spooler directory
	node_ref ref;
	fFolder.GetNodeRef(&ref);
	watch_node(&ref, B_STOP_WATCHING, this);
		// stop sending notifications to this handler
	stop_watching(this);

	if (LockLooper()) {
		BLooper* looper = Looper();
			// and remove it
		looper->RemoveHandler(this);
		looper->Unlock();
	}
}


void
FolderWatcher::SetListener(FolderListener* listener)
{
	fListener = listener;
}


bool
FolderWatcher::BuildEntryRef(BMessage* message, const char* dirName,
	entry_ref* entry)
{
	const char* name;
	if (message->FindInt32("device", &entry->device) == B_OK
		&& message->FindInt64(dirName, &entry->directory) == B_OK
		&& message->FindString("name", &name) == B_OK) {
		entry->set_name(name);
		return true;
	}

	return false;
}


bool
FolderWatcher::BuildNodeRef(BMessage* message, node_ref* node)
{
	return (message->FindInt32("device", &node->device) == B_OK
			&& message->FindInt64("node", &node->node) == B_OK);
}


void
FolderWatcher::HandleCreatedEntry(BMessage* message, const char* dirName)
{
	node_ref node;
	entry_ref entry;
	if (BuildEntryRef(message, dirName, &entry)
		&& BuildNodeRef(message, &node)) {
		if (fWatchAttrChanges)
			StartAttrWatching(&node);

		fListener->EntryCreated(&node, &entry);
	}
}


void
FolderWatcher::HandleRemovedEntry(BMessage* message)
{
	node_ref node;
	if (BuildNodeRef(message, &node)) {
		if (fWatchAttrChanges)
			StopAttrWatching(&node);

		fListener->EntryRemoved(&node);
	}
}


void
FolderWatcher::HandleChangedAttr(BMessage* message)
{
	node_ref node;
	if (BuildNodeRef(message, &node))
		fListener->AttributeChanged(&node);
}


void
FolderWatcher::MessageReceived(BMessage* message)
{
	int32 opcode;
	node_ref folder;
	ino_t dir;

	if (message->what == B_NODE_MONITOR) {
		if (fListener == NULL || message->FindInt32("opcode", &opcode) != B_OK)
			return;

		switch (opcode) {
			case B_ENTRY_CREATED:
				HandleCreatedEntry(message, "directory");
				break;

			case B_ENTRY_REMOVED:
				HandleRemovedEntry(message);
				break;

			case B_ENTRY_MOVED:
			{
				fFolder.GetNodeRef(&folder);

				if (message->FindInt64("to directory", &dir) == B_OK
					&& folder.node == dir) {
					// Entry moved into this folder
					HandleCreatedEntry(message, "to directory");
				} else if (message->FindInt64("from directory", &dir) == B_OK
						   && folder.node == dir) {
					// Entry removed from this folder
					HandleRemovedEntry(message);
				}

				break;
			}

			case B_ATTR_CHANGED:
				HandleChangedAttr(message);
				break;

			default:
				break;
		}
	} else
		inherited::MessageReceived(message);
}


status_t
FolderWatcher::StartAttrWatching(node_ref* node)
{
	return watch_node(node, B_WATCH_ATTR, this);
}


status_t
FolderWatcher::StopAttrWatching(node_ref* node)
{
	return watch_node(node, B_STOP_WATCHING, this);
}
