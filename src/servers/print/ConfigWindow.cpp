/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */


#include "ConfigWindow.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <Application.h>
#include <Autolock.h>
#include <Catalog.h>
#include <Debug.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <IconUtils.h>
#include <Layout.h>
#include <Locale.h>
#include <Resources.h>
#include <Window.h>

#include "pr_server.h"
#include "Printer.h"
#include "PrintServerApp.h"
#include "PrintUtils.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ConfigWindow"


static const float a0_width = 2380.0;
static const float a0_height = 3368.0;
static const float a1_width = 1684.0;
static const float a1_height = 2380.0;
static const float a2_width = 1190.0;
static const float a2_height = 1684.0;
static const float a3_width = 842.0;
static const float a3_height = 1190.0;
static const float a4_width = 595.0;
static const float a4_height = 842.0;
static const float a5_width = 421.0;
static const float a5_height = 595.0;
static const float a6_width = 297.0;
static const float a6_height = 421.0;
static const float b5_width = 501.0;
static const float b5_height = 709.0;
static const float letter_width = 612.0;
static const float letter_height = 792.0;
static const float legal_width  = 612.0;
static const float legal_height  = 1008.0;
static const float ledger_width = 1224.0;
static const float ledger_height = 792.0;
static const float tabloid_width = 792.0;
static const float tabloid_height = 1224.0;
static const float jis_b5_width = 516.0;
static const float jis_b5_height = 729.0;


static struct PageFormat
{
	const char* label;
	float width;
	float height;
} pageFormat[] =
{
	{B_TRANSLATE_MARK_COMMENT("Letter", "ANSI A (letter), a North American "
		"paper size"), letter_width, letter_height },
	{B_TRANSLATE_MARK_COMMENT("Legal", "A North American paper size (216 x 356"
		" mm, or 8.5 x 14 in)"), legal_width,  legal_height },
	{B_TRANSLATE_MARK_COMMENT("Ledger", "ANSI B (ledger), a North American "
		"paper size"), ledger_width, ledger_height },
	{B_TRANSLATE_MARK_COMMENT("Tabloid", "ANSI B (tabloid), a North American "
		"paper size"), tabloid_width, tabloid_height },
	{B_TRANSLATE_MARK_COMMENT("A0", "ISO 216 paper size"),
		a0_width, a0_height },
	{B_TRANSLATE_MARK_COMMENT("A1", "ISO 216 paper size"),
		a1_width, a1_height },
	{B_TRANSLATE_MARK_COMMENT("A2", "ISO 216 paper size"),
		a2_width, a2_height },
	{B_TRANSLATE_MARK_COMMENT("A3", "ISO 216 paper size"),
		a3_width, a3_height },
	{B_TRANSLATE_MARK_COMMENT("A4", "ISO 216 paper size"),
		a4_width, a4_height },
	{B_TRANSLATE_MARK_COMMENT("A5", "ISO 216 paper size"),
		a5_width, a5_height },
	{B_TRANSLATE_MARK_COMMENT("A6", "ISO 216 paper size"),
		a6_width, a6_height },
	{B_TRANSLATE_MARK_COMMENT("B5", "ISO 216 paper size"),
		b5_width, b5_height },
	{B_TRANSLATE_MARK_COMMENT("B5 (JIS)", "JIS P0138 B5, a Japanese "
		"paper size"), jis_b5_width, jis_b5_height },
};


static void
GetPageFormat(float width, float height, BString& label)
{
	width = floor(width + 0.5);
	height = floor(height + 0.5);
	for (uint i = 0; i < sizeof(pageFormat) / sizeof(struct PageFormat); i++) {
		struct PageFormat& pf = pageFormat[i];
		if ((pf.width == width && pf.height == height) || (pf.width == height
			&& pf.height == width)) {
			label = B_TRANSLATE_NOCOLLECT(pf.label);
			return;
		}
	}

	float unit = 72.0;
		// Currently inches only
	label << (width / unit) << "x" << (height / unit) << " in.";
}


static BGroupLayoutBuilder
LeftAlign(BView* view)
{
	return BGroupLayoutBuilder(B_HORIZONTAL)
		.Add(view)
		.AddGlue()
		.SetInsets(0, 0, 0, 0);
}


