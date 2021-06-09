/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _PAGE_SETUP_DLG_H
#define _PAGE_SETUP_DLG_H


#include <View.h>

#include "DialogWindow.h"
#include "JobData.h"


class BRadioButton;
class BPopUpMenu;
class JobData;
class PaperCap;
class PrinterData;
class PrinterCap;
class MarginView;


class PageSetupView : public BView {
public:
								PageSetupView(JobData* job_data,
									PrinterData* printer_data,
									const PrinterCap* printer_cap);
								~PageSetupView();

	virtual void				AttachedToWindow();
			void				MessageReceived(BMessage* message);

			bool				UpdateJobData();


private:
			void				_AddOrientationItem(const char* name,
									JobData::Orientation orientation);
			JobData::Orientation _GetOrientation();
			PaperCap*			_GetPaperCap();

private:
			JobData*			fJobData;
			PrinterData*		fPrinterData;
	const	PrinterCap*			fPrinterCap;
			BPopUpMenu*			fPaper;
			BPopUpMenu*			fOrientation;
			BPopUpMenu*			fResolution;
			BTextControl*		fScaling;
			MarginView*			fMarginView;
};


class PageSetupDlg : public DialogWindow {
public:
								PageSetupDlg(JobData* job_data,
									PrinterData* printer_data,
									const PrinterCap* printer_cap);

	virtual void				MessageReceived(BMessage* message);

private:
			BMessageFilter*		fFilter;
			PageSetupView*		fPageSetupView;
};

#endif	/* _PAGE_SETUP_DLG_H */
