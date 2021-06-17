/*
 * Copyright 2011-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Philippe Houdoin
 */
#ifndef _PRINTERS_TEST_PAGE_VIEW_H
#define _PRINTERS_TEST_PAGE_VIEW_H


#include <View.h>


class PrinterItem;


class TestPageView : public BView {
public:
								TestPageView(BRect rect, PrinterItem* printer);

			void				AttachedToWindow();
			void				DrawAfterChildren(BRect rect);

private:
			PrinterItem*		fPrinter;
};

#endif /* _PRINTERS_TEST_PAGE_VIEW_H */
