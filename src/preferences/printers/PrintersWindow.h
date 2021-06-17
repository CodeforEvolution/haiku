/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PRINTERS_WINDOW_H
#define _PRINTERS_WINDOW_H


#include <Box.h>
#include <Window.h>


class Job;
class JobListView;
class PrinterItem;
class PrinterListView;
class PrintersWindow;
class ScreenSettings;
class SpoolFolder;


class PrintersWindow : public BWindow {
public:
								PrintersWindow(ScreenSettings* settings);
	virtual						~PrintersWindow();

			void				MessageReceived(BMessage* message);
			bool				QuitRequested();

			void				PrintTestPage(PrinterItem* printer);

			void				AddJob(SpoolFolder* folder, Job* job);
			void				RemoveJob(SpoolFolder* folder, Job* job);
			void				UpdateJob(SpoolFolder* folder, Job* job);

private:
			void				_BuildGUI();
			bool				_IsSelected(PrinterItem* printer);
			void				_UpdatePrinterButtons();
			void				_UpdateJobButtons();

private:
			ScreenSettings*		fSettings;

			PrinterListView*	fPrinterListView;
			BButton*			fMakeDefault;
			BButton*			fRemove;
			BButton*			fPrintTestPage;

			JobListView*		fJobListView;
			BButton*			fRestart;
			BButton*			fCancel;

			BBox*				fJobsBox;

			PrinterItem*		fSelectedPrinter;

			bool				fAddingPrinter;
};

#endif	/* _PRINTERS_WINDOW_H */
