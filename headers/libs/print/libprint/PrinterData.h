/*
 * Copyright 1999-2000 Y.Takagi
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _PRINTER_DATA_H
#define _PRINTER_DATA_H


#include <SerialPort.h>
#include <String.h>


using namespace std;


class BNode;


class PrinterData {
public:
								PrinterData(BNode* node = NULL);
	virtual						~PrinterData();

	virtual	void				Load();
	virtual	void				Save();

	const	BString&			GetDriverName() const;
	const	BString&			GetPrinterName() const;
	const	BString&			GetComments() const;
	const	BString&			GetTransport() const;
			int					GetProtocolClass() const;

			void				SetPrinterName(const char* printerName);
			void				SetComments(const char* comments);
			void				SetProtocolClass(int protocolClass);

			bool				GetPath(BString& path) const;

protected:
							PrinterData(const PrinterData& printer_data);

			PrinterData&	operator=(const PrinterData& printer_data);

			BNode*			fNode;

private:
			BString			fDriverName;
			BString			fPrinterName;
			BString			fComments;
			BString			fTransport;
			int				fProtocolClass;
};


inline const BString&
PrinterData::GetDriverName() const
{
	return fDriverName;
}


inline const BString&
PrinterData::GetPrinterName() const
{
	return fPrinterName;
}


inline const BString&
PrinterData::GetComments() const
{
	return fComments;
}


inline const BString&
PrinterData::GetTransport() const
{
	return fTransport;
}


inline int
PrinterData::GetProtocolClass() const
{
	return fProtocolClass;
}


inline void
PrinterData::SetPrinterName(const char* printerName)
{
	fPrinterName = printerName;
}


inline void
PrinterData::SetComments(const char* comments)
{
	fComments = comments;
}


inline void
PrinterData::SetProtocolClass(int protocolClass)
{
	fProtocolClass = protocolClass;
}

#endif	// __PRINTERDATA_H
