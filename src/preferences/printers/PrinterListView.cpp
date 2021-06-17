/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "PrinterListView.h"

#include <Application.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <Directory.h>
#include <IconUtils.h>
#include <Locale.h>
#include <Mime.h>
#include <NodeInfo.h>
#include <Resources.h>
#include <String.h>
#include <StringFormat.h>

#include "Messages.h"
#include "Globals.h"
#include "pr_server.h"
#include "PrintersWindow.h"
#include "SpoolFolder.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PrinterListView"


// #pragma mark -- PrinterListView

PrinterListView::PrinterListView(BRect frame)
	:
	BListView(frame, "printers_list", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL,
		B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE | B_FULL_UPDATE_ON_RESIZE),
	fFolder(NULL),
	fActivePrinter(NULL)
{
	fLayoutData.fLeftColumnMaximumWidth = 100;
	fLayoutData.fRightColumnMaximumWidth = 100;
}


PrinterListView::~PrinterListView()
{
	while (!IsEmpty())
		delete RemoveItem((int32)0);
}


void
PrinterListView::BuildPrinterList()
{
	// clear list
	while (!IsEmpty())
		delete RemoveItem((int32)0);

	// Find directory containing printer definition nodes
	BPath path;
	if (find_directory(B_USER_PRINTERS_DIRECTORY, &path) != B_OK)
		return;

	BDirectory dir(path.Path());
	if (dir.InitCheck() != B_OK)
		return;

	BEntry entry;
	while(dir.GetNextEntry(&entry) == B_OK) {
		BDirectory printer(&entry);
		_AddPrinter(printer, false);
	}

	_LayoutPrinterItems();
}


void
PrinterListView::AttachedToWindow()
{
	BListView::AttachedToWindow();

	SetSelectionMessage(new BMessage(kMsgPrinterSelected));
	SetInvocationMessage(new BMessage(kMsgMakeDefaultPrinter));
	SetTarget(Window());

	BPath path;
	if (find_directory(B_USER_PRINTERS_DIRECTORY, &path) != B_OK)
		return;

	BDirectory dir(path.Path());
	if (dir.InitCheck() != B_OK) {
		// directory has to exist in order to start watching it
		if (create_directory(path.Path(), 0777) != B_OK)
			return;
		dir.SetTo(path.Path());
	}

	fFolder = new FolderWatcher(Window(), dir, true);
	fFolder->SetListener(this);

	BuildPrinterList();

	// Select active printer
	BString activePrinterName(ActivePrinterName());
	for (int32 index = 0; index < CountItems(); index++) {
		PrinterItem* item = dynamic_cast<PrinterItem*>(ItemAt(index));
		if (item != NULL && item->Name() == activePrinterName) {
			Select(index);
			fActivePrinter = item;
			break;
		}
	}
}


bool
PrinterListView::QuitRequested()
{
	delete fFolder;
	return true;
}


void
PrinterListView::UpdateItem(PrinterItem* item)
{
	item->UpdatePendingJobs();
	InvalidateItem(IndexOf(item));
}


PrinterItem*
PrinterListView::ActivePrinter() const
{
	return fActivePrinter;
}


void
PrinterListView::SetActivePrinter(PrinterItem* item)
{
	fActivePrinter = item;
}


PrinterItem*
PrinterListView::SelectedItem() const
{
	return dynamic_cast<PrinterItem*>(ItemAt(CurrentSelection()));
}


// FolderListener interface

void
PrinterListView::EntryCreated(node_ref* node, entry_ref* entry)
{
	BDirectory printer(node);
	_AddPrinter(printer, true);
}


void
PrinterListView::EntryRemoved(node_ref* node)
{
	PrinterItem* item = _FindItem(node);
	if (item != NULL) {
		if (item == fActivePrinter)
			fActivePrinter = NULL;

		RemoveItem(item);
		delete item;
	}
}


void
PrinterListView::AttributeChanged(node_ref* node)
{
	BDirectory printer(node);
	_AddPrinter(printer, true);
}


