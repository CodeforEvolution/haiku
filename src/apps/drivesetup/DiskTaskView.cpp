/*
 * Copyright 2020, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "DiskTaskView.h"

#include "DiskTaskItem.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "DiskTaskView"


DiskTaskView::DiskTaskView(BRect rect)
	:
	BListView(rect, "DiskTaskView", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL),
	fTasksRunning(0)
{
}


DiskTaskView::~DiskTaskView()
{
	CancelAllTasks();
}


DiskTaskView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_UPDATE_STATUS:
			UpdateProgress(message);
		default:
			BListView::MessageReceived(message);
	}
}


void
DiskTaskView::Draw(BRect updateRect)
{
	BListView::Draw(updateRect);

	if (!AreTasksRunning()) {
		BRect rect = Bounds();
		DrawString(B_TRANSLATE("No tasks are currently running."),
			BPoint(rect.left + 10, rect.top + 10));
	}
}


status_t
DiskTaskView::CreateWriteTask(BString source, BString target,
	BMessenger messenger)
{
	AddItem(new DiskTaskItem(source, target, messenger, B_WRITE_DISK_IMAGE));
	fTasksRunning++;
}


status_t
DiskTaskView::CreateSaveTask(BString source, BString target,
	BMessenger messenger, BString targetFolder, int32 partitionID)
{
	AddItem(new DiskTaskItem(source, target, messenger, B_SAVE_DISK_IMAGE,
		targetFolder, partitionID));
	fTasksRunning++;
}


void
DiskTaskView::CancelAllTasks()
{
	for (int32 index = 0; index < CountItems(); index++) {
		DiskTaskItem* item = (DiskTaskItem*)ItemAt(index);
		if (item != NULL)
			item->CancelTask();
	}
}


void
DiskTaskView::UpdateProgress(BMessage* message)
{
	thread_id ourTask = -1;
	if (message->FindInt32("thread", &ourTask) != B_OK)
		return;

	for (int32 index = 0; index < CountItems(); index++) {
		DiskTaskItem* item = (DiskTaskItem*)ItemAt(index);
		if (item != NULL && item->TaskID() == ourTask) {
			float
			item->SetTaskProgress(
			return;
		}
	}
}