ConfigWindow::ConfigWindow(config_setup_kind kind, Printer* defaultPrinter,
	BMessage* settings, AutoReply* sender)
	:
	BWindow(ConfigWindow::GetWindowFrame(),
		B_TRANSLATE("Page setup"),
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS
		| B_CLOSE_ON_ESCAPE),
	fKind(kind),
	fDefaultPrinter(defaultPrinter),
	fSettings(settings),
	fSender(sender),
	fCurrentPrinter(NULL),
	fPageFormatText(NULL),
	fJobSetupText(NULL)
{
	MimeTypeForSender(settings, fSenderMimeType);
	_PrinterForMimeType();

	if (kind == kJobSetup)
		SetTitle(B_TRANSLATE("Print setup"));

	BView* panel = new BBox(Bounds(), "temporary", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(panel);

	// Print selection pop up menu
	BPopUpMenu* menu = new BPopUpMenu(B_TRANSLATE("Select a printer"));
	_SetupPrintersMenu(menu);

	fPrinters = new BMenuField(B_TRANSLATE("Printer:"), menu);

	// Page format button
	fPageSetup = _AddPictureButton(panel, "Paper setup",
		"PAGE_SETUP", MSG_PAGE_SETUP);

	// Add description to button
	BStringView* pageFormatTitle = new BStringView("paperSetupTitle",
		B_TRANSLATE("Paper setup:"));
	fPageFormatText = new BStringView("pageSetupText", "");

	// Page selection button
	fJobSetup = NULL;
	BStringView* jobSetupTitle = NULL;
	if (kind == kJobSetup) {
		fJobSetup = _AddPictureButton(panel, "Job setup",
			"JOB_SETUP", MSG_JOB_SETUP);
		// Add description to button
		jobSetupTitle = new BStringView("jobSetupTitle",
			B_TRANSLATE("Print job setup:"));
		fJobSetupText = new BStringView("jobSetupText", "");
	}

	// Separator line
	BBox* separator = new BBox("separator");
	separator->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, 1));

	// Cancel & OK button
	BButton* cancel = new BButton("Cancel", B_TRANSLATE("Cancel"),
		new BMessage(B_QUIT_REQUESTED));
	fOk = new BButton("OK", B_TRANSLATE("OK"), new BMessage(MSG_OK));

	RemoveChild(panel);

	SetLayout(new BGroupLayout(B_VERTICAL));
	BGroupLayoutBuilder builder(B_VERTICAL);

	builder
		.Add(fPrinters)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, 10)
				.Add(fPageSetup)
				.Add(BGroupLayoutBuilder(B_VERTICAL, 0)
						.Add(LeftAlign(pageFormatTitle))
						.Add(LeftAlign(fPageFormatText))
						.SetInsets(0, 0, 0, 0)
				)
				.AddGlue()
		);

	if (fJobSetup != NULL) {
		builder
			.Add(BGroupLayoutBuilder(B_HORIZONTAL, 10)
					.Add(fJobSetup)
					.Add(BGroupLayoutBuilder(B_VERTICAL, 0)
							.Add(LeftAlign(jobSetupTitle))
							.Add(LeftAlign(fJobSetupText))
							.SetInsets(0, 0, 0, 0)
					)
					.AddGlue()
			);
	}

	builder
		.AddGlue()
		.Add(separator)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
			.AddGlue()
			.Add(cancel)
			.Add(fOk)
		)
		.SetInsets(5, 5, 5, 5);

	AddChild(builder);

	AddShortcut('a', 0, new BMessage(B_ABOUT_REQUESTED));

	SetDefaultButton(fOk);

	fPrinters->MakeFocus(true);

	_UpdateSettings(true);
}


ConfigWindow::~ConfigWindow()
{
	if (fCurrentPrinter != NULL)
		fCurrentPrinter->Release();

	release_sem(fFinished);
}


void
ConfigWindow::Go()
{
	sem_id sid = create_sem(0, "finished");
	if (sid >= 0) {
		fFinished = sid;
		Show();
		acquire_sem(sid);
		delete_sem(sid);
	} else
		Quit();
}


void
ConfigWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_PAGE_SETUP:
			_Setup(kPageSetup);
			break;

		case MSG_JOB_SETUP:
			_Setup(kJobSetup);
			break;

		case MSG_PRINTER_SELECTED:
		{
			BString printer;
			if (message->FindString("name", &printer) == B_OK) {
				_UpdateAppSettings(fSenderMimeType.String(), printer.String());
				_PrinterForMimeType();
				_UpdateSettings(true);
			}

			break;
		}

		case MSG_OK:
		{
			_UpdateSettings(false);
			if (fKind == kPageSetup)
				fSender->SetReply(&fPageSettings);
			else
				fSender->SetReply(&fJobSettings);
			Quit();

			break;
		}

		case B_ABOUT_REQUESTED:
			AboutRequested();
			break;

		default:
			BWindow::MessageReceived(message);
	}
}


