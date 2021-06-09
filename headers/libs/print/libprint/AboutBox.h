/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _ABOUT_BOX_H
#define _ABOUT_BOX_H


#include <Application.h>


class AboutBox : public BApplication {
public:
								AboutBox(const char* signature,
									const char* driverName, const char* version,
									const char* copyright);
};

#endif	/* _ABOUT_BOX_H */
