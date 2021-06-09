/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _DBG_MSG_H
#define _DBG_MSG_H


#include <Directory.h>
#include <File.h>
#include <Message.h>
#include <Node.h>

#include <stdio.h>


// #define DBG

#ifdef DBG
	void	write_debug_stream(const char*, ...)		__PRINTFLIKE(1,2);
	void	DUMP_BFILE(BFile* file, const char* name);
	void	DUMP_BMESSAGE(BMessage* message);
	void	DUMP_BDIRECTORY(BDirectory* directory);
	void	DUMP_BNODE(BNode* node);
	#define DBGMSG(args) 								write_debug_stream args
#else
	#define DUMP_BFILE(file, name)						(void)0
	#define DUMP_BMESSAGE(message)						(void)0
	#define DUMP_BDIRECTORY(directory)					(void)0
	#define DUMP_BNODE(node)							(void)0
	#define DBGMSG(args)								(void)0
#endif

#endif	/* _DBG_MSG_H */
