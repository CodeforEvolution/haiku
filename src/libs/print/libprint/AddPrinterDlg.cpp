/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "AddPrinterDlg.h"

#include <LayoutBuilder.h>

#include "PrinterCap.h"
#include "PrinterData.h"


enum MSGS {
	kMsgCancel = 1,
	kMsgOK,
	kMsgProtocolClassChanged,
};


ProtocolClassItem::ProtocolClassItem(const ProtocolClassCap* cap)
	:
	BStringItem(cap->fLabel.String()),
	fProtocolClassCap(cap)
{
}


int
ProtocolClassItem::GetProtocolClass() const
{
	return fProtocolClassCap->fProtocolClass;
}


const char*
ProtocolClassItem::GetDescription() const
{
	return fProtocolClassCap->fDescription.String();
}


AddPrinterView::AddPrinterView(PrinterData* printerData,
	const PrinterCap* printerCap)
	:
	BView("AddPrinterView", B_WILL_DRAW),
	fPrinterData(printerData),
	fPrinterCap(printerCap)
{
}


AddPrinterView::~AddPrinterView()
{
}


void
AddPrinterView::AttachedToWindow()
{
	// Protocol class box
	BBox* protocolClassBox = new BBox("protocolClassBox");
	protocolClassBox->SetLabel("Protocol classes:");

	// Protocol class
	fProtocolClassList = new BListView("protocolClassListView");
	fProtocolClassList->SetExplicitMinSize(BSize(500, 200));

	BScrollView* protocolClassScroller = new BScrollView(
		"protocolClassListScroller", fProtocolClassList, 0, false, true,
		B_NO_BORDER);
	fProtocolClassList->SetSelectionMessage(
		new BMessage(kMsgProtocolClassChanged));
	fProtocolClassList->SetTarget(this);

	protocolClassBox->AddChild(protocolClassScroller);

	int count = fPrinterCap->CountCap(PrinterCap::kProtocolClass);
	ProtocolClassCap** protocolClasses =
		(ProtocolClassCap**)fPrinterCap->GetCaps(PrinterCap::kProtocolClass);
	while (count--) {
		const ProtocolClassCap* protocolClass = *protocolClasses;

		BStringItem* item = new ProtocolClassItem(protocolClass);
		fProtocolClassList->AddItem(item);

		if (protocolClass->fIsDefault) {
			int index = fProtocolClassList->IndexOf(item);
			fProtocolClassList->Select(index);
		}

		protocolClasses++;
	}

	// Description of protocol class box
	BBox* descriptionBox = new BBox("descriptionBox");
	descriptionBox->SetLabel("Description:");

	// Description of protocol class
	fDescription = new BTextView("description");
	fDescription->SetExplicitMinSize(BSize(200, 200));
	fDescription->SetViewColor(descriptionBox->ViewColor());

	BScrollView* descriptionScroller = new BScrollView("descriptionScroller",
		fDescription, 0, false, true, B_NO_BORDER);
	fDescription->MakeEditable(false);

	descriptionBox->AddChild(descriptionScroller);

	// Separator line
	BBox* separator = new BBox("separator");
	separator->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, 1));

	// Buttons
	BButton* cancelButton = new BButton("cancelButton", "Cancel",
		new BMessage(kMsgCancel));
	BButton* okButton = new BButton("okButton", "OK", new BMessage(kMsgOK));
	okButton->MakeDefault(true);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(protocolClassBox)
		.Add(descriptionBox)
		.AddGlue()
		.Add(separator)
		.AddGroup(B_HORIZONTAL, 10, 1.0f)
			.AddGlue()
			.Add(cancelButton)
			.Add(okButton)
		.End()
		.SetInsets(0, 0, 0, 0);

	// Update description
	BMessage updateDescription(kMsgProtocolClassChanged);
	MessageReceived(&updateDescription);
}


ProtocolClassItem*
AddPrinterView::_CurrentSelection()
{
	int selected = fProtocolClassList->CurrentSelection();
	if (selected >= 0)
		return (ProtocolClassItem*)fProtocolClassList->ItemAt(selected);

	return NULL;
}


void
AddPrinterView::MessageReceived(BMessage* message)
{
	if (message->what == kMsgProtocolClassChanged) {
		ProtocolClassItem* item = _CurrentSelection();
		if (item != NULL)
			fDescription->SetText(item->GetDescription());
	} else
		BView::MessageReceived(message);
}


void
AddPrinterView::Save()
{
	ProtocolClassItem* item = _CurrentSelection();
	if (item != NULL) {
		fPrinterData->SetProtocolClass(item->GetProtocolClass());
		fPrinterData->Save();
	}
}


AddPrinterDlg::AddPrinterDlg(PrinterData* printerData,
	const PrinterCap* printerCap)
	:
	DialogWindow(BRect(100, 100, 120, 120), "Add Printer", B_TITLED_WINDOW_LOOK,
		B_MODAL_APP_WINDOW_FEEL, B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE |
		B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{
	SetResult(B_ERROR);

	fAddPrinterView = new AddPrinterView(printerData, printerCap);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(fAddPrinterView)
		.SetInsets(10, 10, 10, 10);
}


void
AddPrinterDlg::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgOK:
		{
			fAddPrinterView->Save();
			SetResult(B_NO_ERROR);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}

		case kMsgCancel:
			PostMessage(B_QUIT_REQUESTED);
			break;

		default:
			DialogWindow::MessageReceived(message);
	}
}
