/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _TRANSPORT_H
#define _TRANSPORT_H


#include <image.h>
#include <Message.h>
#include <String.h>


class BDataIO;
class PrinterData;


extern "C" {
	typedef	BDataIO* (*PFN_init_transport)(BMessage*);
	typedef	void (*PFN_exit_transport)();
}


class TransportException {
public:
								TransportException(const BString& what);

			const char*			What() const;

private:
			BString				fWhat;
};


class Transport {
public:
								Transport(const PrinterData* printerData);
								~Transport();

			void				Write(const void* buffer, size_t size)
									/* throw (TransportException) */;
			void				Read(void* buffer, size_t size)
									/* throw (TransportException) */;

			bool				CheckAbort() const;
			bool				IsPrintToFileCanceled() const;
	const	BString&			LastError() const;

protected:
								Transport(const Transport& transport);
								Transport &operator=(
									const Transport& transport);

			void				SetLastError(const char* message);

private:
			image_id			fImage;
			PFN_init_transport	fInitTransport;
			PFN_exit_transport	fExitTransport;
			BDataIO*			fDataStream;
			bool				fAbort;
			BString				fLastErrorString;
};

#endif	// _TRANSPORT_H
