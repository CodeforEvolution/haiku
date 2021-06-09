/*
 * Copyright 2003-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _SPOOL_METADATA_H
#define _SPOOL_METADATA_H


#include <File.h>
#include <String.h>
#include <SupportDefs.h>


class SpoolMetaData {
public:
								SpoolMetaData(BFile* spoolFile);
								~SpoolMetaData();

	const	BString&			GetDescription() const;
	const	BString&			GetMimeType() const;
	const	BString&			GetCreationTime() const;

private:
			BString				fDescription;
			BString				fMimeType;
			BString				fCreationTime;
};

#endif	/* _SPOOL_METADATA_H */
