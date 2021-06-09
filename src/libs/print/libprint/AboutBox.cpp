/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "AboutBox.h"

#include <Button.h>
#include <View.h>
#include <Window.h>


enum {
	kMsgOK = 'AbOK'
};


class AboutBoxView : public BView {
public:
								AboutBoxView(BRect frame,
									const char* driverName, const char* version,
									const char* copyright);

	virtual void				Draw(BRect updateRect);
	virtual void				AttachedToWindow();

private:
			BString				fDriverName;
			BString				fVersion;
			BString				fCopyright;
};


AboutBoxView::AboutBoxView(BRect rect, const char* driverName,
	const char* version, const char* copyright)
	:
	BView(rect, "", B_FOLLOW_ALL, B_WILL_DRAW),
	fDriverName(driverName),
	fVersion(version),
	fCopyright(copyright)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetDrawingMode(B_OP_SELECT);
}


void
AboutBoxView::Draw(BRect updateRect)
{
	SetHighColor(0, 0, 0);
	DrawString(fDriverName, BPoint(10.0f, 16.0f));
	DrawString(" Driver for ");

	SetHighColor(0, 0, 0xff);
	DrawString("B");

	SetHighColor(0xff, 0, 0);
	DrawString("e");

	SetHighColor(0, 0, 0);
	DrawString("OS  Version ");
	DrawString(fVersion);
	DrawString(fCopyright, BPoint(10.0f, 30.0f));
}


void
AboutBoxView::AttachedToWindow()
{
	BButton* button = new BButton(BRect(110, 50, 175, 55), "", "OK",
		new BMessage(kMsgOK));
	AddChild(button);
	button->MakeDefault(true);
}


class AboutBoxWindow : public BWindow {
public:
	AboutBoxWindow(BRect frame, const char* driverName, const char* version,
		const char* copyright);

	virtual void MessageReceived(BMessage* message);
	virtual	bool QuitRequested();
};


AboutBoxWindow::AboutBoxWindow(BRect frame, const char* driverName,
	const char* version, const char* copyright)
	:
	BWindow(frame, "", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE |
		B_CLOSE_ON_ESCAPE)
{
	BString title;
	title.SetToFormat("About %s Driver", driverName);

	SetTitle(title);

	AddChild(new AboutBoxView(Bounds(), driverName, version, copyright));
}


void
AboutBoxWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgOK:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;

		default:
			BWindow::MessageReceived(message);
	}
}


bool
AboutBoxWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


AboutBox::AboutBox(const char* signature, const char* driverName,
	const char* version, const char* copyright)
	:
	BApplication(signature)
{
	AboutBoxWindow* window = new AboutBoxWindow(BRect(100, 80, 400, 170),
		driverName, version, copyright);
	window->Show();
}
