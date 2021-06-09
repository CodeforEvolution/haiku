/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _PRINTER_DIALOG_H
#define _PRINTER_DIALOG_H


#include <ListItem.h>
#include <View.h>

#include "DialogWindow.h"


class BListView;
class BTextView;

class PrinterData;
class PrinterCap;
class ProtocolClassCap;


class ProtocolClassItem : public BStringItem {
public:
								ProtocolClassItem(const ProtocolClassCap* cap);

			int					GetProtocolClass() const;
			const char*			GetDescription() const;

private:
	const	ProtocolClassCap*	fProtocolClassCap;
};


class AddPrinterView : public BView {
public:
								AddPrinterView(PrinterData* printerData,
									const PrinterCap* printerCap);
								~AddPrinterView();

	virtual	void				AttachedToWindow();
			void				MessageReceived(BMessage* message);

			void				Save();

private:
			ProtocolClassItem*	_CurrentSelection();

private:
			PrinterData*		fPrinterData;
	const	PrinterCap*			fPrinterCap;

			BListView*			fProtocolClassList;
			BTextView*			fDescription;
};


class AddPrinterDlg : public DialogWindow {
public:
								AddPrinterDlg(PrinterData* printerData,
									const PrinterCap* printerCap);

			void				MessageReceived(BMessage* message);

private:
			AddPrinterView*		fAddPrinterView;
};

#endif /* _PRINTER_DIALOG_H */