// Private methods

void
PrinterListView::_AddPrinter(BDirectory& printer, bool calculateLayout)
{
	BString state;
	node_ref node;
		// If the entry is a directory
	if (printer.InitCheck() == B_OK
		&& printer.GetNodeRef(&node) == B_OK
		&& _FindItem(&node) == NULL
		&& printer.ReadAttrString(PSRV_PRINTER_ATTR_STATE, &state) == B_OK
		&& state == "free") {
			// Check its mime type for a spool director
		BNodeInfo info(&printer);
		char buffer[256];

		if (info.GetType(buffer) == B_OK
			&& strcmp(buffer, PSRV_PRINTER_MIMETYPE) == 0) {
				// Yes, it is a printer definition node
			AddItem(new PrinterItem(static_cast<PrintersWindow*>(Window()),
				printer, fLayoutData));
			if (calculateLayout)
				_LayoutPrinterItems();
		}
	}
}


void
PrinterListView::_LayoutPrinterItems()
{
	float& leftColumnMaximumWidth = fLayoutData.fLeftColumnMaximumWidth;
	float& rightColumnMaximumWidth = fLayoutData.fRightColumnMaximumWidth;

	for (int32 index = 0; index < CountItems(); index++) {
		PrinterItem* item = static_cast<PrinterItem*>(ItemAt(index));

		float leftColumnWidth = 0;
		float rightColumnWidth = 0;
		item->GetColumnWidth(this, leftColumnWidth, rightColumnWidth);

		leftColumnMaximumWidth = MAX(leftColumnMaximumWidth,
			leftColumnWidth);
		rightColumnMaximumWidth = MAX(rightColumnMaximumWidth,
			rightColumnWidth);
	}

	Invalidate();
}


PrinterItem*
PrinterListView::_FindItem(node_ref* node) const
{
	for (int32 index = CountItems() - 1; index >= 0; index--) {
		PrinterItem* item = dynamic_cast<PrinterItem*>(ItemAt(index));
		node_ref ref;
		if (item != NULL && item->Node()->GetNodeRef(&ref) == B_OK
			&& ref == *node)
			return item;
	}

	return NULL;
}


// #pragma mark -- PrinterItem

BBitmap* PrinterItem::sIcon = NULL;
BBitmap* PrinterItem::sSelectedIcon = NULL;


PrinterItem::PrinterItem(PrintersWindow* window, const BDirectory& node,
		PrinterListLayoutData& layoutData)
	:
	BListItem(0, false),
	fFolder(NULL),
	fNode(node),
	fLayoutData(layoutData)
{
	BRect rect(0, 0, B_LARGE_ICON - 1, B_LARGE_ICON - 1);
	if (sIcon == NULL) {
		sIcon = new BBitmap(rect, B_RGBA32);

		BMimeType type(PSRV_PRINTER_MIMETYPE);
		type.GetIcon(sIcon, B_LARGE_ICON);
	}

	if (sIcon != NULL && sIcon->IsValid() && sSelectedIcon == NULL) {
		const float checkMarkIconSize = 20.0;
		BBitmap* checkMark = _LoadVectorIcon("check_mark_icon",
			checkMarkIconSize);
		if (checkMark != NULL && checkMark->IsValid()) {
			sSelectedIcon = new BBitmap(rect, B_RGBA32, true);
			if (sSelectedIcon != NULL && sSelectedIcon->IsValid()) {
				// Draw check mark at bottom left over printer icon
				BView* view = new BView(rect, "offscreen", B_FOLLOW_ALL,
					B_WILL_DRAW);
				float y = rect.Height() - checkMark->Bounds().Height();
				sSelectedIcon->Lock();
				sSelectedIcon->AddChild(view);

				view->DrawBitmap(sIcon);
				view->SetDrawingMode(B_OP_ALPHA);
				view->DrawBitmap(checkMark, BPoint(0, y));
				view->Sync();
				view->RemoveSelf();

				sSelectedIcon->Unlock();
				delete view;
			}
		}

		delete checkMark;
	}

	// Get Name of printer
	_GetStringProperty(PSRV_PRINTER_ATTR_PRINTER_NAME, fName);
	_GetStringProperty(PSRV_PRINTER_ATTR_COMMENTS, fComments);
	_GetStringProperty(PSRV_PRINTER_ATTR_TRANSPORT, fTransport);
	_GetStringProperty(PSRV_PRINTER_ATTR_TRANSPORT_ADDRESS, fTransportAddress);
	_GetStringProperty(PSRV_PRINTER_ATTR_DRIVER_NAME, fDriverName);

	BPath path;
	if (find_directory(B_USER_PRINTERS_DIRECTORY, &path) != B_OK)
		return;

	// Setup spool folder
	path.Append(fName.String());
	BDirectory dir(path.Path());
	if (dir.InitCheck() == B_OK) {
		fFolder = new SpoolFolder(window, this, dir);
		UpdatePendingJobs();
	}
}


