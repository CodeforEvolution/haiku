/*
 * Copyright 2009-2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		Adrien Destugues, pulkomandy@gmail.com
 *		John Scipione, jscipione@gmail.com
 */


/*! Decorator resembling Mac OS 8 and 9 */


#include "MacDecorator.h"

#include <new>
#include <stdio.h>

#include <GradientLinear.h>
#include <Point.h>
#include <View.h>

#include <WindowPrivate.h>

#include "DesktopSettings.h"
#include "DrawingEngine.h"
#include "PatternHandler.h"
#include "RGBColor.h"


//#define DEBUG_DECORATOR
#ifdef DEBUG_DECORATOR
#	define STRACE(x) printf x
#else
#	define STRACE(x) ;
#endif


static const float kResizeKnobSize = 16.0;
static const float kBorderResizeLength = 22.0;

static const int32 kDefaultBorderWidth = 6;

static const RGBColor kButtonHighColor = RGBColor(232, 232, 232);
static const RGBColor kButtonLowColor = RGBColor(128, 128, 128);
static const RGBColor kFrameHighColor = RGBColor(255, 255, 255);
static const RGBColor kFrameMidColor = RGBColor(216, 216, 216);
static const RGBColor kFrameLowColor = RGBColor(156, 156, 156);
static const RGBColor kFrameLowerColor = RGBColor(0, 0, 0);
static const RGBColor kInactiveColor = RGBColor(82, 82, 82);
static const RGBColor kDarkColor = RGBColor(115, 115, 115);
static const RGBColor kButtonColor = RGBColor(33, 33, 33);

static const RGBColor kResizeKnobBackgroundColor = RGBColor(204, 204, 204);
static const RGBColor kResizeKnobLightColor = RGBColor(255, 255, 255);
static const RGBColor kResizeKnobMidColor = RGBColor(170, 170, 170);
static const RGBColor kResizeKnobDarkColor = RGBColor(119, 119, 119);


MacDecorAddOn::MacDecorAddOn(image_id id, const char* name)
	:
	DecorAddOn(id, name)
{
}


Decorator*
MacDecorAddOn::_AllocateDecorator(DesktopSettings& settings, BRect rect, Desktop* desktop)
{
	return new (std::nothrow)MacDecorator(settings, rect, desktop);
}


MacDecorator::MacDecorator(DesktopSettings& settings, BRect frame, Desktop* desktop)
	:
	SATDecorator(settings, frame, desktop),
	fButtonHighColor(kButtonHighColor),
	fButtonLowColor(kButtonLowColor),
	fFrameHighColor(kFrameHighColor),
	fFrameMidColor(kFrameMidColor),
	fFrameLowColor(kFrameLowColor),
	fFrameLowerColor(kFrameLowerColor)
{
	STRACE(("MacDecorator()\n"));
	STRACE(("\tFrame (%.1f,%.1f,%.1f,%.1f)\n", frame.left, frame.top, frame.right, frame.bottom));
}


MacDecorator::~MacDecorator()
{
	STRACE(("~MacDecorator()\n"));
}


// TODO : Add GetSettings


// TODO : add GetSizeLimits


void
MacDecorator::UpdateColors(DesktopSettings& settings)
{
	fFocusTextColor = settings.UIColor(B_WINDOW_TEXT_COLOR);
	fNonFocusTextColor = settings.UIColor(B_WINDOW_INACTIVE_TEXT_COLOR);
}


Decorator::Region
MacDecorator::RegionAt(BPoint where, int32& tabIndex) const
{
	tabIndex = -1;

	for (int32 i = 0; i < fTabList.CountItems(); i++) {
		Decorator::Tab* tab = fTabList.ItemAt(i);
		if (tab->minimizeRect.Contains(where)) {
			tabIndex = i;
			return REGION_MINIMIZE_BUTTON;
		}
	}

	// Let the base class version try to identify hits of
	// the buttons and the tab.
	return TabDecorator::RegionAt(where, tabIndex);
}


bool
MacDecorator::SetRegionHighlight(Region region, uint8 highlight, BRegion* dirty, int32 tabIndex)
{
	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_TabAt(tabIndex));
	if (tab != NULL) {
		tab->isHighlighted = highlight != 0;
		// Invalidate the bitmap caches for the close/minimize/zoom button
		// when the highlight changes.
		switch (region) {
			case REGION_CLOSE_BUTTON:
				if (highlight != RegionHighlight(region))
					memset(&tab->closeBitmaps, 0, sizeof(tab->closeBitmaps));
				break;

			case REGION_MINIMIZE_BUTTON:
				if (highlight != RegionHighlight(region))
					memset(&tab->minimizeBitmaps, 0, sizeof(tab->minimizeBitmaps));
				break;

			case REGION_ZOOM_BUTTON:
				if (highlight != RegionHighlight(region))
					memset(&tab->zoomBitmaps, 0, sizeof(tab->zoomBitmaps));
				break;

			default:
				break;
		}
	}

	return Decorator::SetRegionHighlight(region, highlight, dirty, tabIndex);
}


