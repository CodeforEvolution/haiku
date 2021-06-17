/*
 * Copyright 2008-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ithamar R. Adema
 */
#ifndef _TRANSPORT_H
#define _TRANSPORT_H


class Transport;


#include <FindDirectory.h>
#include <Handler.h>
#include <ObjectList.h>
#include <Path.h>
#include <String.h>


class Transport : public BHandler {
public:
								Transport(const BPath& path);
								~Transport();

			BString				Name() const { return fPath.Leaf(); }

			status_t			ListAvailablePorts(BMessage* message);

	static	status_t			Scan(directory_which which);

	static	Transport*			Find(const BString& name);
	static	void				Remove(Transport* transport);
	static	Transport*			At(int32 index);
	static	int32				CountTransports();

			void				MessageReceived(BMessage* message);

		// Scripting support, see Printer.Scripting.cpp
			status_t			GetSupportedSuites(BMessage* message);
			void				HandleScriptingCommand(BMessage* message);
			BHandler*			ResolveSpecifier(BMessage* message, int32 index,
									BMessage* spec, int32 form,
									const char* prop);

private:
			BPath				fPath;
			image_id			fImageID;
			int					fFeatures;

	static	BObjectList<Transport> sTransports;
};

#endif /* _TRANSPORT_H */
