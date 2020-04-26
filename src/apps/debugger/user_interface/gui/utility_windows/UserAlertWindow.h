/*
 * Copyright 2018, Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef USER_ALERT_WINDOW_H
#define USER_ALERT_WINDOW_H


#include <Alert.h>
#include <Bitmap.h>
#include <Button.h>
#include <ListItem.h>
#include <ListView.h>
#include <String.h>
#include <Window.h>


static const uint32 kMsgClearAlerts = 'iCLA';


class UserAlertItem : public BListItem {
public:
								UserAlertItem(const char* title,
									const char* message,
									alert_type type);
								~UserAlertItem();
								
	virtual void				DrawItem(BView* owner, BRect itemRect,
									bool drawEverything);
	virtual void				Update(BView* owner, const BFont* font);

private:
			BBitmap*			_GetAlertIcon();
			int32				_IconLayoutScale();

			BString				fTitle;
			BString				fMessage;
			alert_type			fAlertType;
			BBitmap*			fAlertIcon;
};


class UserAlertWindow : public BWindow {
public:
								UserAlertWindow();
								
	virtual	void				MessageReceived(BMessage* message);
								
			void				AddAlert(const char* title,
									const char* message,
									alert_type type);

private:
			BListView*			fAlertListView;
			BButton*			fClearAlertsButton;
};


#endif // USER_ALERT_WINDOW_H