PrinterItem::~PrinterItem()
{
	delete fFolder;
}


void
PrinterItem::GetColumnWidth(BView* view, float& leftColumn, float& rightColumn)
{
	BFont font;
	view->GetFont(&font);

	leftColumn = font.StringWidth(fName.String());
	leftColumn = MAX(leftColumn, font.StringWidth(fDriverName.String()));

	rightColumn = font.StringWidth(fPendingJobs.String());
	rightColumn = MAX(rightColumn, font.StringWidth(fTransport.String()));
	rightColumn = MAX(rightColumn, font.StringWidth(fComments.String()));
}


void
PrinterItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner,font);

	font_height height;
	font->GetHeight(&height);

	SetHeight((height.ascent + height.descent + height.leading) * 3.0 + 8.0);
}


bool
PrinterItem::Remove(BListView* view)
{
	BMessenger messenger;
	if (GetPrinterServerMessenger(messenger) == B_OK) {
		BMessage script(B_DELETE_PROPERTY);
		script.AddSpecifier("Printer", view->IndexOf(this));

		BMessage reply;
		if (messenger.SendMessage(&script,&reply) == B_OK)
			return true;
	}

	return false;
}


void
PrinterItem::DrawItem(BView* owner, BRect itemRect, bool complete)
{
	BListView* list = dynamic_cast<BListView*>(owner);
	if (list == NULL)
		return;

	BFont font;
	owner->GetFont(&font);

	font_height height;
	font.GetHeight(&height);

	float fontHeight = height.ascent + height.descent + height.leading;

	BRect bounds = list->ItemFrame(list->IndexOf(this));

	rgb_color color = owner->ViewColor();
	rgb_color oldLowColor = owner->LowColor();
	rgb_color oldHighColor = owner->HighColor();

	if (IsSelected())
		color = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);

	owner->SetLowColor(color);
	owner->SetHighColor(color);

	owner->FillRect(bounds);

	owner->SetLowColor(oldLowColor);
	owner->SetHighColor(oldHighColor);

	float iconColumnWidth = B_LARGE_ICON + 8.0;
	float x = iconColumnWidth;
	BPoint iconPoint(bounds.LeftTop() + BPoint(2.0, 2.0));
	BPoint namePoint(iconPoint + BPoint(x, fontHeight));
	BPoint driverPoint(iconPoint + BPoint(x, fontHeight * 2.0));
	BPoint defaultPoint(iconPoint + BPoint(x, fontHeight * 3.0));
	BPoint transportPoint(iconPoint + BPoint(x, fontHeight * 3.0));

	float totalWidth = bounds.Width() - iconColumnWidth;
	float maximumWidth = fLayoutData.fLeftColumnMaximumWidth +
		fLayoutData.fRightColumnMaximumWidth;
	float width;
	if (totalWidth < maximumWidth) {
		width = fLayoutData.fRightColumnMaximumWidth * totalWidth /
			maximumWidth;
	} else
		width = fLayoutData.fRightColumnMaximumWidth;

	BPoint pendingPoint(bounds.right - width - 8.0, namePoint.y);
	BPoint commentPoint(bounds.right - width - 8.0, driverPoint.y);

	drawing_mode mode = owner->DrawingMode();
	owner->SetDrawingMode(B_OP_ALPHA);

	if (IsActivePrinter()) {
		if (sSelectedIcon != NULL && sSelectedIcon->IsValid())
			owner->DrawBitmap(sSelectedIcon, iconPoint);
		else
			owner->DrawString(B_TRANSLATE("Default Printer"), defaultPoint);
	} else {
		if (sIcon != NULL && sIcon->IsValid())
			owner->DrawBitmap(sIcon, iconPoint);
	}

	owner->SetDrawingMode(B_OP_OVER);

	// Left of item
	BString s = fName;
	owner->SetFont(be_bold_font);
	owner->TruncateString(&s, B_TRUNCATE_MIDDLE, pendingPoint.x - namePoint.x);
	owner->DrawString(s.String(), s.Length(), namePoint);
	owner->SetFont(&font);

	s = B_TRANSLATE("Driver: %driver%");
	s.ReplaceFirst("%driver%", fDriverName);
	owner->TruncateString(&s, B_TRUNCATE_END, commentPoint.x - driverPoint.x);
	owner->DrawString(s.String(), s.Length(), driverPoint);


	if (fTransport.Length() > 0) {
		s = B_TRANSLATE("Transport: %transport% %transport_address%");
		s.ReplaceFirst("%transport%", fTransport);
		s.ReplaceFirst("%transport_address%", fTransportAddress);
		owner->TruncateString(&s, B_TRUNCATE_BEGINNING, totalWidth);
		owner->DrawString(s.String(), s.Length(), transportPoint);
	}

	// Right of item
	s = fPendingJobs;
	owner->TruncateString(&s, B_TRUNCATE_END, bounds.Width() - pendingPoint.x);
	owner->DrawString(s.String(), s.Length(), pendingPoint);

	s = fComments;
	owner->TruncateString(&s, B_TRUNCATE_MIDDLE,
		bounds.Width() - commentPoint.x);
	owner->DrawString(s.String(), s.Length(), commentPoint);

	owner->SetDrawingMode(mode);
}


