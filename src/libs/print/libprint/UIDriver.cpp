/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "UIDriver.h"

#include <Message.h>

#include "DbgMsg.h"
#include "JobData.h"
#include "JobSetupDlg.h"
#include "PageSetupDlg.h"
#include "PrinterData.h"


UIDriver::UIDriver(BMessage* message, PrinterData* printerData,
	const PrinterCap* printerCap)
	:
	fMessage(message),
	fPrinterData(printerData),
	fPrinterCap(printerCap)
{
}


UIDriver::~UIDriver()
{
}


BMessage*
UIDriver::ConfigPage()
{
	BMessage* clonedMessage = new BMessage(*fMessage);
	JobData* jobData = new JobData(clonedMessage, fPrinterCap,
		JobData::kPageSettings);

	if (PageSetup(jobData, fPrinterData, fPrinterCap) < 0) {
		delete clonedMessage;
		clonedMessage = NULL;
	} else
		clonedMessage->what = 'okok';

	delete jobData;

	return clonedMessage;
}


BMessage*
UIDriver::ConfigJob()
{
	BMessage* clonedMessage = new BMessage(*fMessage);
	JobData* jobData = new JobData(clonedMessage, fPrinterCap,
		JobData::kJobSettings);

	if (JobSetup(jobData, fPrinterData, fPrinterCap) < 0) {
		delete clonedMessage;
		clonedMessage = NULL;
	} else
		clonedMessage->what = 'okok';

	delete jobData;

	return clonedMessage;
}


status_t
UIDriver::PageSetup(JobData* jobData, PrinterData* printerData,
	const PrinterCap* printerCap)
{
	PageSetupDlg* dialog = new PageSetupDlg(jobData, printerData, printerCap);
	return dialog->Go();
}


status_t
UIDriver::JobSetup(JobData* jobData, PrinterData* printerData,
	const PrinterCap* printerCap)
{
	JobSetupDlg* dialog = new JobSetupDlg(jobData, printerData, printerCap);
	return dialog->Go();
}
