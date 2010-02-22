/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "aldlist.h"
#include <windows.h>
#include "alc.h"

/* 
 * Init call
 */
ALDeviceList::ALDeviceList()
{
	ALDEVICEINFO	ALDeviceInfo;
	char *devices;
	int index;
	const char *defaultDeviceName;
	const char *actualDeviceName;

	// DeviceInfo vector stores, for each enumerated device, it's device name, selection status, spec version #, and extension support
	vDeviceInfo.empty();

	defaultDeviceIndex = 0;

	// grab function pointers for 1.0-API functions, and if successful proceed to enumerate all devices
	if (LoadOAL10Library(NULL, &ALFunction) == TRUE)
	{
		if (ALFunction.alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT"))
		{
			devices = (char *)ALFunction.alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
			defaultDeviceName = (char *)ALFunction.alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
			index = 0;
		}
		else if(ALFunction.alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
		{
			devices = (char *)ALFunction.alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
			defaultDeviceName = (char *)ALFunction.alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
			index = 0;
		}
		else
		{
			return;
		}
		// go through device list (each device terminated with a single NULL, list terminated with double NULL)
		while (*devices != NULL)
		{
			if (strcmp(defaultDeviceName, devices) == 0)
			{
				defaultDeviceIndex = index;
			}
			ALCdevice *device = ALFunction.alcOpenDevice(devices);
			if (device)
			{
				ALCcontext *context = ALFunction.alcCreateContext(device, NULL);
				if (context)
				{
					ALFunction.alcMakeContextCurrent(context);
					// if new actual device name isn't already in the list, then add it...
					actualDeviceName = ALFunction.alcGetString(device, ALC_DEVICE_SPECIFIER);
					bool bNewName = true;
					for (int i = 0; i < GetNumDevices(); i++)
					{
						if (strcmp(GetDeviceName(i), actualDeviceName) == 0)
						{
							bNewName = false;
						}
					}
					if ((bNewName) && (actualDeviceName != NULL) && (strlen(actualDeviceName) > 0))
					{
						ALDeviceInfo.strDeviceName = actualDeviceName;
						vDeviceInfo.push_back(ALDeviceInfo);
					}
					ALFunction.alcMakeContextCurrent(NULL);
					ALFunction.alcDestroyContext(context);
				}
				ALFunction.alcCloseDevice(device);
			}
			devices += strlen(devices) + 1;
			index += 1;
		}
	}
}

/* 
 * Exit call
 */
ALDeviceList::~ALDeviceList()
{
	vector <ALDEVICEINFO>::iterator it;
	for ( it = vDeviceInfo.begin( ) ; it != vDeviceInfo.end( ) ; it++ )
	{
		ALDEVICEINFO cli = *it;
		cli.strDeviceName.empty();
	}
	vDeviceInfo.empty();

	UnloadOAL10Library();
}

/*
 * Returns the number of devices in the complete device list
 */
int ALDeviceList::GetNumDevices()
{
	return (int)vDeviceInfo.size();	
}

/* 
 * Returns the device name at an index in the complete device list
 */
char * ALDeviceList::GetDeviceName(const int index)
{
	if (index < GetNumDevices())
		return (char*)vDeviceInfo[index].strDeviceName.c_str();
	else
		return NULL;
}

/*
 * returns the index of the default device in the complete device list
 */
int ALDeviceList::GetDefaultDevice()
{
	return defaultDeviceIndex;
}
