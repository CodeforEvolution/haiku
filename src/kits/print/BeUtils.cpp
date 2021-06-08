/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 *		Michael Pfeiffer
 */


#include "BeUtils.h"

#include <Application.h>
#include <Bitmap.h>
#include <Messenger.h>
#include <Resources.h>
#include <Roster.h>
#include <String.h>


// ---------------------------------------------------------------
// TestForAddonExistence
//
// [Method Description]
//
// Parameters:
//
// Returns:
// ---------------------------------------------------------------
status_t
TestForAddonExistence(const char* name, directory_which which,
	const char* section, BPath& outPath)
{
	status_t error = B_ERROR;

	error = find_directory(which, &outPath);
	if (error != B_OK)
		return error;

	outPath.Append(section);
	outPath.Append(name);

	struct stat buf;
	error = stat(outPath.Path(), &buf);

	return error;
}


// Implementation of AutoReply

AutoReply::AutoReply(BMessage* sender, uint32 what)
	:
	fSender(sender),
	fReply(what)
{
}


AutoReply::~AutoReply()
{
	if (fSender != NULL) {
		fSender->SendReply(&fReply);
		delete fSender;
	}
}


bool
MimeTypeForSender(BMessage* sender, BString& mime)
{
	if (sender == NULL)
		return false;

	BMessenger messenger = sender->ReturnAddress();
	team_id team = messenger.Team();

	app_info info;
	if (be_roster->GetRunningAppInfo(team, &info) == B_OK) {
		mime = info.signature;
		return true;
	}

	return false;
}
