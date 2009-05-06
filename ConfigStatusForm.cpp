#include "Out_Wumpus.h"
#include "ConfigStatusForm.h"

namespace WinampOpenALOut {

	void Config::ShowDeviceDetails() {
		// clear down other items already in the list
		listBoxExtensions->Items->Clear();

		listBoxExtensions->Items->Add( "Device: " + comboBoxDevices->Items[currentDevice]->ToString());

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATION_EXT");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATION_EXT");
		}

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATE_ALL_EXT");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATE_ALL_EXT");
		}

		// get the device
		ALCdevice* pDevice = alcGetContextsDevice(alcGetCurrentContext());
		
		if (Framework::getInstance()->ALFWIsXRAMSupported() == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX-RAM");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX-RAM [values below must be > 0]");
		}

		unsigned int setaddr = (unsigned int)alGetProcAddress("EAXSetBufferMode");
		unsigned int getaddr = (unsigned int)alGetProcAddress("EAXGetBufferMode");

		listBoxExtensions->Items->Add("\tDetected? " + alIsExtensionPresent("EAX-RAM"));
		listBoxExtensions->Items->Add("\tEAXSetBufferMode: " + setaddr);
		listBoxExtensions->Items->Add("\tEAXGetBufferMode: " + getaddr);
		listBoxExtensions->Items->Add("\tAL_EAX_RAM_SIZE: " + alGetEnumValue("AL_EAX_RAM_SIZE"));
		listBoxExtensions->Items->Add("\tAL_EAX_RAM_FREE: " + alGetEnumValue("AL_EAX_RAM_FREE"));
		listBoxExtensions->Items->Add("\tAL_STORAGE_AUTOMATIC: " + alGetEnumValue("AL_STORAGE_AUTOMATIC"));
		listBoxExtensions->Items->Add("\tAL_STORAGE_HARDWARE: " + alGetEnumValue("AL_STORAGE_HARDWARE"));
		listBoxExtensions->Items->Add("\tAL_STORAGE_ACCESSIBLE: " + alGetEnumValue("AL_STORAGE_ACCESSIBLE"));

		/*if (alcIsExtensionPresent(pDevice, "ALC_EXT_EFX") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: ALC_EXT_EFX");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_EXT_EFX");
		}*/

		/*if (alcIsExtensionPresent(pDevice, "EAX2.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 2.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 2.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX3.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 3.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 3.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX4.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 4.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 4.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX5.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 5.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 5.0");
		}

		if (alIsExtensionPresent( "AL_EXT_float32") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: AL_EXT_FLOAT32");
		}else{
			listBoxExtensions->Items->Add("Absent: AL_EXT_FLOAT32");
		}*/

	}
}
