/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _EXPORTS_H
#define _EXPORTS_H


#include <BeBuild.h>


class BFile;
class BMessage;
class BNode;


extern "C" {
	_EXPORT char*				add_printer(char* printer_name);
	_EXPORT BMessage*			config_page(BNode* node, BMessage* message);
	_EXPORT BMessage*			config_job(BNode* node, BMessage* message);
	_EXPORT BMessage*			take_job(BFile* spool_file, BNode* node,
									BMessage* message);
}

#endif	// _EXPORTS_H
