/*
 * Copyright 2020, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "UserAlertWindow.h"

#include <File.h>
#include <FindDirectory.h>
#include <IconUtils.h>
#include <LayoutBuilder.h>
#include <ListItem.h>
#include <Path.h>
#include <Resources.h>
#include <View.h>


UserAlertItem::UserAlertItem(const char* title, const char* message,
	alert_type type)
	:
	BListItem(),
	fTitle(title),
	fMessage(message),
	fAlertType(type)
{
	fAlertIcon = _GetAlertIcon();
}


UserAlertItem::~UserAlertItem()
{
	delete fAlertIcon;	
}


void
UserAlertItem::DrawItem(BView* owner, BRect itemRect,
	bool drawEverything)
{
}


void
UserAlertItem::Update(BView* owner, const BFont* font)
{
	BListItem::Update(owner, font);
}


// Implementation borrowed from BAlert
BBitmap*
UserAlertItem::_GetAlertIcon()
{
	if (fAlertType == B_EMPTY_ALERT)
		return NULL;

	// The icons are in the app_server resources
	BBitmap* icon = NULL;
	BPath path;
	status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
	if (status != B_OK) {
		return NULL;
	}

	path.Append("app_server");
	BFile file;
	status = file.SetTo(path.Path(), B_READ_ONLY);
	if (status != B_OK) {
		return NULL;
	}

	BResources resources;
	status = resources.SetTo(&file);
	if (status != B_OK) {
		return NULL;
	}

	// Which icon are we trying to load?
	const char* iconName;
	switch (fAlertType) {
		case B_INFO_ALERT:
			iconName = "info";
			break;
		case B_IDEA_ALERT:
			iconName = "idea";
			break;
		case B_WARNING_ALERT:
			iconName = "warn";
			break;
		case B_STOP_ALERT:
			iconName = "stop";
			break;

		default:
			// Alert type is either invalid or B_EMPTY_ALERT;
			// either way, we're not going to load an icon
			return NULL;
	}

	int32 iconSize = 32 * _IconLayoutScale();
	// Allocate the icon bitmap
	icon = new(std::nothrow) BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1),
		0, B_RGBA32);
	if (icon == NULL || icon->InitCheck() < B_OK) {
		delete icon;
		return NULL;
	}

	// Load the raw icon data
	size_t size = 0;
	const uint8* rawIcon;

	// Try to load vector icon
	rawIcon = (const uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE,
		iconName, &size);
	if (rawIcon != NULL
		&& BIconUtils::GetVectorIcon(rawIcon, size, icon) == B_OK) {
		return icon;
	}

	// Fall back to bitmap icon
	rawIcon = (const uint8*)resources.LoadResource(B_LARGE_ICON_TYPE,
		iconName, &size);
	if (rawIcon == NULL) {
		delete icon;
		return NULL;
	}

	// Handle color space conversion
	if (icon->ColorSpace() != B_CMAP8) {
		BIconUtils::ConvertFromCMAP8(rawIcon, iconSize, iconSize,
			iconSize, icon);
	}

	return icon;	
}


// Implementation borrowed from BStripeView
int32
UserAlertItem::_IconLayoutScale() 
{
	return max_c(1, ((int32)be_plain_font->Size() + 15) / 16);
}


UserAlertWindow::UserAlertWindow()
	:
	BWindow(BRect(), "Debugger alerts", B_FLOATING_WINDOW,
		B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fAlertListView(NULL),
	fClearAlertsButton(NULL)
{
	fAlertListView = new BListView("Alert list view");
	fClearAlertsButton = new BButton("Clear",
		new BMessage(kMsgClearAlerts));
		
	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_ITEM_SPACING)
		.Add(fAlertListView)
		.Add(fClearAlertsButton)
	.End();
}


void
UserAlertWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgClearAlerts:
		{
			fAlertListView->MakeEmpty();
			break;
		}

		default:
			BWindow::MessageReceived(message);	
	}
}


void
UserAlertWindow::AddAlert(const char* title, const char* message,
	alert_type type)
{
	UserAlertItem* item = new UserAlertItem(title, message, type);
	fAlertListView->AddItem(item);
}