void
MacDecorator::_DoLayout()
{
	STRACE(("MacDecorator: Do Layout\n"));

	// Here we determine the size of every rectangle that we use
	// internally when we are given the size of the client rectangle.

	bool hasTab = false;

	if (fTopTab) {
		switch (fTopTab->look) {
			case B_MODAL_WINDOW_LOOK:
				fBorderWidth = kDefaultBorderWidth;
				break;

			case B_TITLED_WINDOW_LOOK:
			case B_DOCUMENT_WINDOW_LOOK:
				hasTab = true;
				fBorderWidth = kDefaultBorderWidth;
				break;

			case B_FLOATING_WINDOW_LOOK:
				hasTab = true;
				fBorderWidth = 3;
				break;

			case B_BORDERED_WINDOW_LOOK:
				fBorderWidth = 1;
				break;

			default:
				fBorderWidth = 0;
		}
	} else
		fBorderWidth = 0;

	fResizeKnobSize = kResizeKnobSize;
	fBorderResizeLength = kBorderResizeLength;
		
	// Calculate Left, Top, Right, Bottom borders
	if (fBorderWidth > 0) {
		// NOTE: No overlapping, the left and right border rects don't include the corners!
		fLeftBorder.Set(fFrame.left - fBorderWidth, fFrame.top,
			fFrame.left - 1, fFrame.bottom);

		fRightBorder.Set(fFrame.right + 1, fFrame.top ,
			fFrame.right + fBorderWidth, fFrame.bottom);

		fTopBorder.Set(fFrame.left - fBorderWidth, fFrame.top - fBorderWidth,
			fFrame.right + fBorderWidth, fFrame.top - 1);

		fBottomBorder.Set(fFrame.left - fBorderWidth, fFrame.bottom + 1,
			fFrame.right + fBorderWidth, fFrame.bottom + fBorderWidth);
	} else {
		// no border
		fLeftBorder.Set(0.0, 0.0, -1.0, -1.0);
		fRightBorder.Set(0.0, 0.0, -1.0, -1.0);
		fTopBorder.Set(0.0, 0.0, -1.0, -1.0);
		fBottomBorder.Set(0.0, 0.0, -1.0, -1.0);
	}
	
	fBorderRect = BRect(fTopBorder.LeftTop(), fBottomBorder.RightBottom());

	// Calculate Resize Rect
	if (fBorderWidth > 1) {
		fResizeRect.Set(fBottomBorder.right - fResizeKnobSize,
			fBottomBorder.bottom - fResizeKnobSize, fBottomBorder.right, fBottomBorder.bottom);
	} else {
		// no border or one pixel border (menus and such)
		fResizeRect.Set(0, 0, -1, -1);
	}

	// calculate our tab rect
	if (hasTab) {
		_DoTabLayout();
	} else {
		for (int32 i = 0; i < fTabList.CountItems(); i++) {
			Decorator::Tab* tab = fTabList.ItemAt(i);
			tab->tabRect.Set(0.0, 0.0, -1.0, -1.0);
		}
		fTabsRegion.MakeEmpty();
		fTitleBarRect.Set(0.0, 0.0, -1.0, -1.0);
	}
}


