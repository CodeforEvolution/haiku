/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "BlockingWindow.h"

#include <Alert.h>
#include <Debug.h>
#include <Message.h>
#include <TextView.h>

#include <string.h>


// #pragma mark -- EscapeMessageFilter


EscapeMessageFilter::EscapeMessageFilter(BWindow* window, int32 what)
	:
	BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE, '_KYD'),
	fWindow(window),
	fWhat(what)
{
}


filter_result
EscapeMessageFilter::Filter(BMessage* message, BHandler** target)
{
	int32 key;
	filter_result result = B_DISPATCH_MESSAGE;
	if (message->FindInt32("key", &key) == B_OK && key == 1) {
		fWindow->PostMessage(fWhat);
		result = B_SKIP_MESSAGE;
	}

	return result;
}


// #pragma mark -- HWindow


HWindow::HWindow(BRect frame, const char* title, window_type type, uint32 flags,
	uint32 workspace, uint32 escapeMessage)
	:
	BWindow(frame, title, type, flags, workspace)
{
	Init(escapeMessage);
}


HWindow::HWindow(BRect frame, const char *title, window_look look,
	window_feel feel, uint32 flags, uint32 workspace, uint32 escapeMessage)
	:
	BWindow(frame, title, look, feel, flags, workspace)
{
	Init(escapeMessage);
}


void
HWindow::Init(uint32 escapeMessage)
{
	AddShortcut('i', 0, new BMessage(B_ABOUT_REQUESTED));
	AddCommonFilter(new EscapeMessageFilter(this, escapeMessage));
}


void
HWindow::MessageReceived(BMessage* message)
{
	if (message->what == B_ABOUT_REQUESTED)
		AboutRequested();
	else
		BWindow::MessageReceived(message);
}


void
HWindow::AboutRequested()
{
	const char* aboutText = AboutText();
	if (aboutText == NULL)
		return;

	BAlert* aboutAlert = new BAlert("About", aboutText, "Cool");
	BTextView* textView = aboutAlert->TextView();
	if (textView != NULL) {
		rgb_color red = make_color(255, 0, 51, 255);
		rgb_color blue = make_color(0, 102, 255, 255);

		textView->SetStylable(true);
		char* text = (char*)textView->Text();
		char* s = text;

		// Set all Be in blue and red
		while ((s = strstr(s, "Be")) != NULL) {
			int32 i = s - text;
			textView->SetFontAndColor(i, i + 1, NULL, 0, &blue);
			textView->SetFontAndColor(i + 1, i + 2, NULL, 0, &red);
			s += 2;
		}

		// First text line
		s = strchr(text, '\n');
		BFont font;
		textView->GetFontAndColor(0, &font);
		font.SetSize(12);
		textView->SetFontAndColor(0, s - text + 1, &font, B_FONT_SIZE);
	};

	aboutAlert->SetFlags(aboutAlert->Flags() | B_CLOSE_ON_ESCAPE);
	aboutAlert->Go();
}


// #pragma mark -- BlockingWindow


BlockingWindow::BlockingWindow(BRect frame, const char* title, window_type type,
	uint32 flags, uint32 workspace, uint32 escapeMessage)
	:
	HWindow(frame, title, type, flags, workspace, escapeMessage)
{
	_Init(title);
}


BlockingWindow::BlockingWindow(BRect frame, const char* title, window_look look,
	window_feel feel, uint32 flags, uint32 workspace, uint32 escapeMessage)
	:
	HWindow(frame, title, look, feel, flags, workspace, escapeMessage)
{
	_Init(title);
}


BlockingWindow::~BlockingWindow()
{
	delete_sem(fExitSem);
}


void
BlockingWindow::_Init(const char* title)
{
	fUserQuitResult = B_OK;
	fResult = NULL;
	fExitSem = create_sem(0, title);
	fReadyToQuit = false;
}


bool
BlockingWindow::QuitRequested()
{
	if (fReadyToQuit)
		return true;

	// user requested to quit the window
	*fResult = fUserQuitResult;
	release_sem(fExitSem);

	return false;
}


void
BlockingWindow::Quit()
{
	fReadyToQuit = false; // Finally allow window to quit
	HWindow::Quit(); // and quit it
}


void
BlockingWindow::Quit(status_t result)
{
	if (fResult != NULL)
		*fResult = result;

	release_sem(fExitSem);
}


void
BlockingWindow::SetUserQuitResult(status_t result)
{
	fUserQuitResult = result;
}


status_t
BlockingWindow::Go()
{
	status_t result = B_ERROR;

	fResult = &result;
	Show();
	acquire_sem(fExitSem);

	// Here the window still exists, because QuitRequested returns false if
	// fReadyToQuit is false, now we can quit the window and am sure that the
	// window thread dies before this thread
	if (Lock())
		Quit();
	else
		ASSERT(false);
			// Should not reach here!!!

	// Here the window does not exist,
	// good to have the result in a local variable
	return result;
}
