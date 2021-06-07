// Sun, 18 Jun 2000
// Y.Takagi
#ifndef __IPP_SETUP_DLG_H
#define __IPP_SETUP_DLG_H


#include <Window.h>


class BDirectory;


class IppSetupDlg : public BWindow {
public:
	IppSetupDlg(BDirectory* directory);
	~IppSetupDlg() {}

	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage* message);

	int Go();

private:
	status_t fResult;
	sem_id fSemaphore;
};

#endif	// __IPP_SETUP_DLG_H