void
MacDecorator::_DoTabLayout()
{
	float tabOffset = 0;
	if (fTabList.CountItems() == 1) {
		float tabSize;
		tabOffset = _SingleTabOffsetAndSize(tabSize);
	}

	// Calculate the Tab Rect!
	float sumTabWidth = 0;
	for (int32 index = 0; index < fTabList.CountItems(); index++) {
		Decorator::Tab* tab = _TabAt(index);

		BRect& tabRect = tab->tabRect;
		// distance from one item of the tab bar to another.
		// In this case the text and close/zoom rects
//		tab->textOffset = _DefaultTextOffset();

		font_height fontHeight;
		fDrawState.Font().GetHeight(fontHeight);

		if (tab->look != kLeftTitledWindowLook) {
			const float spacing = fBorderWidth * 1.4f;
			tabRect.Set(fFrame.left - fBorderWidth,
				fFrame.top - fBorderWidth
					- ceilf(fontHeight.ascent + fontHeight.descent + spacing),
				((fFrame.right - fFrame.left) < (spacing * 5) ?
					fFrame.left + (spacing * 5) : fFrame.right) + fBorderWidth,
				fFrame.top - fBorderWidth);
		} else {
			tabRect.Set(fFrame.left - fBorderWidth
				- ceilf(fontHeight.ascent + fontHeight.descent + fBorderWidth),
					fFrame.top - fBorderWidth, fFrame.left - fBorderWidth,
				fFrame.bottom + fBorderWidth);
		}

		float offset;
		float size;
		float inset;
		_GetButtonSizeAndOffset(tabRect, &offset, &size, &inset);

		// tab->minTabSize contains just the room for the buttons
		tab->minTabSize = inset * 2 + tab->textOffset;
		if ((tab->flags & B_NOT_CLOSABLE) == 0)
			tab->minTabSize += offset + size;
		if ((tab->flags & B_NOT_ZOOMABLE) == 0)
			tab->minTabSize += offset + size;

		// tab->maxTabSize contains tab->minTabSize + the width required for the
		// title
		tab->maxTabSize = fDrawingEngine
			? ceilf(fDrawingEngine->StringWidth(Title(tab), strlen(Title(tab)), fDrawState.Font()))
				: 0.0;
		if (tab->maxTabSize > 0.0)
			tab->maxTabSize += tab->textOffset;
		tab->maxTabSize += tab->minTabSize;

		float tabSize = (tab->look != kLeftTitledWindowLook
			? fFrame.Width() : fFrame.Height()) + fBorderWidth * 2;
		if (tabSize < tab->minTabSize)
			tabSize = tab->minTabSize;
		if (tabSize > tab->maxTabSize)
			tabSize = tab->maxTabSize;

		// layout buttons and truncate text
		if (tab->look != kLeftTitledWindowLook)
			tabRect.right = tabRect.left + tabSize;
		else
			tabRect.bottom = tabRect.top + tabSize;

		// make sure fTabOffset is within limits and apply it to
		// the tabRect
		tab->tabOffset = (uint32)tabOffset;
		if (tab->tabLocation != 0.0 && fTabList.CountItems() == 1
			&& tab->tabOffset > (fRightBorder.right - fLeftBorder.left
				- tabRect.Width())) {
			tab->tabOffset = uint32(fRightBorder.right - fLeftBorder.left
				- tabRect.Width());
		}
		tabRect.OffsetBy(tab->tabOffset, 0);
		tabOffset += tabRect.Width();

		sumTabWidth += tabRect.Width();
	}

	// Finally, layout the buttons and text within the tab rect!
	for (int32 i = 0; i < fTabList.CountItems(); i++) {
		Decorator::Tab* tab = fTabList.ItemAt(i);

		if (i == 0)
			fTitleBarRect = tab->tabRect;
		else
			fTitleBarRect = fTitleBarRect | tab->tabRect;

		_LayoutTabItems(tab, tab->tabRect);
	}

	fTabsRegion = fTitleBarRect;
}


void
MacDecorator::_LayoutTabItems(Decorator::Tab* tab, const BRect& tabRect)
{
	tab->zoomRect = tabRect;
	tab->zoomRect.left = tab->zoomRect.right - 12;
	tab->zoomRect.bottom = tab->zoomRect.top + 12;
	tab->zoomRect.OffsetBy(-4, 4);

	tab->closeRect = tab->zoomRect;
	tab->minimizeRect = tab->zoomRect;

	tab->closeRect.OffsetTo(tabRect.left + 4, tabRect.top + 4);

	tab->zoomRect.OffsetBy(0 - (tab->zoomRect.Width() + 4), 0);
	if (Title(tab) != NULL && fDrawingEngine != NULL) {
		tab->truncatedTitle = Title(tab);
		fDrawingEngine->SetFont(fDrawState.Font());
		tab->truncatedTitleLength = (int32)fDrawingEngine->StringWidth(Title(tab),
			strlen(Title(tab)));

		if (tab->truncatedTitleLength < (tab->zoomRect.left - tab->closeRect.right - 10)) {
			// start with offset from closerect.right
			tab->textOffset = int(((tab->zoomRect.left - 5)
				- (tab->closeRect.right + 5)) / 2);
			tab->textOffset -= int(tab->truncatedTitleLength / 2);

			// now make it the offset from fTabRect.left
			tab->textOffset += int(tab->closeRect.right + 5 - tabRect.left);
		} else
			tab->textOffset = int(tab->closeRect.right) + 5;
	} else
		tab->textOffset = 0;
}


