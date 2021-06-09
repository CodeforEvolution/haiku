/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINT_JOB_READER_H
#define _PRINT_JOB_READER_H


#include <File.h>
#include <Message.h>
#include <Picture.h>


class PrintJobPage {
public:
								PrintJobPage();
								PrintJobPage(const PrintJobPage& copy);
								PrintJobPage(BFile* jobFile, off_t start);

			PrintJobPage&		operator=(const PrintJobPage& copy);

			status_t			InitCheck() const;

			int32				NumberOfPictures() const
									{ return fNumberOfPictures; }

			status_t			NextPicture(BPicture& picture, BPoint& point,
									BRect& rect);



private:
			BFile				fJobFile;
									// The job file
			off_t				fNextPicture;
									// Offset to first picture
			int32				fNumberOfPictures;
									// Of this page
			int32				fPicture;
									// The picture returned by NextPicture()
			status_t			fStatus;
};


class PrintJobReader {
public:
								PrintJobReader(BFile* jobFile);
	virtual						~PrintJobReader();

			// Test after construction if this is a valid job file
			status_t			InitCheck() const;

			// Accessors to informations from job file
			int32				NumberOfPages() const { return fNumberOfPages; }
			int32				FirstPage() const;
			int32				LastPage() const;
	const	BMessage*			JobSettings() const { return &fJobSettings; }
			BRect				PaperRect() const;
			BRect				PrintableRect() const;
			void				GetResolution(int32* xdpi, int32* ydpi) const;
			float				GetScale() const;

			// Retrieve page
			status_t			GetPage(int32 page, PrintJobPage& pjp);

private:
			void				_BuildPageIndex();

private:
			BFile				fJobFile;
									// The job file
			int32				fNumberOfPages;
									// The number of pages in the job file
			BMessage			fJobSettings;
									// The settings extracted from the job file
			off_t*				fPageIndex;
									// Start positions of pages in the job file
};

#endif /* _PRINT_JOB_READER_H */
