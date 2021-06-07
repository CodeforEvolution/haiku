// Sun, 18 Jun 2000
// Y.Takagi


#include "IppSetupDlg.h"

#include <string.h>
#include <strings.h>

#include <Alert.h>
#include <Button.h>
#include <Directory.h>
#include <Rect.h>
#include <TextControl.h>
#include <View.h>
#include <Url.h>

#include "DbgMsg.h"
#include "IppContent.h"
#include "IppDefs.h"
#include "IppURLConnection.h"


#define	DLG_WIDTH		350
#define DLG_HEIGHT		80

#define BUTTON_WIDTH	70
#define BUTTON_HEIGHT	20

#define URL_H			10
#define URL_V			10
#define URL_WIDTH		(DLG_WIDTH - URL_H - URL_H)
#define URL_HEIGHT		20
#define URL_TEXT		"URL"

#define OK_H			(DLG_WIDTH  - BUTTON_WIDTH  - 11)
#define OK_V			(DLG_HEIGHT - BUTTON_HEIGHT - 11)
#define OK_TEXT			"OK"

#define CANCEL_H		(OK_H - BUTTON_WIDTH - 12)
#define CANCEL_V		OK_V
#define CANCEL_TEXT		"Cancel"


const BRect URL_RECT(
	URL_H,
	URL_V,
	URL_H + URL_WIDTH,
	URL_V + URL_HEIGHT);

const BRect OK_RECT(
	OK_H,
	OK_V,
	OK_H + BUTTON_WIDTH,
	OK_V + BUTTON_HEIGHT);

const BRect CANCEL_RECT(
	CANCEL_H,
	CANCEL_V,
	CANCEL_H + BUTTON_WIDTH,
	CANCEL_V + BUTTON_HEIGHT);

enum MSGS {
	M_CANCEL = 1,
	M_OK
};


class IppSetupView : public BView {
public:
	IppSetupView(BRect frame, BDirectory* directory);
	~IppSetupView() {}

	virtual void AttachedToWindow();

	bool UpdateViewData();

private:
	BTextControl* fUrlTextControl;
	BDirectory* fDirectory;
};


IppSetupView::IppSetupView(BRect frame, BDirectory* directory)
	:
	BView(frame, "", B_FOLLOW_ALL, B_WILL_DRAW),
	fDirectory(directory)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
}


void
IppSetupView::AttachedToWindow()
{
	/* url box */

	fUrlTextControl = new BTextControl(URL_RECT, "", URL_TEXT, "", NULL);
	AddChild(fUrlTextControl);
	fUrlTextControl->SetDivider(StringWidth(URL_TEXT) + 10);

	/* cancel */

	BButton* button = new BButton(CANCEL_RECT, "", CANCEL_TEXT,
		new BMessage(M_CANCEL));
	AddChild(button);

	/* ok */

	button = new BButton(OK_RECT, "", OK_TEXT, new BMessage(M_OK));
	AddChild(button);
	button->MakeDefault(true);
}


bool
IppSetupView::UpdateViewData()
{
	BString errorMsg = B_EMPTY_STRING;

	if (fUrlTextControl->Text() != NULL) {
		IppContent* request = new IppContent();
		request->setOperationId(IPP_GET_PRINTER_ATTRIBUTES);
		request->setDelimiter(IPP_OPERATION_ATTRIBUTES_TAG);
		request->setCharset("attributes-charset", "utf-8");
		request->setNaturalLanguage("attributes-natural-language", "en-us");
		request->setURI("printer-uri", fUrlTextControl->Text());
		request->setDelimiter(IPP_END_OF_ATTRIBUTES_TAG);



		IppURLConnection conn(BUrl(fUrlTextControl->Text()));
		conn.setIppRequest(request);
		conn.setRequestProperty("Connection", "close");

		HTTP_RESPONSECODE response_code = conn.getResponseCode();
		if (response_code == HTTP_OK) {
			const char* content_type = conn.getContentType();
			if (content_type == NULL
				|| strncasecmp(content_type, "application/ipp", 15) == 0) {
				const IppContent* ipp_response = conn.getIppResponse();
				if (ipp_response->good()) {
					dir->WriteAttrString(IPP_URL, url->Text());
					return true;
				} else
					errorMsg = ipp_response->getStatusMessage();
			} else
				errorMsg = "cannot get a IPP response.";
		} else if (response_code != HTTP_UNKNOWN)
			errorMsg = conn.getResponseMessage();
		else
			errorMsg = "cannot connect to the IPP server.";
	} else
		errorMsg = "please input the printer URL.";

	BAlert* alert = new BAlert("", errorMsg, "OK");
	alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
	alert->Go();

	return false;
}


IppSetupDlg::IppSetupDlg(BDirectory* directory)
	:
	BWindow(BRect(100, 100, 100 + DLG_WIDTH, 100 + DLG_HEIGHT), "IPP Setup",
	B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_RESIZABLE |
	B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_CLOSE_ON_ESCAPE),
	fResult(B_ERROR)
{
	Lock();
	IppSetupView* view = new IppSetupView(Bounds(), directory);
	AddChild(view);
	Unlock();

	fSemaphore = create_sem(0, "IppSetupSem");
}


bool
IppSetupDlg::QuitRequested()
{
	fResult = B_ERROR;
	release_sem(fSemaphore);

	return true;
}


void
IppSetupDlg::MessageReceived(BMessage* msg)
{
	bool success = false;

	switch (msg->what) {
		case M_OK:
		{
			Lock();
			success = ((IppSetupView*)ChildAt(0))->UpdateViewData();
			Unlock();

			if (success) {
				fResult = B_OK;
				release_sem(fSemaphore);
			}

			break;
		}

		case M_CANCEL:
		{
			fResult = B_ERROR;
			release_sem(fSemaphore);

			break;
		}

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}


int
IppSetupDlg::Go()
{
	Show();

	acquire_sem(fSemaphore);
	delete_sem(fSemaphore);
	int value = fResult;

	Lock();
	Quit();

	return value;
}
