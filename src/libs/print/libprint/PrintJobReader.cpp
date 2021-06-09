/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PrintJobReader.h"

#include <stdio.h>

#include <Picture.h>
#include <PrintJob.h>


// #pragma mark --- PrintJobPage

PrintJobPage::PrintJobPage()
	:
	fNextPicture(-1),
	fNumberOfPictures(0),
	fPicture(0),
	fStatus(B_ERROR)
{
}


PrintJobPage::PrintJobPage(const PrintJobPage& copy)
	:
	fJobFile(copy.fJobFile),
	fNextPicture(copy.fNextPicture),
	fNumberOfPictures(copy.fNumberOfPictures),
	fPicture(copy.fPicture),
	fStatus(copy.fStatus)
{
}


PrintJobPage&
PrintJobPage::operator=(const PrintJobPage& copy)
{
	if (this != &copy) {
		fJobFile = copy.fJobFile;
		fNextPicture = copy.fNextPicture;
		fNumberOfPictures = copy.fNumberOfPictures;
		fPicture = copy.fPicture;
		fStatus = copy.fStatus;
	}

	return *this;
}


PrintJobPage::PrintJobPage(BFile* jobFile, off_t start)
	:
	fJobFile(*jobFile),
	fPicture(0),
	fStatus(B_ERROR)
{
	off_t size;
	if (fJobFile.GetSize(&size) != B_OK || start > size)
		return;

	if (fJobFile.Seek(start, SEEK_SET) != start)
		return;

	if (fJobFile.Read(&fNumberOfPictures, sizeof(int32)) == sizeof(int32)) {
		// (sizeof(int32) * 10) == padding in _page_header_
		fJobFile.Seek(sizeof(off_t) + sizeof(int32) * 10, SEEK_CUR);
		fNextPicture = fJobFile.Position();
		fStatus = B_OK;
	}
}


status_t
PrintJobPage::InitCheck() const
{
	return fStatus;
}


status_t
PrintJobPage::NextPicture(BPicture& picture, BPoint& point, BRect& rect)
{
	if (fPicture >= fNumberOfPictures)
		return B_ERROR;
	fPicture++;

	fJobFile.Seek(fNextPicture, SEEK_SET);
	fJobFile.Read(&point, sizeof(BPoint));
	fJobFile.Read(&rect, sizeof(BRect));
	status_t rc = picture.Unflatten(&fJobFile);
	fNextPicture = fJobFile.Position();

	if (rc != B_OK)
		fPicture = fNumberOfPictures;

	return rc;
}


// # pragma mark --- PrintJobReader


PrintJobReader::PrintJobReader(BFile* jobFile)
	:
	fJobFile(*jobFile),
	fNumberOfPages(-1),
	fPageIndex(NULL)
{
	print_file_header header;
	fJobFile.Seek(0, SEEK_SET);
	if (fJobFile.Read(&header, sizeof(header)) == sizeof(header)) {
		if (fJobSettings.Unflatten(&fJobFile) == B_OK) {
			fNumberOfPages = header.page_count;
			fPageIndex = new off_t[fNumberOfPages];

			_BuildPageIndex();
		}
	}
}


PrintJobReader::~PrintJobReader()
{
	delete[] fPageIndex;
}


status_t
PrintJobReader::InitCheck() const
{
	return fNumberOfPages > 0 ? B_OK : B_ERROR;
}


void
PrintJobReader::_BuildPageIndex()
{
	off_t next_page;
	int32 number_of_pictures;
	for (int32 page = 0; page < fNumberOfPages; ++page) {
		fPageIndex[page] = fJobFile.Position();
		if (fJobFile.Read(&number_of_pictures, sizeof(int32)) == sizeof(int32)
			&& fJobFile.Read(&next_page, sizeof(off_t)) == sizeof(off_t)
			&& fPageIndex[page] < next_page) {
			fJobFile.Seek(next_page, SEEK_SET);
		} else {
			fNumberOfPages = 0;
			delete[] fPageIndex;
			fPageIndex = NULL;
			return;
		}
	}
}


status_t
PrintJobReader::GetPage(int32 page, PrintJobPage& pjp)
{
	if (0 <= page && page < fNumberOfPages) {
		PrintJobPage p(&fJobFile, fPageIndex[page]);
		if (p.InitCheck() == B_OK) {
			pjp = p;
			return B_OK;
		}
	}

	return B_ERROR;
}


int32
PrintJobReader::FirstPage() const
{
	int32 firstPage = -1;
	fJobSettings.FindInt32("first_page", &firstPage);

	return firstPage;
}


int32
PrintJobReader::LastPage() const
{
	int32 lastPage = -1;
	fJobSettings.FindInt32("last_page", &lastPage);

	return lastPage;
}


BRect
PrintJobReader::PaperRect() const
{
	BRect rect;
	fJobSettings.FindRect("paper_rect", &rect);

	return rect;
}


BRect
PrintJobReader::PrintableRect() const
{
	BRect rect;
	fJobSettings.FindRect("printable_rect", &rect);

	return rect;
}


void
PrintJobReader::GetResolution(int32* xdpi, int32* ydpi) const
{
	fJobSettings.FindInt32("xres", xdpi);
	fJobSettings.FindInt32("yres", ydpi);
}


float
PrintJobReader::GetScale() const
{
	float scale = 1.0;
	fJobSettings.FindFloat("scale", &scale);

	return scale;
}
