/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "Template.h"


void
Template::Op(int number)
{
}


void
Template::MovePenBy(BPoint delta)
{
}


void
Template::StrokeLine(BPoint start, BPoint end)
{
}


void
Template::StrokeRect(BRect rect)
{
}


void
Template::FillRect(BRect rect)
{
}


void
Template::StrokeRoundRect(BRect rect, BPoint radii)
{
}


void
Template::FillRoundRect(BRect rect, BPoint radii)
{
}


void
Template::StrokeBezier(BPoint* control)
{
}


void
Template::FillBezier(BPoint* control)
{
}


void
Template::StrokeArc(BPoint center, BPoint radii, float startTheta,
	float arcTheta)
{
}


void
Template::FillArc(BPoint center, BPoint radii, float startTheta, float arcTheta)
{
}


void
Template::StrokeEllipse(BPoint center, BPoint radii)
{
}


void
Template::FillEllipse(BPoint center, BPoint radii)
{
}


void
Template::StrokePolygon(int32 numPoints, BPoint* points, bool isClosed)
{
}


void
Template::FillPolygon(int32 numPoints, BPoint* points, bool isClosed)
{
}


void
Template::StrokeShape(BShape* shape)
{
}


void
Template::FillShape(BShape* shape)
{
}


void
Template::DrawString(char* string, float escapement_nospace,
	float escapement_space)
{
}


void
Template::DrawPixels(BRect src, BRect dest, int32 width, int32 height,
	int32 bytesPerRow, int32 pixelFormat, int32 flags, void* data)
{
}


void
Template::SetClippingRects(BRect* rects, uint32 numRects)
{
}


void
Template::ClipToPicture(BPicture* picture, BPoint point,
	bool clip_to_inverse_picture)
{
}


void
Template::PushState()
{
}


void
Template::PopState()
{
}


void
Template::EnterStateChange()
{
}


void
Template::ExitStateChange()
{
}


void
Template::EnterFontState()
{
}


void
Template::ExitFontState()
{
}


void
Template::SetOrigin(BPoint pt)
{
}


void
Template::SetPenLocation(BPoint pt)
{
}


void
Template::SetDrawingMode(drawing_mode mode)
{
}


void
Template::SetLineMode(cap_mode capMode, join_mode joinMode, float miterLimit)
{
}


void
Template::SetPenSize(float size)
{
}


void
Template::SetForeColor(rgb_color color)
{
}


void
Template::SetBackColor(rgb_color color)
{
}


void
Template::SetStipplePattern(pattern p)
{
}


void
Template::SetScale(float scale)
{
}


void
Template::SetFontFamily(char* family)
{
}


void
Template::SetFontStyle(char* style)
{
}


void
Template::SetFontSpacing(int32 spacing)
{
}


void
Template::SetFontSize(float size)
{
}


void
Template::SetFontRotate(float rotation)
{
}


void
Template::SetFontEncoding(int32 encoding)
{
}


void
Template::SetFontFlags(int32 flags)
{
}


void
Template::SetFontShear(float shear)
{
}


void
Template::SetFontFace(int32 flags)
{
}
