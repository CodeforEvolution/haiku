/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _CONFIG_WINDOW_H
#define _CONFIG_WINDOW_H


#include <InterfaceKit.h>
#include <Window.h>

#include "BeUtils.h"
#include "ObjectList.h"
#include "Printer.h"


enum config_setup_kind {
	kPageSetup,
	kJobSetup,
};


class ConfigWindow : public BWindow {
	enum {
		MSG_PAGE_SETUP       = 'cwps',
		MSG_JOB_SETUP        = 'cwjs',
		MSG_PRINTER_SELECTED = 'cwpr',
		MSG_OK               = 'cwok',
		MSG_CANCEL           = 'cwcl',
	};

public:
								ConfigWindow(config_setup_kind kind,
									Printer* defaultPrinter, BMessage* settings,
									AutoReply* sender);
								~ConfigWindow();

			void				Go();

			void				MessageReceived(BMessage* message);
			void				AboutRequested();
			void				FrameMoved(BPoint point);

	static	BRect				GetWindowFrame();
	static	void				SetWindowFrame(BRect frame);

private:
			BButton* 			_AddPictureButton(BView* panel,
									const char* name, const char* picture,
									uint32 what);
			void				_PrinterForMimeType();
			void				_SetupPrintersMenu(BMenu* menu);
			void				_UpdateAppSettings(const char* mime,
									const char* printer);
			void				_UpdateSettings(bool read);
			void				_UpdateUI();
			void				_Setup(config_setup_kind kind);

private:
			config_setup_kind	fKind;
			Printer*			fDefaultPrinter;
			BMessage*			fSettings;
			AutoReply*			fSender;
			BString				fSenderMimeType;

			BString				fPrinterName;
			Printer*			fCurrentPrinter;
			BMessage			fPageSettings;
			BMessage			fJobSettings;

			sem_id				fFinished;

			BMenuField*			fPrinters;
			BButton*			fPageSetup;
			BButton*			fJobSetup;
			BButton*			fOk;
			BStringView*		fPageFormatText;
			BStringView*		fJobSetupText;
};

#endif /* _CONFIG_WINDOW_H */