void
MacDecorator::_DrawFrame(BRect invalid)
{
	STRACE(("_DrawFrame(%f,%f,%f,%f)\n", invalid.left, invalid.top, invalid.right,
		invalid.bottom));

	// NOTE: the DrawingEngine needs to be locked for the entire
	// time for the clipping to stay valid for this decorator

	if (fTopTab->look == B_NO_BORDER_WINDOW_LOOK)
		return;

	if (fBorderWidth <= 0)
		return;

	// Draw the border frame
	BRect r = BRect(fTopBorder.LeftTop(), fBottomBorder.RightBottom());
	switch ((int)fTopTab->look) {
		case B_TITLED_WINDOW_LOOK:
		case B_DOCUMENT_WINDOW_LOOK:
		case B_MODAL_WINDOW_LOOK:
		{
			if (IsFocus(fTopTab)) {
				BPoint offset = r.LeftTop();
				BPoint pt2 = r.LeftBottom();

				// Draw the left side of the frame
				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
				offset.x++;
				pt2.x++;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameHighColor);
				offset.x++;
				pt2.x++;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x++;
				pt2.x++;
				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x++;
				pt2.x++;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowColor);
				offset.x++;
				offset.y += 2;
				BPoint topleftpt = offset;
				pt2.x++;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);

				offset = r.RightTop();
				pt2 = r.RightBottom();

				// Draw the right side of the frame
				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
				offset.x--;
				pt2.x--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowColor);
				offset.x--;
				pt2.x--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x--;
				pt2.x--;
				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x--;
				pt2.x--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameHighColor);
				offset.x--;
				offset.y += 2;
				BPoint toprightpt = offset;
				pt2.x--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);

				// Draw the top side of the frame that is not in the tab
				if (fTopTab->look == B_MODAL_WINDOW_LOOK) {
					offset = r.LeftTop();
					pt2 = r.RightTop();

					fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
					offset.x++;
					offset.y++;
					pt2.x--;
					pt2.y++;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameHighColor);
					offset.x++;
					offset.y++;
					pt2.x--;
					pt2.y++;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
					offset.x++;
					offset.y++;
					pt2.x--;
					pt2.y++;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
					offset.x++;
					offset.y++;
					pt2.x--;
					pt2.y++;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameLowColor);
					offset.x++;
					offset.y++;
					pt2.x--;
					pt2.y++;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
				} else {
					// Some odd stuff here where the title bar is melded into the
					// window border so that the sides are drawn into the title
					// so we draw this bottom up
					offset = topleftpt;
					pt2 = toprightpt;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
					offset.y--;
					offset.x++;
					pt2.y--;

					fDrawingEngine->StrokeLine(offset, pt2, fFrameLowColor);
				}

				// Draw the bottom side of the frame
				offset = r.LeftBottom();
				pt2 = r.RightBottom();

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
				offset.x++;
				offset.y--;
				pt2.x--;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowColor);
				offset.x++;
				offset.y--;
				pt2.x--;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x++;
				offset.y--;
				pt2.x--;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameMidColor);
				offset.x++;
				offset.y--;
				pt2.x--;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameHighColor);
				offset.x += 2;
				offset.y--;
				pt2.x--;
				pt2.y--;

				fDrawingEngine->StrokeLine(offset, pt2, fFrameLowerColor);
				offset.y--;
				pt2.x--;
				pt2.y--;
			} else {
				r.top -= 3;
				
				fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
					kInactiveColor);
				fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(),
					kInactiveColor);
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.RightBottom(),
					kInactiveColor);

				for (uint8 i = 0; i < 4; i++) {
					r.InsetBy(1, 1);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
						fFrameMidColor);
					fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(),
						fFrameMidColor);
					fDrawingEngine->StrokeLine(r.LeftBottom(), r.RightBottom(),
						fFrameMidColor);
					fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
						fFrameMidColor);
				}

				r.InsetBy(1, 1);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.LeftBottom(),
					kInactiveColor);
				fDrawingEngine->StrokeLine(r.RightTop(), r.RightBottom(),
					kInactiveColor);
				fDrawingEngine->StrokeLine(r.LeftBottom(), r.RightBottom(),
					kInactiveColor);
				fDrawingEngine->StrokeLine(r.LeftTop(), r.RightTop(),
					kInactiveColor);
			}
			break;
		}
		case B_FLOATING_WINDOW_LOOK:
		case kLeftTitledWindowLook:
		{
			break;
		}
		case B_BORDERED_WINDOW_LOOK:
			fDrawingEngine->StrokeRect(r, fFrameMidColor);
			break;

		default:
			// don't draw a border frame
		break;
	}
	
	// Draw resize knob if necessary!
	if (!(fTopTab->flags & B_NOT_RESIZABLE)) {
		BRect resizeKnob(fResizeRect);
		
		switch (static_cast<int>(fTopTab->look)) {
			case B_DOCUMENT_WINDOW_LOOK:
			{
				if (!invalid.Intersects(resizeKnob))
					break;
					
				fDrawingEngine->FillRect(resizeKnob, make_color(255, 0, 0));

				break;
			}

			default:
				// Nothing to do!
				break;
		}	
	}
}


