/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PicturePrinter.h"

#include <stdio.h>


PicturePrinter::PicturePrinter(int indent)
	:
	fIndent(indent)
 {
 }


void
PicturePrinter::_Print(const char* text)
{
	printf("%s ", text);
}


void
PicturePrinter::_Print(BPoint* p)
{
	printf("point (%f, %f) ", p->x, p->y);
}


void
PicturePrinter::_Print(BRect* r)
{
	printf("rect [l: %f, t: %f, r: %f, b: %f] ", r->left, r->top, r->right,
		r->bottom);
}


void
PicturePrinter::_Print(int numPoints, BPoint* points)
{
	for (int i = 0; i < numPoints; i++) {
		_Indent(1);
		printf("%d ", i);
		_Print(&points[i]);
		_Cr();
	}
}


void
PicturePrinter::_Print(int numRects, BRect* rects)
{
	for (int i = 0; i < numRects; i++) {
		_Indent(1);
		printf("%d ", i);
		_Print(&rects[i]);
		_Cr();
	}
}


void
PicturePrinter::_Print(BShape* shape)
{
	printf("Shape %p\n", shape);
	ShapePrinter printer(this);
	printer.Iterate(shape);
}


void
PicturePrinter::_Print(const char* text, float f)
{
	printf("%s %f ", text, f);
}


void
PicturePrinter::_Print(const char* text, BPoint* point)
{
	_Print(text);
	_Print(point);
}


void
PicturePrinter::_Print(rgb_color color)
{
	printf("color r: %d g: %d b: %d", color.red, color.green, color.blue);
}


void
PicturePrinter::_Print(float f)
{
	printf("%f ", f);
}


void
PicturePrinter::_Cr()
{
	printf("\n");
}


void
PicturePrinter::_Indent(int inc)
{
	for (int i = fIndent + inc; i > 0; i--)
		printf("  ");
}


void
PicturePrinter::_IncIndent()
{
	fIndent++;
}


void
PicturePrinter::_DecIndent()
{
	fIndent--;
}


void
PicturePrinter::Op(int number)
{
	_Indent();
	printf("Unknown operator %d\n", number);
	_Cr();
}


void
PicturePrinter::MovePenBy(BPoint delta)
{
	_Indent();
	_Print("MovePenBy");
	_Print(&delta);
	_Cr();
}


void
PicturePrinter::StrokeLine(BPoint start, BPoint end)
{
	_Indent();
	_Print("StrokeLine");
	_Print(&start);
	_Print(&end);
	_Cr();
}


void
PicturePrinter::StrokeRect(BRect rect)
{
	_Indent();
	_Print("StrokeRect");
	_Print(&rect);
	_Cr();
}


void
PicturePrinter::FillRect(BRect rect)
{
	_Indent();
	_Print("FillRect");
	_Print(&rect);
	_Cr();
}


void
PicturePrinter::StrokeRoundRect(BRect rect, BPoint radii)
{
	_Indent();
	_Print("StrokeRoundRect");
	_Print(&rect);
	_Print("radii", &radii);
	_Cr();
}


void
PicturePrinter::FillRoundRect(BRect rect, BPoint radii)
{
	_Indent();
	_Print("FillRoundRect");
	_Print(&rect);
	_Print("radii", &radii);
	_Cr();
}


void
PicturePrinter::StrokeBezier(BPoint* control)
{
	_Indent();
	_Print("StrokeBezier");
	_Print(4, control);
	_Cr();
}


void
PicturePrinter::FillBezier(BPoint* control)
{
	_Indent();
	_Print("FillBezier");
	_Print(4, control);
	_Cr();
}


void
PicturePrinter::StrokeArc(BPoint center, BPoint radii, float startTheta,
	float arcTheta)
{
	_Indent();
	_Print("StrokeArc center=");
	_Print(&center);
	_Print("radii=");
	_Print(&radii);
	_Print("arcTheta=", arcTheta);
	_Cr();
}


void
PicturePrinter::FillArc(BPoint center, BPoint radii, float startTheta,
	float arcTheta)
{
	_Indent();
	_Print("FillArc center=");
	_Print(&center);
	_Print("radii=");
	_Print(&radii);
	_Print("arcTheta=", arcTheta);
	_Cr();
}