bool
PrinterItem::IsActivePrinter() const
{
	return fName == ActivePrinterName();
}


bool
PrinterItem::HasPendingJobs() const
{
	return fFolder && fFolder->CountJobs() > 0;
}


SpoolFolder*
PrinterItem::Folder() const
{
	return fFolder;
}


BDirectory*
PrinterItem::Node()
{
	return &fNode;
}


void
PrinterItem::UpdatePendingJobs()
{
	uint32 pendingJobs = 0;
	if (fFolder != NULL)
		pendingJobs = fFolder->CountJobs();

	static BStringFormat format(B_TRANSLATE("{0, plural,"
		"=0{No pending jobs}"
		"=1{1 pending job}"
		"other{# pending jobs}}"));

	format.Format(fPendingJobs, pendingJobs);
}


void
PrinterItem::_GetStringProperty(const char* propName, BString& outString)
{
	fNode.ReadAttrString(propName, &outString);
}


BBitmap*
PrinterItem::_LoadVectorIcon(const char* resourceName, float iconSize)
{
	size_t dataSize;
	BResources* resources = BApplication::AppResources();
	const void* data = resources->LoadResource(B_VECTOR_ICON_TYPE,
		resourceName, &dataSize);

	if (data != NULL) {
		BBitmap* iconBitmap = new BBitmap(BRect(0, 0, iconSize - 1,
			iconSize - 1), 0, B_RGBA32);
		if (BIconUtils::GetVectorIcon(reinterpret_cast<const uint8*>(data),
				dataSize, iconBitmap) == B_OK)
			return iconBitmap;
		else
			delete iconBitmap;
	}

	return NULL;
}
