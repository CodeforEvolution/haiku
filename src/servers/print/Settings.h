/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _SETTINGS_H
#define _SETTINGS_H


#include <ObjectList.h>
#include <String.h>

#include "BeUtils.h"


class AppSettings {
public:
								AppSettings(const char* mimeType,
									const char* printer = NULL);

			const char*			GetMimeType() const
									{ return fMimeType.String(); }
			bool				UsesDefaultPrinter() const
									{ return fMimeType.Length() == 0; }

			const char*			GetPrinter() const
									{ return fPrinter.String(); }
			void				SetPrinter(const char* printer)
									{ fPrinter = printer; }
			void				SetDefaultPrinter()
									{ fPrinter = ""; }

private:
			BString				fMimeType;
				// Application signature
			BString				fPrinter;
				// Printer used by application (default == empty string)
};


class PrinterSettings {
public:
								PrinterSettings(const char* printer,
												BMessage* pageSettings = NULL,
												BMessage* jobSettings = NULL);

			const char*			GetPrinter() const
									{ return fPrinter.String(); }
			BMessage*			GetPageSettings()
									{ return &fPageSettings; }
			BMessage*			GetJobSettings()
									{ return &fJobSettings; }

			void				SetPrinter(const char* printer)
									{ fPrinter = printer; }
			void				SetPageSettings(BMessage* settings)
									{ fPageSettings = *settings; }
			void				SetJobSettings(BMessage* settings)
									{ fJobSettings = *settings; }

private:
			BString				fPrinter;
			BMessage			fPageSettings; // Default page settings
			BMessage			fJobSettings;  // Default job settings
};


class Settings {
public:
	static	Settings* 			GetSettings();
								~Settings();

			int					AppSettingsCount() const
									{ return fApps.CountItems(); }
			AppSettings*		AppSettingsAt(int index)
									{ return fApps.ItemAt(index); }
			void				AddAppSettings(AppSettings* settings)
									{ fApps.AddItem(settings); }
			void				RemoveAppSettings(int index);
			AppSettings*		FindAppSettings(const char* mimeType);

			int					PrinterSettingsCount() const
									{ return fPrinters.CountItems(); }
			PrinterSettings* 	PrinterSettingsAt(int index)
									{ return fPrinters.ItemAt(index); }
			void				AddPrinterSettings(PrinterSettings* settings)
									{ fPrinters.AddItem(settings); }
			void				RemovePrinterSettings(int index);
			PrinterSettings*	FindPrinterSettings(const char* printer);

			bool				UseConfigWindow() const
									{ return fUseConfigWindow; }
			void				SetUseConfigWindow(bool use)
									{ fUseConfigWindow = use; }
			BRect				ConfigWindowFrame() const
									{ return fConfigWindowFrame; }
			void				SetConfigWindowFrame(BRect rect)
									{ fConfigWindowFrame = rect; }
			const char*			DefaultPrinter() const
									{ return fDefaultPrinter.String(); }
			void				SetDefaultPrinter(const char* name)
									{ fDefaultPrinter = name; }

			void				Save(BFile* settingsFile);
			void				Load(BFile* settingsFile);

private:
								Settings();

private:
	static	Settings*			sSingleton;

			BObjectList<AppSettings> fApps;
			BObjectList<PrinterSettings> fPrinters;
			bool				fUseConfigWindow;
			BRect				fConfigWindowFrame;
			BString				fDefaultPrinter;
};

#endif /* _SETTINGS_H */
