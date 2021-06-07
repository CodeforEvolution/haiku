// Sun, 18 Jun 2000
// Y.Takagi


#include <sys/socket.h>
#include <iostream>
#include <strings.h>
#include <stdio.h>
#include <list>

#include "IppURLConnection.h"
#include "IppContent.h"


IppURLConnection::IppURLConnection(const BUrl& url)
	:
	HttpURLConnection(url)
{
	fIppRequest  = NULL;
	fIppResponse = new IppContent();

	setRequestMethod("POST");
	setRequestProperty("Content-Type", "application/ipp");
	setRequestProperty("Cache-Control", "no-cache");
	setRequestProperty("Pragma", "no-cache");
}


IppURLConnection::~IppURLConnection()
{
	delete fIppRequest;
	delete fIppResponse;
}


void
IppURLConnection::setIppRequest(IppContent* obj)
{
	if (fIppRequest != NULL) {
		delete fIppRequest;
		fIppRequest = NULL;
	}

	fIppRequest = obj;
}


const IppContent*
IppURLConnection::getIppResponse() const
{
	return fIppResponse;
}


void
IppURLConnection::setRequest()
{
	if (connected) {
		char buf[64];
		itoa(fIppRequest->length(), buf, 10);
		setRequestProperty("Content-Length", buf);
		HttpURLConnection::setRequest();
	}
}


void
IppURLConnection::setContent()
{
	if (connected && fIppRequest != NULL) {
		ostream& os = getOutputStream();
		os << *fIppRequest;
	}
}


inline bool
is_contenttype_ipp(const char* s)
{
	return strncasecmp(s, "application/ipp", 15) ? false : true;
}


void
IppURLConnection::getContent()
{
	if (connected) {
		if (getResponseCode() == HTTP_OK && is_contenttype_ipp(getContentType())) {
			istream& is = getInputStream();
			is >> *fIppResponse;
		} else {
			HttpURLConnection::getContent();
		}
	}
}


ostream&
IppURLConnection::printIppRequest(ostream &os)
{
	return fIppRequest->print(os);
}


ostream&
IppURLConnection::printIppResponse(ostream &os)
{
	if (getResponseCode() == HTTP_OK && is_contenttype_ipp(getContentType())) {
		return fIppResponse->print(os);
	}

	return os;
}
