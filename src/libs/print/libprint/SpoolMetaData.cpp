/*
 * Copyright 2003-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "SpoolMetaData.h"


const char* kSDDescription = "_spool/Description";
const char* kSDMimeType = "_spool/MimeType";


SpoolMetaData::SpoolMetaData(BFile* spoolFile)
{
	BString string;
	time_t time;
	if (spoolFile->ReadAttrString(kSDDescription, &string) == B_OK)
		fDescription = string;

	if (spoolFile->ReadAttrString(kSDMimeType, &string) == B_OK)
		fMimeType = string;

	if (spoolFile->GetCreationTime(&time) == B_OK)
		fCreationTime = ctime(&time);
}


SpoolMetaData::~SpoolMetaData()
{
}


const BString&
SpoolMetaData::GetDescription() const
{
	return fDescription;
}


const BString&
SpoolMetaData::GetMimeType() const
{
	return fMimeType;
}


const BString&
SpoolMetaData::GetCreationTime() const
{
	return fCreationTime;
}
