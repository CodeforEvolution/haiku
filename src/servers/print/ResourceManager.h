/*
 * Copyright 2002-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */
#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H


#include <Locker.h>
#include <ObjectList.h>
#include <String.h>

#include "BeUtils.h"


class Resource : public Object {
public:
								Resource(const char* transport,
									const char* address,
									const char* connection);
							~Resource();

			bool 			NeedsLocking();

			bool			Equals(const char* transport, const char* address,
								const char* connection);

	const	BString&		Transport() const { return fTransport; }

			bool			Lock();
			void			Unlock();

private:
			BString			fTransport;
			BString			fTransportAddress;
			BString			fConnection;
			sem_id			fResourceAvailable;
};


class ResourceManager {
public:
							~ResourceManager();

			Resource*		Allocate(const char* transport, const char* address,
								const char* connection);
			void			Free(Resource* resource);

private:
			Resource*		_Find(const char* transport, const char* address,
								const char* connection);

private:
			BObjectList<Resource> fResources;
};

#endif /* _RESOURCE_MANAGER_H */
