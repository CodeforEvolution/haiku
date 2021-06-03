/*
 * Copyright 2021 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef PRINT_DEFS_H
#define PRINT_DEFS_H


struct printer_status_t {
	printer_status_t()
		:
		status(B_UNKNOWN),
		time_sec(-1)
	{
		memset(CMYKcmyk, 0xFF, sizeof(CMYKcmyk));
		memset(_reserved, 0, sizeof(_reserved));
	}

	typedef enum {
		B_UNKNOWN = -1,
		B_ONLINE,
		B_OFFLINE,
		B_PRINTING,
		B_CLEANING,
		B_PAPER_JAM,
		B_NO_PAPER,
		B_NO_INK,
		B_ERROR,
		B_COVER_OPEN
	} printer_status;

	printer_status status;
		// Printer's status
	bigtime_t time_sec;
		// Time remaining until the end of the current action (if applicable)
	uint8 CMYKcmyk[8];
		// Ink levels (%), 0xFF=unknown/not applicable
	uint32 _reserved[4];
};


#endif // PRINT_DEFS_H
