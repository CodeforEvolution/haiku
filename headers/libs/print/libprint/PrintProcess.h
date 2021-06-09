/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _PRINT_PROCESS_H
#define _PRINT_PROCESS_H


#include <list>
#include <memory>
#include <vector>

#include <Point.h>
#include <Rect.h>


using namespace std;


class BFile;
class BPicture;


class PictureData {
public:
								PictureData(BFile* file);
								~PictureData();

public:
			BPoint				point;
			BRect				rect;
			BPicture*			picture;
};


class PageData {
public:
								PageData();
								PageData(BFile* file, bool reverse);

			bool				StartEnum();
			bool				EnumObject(PictureData** picture_data);

private:
			BFile*				fFile;
			bool				fReverse;
			int32				fPictureCount;
			int32				fRest;
			off_t				fOffset;
			bool				fHollow;
};


typedef list<PageData*>	PageDataList;


class SpoolData {
public:
								SpoolData(BFile* file, int32 page_count,
									int32 nup, bool reverse);
								~SpoolData();

			bool				StartEnum();
			bool				EnumObject(PageData** page_data);

private:
			PageDataList		fPages;
			PageDataList::iterator	fIt;
};

#endif	/* _PRINT_PROCESS_H */
