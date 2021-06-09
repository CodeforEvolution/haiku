/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _VALID_RECT_H
#define _VALID_RECT_H


#include <GraphicsDefs.h>
#include <Rect.h>
#include <Region.h>


class BBitmap;


bool get_valid_rect(BBitmap* bitmap, clipping_rect* rect);

int color_space2pixel_depth(color_space cs);

#endif	// _VALID_RECT_H
