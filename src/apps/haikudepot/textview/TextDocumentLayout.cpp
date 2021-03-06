/*
 * Copyright 2013-2015, Stephan Aßmus <superstippi@gmx.de>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "TextDocumentLayout.h"

#include <new>
#include <stdio.h>

#include <View.h>


class LayoutTextListener : public TextListener {
public:
	LayoutTextListener(TextDocumentLayout* layout)
		:
		fLayout(layout)
	{
	}

	virtual	void TextChanging(TextChangingEvent& event)
	{
	}

	virtual	void TextChanged(const TextChangedEvent& event)
	{
//		printf("TextChanged(%" B_PRIi32 ", %" B_PRIi32 ")\n",
//			event.FirstChangedParagraph(),
//			event.ChangedParagraphCount());
		// TODO: The event does not contain useful data. Make the event
		// system work so only the affected paragraphs are updated.
		// I think we need "first affected", "last affected" (both relative
		// to the original paragraph count), and than how many new paragraphs
		// are between these. From the difference in old number of paragraphs
		// inbetween and the new count, we know how many new paragraphs are
		// missing, and the rest in the range needs to be updated.
//		fLayout->InvalidateParagraphs(event.FirstChangedParagraph(),
//			event.ChangedParagraphCount());
		fLayout->Invalidate();
	}

private:
	TextDocumentLayout*	fLayout;
};



TextDocumentLayout::TextDocumentLayout()
	:
	fWidth(0.0f),
	fLayoutValid(false),

	fDocument(),
	fTextListener(new(std::nothrow) LayoutTextListener(this), true),
	fParagraphLayouts()
{
}


TextDocumentLayout::TextDocumentLayout(const TextDocumentRef& document)
	:
	fWidth(0.0f),
	fLayoutValid(false),

	fDocument(),
	fTextListener(new(std::nothrow) LayoutTextListener(this), true),
	fParagraphLayouts()
{
	SetTextDocument(document);
}


TextDocumentLayout::TextDocumentLayout(const TextDocumentLayout& other)
	:
	fWidth(other.fWidth),
	fLayoutValid(other.fLayoutValid),

	fDocument(other.fDocument),
	fTextListener(new(std::nothrow) LayoutTextListener(this), true),
	fParagraphLayouts(other.fParagraphLayouts)
{
	if (fDocument.IsSet())
		fDocument->AddListener(fTextListener);
}


TextDocumentLayout::~TextDocumentLayout()
{
	SetTextDocument(NULL);
}


void
TextDocumentLayout::SetTextDocument(const TextDocumentRef& document)
{
	if (fDocument == document)
		return;

	if (fDocument.IsSet())
		fDocument->RemoveListener(fTextListener);

	fDocument = document;
	_Init();
	fLayoutValid = false;

	if (fDocument.IsSet())
		fDocument->AddListener(fTextListener);
}


void
TextDocumentLayout::Invalidate()
{
	if (fDocument.IsSet())
		InvalidateParagraphs(0, fDocument->CountParagraphs());
}


void
TextDocumentLayout::InvalidateParagraphs(int32 start, int32 count)
{
	if (start < 0 || count == 0 || !fDocument.IsSet())
		return;

	fLayoutValid = false;

	while (count > 0) {
		const int32 paragraphCount = fDocument->CountParagraphs();
		if (start >= paragraphCount)
			break;
		const Paragraph& paragraph = fDocument->ParagraphAtIndex(start);
		if (start >= static_cast<int32>(fParagraphLayouts.size())) {
			ParagraphLayoutRef layout(new(std::nothrow) ParagraphLayout(
				paragraph), true);
			if (!layout.IsSet()) {
				fprintf(stderr, "TextDocumentLayout::InvalidateParagraphs() - "
					"out of memory\n");
				return;
			}
			try {
				fParagraphLayouts.push_back(ParagraphLayoutInfo(0.0f, layout));
			}
			catch (std::bad_alloc& ba) {
				fprintf(stderr, "bad_alloc when invalidating paragraphs\n");
				return;
			}
		} else {
			const ParagraphLayoutInfo& info = fParagraphLayouts[start];
			info.layout->SetParagraph(paragraph);
		}

		start++;
		count--;
	}

	// Remove any extra paragraph layouts
	while (fDocument->CountParagraphs()
			< static_cast<int32>(fParagraphLayouts.size()))
		fParagraphLayouts.erase(fParagraphLayouts.end() - 1);
}


void
TextDocumentLayout::SetWidth(float width)
{
	if (fWidth != width) {
		fWidth = width;
		fLayoutValid = false;
	}
}


float
TextDocumentLayout::Height()
{
	_ValidateLayout();

	float height = 0.0f;

	if (fParagraphLayouts.size() > 0) {
		const ParagraphLayoutInfo& lastLayout
			= fParagraphLayouts[fParagraphLayouts.size() - 1];
		height = lastLayout.y + lastLayout.layout->Height();
	}

	return height;
}


void
TextDocumentLayout::Draw(BView* view, const BPoint& offset,
	const BRect& updateRect)
{
	_ValidateLayout();

	int layoutCount = fParagraphLayouts.size();
	for (int i = 0; i < layoutCount; i++) {
		const ParagraphLayoutInfo& layout = fParagraphLayouts[i];
		BPoint location(offset.x, offset.y + layout.y);
		if (location.y > updateRect.bottom)
			break;
		if (location.y + layout.layout->Height() > updateRect.top)
			layout.layout->Draw(view, location);
	}
}


int32
TextDocumentLayout::LineIndexForOffset(int32 textOffset)
{
	int32 index = _ParagraphLayoutIndexForOffset(textOffset);
	if (index >= 0) {
		int32 lineIndex = 0;
		for (int32 i = 0; i < index; i++) {
			lineIndex += fParagraphLayouts[i].layout->CountLines();
		}

		const ParagraphLayoutInfo& info = fParagraphLayouts[index];
		return lineIndex + info.layout->LineIndexForOffset(textOffset);
	}

	return 0;
}


int32
TextDocumentLayout::FirstOffsetOnLine(int32 lineIndex)
{
	int32 paragraphOffset;
	int32 index = _ParagraphLayoutIndexForLineIndex(lineIndex, paragraphOffset);
	if (index >= 0) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[index];
		return info.layout->FirstOffsetOnLine(lineIndex) + paragraphOffset;
	}

	return 0;
}


int32
TextDocumentLayout::LastOffsetOnLine(int32 lineIndex)
{
	int32 paragraphOffset;
	int32 index = _ParagraphLayoutIndexForLineIndex(lineIndex, paragraphOffset);
	if (index >= 0) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[index];
		return info.layout->LastOffsetOnLine(lineIndex) + paragraphOffset;
	}

	return 0;
}


int32
TextDocumentLayout::CountLines()
{
	_ValidateLayout();

	int32 lineCount = 0;

	int32 count = fParagraphLayouts.size();
	for (int32 i = 0; i < count; i++) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[i];
		lineCount += info.layout->CountLines();
	}

	return lineCount;
}


void
TextDocumentLayout::GetLineBounds(int32 lineIndex, float& x1, float& y1,
	float& x2, float& y2)
{
	int32 paragraphOffset;
	int32 index = _ParagraphLayoutIndexForLineIndex(lineIndex, paragraphOffset);
	if (index >= 0) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[index];
		info.layout->GetLineBounds(lineIndex, x1, y1, x2, y2);
		y1 += info.y;
		y2 += info.y;
		return;
	}

	x1 = 0.0f;
	y1 = 0.0f;
	x2 = 0.0f;
	y2 = 0.0f;
}


void
TextDocumentLayout::GetTextBounds(int32 textOffset, float& x1, float& y1,
	float& x2, float& y2)
{
	int32 index = _ParagraphLayoutIndexForOffset(textOffset);
	if (index >= 0) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[index];
		info.layout->GetTextBounds(textOffset, x1, y1, x2, y2);
		y1 += info.y;
		y2 += info.y;
		return;
	}

	x1 = 0.0f;
	y1 = 0.0f;
	x2 = 0.0f;
	y2 = 0.0f;
}


int32
TextDocumentLayout::TextOffsetAt(float x, float y, bool& rightOfCenter)
{
	_ValidateLayout();

	int32 textOffset = 0;
	rightOfCenter = false;

	int32 paragraphs = fParagraphLayouts.size();
	for (int32 i = 0; i < paragraphs; i++) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[i];
		if (y > info.y + info.layout->Height()) {
			textOffset += info.layout->CountGlyphs();
			continue;
		}

		textOffset += info.layout->TextOffsetAt(x, y - info.y, rightOfCenter);
		break;
	}

	return textOffset;
}

// #pragma mark - private


void
TextDocumentLayout::_ValidateLayout()
{
	if (!fLayoutValid) {
		_Layout();
		fLayoutValid = true;
	}
}


void
TextDocumentLayout::_Init()
{
	fParagraphLayouts.clear();

	if (!fDocument.IsSet())
		return;

	int paragraphCount = fDocument->CountParagraphs();
	for (int i = 0; i < paragraphCount; i++) {
		const Paragraph& paragraph = fDocument->ParagraphAtIndex(i);
		ParagraphLayoutRef layout(new(std::nothrow) ParagraphLayout(paragraph),
			true);
		if (!layout.IsSet()) {
			fprintf(stderr, "TextDocumentLayout::_Layout() - out of memory\n");
			return;
		}
		try {
			fParagraphLayouts.push_back(ParagraphLayoutInfo(0.0f, layout));
		}
		catch (std::bad_alloc& ba) {
			fprintf(stderr, "bad_alloc when inititalizing the text document "
				"layout\n");
			return;
		}
	}
}


void
TextDocumentLayout::_Layout()
{
	float y = 0.0f;

	int layoutCount = fParagraphLayouts.size();
	for (int i = 0; i < layoutCount; i++) {
		ParagraphLayoutInfo info = fParagraphLayouts[i];
		const ParagraphStyle& style = info.layout->Style();

		if (i > 0)
			y += style.SpacingTop();

		fParagraphLayouts[i] = ParagraphLayoutInfo(y, info.layout);

		info.layout->SetWidth(fWidth);
		y += info.layout->Height() + style.SpacingBottom();
	}
}


int32
TextDocumentLayout::_ParagraphLayoutIndexForOffset(int32& textOffset)
{
	_ValidateLayout();

	int32 paragraphs = fParagraphLayouts.size();
	for (int32 i = 0; i < paragraphs - 1; i++) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[i];

		int32 length = info.layout->CountGlyphs();
		if (textOffset >= length) {
			textOffset -= length;
			continue;
		}

		return i;
	}

	if (paragraphs > 0) {
		const ParagraphLayoutInfo& info
			= fParagraphLayouts[fParagraphLayouts.size() - 1];

		// Return last paragraph if the textOffset is still within or
		// exactly behind the last valid offset in that paragraph.
		int32 length = info.layout->CountGlyphs();
		if (textOffset <= length)
			return paragraphs - 1;
	}

	return -1;
}

int32
TextDocumentLayout::_ParagraphLayoutIndexForLineIndex(int32& lineIndex,
	int32& paragraphOffset)
{
	_ValidateLayout();

	paragraphOffset = 0;
	int32 paragraphs = fParagraphLayouts.size();
	for (int32 i = 0; i < paragraphs; i++) {
		const ParagraphLayoutInfo& info = fParagraphLayouts[i];

		int32 lineCount = info.layout->CountLines();
		if (lineIndex >= lineCount) {
			lineIndex -= lineCount;
			paragraphOffset += info.layout->CountGlyphs();
			continue;
		}

		return i;
	}

	return -1;
}
