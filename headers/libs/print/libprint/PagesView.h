/*
 * Copyright 2005-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _PAGES_VIEW_H
#define _PAGES_VIEW_H


#include <View.h>


class PagesView : public BView {
public:
								PagesView(BRect frame, const char* name,
									uint32 resizeMask, uint32 flags);

			void				GetPreferredSize(float* width, float* height);
			void				Draw(BRect rect);

			void				SetCollate(bool collate);
			void				SetReverse(bool reverse);

private:
			void				_DrawPages(BPoint position, int number,
									int count);
			void				_DrawPage(BPoint position, int number);

private:
			bool				fCollate;
			bool				fReverse;
};

#endif /* _PAGES_VIEW_H */
