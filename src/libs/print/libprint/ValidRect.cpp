/*
 * Copyright 1999-2000 Y.Takagi
 * Copyright 2005 Michael Pfeiffer
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "ValidRect.h"

#include <Bitmap.h>


class BoundsCalculator {
public:
	bool GetValidRect(BBitmap* bitmap, clipping_rect* rect);

private:
	inline bool _IsEmpty(const rgb_color* pixel);

	inline bool _IsRowEmpty(const rgb_color* row);

	inline const uchar* _GetRow(int x, int y);

	int _GetTop();

	int _GetBottom();

	inline void _UpdateLeftBound(const rgb_color* row);
	inline void _UpdateRightBound(const rgb_color* row);

private:
	const uchar* fBits;
	int fBPR;

	int fLeft;
	int fRight;
	int fTop;
	int fBottom;

	int fWidth;
	int fLeftBound;
	int fRightBound;
};


bool
BoundsCalculator::_IsEmpty(const rgb_color* pixel)
{
	return pixel->red == 0xff && pixel->green == 0xff && pixel->blue == 0xff;
}


bool
BoundsCalculator::_IsRowEmpty(const rgb_color* row)
{
	for (int x = 0; x < fWidth; x++) {
		if (!_IsEmpty(row))
			return false;

		row ++;
	}

	return true;
}


const uchar*
BoundsCalculator::_GetRow(int x, int y)
{
	return fBits + x + fBPR * y;
}


int
BoundsCalculator::_GetTop()
{
	const uchar* row = _GetRow(fLeft, fTop);

	int top;
	for (top = fTop; top <= fBottom; top++) {
		if (!_IsRowEmpty((const rgb_color*)row))
			break;

		row += fBPR;
	}

	return top;
}


int
BoundsCalculator::_GetBottom()
{
	const uchar* row = _GetRow(fLeft, fBottom);

	int bottom;
	for (bottom = fBottom; bottom >= fTop; bottom--) {
		if (!_IsRowEmpty((const rgb_color*)row))
			break;

		row -= fBPR;
	}

	return bottom;
}


void
BoundsCalculator::_UpdateLeftBound(const rgb_color* row)
{
	for (int x = fLeft; x < fLeftBound; x++) {
		if (!_IsEmpty(row)) {
			fLeftBound = x;
			return;
		}
		row ++;
	}
}


void
BoundsCalculator::_UpdateRightBound(const rgb_color* row)
{
	row += fWidth - 1;
	for (int x = fRight; x > fRightBound; x--) {
		if (!_IsEmpty(row)) {
			fRightBound = x;
			return;
		}

		row --;
	}
}


// returns false if the bitmap is empty or has wrong color space.
bool
BoundsCalculator::GetValidRect(BBitmap* bitmap, clipping_rect* rect)
{
	enum {
		kRectIsInvalid = false,
		kRectIsEmpty = false,
		kRectIsValid = true
	};

	switch (bitmap->ColorSpace()) {
		case B_RGB32:
		case B_RGB32_BIG:
			break;

		default:
			return kRectIsInvalid;
	};

	// Initialize member variables
	fBits = (uchar*)bitmap->Bits();
	fBPR  = bitmap->BytesPerRow();

	fLeft = rect->left;
	fRight = rect->right;
	fTop = rect->top;
	fBottom = rect->bottom;

	fWidth = fRight - fLeft + 1;

	// Get top bound
	fTop = _GetTop();
	if (fTop > fBottom)
		return kRectIsEmpty;

	// Get bottom bound
	fBottom = _GetBottom();

	// calculate left and right bounds
	fLeftBound = fRight + 1;
	fRightBound = fLeft - 1;

	const uchar* row = _GetRow(fLeft, fTop);
	for (int y = fTop; y <= fBottom; y++) {
		_UpdateLeftBound((const rgb_color*)row);
		_UpdateRightBound((const rgb_color*)row);

		if (fLeft == fLeftBound && fRight == fRightBound)
			break;

		row += fBPR;
	}

	// Return bounds in rectangle
	rect->left = fLeftBound;
	rect->right = fRightBound;
	rect->top = fTop;
	rect->bottom = fBottom;

	return kRectIsValid;
}


bool
get_valid_rect(BBitmap* bitmap, clipping_rect* rect)
{
	BoundsCalculator calculator;
	return calculator.GetValidRect(bitmap, rect);
}


int
color_space2pixel_depth(color_space cs)
{
	int pixel_depth;

	switch (cs) {
		case B_GRAY1: /* Y0[0],Y1[0],Y2[0],Y3[0],Y4[0],Y5[0],Y6[0],Y7[0] */
			pixel_depth = 1;
			break;

		case B_GRAY8: /* Y[7:0] */
		case B_CMAP8: /* D[7:0] */
			pixel_depth = 8;
			break;

		case B_RGB15: /* G[2:0],B[4:0] -[0],R[4:0],G[4:3] */
		case B_RGB15_BIG: /* -[0],R[4:0],G[4:3] G[2:0],B[4:0] */
			pixel_depth = 16;
			break;

		case B_RGB32: /* B[7:0] G[7:0] R[7:0] -[7:0] */
		case B_RGB32_BIG: /* -[7:0] R[7:0] G[7:0] B[7:0]	*/
			pixel_depth = 32;
			break;

		default:
			pixel_depth = 0;
	}

	return pixel_depth;
}