void
MacDecorator::_DrawTab(Decorator::Tab* tab, BRect invalid)
{
	STRACE(("_DrawTab(%.1f, %.1f, %.1f, %.1f)\n", invalid.left, invalid.top, invalid.right,
		invalid.bottom));
	
	const BRect& tabRect = tab->tabRect;

	// If a window has a tab, this will draw it and any buttons which are in it.
	if (!tabRect.IsValid() || !invalid.Intersects(tabRect))
		return;

	BRect rect(tab->tabRect);
	fDrawingEngine->SetHighColor(RGBColor(fFrameMidColor));
	fDrawingEngine->FillRect(tabRect, fFrameMidColor);

	if (IsFocus(tab)) {
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameLowerColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(), fFrameLowerColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(), fFrameLowerColor);

		rect.InsetBy(1, 1);
		rect.bottom++;

		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), fFrameHighColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(), fFrameHighColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(), fFrameLowColor);

		// Draw the neat little lines on either side of the title if there's room
		float left;
		if ((tab->flags & B_NOT_CLOSABLE) == 0)
			left = tab->closeRect.right;
		else
			left = tab->tabRect.left;

		float right;
		if ((tab->flags & B_NOT_ZOOMABLE) == 0)
			right = tab->zoomRect.left;
		else if ((tab->flags & B_NOT_MINIMIZABLE) == 0)
			right = tab->minimizeRect.left;
		else
			right = tab->tabRect.right;

		if (tab->tabRect.left + tab->textOffset > left + 5) {
			// Left side

			BPoint offset(left + 5, tab->closeRect.top);
			BPoint pt2(tab->tabRect.left + tab->textOffset - 5, tab->closeRect.top);

			fDrawState.SetHighColor(RGBColor(fFrameHighColor));
			for (int32 i = 0; i < 6; i++) {
				fDrawingEngine->StrokeLine(offset, pt2, fDrawState.HighColor());
				offset.y += 2;
				pt2.y += 2;
			}

			offset.Set(left + 6, tab->closeRect.top + 1);
			pt2.Set(tab->tabRect.left + tab->textOffset - 4, tab->closeRect.top + 1);

			fDrawState.SetHighColor(kDarkColor);
			for (uint32 i = 0; i < 6; i++) {
				fDrawingEngine->StrokeLine(offset, pt2, fDrawState.HighColor());
				offset.y += 2;
				pt2.y += 2;
			}

			// Right side

			offset.Set(tab->tabRect.left + tab->textOffset + tab->truncatedTitleLength + 3,
				tab->zoomRect.top);
			pt2.Set(right - 8, tab->zoomRect.top);

			if (offset.x < pt2.x) {
				fDrawState.SetHighColor(kFrameHighColor);
				for (uint32 i = 0; i < 6; i++) {
					fDrawingEngine->StrokeLine(offset, pt2, fDrawState.HighColor());
					offset.y += 2;
					pt2.y += 2;
				}

				offset.Set(tab->tabRect.left + tab->textOffset + tab->truncatedTitleLength + 4,
					tab->zoomRect.top + 1);
				pt2.Set(right - 7, tab->zoomRect.top + 1);

				fDrawState.SetHighColor(kDarkColor);
				for (uint32 i = 0; i < 6; i++) {
					fDrawingEngine->StrokeLine(offset, pt2, fDrawState.HighColor());
					offset.y += 2;
					pt2.y += 2;
				}
			}
		}

		_DrawButtons(tab, rect);
	} else {
		// Not focused - Just draw a plain light grey area with the title
		// in the middle
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.RightTop(), kInactiveColor);
		fDrawingEngine->StrokeLine(rect.LeftTop(), rect.LeftBottom(), kInactiveColor);
		fDrawingEngine->StrokeLine(rect.RightBottom(), rect.RightTop(), kInactiveColor);
	}

	_DrawTitle(tab, tab->tabRect);
}


void
MacDecorator::_DrawButtons(Decorator::Tab* tab, const BRect& invalid)
{
	if ((tab->flags & B_NOT_CLOSABLE) == 0 && invalid.Intersects(tab->closeRect))
		_DrawClose(tab, false, tab->closeRect);

	if ((tab->flags & B_NOT_MINIMIZABLE) == 0 && invalid.Intersects(tab->minimizeRect))
		_DrawMinimize(tab, false, tab->minimizeRect);

	if ((tab->flags & B_NOT_ZOOMABLE) == 0 && invalid.Intersects(tab->zoomRect))
		_DrawZoom(tab, false, tab->zoomRect);
}


void
MacDecorator::_DrawTitle(Decorator::Tab* tab, BRect rect)
{
	fDrawingEngine->SetHighColor(IsFocus(tab) ? fFocusTextColor : fNonFocusTextColor);
	fDrawingEngine->SetLowColor(fFrameMidColor);

	tab->truncatedTitle = Title(tab);
	fDrawState.Font().TruncateString(&tab->truncatedTitle, B_TRUNCATE_END,
		(tab->zoomRect.left - 5) - (tab->closeRect.right + 5));
	fDrawingEngine->SetFont(fDrawState.Font());

	fDrawingEngine->DrawString(tab->truncatedTitle, tab->truncatedTitle.Length(),
		BPoint(fTitleBarRect.left + tab->textOffset, tab->closeRect.bottom - 1));
}


void
MacDecorator::_DrawClose(Decorator::Tab* tab, bool direct, BRect rect)
{
	_DrawButton(tab, direct, rect, tab->closePressed);
}


