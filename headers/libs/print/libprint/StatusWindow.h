/*
 * Copyright 2005-2006, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Dr.H.Reh
 */
#ifndef _STATUS_WINDOW_H
#define _STATUS_WINDOW_H


#include <StatusBar.h>
#include <String.h>
#include <Window.h>


class StatusWindow : public BWindow {
public:
								StatusWindow(bool oddPages, bool evenPages,
									uint32 firstPage, uint32 numPages,
									uint32 docCopies, uint32 nup);
								~StatusWindow();

	virtual	void				MessageReceived(BMessage* message);

			void				ResetStatusBar();
			bool				UpdateStatusBar(uint32 page, uint32 copy);
			void				SetPageCopies(uint32 copies);

private:
			BStatusBar*			fStatusBar;
			BButton*			fCancelButton;
			BButton*			fHideButton;
			bool				fCancelled;
			bool				fDocumentCopy;
			uint32				fNops;
			uint32				fFirstPage;
			uint32				fCopies;
			uint32				fDocCopies;
			float				fStatusDelta;
			float				fDelta;
};

#endif /* _STATUS_WINDOW_H */