void
PicturePrinter::StrokeEllipse(BPoint center, BPoint radii)
{
	_Indent();
	_Print("StrokeEllipse center=");
	_Print(&center);
	_Print("radii=");
	_Print(&radii);
	_Cr();
}


void
PicturePrinter::FillEllipse(BPoint center, BPoint radii)
{
	_Indent();
	_Print("FillEllipse center=");
	_Print(&center);
	_Print("radii=");
	_Print(&radii);
	_Cr();
}


void
PicturePrinter::StrokePolygon(int32 numPoints, BPoint* points, bool isClosed)
{
	_Indent();
	_Print("StrokePolygon");
	printf("%s ", isClosed ? "closed" : "open");
	_Cr();
	_Print(numPoints, points);
}


void
PicturePrinter::FillPolygon(int32 numPoints, BPoint* points, bool isClosed)
{
	_Indent();
	_Print("FillPolygon");
	printf("%s ", isClosed ? "closed" : "open");
	_Cr();
	_Print(numPoints, points);
}


void
PicturePrinter::StrokeShape(BShape* shape)
{
	_Indent();
	_Print("StrokeShape");
	_Print(shape);
	_Cr();
}


void
PicturePrinter::FillShape(BShape* shape)
{
	_Indent();
	_Print("FillShape");
	_Print(shape);
	_Cr();
}


void
PicturePrinter::DrawString(char* string, float escapement_nospace,
	float escapement_space)
{
	_Indent();
	_Print("DrawString");
	_Print("escapement_nospace", escapement_nospace);
	_Print("escapement_space", escapement_space);
	_Print("text:");
	_Print(string);
	_Cr();
}


void
PicturePrinter::DrawPixels(BRect src, BRect dest, int32 width, int32 height,
	int32 bytesPerRow, int32 pixelFormat, int32 flags, void* data)
{
	_Indent();
	_Print("DrawPixels");
	_Cr();
}


void
PicturePrinter::SetClippingRects(BRect* rects, uint32 numRects)
{
	_Indent();
	_Print("SetClippingRects");
	if (numRects == 0)
		_Print("none");
	_Cr();
	_Print(numRects, rects);
}


void
PicturePrinter::ClipToPicture(BPicture* picture, BPoint point,
	bool clip_to_inverse_picture)
{
	_Indent();
	_Print(clip_to_inverse_picture ? "ClipToInversePicture" : "ClipToPicture");
	_Print("point=", &point);
	_Cr();

	PicturePrinter printer(fIndent + 1);
	printer.Iterate(picture);
}


void
PicturePrinter::PushState()
{
	_Indent();
	_Print("PushState");
	_Cr();

	_IncIndent();
}


void
PicturePrinter::PopState()
{
	_DecIndent();

	_Indent();
	_Print("PopState");
	_Cr();
}


void
PicturePrinter::EnterStateChange()
{
	_Indent();
	_Print("EnterStateChange");
	_Cr();
}


void
PicturePrinter::ExitStateChange()
{
	_Indent();
	_Print("ExitStateChange");
	_Cr();
}


void
PicturePrinter::EnterFontState()
{
	_Indent();
	_Print("EnterFontState");
	_Cr();
}


void
PicturePrinter::ExitFontState()
{
	_Indent();
	_Print("ExitFontState");
	_Cr();
}


void
PicturePrinter::SetOrigin(BPoint pt)
{
	_Indent();
	_Print("SetOrigin");
	_Print(&pt);
	_Cr();
}


void
PicturePrinter::SetPenLocation(BPoint pt)
{
	_Indent();
	_Print("SetPenLocation");
	_Print(&pt);
	_Cr();
}


void
PicturePrinter::SetDrawingMode(drawing_mode mode)
{
	_Indent();
	_Print("SetDrawingMode");

	switch (mode) {
		case B_OP_COPY:
			_Print("B_OP_COPY");
			break;

		case B_OP_OVER:
			_Print("B_OP_OVER");
			break;

		case B_OP_ERASE:
			_Print("B_OP_ERASE");
			break;

		case B_OP_INVERT:
			_Print("B_OP_INVERT");
			break;

		case B_OP_SELECT:
			_Print("B_OP_SELECT");
			break;

		case B_OP_ALPHA:
			_Print("B_OP_ALPHA");
			break;

		case B_OP_MIN:
			_Print("B_OP_MIN");
			break;

		case B_OP_MAX:
			_Print("B_OP_MAX");
			break;

		case B_OP_ADD:
			_Print("B_OP_ADD");
			break;

		case B_OP_SUBTRACT:
			_Print("B_OP_SUBTRACT");
			break;

		case B_OP_BLEND:
			_Print("B_OP_BLEND");
			break;

		default:
			_Print("Unknown mode: ", (float)mode);
	}

	_Cr();
}


