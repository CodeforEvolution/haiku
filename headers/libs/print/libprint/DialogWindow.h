/*
 * Copyright 2004 Michael Pfeiffer
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _DIALOG_WINDOW_H
#define _DIALOG_WINDOW_H


#include <OS.h>
#include <Window.h>


class DialogWindow : public BWindow {
public:
								DialogWindow(BRect frame, const char* title,
									window_type type, uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE);
								DialogWindow(BRect frame, const char* title,
									window_look look, window_feel feel,
									uint32 flags,
									uint32 workspace = B_CURRENT_WORKSPACE);

			status_t			Go();

			void				SetResult(status_t result);

			void				MessageReceived(BMessage* message);

			enum {
				kGetThreadId = 'dwti' // request thread id from window
			};

private:
			status_t           fPreviousResult;

	volatile status_t*			fResult;
				// Holds the result as long as fResult == NULL
};

#endif /* _DIALOG_WINDOW_H */