void
MacDecorator::_DrawZoom(Decorator::Tab* tab, bool direct, BRect rect)
{
	_DrawButton(tab, direct, rect, tab->zoomPressed);

	rect.top++;
	rect.left++;
	rect.bottom = rect.top + 6;
	rect.right = rect.left + 6;

	fDrawState.SetHighColor(kButtonColor);
	fDrawingEngine->StrokeRect(rect, fDrawState.HighColor());
}


void
MacDecorator::_DrawMinimize(Decorator::Tab* tab, bool direct, BRect rect)
{
	_DrawButton(tab, direct, rect, tab->minimizePressed);

	rect.InsetBy(1, 5);

	fDrawState.SetHighColor(kButtonColor);
	fDrawingEngine->StrokeRect(rect, fDrawState.HighColor());
}


void
MacDecorator::_MoveBy(BPoint offset)
{
	STRACE(("MacDecorator: Move By (%.1f, %.1f)\n", offset.x, offset.y));

	// Move all internal rectangles the appropriate amount
	for (int32 i = 0; i < fTabList.CountItems(); i++) {
		Decorator::Tab* tab = fTabList.ItemAt(i);
		tab->zoomRect.OffsetBy(offset);
		tab->closeRect.OffsetBy(offset);
		tab->minimizeRect.OffsetBy(offset);
		tab->tabRect.OffsetBy(offset);
	}

	fFrame.OffsetBy(offset);
	fTitleBarRect.OffsetBy(offset);
	fTabsRegion.OffsetBy(offset);
	fResizeRect.OffsetBy(offset);
	fBorderRect.OffsetBy(offset);

	fLeftBorder.OffsetBy(offset);
	fRightBorder.OffsetBy(offset);
	fTopBorder.OffsetBy(offset);
	fBottomBorder.OffsetBy(offset);
}


void
MacDecorator::_ResizeBy(BPoint offset, BRegion* dirty)
{
	STRACE(("MacDecorator: Resize By (%.1f, %.1f)\n", offset.x, offset.y));

	// Move all internal rectangles the appropriate amount
	fFrame.right += offset.x;
	fFrame.bottom += offset.y;

	// Handle invalidation of resize rect
	if (dirty != NULL && !(fTopTab->flags & B_NOT_RESIZABLE)) {
		BRect realResizeRect;
		switch ((int)fTopTab->look) {
			case B_DOCUMENT_WINDOW_LOOK:
				realResizeRect = fResizeRect;
				// Resize rect at old location
				dirty->Include(realResizeRect);
				realResizeRect.OffsetBy(offset);
				// Resize rect at new location
				dirty->Include(realResizeRect);
				break;

			case B_TITLED_WINDOW_LOOK:
			case B_FLOATING_WINDOW_LOOK:
			case B_MODAL_WINDOW_LOOK:
			case kLeftTitledWindowLook:
				// The bottom border resize line
				realResizeRect.Set(fRightBorder.right - fBorderResizeLength,
					fBottomBorder.top,
					fRightBorder.right - fBorderResizeLength,
					fBottomBorder.bottom - 1);
				// Old location
				dirty->Include(realResizeRect);
				realResizeRect.OffsetBy(offset);
				// New location
				dirty->Include(realResizeRect);

				// The right border resize line
				realResizeRect.Set(fRightBorder.left,
					fBottomBorder.bottom - fBorderResizeLength,
					fRightBorder.right - 1,
					fBottomBorder.bottom - fBorderResizeLength);
				// Old location
				dirty->Include(realResizeRect);
				realResizeRect.OffsetBy(offset);
				// New location
				dirty->Include(realResizeRect);
				break;

			default:
				break;
		}
	}

	fResizeRect.OffsetBy(offset);

	fBorderRect.right += offset.x;
	fBorderRect.bottom += offset.y;

	fLeftBorder.bottom += offset.y;
	fTopBorder.right += offset.x;

	fRightBorder.OffsetBy(offset.x, 0.0);
	fRightBorder.bottom	+= offset.y;

	fBottomBorder.OffsetBy(0.0, offset.y);
	fBottomBorder.right	+= offset.x;

	if (dirty) {
		if (offset.x > 0.0) {
			BRect t(fRightBorder.left - offset.x, fTopBorder.top,
				fRightBorder.right, fTopBorder.bottom);
			dirty->Include(t);
			t.Set(fRightBorder.left - offset.x, fBottomBorder.top,
				fRightBorder.right, fBottomBorder.bottom);
			dirty->Include(t);
			dirty->Include(fRightBorder);
		} else if (offset.x < 0.0) {
			dirty->Include(BRect(fRightBorder.left, fTopBorder.top,
				fRightBorder.right, fBottomBorder.bottom));
		}
		if (offset.y > 0.0) {
			BRect t(fLeftBorder.left, fLeftBorder.bottom - offset.y,
				fLeftBorder.right, fLeftBorder.bottom);
			dirty->Include(t);
			t.Set(fRightBorder.left, fRightBorder.bottom - offset.y,
				fRightBorder.right, fRightBorder.bottom);
			dirty->Include(t);
			dirty->Include(fBottomBorder);
		} else if (offset.y < 0.0) {
			dirty->Include(fBottomBorder);
		}
	}

	// resize tab and layout tab items
	if (fTitleBarRect.IsValid()) {
		if (fTabList.CountItems() > 1) {
			_DoTabLayout();
			if (dirty != NULL)
				dirty->Include(fTitleBarRect);
			return;
		}

		Decorator::Tab* tab = _TabAt(0);
		BRect& tabRect = tab->tabRect;
		BRect oldTabRect(tabRect);

		float tabSize;
		float tabOffset = _SingleTabOffsetAndSize(tabSize);

		float delta = tabOffset - tab->tabOffset;
		tab->tabOffset = (uint32)tabOffset;
		if (fTopTab->look != kLeftTitledWindowLook)
			tabRect.OffsetBy(delta, 0.0);
		else
			tabRect.OffsetBy(0.0, delta);

		if (tabSize < tab->minTabSize)
			tabSize = tab->minTabSize;
		if (tabSize > tab->maxTabSize)
			tabSize = tab->maxTabSize;

		if (fTopTab->look != kLeftTitledWindowLook
			&& tabSize != tabRect.Width()) {
			tabRect.right = tabRect.left + tabSize;
		} else if (fTopTab->look == kLeftTitledWindowLook
			&& tabSize != tabRect.Height()) {
			tabRect.bottom = tabRect.top + tabSize;
		}

		if (oldTabRect != tabRect) {
			_LayoutTabItems(tab, tabRect);

			if (dirty) {
				// NOTE: the tab rect becoming smaller only would
				// handled be the Desktop anyways, so it is sufficient
				// to include it into the dirty region in it's
				// final state
				BRect redraw(tabRect);
				if (delta != 0.0) {
					redraw = redraw | oldTabRect;
					if (fTopTab->look != kLeftTitledWindowLook)
						redraw.bottom++;
					else
						redraw.right++;
				}
				dirty->Include(redraw);
			}
		}
		fTitleBarRect = tabRect;
		fTabsRegion = fTitleBarRect;
	}
}


