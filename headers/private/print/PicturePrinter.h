/*
 * Copyright 2001-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PICTURE_PRINTER_H
#define _PICTURE_PRINTER_H


#include "PictureIterator.h"


class PicturePrinter : public PictureIterator {
public:
								PicturePrinter(int indent = 0);

	// BPicture playback handlers
	virtual	void				Op(int number);

	virtual void				MovePenBy(BPoint delta);

	virtual void				StrokeLine(BPoint start, BPoint end);

	virtual void				StrokeRect(BRect rect);
	virtual void				FillRect(BRect rect);

	virtual void				StrokeRoundRect(BRect rect, BPoint radii);
	virtual void				FillRoundRect(BRect rect, BPoint radii);

	virtual void				StrokeBezier(BPoint* control);
	virtual void				FillBezier(BPoint* control);

	virtual void				StrokeArc(BPoint center, BPoint radii,
									float startTheta, float arcTheta);
	virtual void				FillArc(BPoint center, BPoint radii,
									float startTheta, float arcTheta);

	virtual void				StrokeEllipse(BPoint center, BPoint radii);
	virtual void				FillEllipse(BPoint center, BPoint radii);

	virtual void				StrokePolygon(int32 numPoints, BPoint* points,
									bool isClosed);
	virtual void				FillPolygon(int32 numPoints, BPoint* points,
									bool isClosed);

	virtual void        		StrokeShape(BShape* shape);
	virtual void       			 FillShape(BShape* shape);

	virtual void				DrawString(char* string,
									float escapement_nospace,
									float escapement_space);

	virtual void				DrawPixels(BRect src, BRect dest, int32 width,
									int32 height, int32 bytesPerRow,
									int32 pixelFormat, int32 flags, void* data);

	virtual void				SetClippingRects(BRect* rects, uint32 numRects);
	virtual void    			ClipToPicture(BPicture* picture, BPoint point,
									bool clip_to_inverse_picture);

	virtual void				PushState();
	virtual void				PopState();

	virtual void				EnterStateChange();
	virtual void				ExitStateChange();
	virtual void				EnterFontState();
	virtual void				ExitFontState();

	virtual void				SetOrigin(BPoint point);
	virtual void				SetPenLocation(BPoint point);
	virtual void				SetDrawingMode(drawing_mode mode);
	virtual void				SetLineMode(cap_mode capMode,
									join_mode joinMode, float miterLimit);
	virtual void				SetPenSize(float size);
	virtual void				SetForeColor(rgb_color color);
	virtual void				SetBackColor(rgb_color color);
	virtual void				SetStipplePattern(pattern p);
	virtual void				SetScale(float scale);

	virtual void				SetFontFamily(char* family);
	virtual void				SetFontStyle(char* style);
	virtual void				SetFontSpacing(int32 spacing);
	virtual void				SetFontSize(float size);
	virtual void				SetFontRotate(float rotation);
	virtual void				SetFontEncoding(int32 encoding);
	virtual void				SetFontFlags(int32 flags);
	virtual void				SetFontShear(float shear);
	virtual void				SetFontFace(int32 flags);

private:
	friend class ShapePrinter;

			void				_Print(const char* text);
			void				_Print(BPoint* point);
			void				_Print(BRect* rect);
			void				_Print(int numPoints, BPoint* points);
			void				_Print(int numRects, BRect* rects);
			void				_Print(BShape* shape);
			void				_Print(const char* label, float f);
			void				_Print(const char* label, BPoint *point);
			void				_Print(rgb_color color);
			void				_Print(float f);

			void				_Cr();

			void				_Indent(int inc = 0);
			void				_IncIndent();
			void				_DecIndent();

private:
			int					fIndent;
};


class ShapePrinter : public BShapeIterator {
public:
								ShapePrinter(PicturePrinter* printer);
								~ShapePrinter();

			status_t			IterateBezierTo(int32 bezierCount,
									BPoint* bezierPoints);
			status_t			IterateClose();
			status_t			IterateLineTo(int32 lineCount,
									BPoint* linePoints);
			status_t			IterateMoveTo(BPoint* point);

private:
			PicturePrinter*		fPrinter;
};

#endif // _PICTURE_PRINTER_H
