/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef	_UI_DRIVER_H
#define	_UI_DRIVER_H


#include <SupportDefs.h>


class BMessage;
class PrinterData;
class PrinterCap;
class JobData;


class UIDriver {
public:
								UIDriver(BMessage* message, PrinterData*
									printerData, const PrinterCap* printerCap);
	virtual						~UIDriver();

			BMessage*			ConfigPage();
			BMessage*			ConfigJob();

protected:
								UIDriver(const UIDriver&);

			UIDriver&			operator=(const UIDriver&);

	virtual	status_t			PageSetup(JobData* jobData,
									PrinterData* printerData,
									const PrinterCap* printerCap);
	virtual	status_t			JobSetup(JobData* jobData,
									PrinterData* printerData,
									const PrinterCap* printerCap);

private:
			BMessage*			fMessage;
			PrinterData*		fPrinterData;
	const	PrinterCap*			fPrinterCap;
};

#endif	/* _UI_DRIVER_H */