void
MacDecorator::_SetFocus(Decorator::Tab* tab)
{
	Decorator::Tab* decoratorTab = static_cast<Decorator::Tab*>(tab);

	decoratorTab->buttonFocus = IsFocus(tab)
		|| ((decoratorTab->look == B_FLOATING_WINDOW_LOOK
			|| decoratorTab->look == kLeftTitledWindowLook)
			&& (decoratorTab->flags & B_AVOID_FOCUS) != 0);
	if (CountTabs() > 1)
		_LayoutTabItems(decoratorTab, decoratorTab->tabRect);
}


bool
MacDecorator::_SetTabLocation(Decorator::Tab* _tab, float location, bool isShifting,
	BRegion* updateRegion)
{
	STRACE(("MacDecorator: Set Tab Location(%.1f)\n", location));

	if (CountTabs() > 1) {
		if (isShifting == false) {
			_DoTabLayout();
			if (updateRegion != NULL)
				updateRegion->Include(fTitleBarRect);

			fOldMovingTab = BRect(0, 0, -1, -1);
			return true;
		} else {
			if (fOldMovingTab.IsValid() == false)
				fOldMovingTab = _tab->tabRect;
		}
	}

	Decorator::Tab* tab = static_cast<Decorator::Tab*>(_tab);
	BRect& tabRect = tab->tabRect;
	if (tabRect.IsValid() == false)
		return false;

	if (location < 0)
		location = 0;

	float maxLocation
		= fRightBorder.right - fLeftBorder.left - tabRect.Width();
	if (CountTabs() > 1)
		maxLocation = fTitleBarRect.right - fLeftBorder.left - tabRect.Width();

	if (location > maxLocation)
		location = maxLocation;

	float delta = floor(location - tab->tabOffset);
	if (delta == 0.0)
		return false;

	// redraw old rect (1 pixel on the border must also be updated)
	BRect rect(tabRect);
	rect.bottom++;
	if (updateRegion != NULL)
		updateRegion->Include(rect);

	tabRect.OffsetBy(delta, 0);
	tab->tabOffset = (int32)location;
	_LayoutTabItems(_tab, tabRect);
	tab->tabLocation = maxLocation > 0.0 ? tab->tabOffset / maxLocation : 0.0;

	if (fTabList.CountItems() == 1)
		fTitleBarRect = tabRect;

	_CalculateTabsRegion();

	// redraw new rect as well
	rect = tabRect;
	rect.bottom++;
	if (updateRegion != NULL)
		updateRegion->Include(rect);

	return true;
}


