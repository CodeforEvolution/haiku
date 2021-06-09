/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _JOB_SETUP_DLG_H
#define _JOB_SETUP_DLG_H


#include <map>

#include <View.h>

#include "DialogWindow.h"
#include "Halftone.h"
#include "JobData.h"
#include "JSDSlider.h"
#include "PrinterCap.h"


class BCheckBox;
class BGridLayout;
class BPopUpMenu;
class BRadioButton;
class BSlider;
class BTextControl;
class BTextView;
class HalftoneView;
class JobData;
class PagesView;
class PrinterCap;
class PrinterData;


template<typename T, typename R>
class Range {
public:
								Range();
								Range(const char* label, const char* key,
									const R* range, BSlider* slider);

			const char*			Key() const;
			T					Value();
			void				UpdateLabel();

private:
			const char*			fLabel;
			const char*			fKey;
			const R*			fRange;
			BSlider*			fSlider;
};


template<typename T, typename R>
Range<T, R>::Range()
	:
	fKey(NULL),
	fRange(NULL),
	fSlider(NULL)
{
}


template<typename T, typename R>
Range<T, R>::Range(const char* label, const char* key, const R* range,
	BSlider* slider)
	:
	fLabel(label),
	fKey(key),
	fRange(range),
	fSlider(slider)
{

}


template<typename T, typename R>
const char*
Range<T, R>::Key() const
{
	return fKey;
}


template<typename T, typename R>
T
Range<T, R>::Value()
{
	return static_cast<T>(fRange->Lower() +
		(fRange->Upper() - fRange->Lower()) * fSlider->Position());
}


template<typename T, typename R>
void
Range<T, R>::UpdateLabel()
{
	BString label = fLabel;
	label << " (" << Value() << ")";
	fSlider->SetLabel(label.String());
}


typedef Range<int32, IntRangeCap> IntRange;
typedef Range<double, DoubleRangeCap> DoubleRange;


class JobSetupView : public BView {
public:
								JobSetupView(JobData* jobData,
									PrinterData* printerData,
									const PrinterCap* printerCap);

	virtual	void				AttachedToWindow();
	virtual void				MessageReceived(BMessage* message);
			bool				UpdateJobData();

private:
			void				_UpdateButtonEnabledState();

			bool				_IsHalftoneConfigurationNeeded();
			void				_CreateHalftoneConfigurationUI();

			void				_AddDriverSpecificSettings(
									BGridLayout* gridLayout, int row);
			void				_AddPopUpMenu(
									const DriverSpecificCap* capability,
									BGridLayout* gridLayout, int& row);
			void				_AddCheckBox(
									const DriverSpecificCap* capability,
									BGridLayout* gridLayout, int& row);
			void				_AddIntSlider(
									const DriverSpecificCap* capability,
									BGridLayout* gridLayout, int& row);
			void				_AddDoubleSlider(
									const DriverSpecificCap* capability,
									BGridLayout* gridLayout, int& row);

			BString				_GetDriverSpecificValue(
									PrinterCap::CapID category,
									const char* key);

								template<typename Predicate>
			void				_FillCapabilityMenu(BPopUpMenu* menu,
									uint32 message,
									const BaseCap** capabilities, int count,
									Predicate& predicate);
			void				_FillCapabilityMenu(BPopUpMenu* menu,
									uint32 message, PrinterCap::CapID category,
									int id);
			void				_FillCapabilityMenu(BPopUpMenu* menu,
									uint32 message,
									const BaseCap** capabilities, int count,
									int id);

			int					_GetID(const BaseCap** capabilities, int count,
									const char* label, int defaultValue);

			BRadioButton*		_CreatePageSelectionItem(const char* name,
									const char* label,
									JobData::PageSelection pageSelection);

			void				_AllowOnlyDigits(BTextView* textView,
									int maxDigits);

			void				_UpdateHalftonePreview();
			void				_UpdateIntSlider(BMessage* message);
			void				_UpdateDoubleSlider(BMessage* message);

			JobData::Color		_Color();
			Halftone::DitherType _DitherType();
			float				_Gamma();
			float				_InkDensity();
			JobData::PaperSource _PaperSource();

private:
			BTextControl*		fCopies;
			BTextControl*		fFromPage;
			BTextControl*		fToPage;
			JobData*			fJobData;
			PrinterData*		fPrinterData;
			const PrinterCap*	fPrinterCap;
			BPopUpMenu*			fColorType;
			BPopUpMenu*			fDitherType;
			BMenuField*			fDitherMenuField;
			JSDSlider*			fGamma;
			JSDSlider*			fInkDensity;
			HalftoneView*		fHalftone;
			BBox*				fHalftoneBox;
			BRadioButton*		fAll;
			BCheckBox*			fCollate;
			BCheckBox*			fReverse;
			PagesView*			fPages;
			BPopUpMenu*			fPaperFeed;
			BCheckBox*			fDuplex;
			BPopUpMenu*			fNup;
			BRadioButton*		fAllPages;
			BRadioButton*		fOddNumberedPages;
			BRadioButton*		fEvenNumberedPages;
			std::map<PrinterCap::CapID, BPopUpMenu*>
				fDriverSpecificPopUpMenus;
			std::map<BString, BCheckBox*>
				fDriverSpecificCheckBoxes;
			std::map<PrinterCap::CapID, IntRange>
				fDriverSpecificIntSliders;
			std::map<PrinterCap::CapID, DoubleRange>
				fDriverSpecificDoubleSliders;
			BCheckBox*			fPreview;
};


class JobSetupDlg : public DialogWindow {
public:
								JobSetupDlg(JobData* jobData,
									PrinterData* printerData,
									const PrinterCap* printerCap);

	virtual	void				MessageReceived(BMessage* message);

private:
			JobSetupView*		fJobSetup;
};

#endif	/* _JOB_SETUP_DLG_H */
