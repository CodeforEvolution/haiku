/*
 * Copyright 2020, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef DISK_TASK_ITEM_H
#define DISK_TASK_ITEM_H


#include <Handler.h>
#include <StringItem.h>
#include <SupportDefs.h>


enum disk_task_type {
	B_WRITE_DISK_IMAGE,
	B_SAVE_DISK_IMAGE
};

class DiskTaskItem : public BStringItem, public BHandler {
public:
								DiskTaskItem(BString source, BString target,
									BMessenger messenger,
									disk_task_type taskType,
									BString targetFolder = NULL,
									int32 partitionID = -1);
	virtual						~DiskTaskItem();

	virtual	void				DrawItem(BView* owner, BRect frame,
									bool complete = false);

			void				SetTaskProgress(float taskProgress);
			void				SetStatusText(BString text);

			thread_id			TaskID() { return fTaskID; }

			status_t			StartTask();
			status_t			CancelTask();

			void				TaskCompleted(void* data);

private:
			bool				fTaskActive;
			thread_id			fTaskID;
			float				fTaskProgress;
			bool				fTimeToBeCanceled;

			BString				fSourceDisk;
			BString				fTargetDisk;
			BMessenger			fMessenger;
			disk_task_type		fDiskTaskType;
			BString				fTargetFolder;
			int32				fPartitionID;

			BBitmap*			fCancelBitmap;
};


#endif // DISK_TASK_ITEM_H