void
ConfigWindow::AboutRequested()
{
	BString text = B_TRANSLATE("Printer server");
	text <<	"\n"
		"© 2001-2010 Haiku, Inc.\n"
		"\n"
		"\tIthamar R. Adema\n"
		"\tMichael Pfeiffer\n";

	BAlert* aboutAlert = new BAlert("About printer server", text.String(),
		B_TRANSLATE("OK"));
	aboutAlert->SetFlags(aboutAlert->Flags() | B_CLOSE_ON_ESCAPE);
	aboutAlert->Go();
}


void
ConfigWindow::FrameMoved(BPoint point)
{
	BRect frame = GetWindowFrame();
	frame.OffsetTo(point);
	SetWindowFrame(frame);
}


BRect
ConfigWindow::GetWindowFrame()
{
	BRect frame(0, 0, 10, 10);
	BAutolock lock(gLock);
	if (lock.IsLocked())
		frame.OffsetBy(Settings::GetSettings()->ConfigWindowFrame().LeftTop());

	frame.OffsetBy(30, 30);
	return frame;
}


void
ConfigWindow::SetWindowFrame(BRect rect)
{
	BAutolock lock(gLock);
	if (lock.IsLocked())
		Settings::GetSettings()->SetConfigWindowFrame(rect);
}


BButton*
ConfigWindow::_AddPictureButton(BView* panel, const char* name,
	const char* picture, uint32 what)
{
	BResources* res = BApplication::AppResources();
	if (res == NULL)
		return NULL;

	size_t length;
	const void* bits = res->LoadResource('VICN', picture, &length);
	BButton* button = NULL;

	BBitmap* onBitmap = new BBitmap(BRect(0, 0, 24, 24), B_RGBA32);

	if (onBitmap != NULL) {
		if (BIconUtils::GetVectorIcon((uint8*)bits, length, onBitmap) != B_OK) {
			delete onBitmap;
			return NULL;
		}

		button = new BButton(name, new BMessage(what));
		button->SetIcon(onBitmap, B_TRIM_ICON_BITMAP_KEEP_ASPECT);
		button->SetViewColor(B_TRANSPARENT_COLOR);
		button->SetLabel(NULL);
	}

	delete onBitmap;

	return button;
}


void
ConfigWindow::_PrinterForMimeType()
{
	BAutolock lock(gLock);
	if (fCurrentPrinter != NULL) {
		fCurrentPrinter->Release();
		fCurrentPrinter = NULL;
	}

	if (lock.IsLocked()) {
		Settings* settings = Settings::GetSettings();

		AppSettings* app = settings->FindAppSettings(fSenderMimeType.String());
		if (app != NULL)
			fPrinterName = app->GetPrinter();
		else
			fPrinterName = fDefaultPrinter != NULL ?
				fDefaultPrinter->Name() : "";

		fCurrentPrinter = Printer::Find(fPrinterName);
		if (fCurrentPrinter != NULL)
			fCurrentPrinter->Acquire();
	}
}


void
ConfigWindow::_SetupPrintersMenu(BMenu* menu)
{
	// clear menu
	while (menu->CountItems() != 0)
		delete menu->RemoveItem((int32)0);

	// fill menu with printer names
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		BString name;
		BMessage* message;
		BMenuItem* item;
		for (int i = 0; i < Printer::CountPrinters(); i++) {
			Printer::At(i)->GetName(name);
			message = new BMessage(MSG_PRINTER_SELECTED);
			message->AddString("name", name.String());
			menu->AddItem(item = new BMenuItem(name.String(), message));
			if (name == fPrinterName)
				item->SetMarked(true);
		}
	}
}


void
ConfigWindow::_UpdateAppSettings(const char* mime, const char* printer)
{
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		Settings* settings = Settings::GetSettings();
		AppSettings* app = settings->FindAppSettings(mime);
		if (app != NULL)
			app->SetPrinter(printer);
		else
			settings->AddAppSettings(new AppSettings(mime, printer));
	}
}


