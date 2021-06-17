/*
 * Copyright 2001-2021, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Rafael Romo
 *		Stefano Ceccherini (burton666@libero.it)
 *		Axel Dörfler, axeld@pinc-software.de
 */
#ifndef _SCREEN_SETTINGS_H
#define _SCREEN_SETTINGS_H


#include <Rect.h>


class ScreenSettings {
public:
								ScreenSettings();
	virtual						~ScreenSettings();

			BRect				WindowFrame() const { return fWindowFrame; };
			void				SetWindowFrame(BRect frame);

private:
			BRect				fWindowFrame;
};

#endif	/* _SCREEN_SETTINGS_H */
