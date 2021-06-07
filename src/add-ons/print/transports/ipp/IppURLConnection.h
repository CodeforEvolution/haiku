// Sun, 18 Jun 2000
// Y.Takagi
#ifndef _IPP_URL_CONNECTION_H
#define _IPP_URL_CONNECTION_H


#include "HttpURLConnection.h"


class IppContent;


class IppURLConnection : public HttpURLConnection {
public:
	IppURLConnection(const BUrl& url);
	~IppURLConnection();

	void setIppRequest(IppContent*);
	const IppContent* getIppResponse() const;

	int length();

	ostream& printIppRequest(ostream&);
	ostream& printIppResponse(ostream&);

protected:
	virtual void setRequest();
	virtual void setContent();
	virtual void getContent();

private:
	IppContent* fIppRequest;
	IppContent* fIppResponse;
};

#endif	// _IPP_URL_CONNECTION_H
