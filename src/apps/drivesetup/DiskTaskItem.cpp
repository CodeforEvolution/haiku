/*
 * Copyright 2020, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "DiskTaskItem.h"

#include <Button.h>
#include <String.h>


DiskTaskItem::DiskTaskItem(BString source, BString target, BMessenger messenger,
	disk_task_type taskType, BString targetFolder, int32 partitionID)
	:
	BStringItem(""),
	fSourceDisk(source),
	fTargetDisk(target),
	fMessenger(messenger),
	fDiskTaskType(taskType),
	fTargetFolder(targetFolder),
	fPartitionID(partitionID),
	fTaskActive(false),
	fTaskID(-1),
	fTaskProgress(0),
	fTimeToBeCanceled(false)
{
	InitTask();
}


DiskTaskItem::~DiskTaskItem()
{
	CancelTask();
}


DiskTaskItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	if (fTimeToBeCanceled) {
		owner->SetHighUIColor(B_FAILURE_COLOR);
		owner->FillRect(frame);
	} else if (fTaskProgress > 0 && fTaskProgress <= 100) {
		owner->SetHighUIColor(B_SUCCESS_COLOR);
		owner->FillRect(BRect(frame.left, frame.top,
			(frame.right / 100) * fTaskProgress, frame.bottom));
	}

	BStringItem::DrawItem(owner, frame, complete);
}


DiskTaskItem::SetTaskProgress(float taskProgress)
{
	fTaskProgress = taskProgress;
	Invalidate();
}


void
DiskTaskItem::InitTask()
{
		case MSG_WRITE:
		{
			entry_ref entryRef;
			message->FindRef("refs", &entryRef);
			BEntry entry(&entryRef);
			BPath path;
			if (entry.GetPath(&path) == B_OK) {
				BMessage* msg = new BMessage();

				PartitionListRow* row = dynamic_cast<PartitionListRow*>(
					fListView->CurrentSelection());

				msg->AddString("source", path.Path());
				msg->AddString("target", row->DevicePath());
				msg->AddMessenger("messenger", BMessenger(this));

				fStatusView->MoveBy(0, -kStatusViewHeight);
				fListView->ResizeBy(0, -kStatusViewHeight);

				thread_id write_thread = spawn_thread(WriteDiskImage,
					"WriteDiskImage", B_LOW_PRIORITY, (void*)msg);
				resume_thread(write_thread);
			} else {
				if (fWriteImageFilePanel == NULL) {
					fWriteImageFilePanel = new(std::nothrow) BFilePanel(
						B_OPEN_PANEL, new BMessenger(this), NULL, B_FILE_NODE,
						false, new BMessage(MSG_WRITE), NULL, true);
				}
				if (fWriteImageFilePanel != NULL)
					fWriteImageFilePanel->Show();
			}
			break;
		}
		case MSG_SAVE:
		{
			PartitionListRow* row = dynamic_cast<PartitionListRow*>(
				fListView->CurrentSelection());

			if (fReadImageFilePanel == NULL) {
				fReadImageFilePanel = new(std::nothrow) BFilePanel(B_SAVE_PANEL,
					new BMessenger(this), NULL, B_FILE_NODE, false, NULL, NULL,
					true);
			}

			if (fReadImageFilePanel != NULL) {
				BStringField* field = (BStringField*)row->GetField(2);
				fReadImageFilePanel->SetSaveText(field->String());
				fReadImageFilePanel->Show();
			}
			break;
		}
		case B_SAVE_REQUESTED:
		{
			BMessage* msg = new BMessage();

			entry_ref entryRef;
			message->FindRef("directory", &entryRef);
			BEntry entry(&entryRef);
			BPath path;
			entry.GetPath(&path);
			msg->AddString("targetfolder", path.Path());
			const char* name;
			message->FindString("name", &name);
			path.Append(name);

			PartitionListRow* row = dynamic_cast<PartitionListRow*>(
				fListView->CurrentSelection());

			msg->AddString("source", row->DevicePath());
			msg->AddString("target", path.Path());
			msg->AddInt32("partitionID", row->ID());
			msg->AddMessenger("messenger", BMessenger(this));

			// TODO handle multiple concurrent writes with separate status views
			fStatusView->MoveBy(0, -kStatusViewHeight);
			fListView->ResizeBy(0, -kStatusViewHeight);

			thread_id save_thread = spawn_thread(SaveDiskImage, "SaveDiskImage",
				B_LOW_PRIORITY, (void*)msg);
			resume_thread(save_thread);

			break;
		}
		case MSG_UPDATE_STATUS:
		{
			float maximumvalue, currentvalue, delta;
			const char* statustext = "";
			BString statusprogress;

			message->FindFloat("maximumvalue", &maximumvalue);
			message->FindFloat("currentvalue", &currentvalue);
			message->FindFloat("delta", &delta);
			message->FindString("statustext", &statustext);

			statusprogress << (currentvalue / (1024 * 1024)) << " "
				<< B_TRANSLATE("MiB") << " / ";
			statusprogress << (maximumvalue / (1024 * 1024)) << " "
				<< B_TRANSLATE("MiB");

			fStatusBar->SetMaxValue(maximumvalue);
			fStatusBar->Update(delta);
			fStatusBar->SetText(statustext);
			fStatusBar->SetTrailingText(statusprogress);

			break;
		}
		case MSG_COMPLETE:
		{
			fStatusBar->Reset();
			fStatusView->MoveBy(0, kStatusViewHeight);
			fListView->ResizeBy(0, kStatusViewHeight);

			const char* status = "";

			message->FindString("status", &status);

			fNotification->SetTitle(BString(B_TRANSLATE("Disk image")));
			fNotification->SetContent(BString(status));
			fNotification->Send();

			break;
		}
}


status_t
DiskTaskItem::StartTask()
{
	switch (fDiskTaskType) {
		case B_WRITE_DISK_IMAGE:

			break;
		case B_SAVE_DISK_IMAGE:

			break;
		default:
			break;
}


status_t
DiskTaskItem::CancelTask()
{
	fTimeToBeCanceled = true;
}


int32
DiskTaskItem::WriteDiskImage(void* data)
{
	BMessage* msg = (BMessage*)data;
	const char* sourcepath;
	const char* targetpath;
	BMessenger messenger;

	msg->FindString("source", &sourcepath);
	msg->FindString("target", &targetpath);
	msg->FindMessenger("messenger", &messenger);

	BFile source(sourcepath, B_READ_ONLY);
	BFile target(targetpath, B_READ_WRITE);

	if (target.InitCheck() != B_OK) {
		FileErrorAlert(B_TRANSLATE("Cannot init target partition."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	BPartition* partition;
	BDiskDevice device;

	BDiskDeviceRoster().GetPartitionForPath(targetpath, &device, &partition);

	if (partition->IsReadOnly()) {
		FileErrorAlert(B_TRANSLATE("Target partition is read only."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	off_t size;
	source.GetSize(&size);

	if (partition->Size() < size) {
		FileErrorAlert(B_TRANSLATE("The target partition is smaller than the "
			"image file."), NULL, B_STOP_ALERT);
		return -1;
	}

	size_t bufsize = partition->BlockSize();
	off_t sourcesize;
	ssize_t targetbytes = 0;

	source.GetSize(&sourcesize);
	BMessage message(MSG_UPDATE_STATUS);
	message.AddFloat("maximumvalue", (float)sourcesize);
	message.AddFloat("currentvalue", 0);
	message.AddFloat("delta", 0);
	message.AddString("statustext", "");
	message.AddInt32("thread", fTaskID);

	BString statustext;
	statustext << B_TRANSLATE("Writing image to disk") << ": " << targetpath;

	char* buffer = new char[bufsize];
	while (!fTimeToBeCanceled) {
		ssize_t bytes = source.Read(buffer, bufsize);

		if (bytes > 0) {
			target.Write(buffer, (size_t)bytes);

			targetbytes += bytes;

			message.SetFloat("currentvalue", (float)targetbytes);
			message.SetFloat("delta", (float)bytes);
			message.SetString("statustext", statustext);
			messenger.SendMessage(&message);
		} else
			break;
	}
	delete[] buffer;

	message = BMessage(MSG_COMPLETE);
	message.AddString("status",
		B_TRANSLATE("Disk image successfully written to the target."));
	messenger.SendMessage(&message);

	return 0;
}


int32
DiskTaskItem::SaveDiskImage(void* data)
{
	BMessage* msg = (BMessage*)data;
	const char* sourcepath;
	const char* targetpath;
	const char* targetfolder;
	int32 partitionID;
	BMessenger messenger;

	msg->FindString("source", &sourcepath);
	msg->FindString("target", &targetpath);
	msg->FindString("targetfolder", &targetfolder);
	msg->FindInt32("partitionID", &partitionID);
	msg->FindMessenger("messenger", &messenger);

	BFile source(sourcepath, B_READ_ONLY);
	BFile target(targetpath, B_READ_WRITE | B_CREATE_FILE);

	if (source.InitCheck() != B_OK) {
		FileErrorAlert(B_TRANSLATE("Cannot init source volume."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	if (target.InitCheck() != B_OK) {
		FileErrorAlert(B_TRANSLATE("Cannot init target file."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	BDirectory* targetdir = new BDirectory(targetfolder);
	node_ref node;
	targetdir->GetNodeRef(&node);
	BVolume volume(node.device);

	if (volume.InitCheck()) {
		FileErrorAlert(B_TRANSLATE("Cannot init target volume."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	if (volume.IsReadOnly()) {
		FileErrorAlert(B_TRANSLATE("Target volume is read only."), NULL,
			B_STOP_ALERT);
		return -1;
	}

	BPartition *partition;
	BDiskDevice device;

	BDiskDeviceRoster().GetPartitionWithID(partitionID, &device, &partition);

	if (volume.FreeBytes() < partition->Size()) {
		FileErrorAlert(B_TRANSLATE("There is not enough free space on target "
			"device."), NULL, B_STOP_ALERT);
		return -1;
	}

	size_t bufsize = partition->BlockSize();
	off_t sourcesize, targetsize;
	ssize_t targetbytes = 0;

	source.GetSize(&sourcesize);
	BMessage message(MSG_UPDATE_STATUS);
	message.AddFloat("maximumvalue", (float)sourcesize);
	message.AddFloat("currentvalue", 0);
	message.AddFloat("delta", 0);
	message.AddString("statustext", "");
	message.AddInt32("thread", fTaskID);

	BString statustext;
	statustext << B_TRANSLATE("Creating disk image") << ": " << targetpath;

	char* buffer = new char[bufsize];
	while (!fTimeToBeCanceled) {
		ssize_t bytes = source.Read(buffer, bufsize);

		if (bytes > 0) {
			target.Write(buffer, (size_t)bytes);
			target.GetSize(&targetsize);

			targetbytes += bytes;

			message.SetFloat("currentvalue", (float)targetbytes);
			message.SetFloat("delta", (float)bytes);
			message.SetString("statustext", statustext);
			messenger.SendMessage(&message);
		} else
			break;
	}

	if (!fTimeToBeCanceled) {
		message = BMessage(MSG_COMPLETE);
		message.AddString("status",
			B_TRANSLATE("Disk image successfully created."));
	} else {
		message = BMessage(MSG_CANCELED);
		message.AddString("status",
			B_TRANSLATE("Disk image creation was canceled."));
	}

	messenger.SendMessage(&message);

	return 0;
}


void
DiskTaskItem::TaskCompleted(void* data)
{

}


