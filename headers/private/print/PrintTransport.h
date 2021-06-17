/*
 * Copyright 2004-2021, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 *		Philippe Houdoin
 */
#ifndef _PRINT_TRANSPORT_H
#define _PRINT_TRANSPORT_H


#include <DataIO.h>
#include <image.h>
#include <Message.h>
#include <Node.h>


// Utilized by a printer driver.
class PrintTransport {
public:
								PrintTransport();
								~PrintTransport();

	// Opens the transport add-on associated with the printerFolder
			status_t			Open(BNode* printerFolder);
	// Returns the output stream created by the transport add-on
			BDataIO*			GetDataIO();
	// Returns false if the user has canceled the save to file dialog
	// of the "Print To File" transport add-on.
			bool				IsPrintToFileCanceled() const;

private:
			BDataIO*			fDataIO;
			image_id			fAddOnID;
			void				(*fExitProc)(void);
};

#endif /* _PRINT_TRANSPORT_H */