void
ConfigWindow::_UpdateSettings(bool read)
{
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		Settings* settings = Settings::GetSettings();
		PrinterSettings* printerSettings =
			settings->FindPrinterSettings(fPrinterName.String());

		if (printerSettings == NULL) {
			printerSettings = new PrinterSettings(fPrinterName.String());
			settings->AddPrinterSettings(printerSettings);
		}

		ASSERT(printerSettings != NULL);

		if (read) {
			fPageSettings = *printerSettings->GetPageSettings();
			fJobSettings = *printerSettings->GetJobSettings();
		} else {
			printerSettings->SetPageSettings(&fPageSettings);
			printerSettings->SetJobSettings(&fJobSettings);
		}
	}

	_UpdateUI();
}


void
ConfigWindow::_UpdateUI()
{
	if (fCurrentPrinter == NULL) {
		fPageSetup->SetEnabled(false);

		if (fJobSetup != NULL) {
			fJobSetup->SetEnabled(false);
			fJobSetupText->SetText(B_TRANSLATE("Undefined"));
		}

		fOk->SetEnabled(false);
		fPageFormatText->SetText(B_TRANSLATE("Undefined"));
	} else {
		fPageSetup->SetEnabled(true);

		if (fJobSetup != NULL) {
			fJobSetup->SetEnabled(fKind == kJobSetup
				&& !fPageSettings.IsEmpty());
		}

		fOk->SetEnabled((fKind == kJobSetup && !fJobSettings.IsEmpty())
			|| (fKind == kPageSetup && !fPageSettings.IsEmpty()));

		// Display information about page format
		BRect paperRect;
		BString pageFormat;
		if (fPageSettings.FindRect(PSRV_FIELD_PAPER_RECT, &paperRect) == B_OK) {
			GetPageFormat(paperRect.Width(), paperRect.Height(), pageFormat);

			int32 orientation = 0;
			fPageSettings.FindInt32(PSRV_FIELD_ORIENTATION, &orientation);
			if (orientation == 0)
				pageFormat << ", " << B_TRANSLATE("Portrait");
			else
				pageFormat << ", " << B_TRANSLATE("Landscape");
		} else
			pageFormat << B_TRANSLATE("Undefined");

		fPageFormatText->SetText(pageFormat.String());

		// Display information about job
		if (fKind == kJobSetup) {
			BString job;
			int32 first, last, copies;
			if (fJobSettings.FindInt32(PSRV_FIELD_FIRST_PAGE, &first) == B_OK
				&& fJobSettings.FindInt32(PSRV_FIELD_LAST_PAGE, &last) ==
				B_OK) {

				bool printRange = first >= 1 && first <= last
					&& last != INT_MAX;

				char number[12];
				if (fJobSettings.FindInt32(PSRV_FIELD_COPIES, &copies)
					== B_OK && copies > 1) {
					if (printRange) {
						job = B_TRANSLATE("Page %1 to %2, %3 copies");
						snprintf(number, sizeof(number), "%d", (int)first);
						job.ReplaceFirst("%1", number);
						snprintf(number, sizeof(number), "%d", (int)last);
						job.ReplaceFirst("%2", number);
						snprintf(number, sizeof(number), "%d", (int)copies);
						job.ReplaceFirst("%3", number);
					} else {
						job = B_TRANSLATE("All pages, %1 copies");
						snprintf(number, sizeof(number), "%d", (int)copies);
						job.ReplaceFirst("%1", number);
					}
				} else {
					if (printRange) {
						job = B_TRANSLATE("Page %1 to %2");
						snprintf(number, sizeof(number), "%d", (int)first);
						job.ReplaceFirst("%1", number);
						snprintf(number, sizeof(number), "%d", (int)last);
						job.ReplaceFirst("%2", number);
					} else
						job = B_TRANSLATE("All pages");
				}
			} else
				job << B_TRANSLATE("Undefined");

			fJobSetupText->SetText(job.String());
		}
	}
}


void
ConfigWindow::_Setup(config_setup_kind kind)
{
	if (fCurrentPrinter != NULL) {
		Hide();
		if (kind == kPageSetup) {
			BMessage settings = fPageSettings;
			if (fCurrentPrinter->ConfigurePage(settings) == B_OK) {
				fPageSettings = settings;
				if (!fJobSettings.IsEmpty())
					AddFields(&fJobSettings, &fPageSettings);
			}
		} else {
			BMessage settings;
			if (fJobSettings.IsEmpty())
				settings = fPageSettings;
			else
				settings = fJobSettings;

			if (fCurrentPrinter->ConfigureJob(settings) == B_OK)
				fJobSettings = settings;
		}

		_UpdateUI();
		Show();
	}
}
