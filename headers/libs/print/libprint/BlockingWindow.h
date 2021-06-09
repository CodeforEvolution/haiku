/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _BLOCKING_WINDOW_H
#define _BLOCKING_WINDOW_H


#include <AppDefs.h>
#include <MessageFilter.h>
#include <Window.h>


class BHandler;
class BMessage;


class EscapeMessageFilter : public BMessageFilter {
public:
								EscapeMessageFilter(BWindow* window,
									int32 what);
			filter_result		Filter(BMessage* message, BHandler** target);

private:
			BWindow*			fWindow;
			int32				fWhat;
};


class HWindow : public BWindow {
public:
								HWindow(BRect frame, const char* title,
									window_type type, uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE,
									uint32 escapeMessage = B_QUIT_REQUESTED);
								HWindow(BRect frame, const char* title,
									window_look look, window_feel feel,
									uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE,
									uint32 escapeMessage = B_QUIT_REQUESTED);
	virtual						~HWindow() {}

	virtual void				MessageReceived(BMessage* message);
	virtual void				AboutRequested();
	virtual const char*			AboutText() const { return NULL; }

protected:
			void				Init(uint32 escapeMessage);
};


class BlockingWindow : public HWindow {
public:
								BlockingWindow(BRect frame, const char* title,
									window_type type, uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE,
									uint32 escapeMessage = B_QUIT_REQUESTED);
								BlockingWindow(BRect frame, const char* title,
									window_look look, window_feel feel,
									uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE,
									uint32 escapeMessage = B_QUIT_REQUESTED);
	virtual						~BlockingWindow();

	virtual bool				QuitRequested();
	// Quit() is called by child class with result code
			void				Quit(status_t result);
	// Show window and wait for it to quit, returns result code
	virtual status_t			Go();
	// Or quit window e.g. something went wrong in constructor
	virtual void				Quit();
	// Sets the result that is returned when the user closes the window.
	// Default is B_OK.
			void				SetUserQuitResult(status_t result);

private:
			void				_Init(const char* title);

private:
			status_t			fUserQuitResult;
			bool				fReadyToQuit;
			sem_id				fExitSem;
			status_t*			fResult;
};

#endif	/* _BLOCKING_WINDOW_H */
