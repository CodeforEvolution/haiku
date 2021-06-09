/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Philippe Houdoin
 *		Simon Gauvin
 *		Michael Pfeiffer
 */
#ifndef _MARGIN_VIEW_H
#define _MARGIN_VIEW_H


#include <InterfaceKit.h>
#include <Looper.h>


/*
	Documentation:

	The MarginView is designed to be a self contained component that manages
	the display of a BBox control that shows a graphic of a page and its'
	margings. The component also includes text fields that are used to mofify
	the margin values and a popup to change the units used for the margins.

	There are two interfaces for the MarginView component:

	1) Set methods:
		- page size
		- orientation

	   Get methods to retrieve:
		- margins
		- page size

		The method interface is available for the parent Component to call on
		the MarginView in response to the Window receiveing messages from
		other BControls that it contains, such as a Page Size popup. The
		Get methods are used to extract the page size and margins so that
		the printer driver may put these values into a BMessage for printing.

	2) 'Optional' Message interface:
		- Set Page Size
		- Flip Orientation

		The message interface is available for GUI Controls, BPopupMenu to send
		messages to the MarginView if the parent Window is not used to handle
		the messages.

	General Use of MarginView component:

		1) Simply construct a new MarginView object with the margins
			you want as defaults and add this view to the parent view
			of the dialog.

			MarginView *mv;
			mv = new MarginView(viewSizeRect, pageWidth, pageHeight);
			parentView->AddChild(mv);

			* you can also set the margins in the constructor, and the units:

			mv = new MarginView(viewSizeRect, pageWidth, pageHeight
						marginRect, kUnitPointS);

			! but remeber to have the marginRect values match the UNITS :-)

		2) Set Page Size with methods:

			mv-SetPageSize( pageWidth, pageHeight );
			mv->UpdateView();

		3) Set Page Size with BMessage:

			BMessage* msg = new BMessage(CHANGE_PAGE_SIZE);
			msg->AddFloat("width", pageWidth);
			msg->AddFloat("height", pageHeight);
			mv->PostMessage(msg);

		4) Flip Page with methods:

			mv-SetPageSize( pageHeight, pageWidth );
			mv->UpdateView();

		5) Flip Page with BMessage:

			BMessage* msg = new BMessage(FLIP_PAGE);
			mv->Looper()->PostMessage(msg);

		Note: the MarginView DOES NOT keep track of the orientation. This
				should be done by the code for the Page setup dialog.

		6) Get Page Size

			BPoint pageSize = mv->GetPageSize();

		7) Get Margins

			BRect margins = mv->GetMargins();

		8) Get Units

			uint32 units = mv->GetUnits();

			where units is one of:
				kUnitInch,  72 points/in
				kUnitCM,    28.346 points/cm
				kUnitPoint, 1 point/point
*/


class BTextControl;
class MarginManager;


// Messages that the MarginManager accepts
const uint32 TOP_MARGIN_CHANGED    = 'tchg';
const uint32 RIGHT_MARGIN_CHANGED  = 'rchg';
const uint32 LEFT_MARGIN_CHANGED   = 'lchg';
const uint32 BOTTOM_MARGIN_CHANGED = 'bchg';
const uint32 MARGIN_CHANGED        = 'mchg';
const uint32 CHANGE_PAGE_SIZE      = 'chps';
const uint32 FLIP_PAGE             = 'flip';
const uint32 MARGIN_UNIT_CHANGED   = 'mucg';


enum MarginUnit {
	kUnitInch = 0,
	kUnitCM,
	kUnitPoint
};


class PageView : public BView {
public:
								PageView();

			void				SetPageSize(float pageWidth, float pageHeight);
			void				SetMargins(BRect margins);

	virtual	void				Draw(BRect bounds);


private:
			float				fPageWidth;
			float				fPageHeight;

			BRect				fMargins;
};


/**
 * Class MarginView
 */
class MarginView : public BBox {
friend class MarginManager;

public:
								MarginView(int32 pageWidth = 0,
									int32 pageHeight = 0,
									BRect margins = BRect(1, 1, 1, 1), // 1 inch
									MarginUnit unit = kUnitInch);

	virtual						~MarginView();

	virtual	void				AttachedToWindow();
	virtual	void				MessageReceived(BMessage* message);

			// point.x = width, point.y = height
			BPoint				PageSize() const;
			void				SetPageSize(float pageWidth, float pageHeight);

			// Margin
			BRect				Margin() const;

			// Units
			MarginUnit			Unit() const;

			// Will cause a recalc and redraw
			void				UpdateView(uint32 message);

private:
			// All the GUI construction code
			void				_ConstructGUI();

			// Utility method
			void				_AllowOnlyNumbers(BTextControl* textControl,
									int32 maxNum);

			// Performed internally using text fields
			void				_SetMargin(BRect margin);

			// Performed internally using the supplied popup
			void				_SetMarginUnit(MarginUnit unit);

private:
			BTextControl*		fTop;
			BTextControl*		fBottom;
			BTextControl*		fLeft;
			BTextControl*		fRight;

			// The actual size of the page in points
			float				fPageHeight;
			float				fPageWidth;

			// Rect that holds the margins for the page as a set of point
			// offsets
			BRect				fMargins;

			// The units used to calculate the page size
			MarginUnit			fMarginUnit;
			float				fUnitValue;

			PageView*			fPage;
			BStringView*		fPageSize;
};

#endif /* _MARGIN_VIEW_H */