void
PicturePrinter::SetLineMode(cap_mode capMode, join_mode joinMode,
	float miterLimit)
{
	_Indent();
	_Print("SetLineMode");

	switch (capMode) {
		case B_BUTT_CAP:
			_Print("B_BUTT_CAP");
			break;

		case B_ROUND_CAP:
			_Print("B_ROUND_CAP");
			break;

		case B_SQUARE_CAP:
			_Print("B_SQUARE_CAP");
			break;
	}

	switch (joinMode) {
		case B_MITER_JOIN:
			_Print("B_MITER_JOIN");
			break;

		case B_ROUND_JOIN:
			_Print("B_ROUND_JOIN");
			break;

		case B_BUTT_JOIN:
			_Print("B_BUTT_JOIN");
			break;

		case B_SQUARE_JOIN:
			_Print("B_SQUARE_JOIN");
			break;

		case B_BEVEL_JOIN:
			_Print("B_BEVEL_JOIN");
			break;
	}

	_Print("miterLimit", miterLimit);
	_Cr();
}


void
PicturePrinter::SetPenSize(float size)
{
	_Indent();
	_Print("SetPenSize", size);
	_Cr();
}


void
PicturePrinter::SetForeColor(rgb_color color)
{
	_Indent();
	_Print("SetForeColor");
	_Print(color);
	_Cr();
}


void
PicturePrinter::SetBackColor(rgb_color color)
{
	_Indent();
	_Print("SetBackColor");
	_Print(color);
	_Cr();
}


static bool
compare(pattern a, pattern b)
{
	for (int i = 0; i < 8; i ++) {
		if (a.data[i] != b.data[i])
			return false;
	}

	return true;
}


void
PicturePrinter::SetStipplePattern(pattern p)
{
	_Indent();
	_Print("SetStipplePattern");

	if (compare(p, B_SOLID_HIGH))
		_Print("B_SOLID_HIGH");
	else if (compare(p, B_SOLID_LOW))
		_Print("B_SOLID_LOW");
	else if (compare(p, B_MIXED_COLORS))
		_Print("B_MIXED_COLORS");
	else {
		for (int i = 0; i < 8; i++)
			printf("%2.2x ", (unsigned int)p.data[i]);
	}

	_Cr();
}


void
PicturePrinter::SetScale(float scale)
{
	_Indent();
	_Print("SetScale", scale);
	_Cr();
}


void
PicturePrinter::SetFontFamily(char* family)
{
	_Indent();
	_Print("SetFontFamily");
	_Print(family);
	_Cr();
}


void
PicturePrinter::SetFontStyle(char* style)
{
	_Indent();
	_Print("SetFontStyle");
	_Print(style);
	_Cr();
}


void
PicturePrinter::SetFontSpacing(int32 spacing)
{
	_Indent();
	_Print("SetFontSpacing");

	switch(spacing) {
		case B_CHAR_SPACING:
			_Print("B_CHAR_SPACING");
			break;

		case B_STRING_SPACING:
			_Print("B_STRING_SPACING");
			break;

		case B_BITMAP_SPACING:
			_Print("B_BITMAP_SPACING");
			break;

		case B_FIXED_SPACING:
			_Print("B_FIXED_SPACING");
			break;

		default:
			_Print("Unknown: ", (float)spacing);
	}

	_Cr();
}


void
PicturePrinter::SetFontSize(float size)
{
	_Indent();
	_Print("SetFontSize", size);
	_Cr();
}


void
PicturePrinter::SetFontRotate(float rotation)
{
	_Indent();
	_Print("SetFontRotation", rotation);
	_Cr();
}


