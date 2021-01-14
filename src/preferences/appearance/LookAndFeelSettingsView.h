/*
 *  Copyright 2010-2020 Haiku, Inc. All rights reserved.
 *  Distributed under the terms of the MIT license.
 *
 *	Authors:
 *		Stephan Aßmus <superstippi@gmx.de>
 *		Alexander von Gluck <kallisti5@unixzen.com>
 *		Ryan Leavengood <leavengood@gmail.com>
 *		John Scipione <jscipione@gmail.com>
 */
#ifndef LOOK_AND_FEEL_SETTINGS_VIEW_H
#define LOOK_AND_FEEL_SETTINGS_VIEW_H


#include <DecorInfo.h>
#include <MediaTheme.h>
#include <String.h>
#include <View.h>


class BButton;
class BCheckBox;
class BMenuField;
class BPopUpMenu;
class FakeScrollBar;

class BPrivate::DefaultMediaTheme;


class LookAndFeelSettingsView : public BView {
public:
								LookAndFeelSettingsView(const char* name);
	virtual						~LookAndFeelSettingsView();

	virtual	void				AttachedToWindow();
	virtual	void				MessageReceived(BMessage* message);

			bool				IsDefaultable();
			void				SetDefaults();

			bool				IsRevertable();
			void				Revert();

private:
			void				_SetDecor(const BString& name);
			void				_SetDecor(BPrivate::DecorInfo* decorInfo);
			void				_BuildDecorMenu();
			const char*			_DecorLabel(const BString& name);

			void				_SetControlLook(const BString& path);
			void				_BuildControlLookMenu();
			const char*			_ControlLookLabel(const char* name);

			bool				_DoubleScrollBarArrows();
			void				_SetDoubleScrollBarArrows(bool doubleArrows);

			BMediaTheme*		_LoadMediaTheme(const entry_ref ref, int32 id);
			void				_SetMediaTheme(const entry_ref ref, int32 id);
			void				_BuildMediaThemeMenu();

private:
			DecorInfoUtility	fDecorUtility;

			BButton*			fDecorInfoButton;
			BMenuField*			fDecorMenuField;
			BPopUpMenu*			fDecorMenu;

			BButton*			fControlLookInfoButton;
			BMenuField*			fControlLookMenuField;
			BPopUpMenu*			fControlLookMenu;

			FakeScrollBar*		fArrowStyleSingle;
			FakeScrollBar*		fArrowStyleDouble;

			BButton*			fMediaThemeInfoButton;
			BMenuField*			fMediaThemeMenuField;
			BPopUpMenu*			fMediaThemeMenu;

			BString				fSavedDecor;
			BString				fCurrentDecor;

			BString				fSavedControlLook;
			BString				fCurrentControlLook;

			bool				fSavedDoubleArrowsValue : 1;

			entry_ref			fSavedMediaThemeRef;
			int32				fSavedMediaThemeID;

			entry_ref			fCurrentMediaThemeRef;
			int32				fCurrentMediaThemeID;
};


#endif // LOOK_AND_FEEL_SETTINGS_VIEW_H
