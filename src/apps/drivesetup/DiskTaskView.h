/*
 * Copyright 2020, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef DISK_TASK_VIEW_H
#define DISK_TASK_VIEW_H


#include <ListView.h>
#include <SupportDefs.h>


class DiskTaskView : public BListView {
public:
								DiskTaskView(BRect rect);
	virtual						~DiskTaskView();

	virtual void				MessageReceived(BMessage* message);

	virtual	void				Draw(BRect updateRect);

			status_t			CreateWriteTask(BString source, BString target,
									BMessenger messenger);
			status_t			CreateSaveTask(BString source, BString target,
									BMessenger messenger, BString targetFolder,
									int32 partitionID);

			bool				AreTasksRunning() { return fTasksRunning != 0; }
			void				CancelAllTasks();

private:
			void				UpdateProgress(BMessage* message);

			int32				fTasksRunning;
};


#endif // DISK_TASK_VIEW_H