void
PicturePrinter::SetFontEncoding(int32 encoding)
{
	_Indent();
	_Print("SetFontEncoding");

	switch (encoding) {
		case B_UNICODE_UTF8:
			_Print("B_UNICODE_UTF8");
			break;

		case B_ISO_8859_1:
			_Print("B_ISO_8859_1");
			break;

		case B_ISO_8859_2:
			_Print("B_ISO_8859_2");
			break;

		case B_ISO_8859_3:
			_Print("B_ISO_8859_3");
			break;

		case B_ISO_8859_4:
			_Print("B_ISO_8859_4");
			break;

		case B_ISO_8859_5:
			_Print("B_ISO_8859_5");
			break;

		case B_ISO_8859_6:
			_Print("B_ISO_8859_6");
			break;

		case B_ISO_8859_7:
			_Print("B_ISO_8859_7");
			break;

		case B_ISO_8859_8:
			_Print("B_ISO_8859_8");
			break;

		case B_ISO_8859_9:
			_Print("B_ISO_8859_9");
			break;

		case B_ISO_8859_10:
			_Print("B_ISO_8859_10");
			break;

		case B_MACINTOSH_ROMAN:
			_Print("B_MACINTOSH_ROMAN");
			break;

		default:
			_Print("Unknown:", (float)encoding);
	}

	_Cr();
}


#define PRINT_FLAG(flag) \
  if (flags & flag) { f |= flag; _Print(#flag); }


void
PicturePrinter::SetFontFlags(int32 flags)
{
	_Indent();
	_Print("SetFontFlags");

	int f = 0;
	if (flags == 0)
		_Print("none set");

	PRINT_FLAG(B_DISABLE_ANTIALIASING);
	PRINT_FLAG(B_FORCE_ANTIALIASING);

	if (flags != f)
		printf("Unknown Additional Flags %" B_PRId32 "", flags & ~f);

	_Cr();
}


void
PicturePrinter::SetFontShear(float shear)
{
	_Indent();
	_Print("SetFontShear", shear);
	_Cr();
}


void
PicturePrinter::SetFontFace(int32 flags)
{
	_Indent();
	_Print("SetFontFace");

	int32 f = 0;
	if (flags == 0)
		_Print("none set");

	PRINT_FLAG(B_REGULAR_FACE);
	PRINT_FLAG(B_BOLD_FACE);
	PRINT_FLAG(B_ITALIC_FACE);
	PRINT_FLAG(B_NEGATIVE_FACE);
	PRINT_FLAG(B_OUTLINED_FACE);
	PRINT_FLAG(B_UNDERSCORE_FACE);
	PRINT_FLAG(B_STRIKEOUT_FACE);

	if (flags != f)
		printf("Unknown Additional Flags %" B_PRId32 "", flags & ~f);

	_Cr();
}


// Implementation of ShapePrinter
ShapePrinter::ShapePrinter(PicturePrinter* printer)
	:
	fPrinter(printer)
{
	fPrinter->_IncIndent();
}


ShapePrinter::~ShapePrinter()
{
	fPrinter->_DecIndent();
}


status_t
ShapePrinter::IterateBezierTo(int32 bezierCount, BPoint* control)
{
	fPrinter->_Indent();
	fPrinter->_Print("BezierTo");
	fPrinter->_Cr();

	for (int32 i = 0; i < bezierCount; i++, control += 3) {
		fPrinter->_Indent(1);
		fPrinter->_Print(i / 3.0);
		fPrinter->_Print(&control[0]);
		fPrinter->_Print(&control[1]);
		fPrinter->_Print(&control[2]);
		fPrinter->_Cr();
	}

	return B_OK;
}


status_t
ShapePrinter::IterateClose()
{
	fPrinter->_Indent();
	fPrinter->_Print("Close");
	fPrinter->_Cr();

	return B_OK;
}


status_t
ShapePrinter::IterateLineTo(int32 lineCount, BPoint* linePoints)
{
	fPrinter->_Indent();
	fPrinter->_Print("LineTo");
	fPrinter->_Cr();

	BPoint* p = linePoints;
	for (int32 i = 0; i < lineCount; i++) {
		fPrinter->_Indent(1);
		fPrinter->_Print(p);
		fPrinter->_Cr();

		p++;
	}

	return B_OK;
}


status_t
ShapePrinter::IterateMoveTo(BPoint* point)
{
	fPrinter->_Indent();
	fPrinter->_Print("MoveTo", point);
	fPrinter->_Cr();

	return B_OK;
}
