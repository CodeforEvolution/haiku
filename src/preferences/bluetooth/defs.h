#ifndef DEFS_H_
#define DEFS_H_

#include <bluetooth/LocalDevice.h>

#include <bluetoothserver_p.h>


#define APPLY_SETTINGS 'aply'
#define REVERT_SETTINGS 'rvrt'
#define DEFAULT_SETTINGS 'dflt'
#define TRY_SETTINGS 'trys'

#define ATTRIBUTE_CHOSEN 'atch'

#define SET_VISIBLE 		'sVis'
#define SET_DISCOVERABLE 	'sDis'
#define SET_AUTHENTICATION 	'sAth'

#define PREFS_CHOSEN 'prch'

// user interface
const uint32 kBorderSpace = 10;
const uint32 kItemSpace = 7;

// message types
static const uint32 kMsgAddToRemoteList = 'aDdL';
static const uint32 kMsgRefresh = 'rFLd';

static const int32 kMsgSetConnectionPolicy = 'sCpo';
static const int32 kMsgSetDeviceClass = 'sDC0';
static const int32 kMsgSetInquiryTime = 'afEa';
static const int32 kMsgLocalSwitched = 'lDsW';

static const uint32 kMsgDeviceListSelection = 'devs';
static const uint32 kMsgDeviceListInvoke = 'devi';

// globals
extern LocalDevice* gActiveLocalDevice;

#endif
