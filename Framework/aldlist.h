#ifndef ALDEVICELIST_H
#define ALDEVICELIST_H

#pragma warning(disable: 4786)  //disable warning "identifier was truncated to '255' characters in the browser information"
#include <vector>
#include <string>
#include "LoadOAL.h"

using namespace std;

#define MAX_DEVICE_NAME_LEN 256

typedef struct
{
	string			strDeviceName;
} ALDEVICEINFO, *LPALDEVICEINFO;

class ALDeviceList
{
private:
	OPENALFNTABLE	ALFunction;
	vector<ALDEVICEINFO> vDeviceInfo;
	int defaultDeviceIndex;

public:
	ALDeviceList ();
	~ALDeviceList ();
	int GetNumDevices();
	char *GetDeviceName(const int index);
	int GetDefaultDevice();

private:

};

#endif // ALDEVICELIST_H