bool
MacDecorator::_MoveTab(int32 from, int32 to, bool isMoving, BRegion* updateRegion)
{
	Decorator::Tab* toTab = _TabAt(to);
	if (toTab == NULL)
		return false;

	if (from < to) {
		fOldMovingTab.OffsetBy(toTab->tabRect.Width(), 0);
		toTab->tabRect.OffsetBy(-fOldMovingTab.Width(), 0);
	} else {
		fOldMovingTab.OffsetBy(-toTab->tabRect.Width(), 0);
		toTab->tabRect.OffsetBy(fOldMovingTab.Width(), 0);
	}

	toTab->tabOffset = uint32(toTab->tabRect.left - fLeftBorder.left);
	_LayoutTabItems(toTab, toTab->tabRect);

	_CalculateTabsRegion();

	if (updateRegion != NULL)
		updateRegion->Include(fTitleBarRect);
	return true;
}


// TODO: _SetSettings


// #pragma mark - Private methods


// Draw a mac-style button
void
MacDecorator::_DrawButton(Decorator::Tab* tab, bool direct, BRect rect, bool down)
{
	BRect buttonRect(rect);

	BPoint offset(rect.LeftTop()), pt2(rect.RightTop());

	// Topleft dark grey border
	pt2.x--;
	fDrawingEngine->SetHighColor(RGBColor(136, 136, 136));
	fDrawingEngine->StrokeLine(offset, pt2);

	pt2 = rect.LeftBottom();
	pt2.y--;
	fDrawingEngine->StrokeLine(offset, pt2);

	// Bottomright white border
	offset = rect.RightBottom();
	pt2 = rect.RightTop();
	pt2.y++;
	fDrawingEngine->SetHighColor(RGBColor(255, 255, 255));
	fDrawingEngine->StrokeLine(offset, pt2);

	pt2 = rect.LeftBottom();
	pt2.x++;
	fDrawingEngine->StrokeLine(offset, pt2);

	// Black outline
	buttonRect.InsetBy(1, 1);
	fDrawingEngine->SetHighColor(RGBColor(33, 33, 33));
	fDrawingEngine->StrokeRect(buttonRect);

	// Double-shaded button
	buttonRect.InsetBy(1, 1);
	fDrawingEngine->SetHighColor(RGBColor(140, 140, 140));
	fDrawingEngine->StrokeLine(buttonRect.RightBottom(), buttonRect.RightTop());
	fDrawingEngine->StrokeLine(buttonRect.RightBottom(), buttonRect.LeftBottom());
	fDrawingEngine->SetHighColor(RGBColor(206, 206, 206));
	fDrawingEngine->StrokeLine(buttonRect.LeftBottom(), buttonRect.LeftTop());
	fDrawingEngine->StrokeLine(buttonRect.LeftTop(), buttonRect.RightTop());
	fDrawingEngine->SetHighColor(RGBColor(255, 255, 255));
	fDrawingEngine->StrokeLine(buttonRect.LeftTop(), buttonRect.LeftTop());

	buttonRect.InsetBy(1, 1);
	_DrawBlendedRect(fDrawingEngine, buttonRect, !down);
}


/*!	\brief Draws a rectangle with a gradient.
  \param down The rectangle should be drawn recessed or not
*/
void
MacDecorator::_DrawBlendedRect(DrawingEngine* engine, BRect buttonRect, bool down)
{
	// figure out which colors to use
	RGBColor startColor, endColor;
	if (down) {
		startColor = fButtonLowColor;
		endColor = fFrameHighColor;
	} else {
		startColor = fButtonHighColor;
		endColor = fFrameLowerColor;
	}

	// fill
	BGradientLinear gradient;
	gradient.SetStart(buttonRect.LeftTop());
	gradient.SetEnd(buttonRect.RightBottom());
	gradient.AddColor(startColor, 0);
	gradient.AddColor(endColor, 255);

	engine->FillRect(buttonRect, gradient);
}


// #pragma mark - Janky Stuff...


inline float
TabDecorator::_DefaultTextOffset() const
{
	if (fTopTab->look == B_FLOATING_WINDOW_LOOK
			|| fTopTab->look == kLeftTitledWindowLook)
		return int32(fBorderWidth * 3.4f);
	return int32(fBorderWidth * 3.6f);
}


inline float
TabDecorator::_SingleTabOffsetAndSize(float& tabSize)
{
	float maxLocation;
	if (fTopTab->look != kLeftTitledWindowLook) {
		tabSize = fRightBorder.right - fLeftBorder.left;
	} else {
		tabSize = fBottomBorder.bottom - fTopBorder.top;
	}
	Decorator::Tab* tab = _TabAt(0);
	maxLocation = tabSize - tab->maxTabSize;
	if (maxLocation < 0)
		maxLocation = 0;

	return floorf(tab->tabLocation * maxLocation);
}


// #pragma mark - DecorAddOn


extern "C" DecorAddOn*
instantiate_decor_addon(image_id id, const char* name)
{
	return new (std::nothrow)MacDecorAddOn(id, name);
}
