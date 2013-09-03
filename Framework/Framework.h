#ifndef _FRAMEWORK_H_ // Win32 version
#define _FRAMEWORK_H_

#pragma once

// Get some classic includes
#include<Windows.h>
#include<stdio.h>
#include<io.h>
#include<fcntl.h>
#include<conio.h>
#include"al.h"
#include"alc.h"
#include"efx.h"
#include"efx-creative.h"

class Framework {
	public:

		Framework();
		~Framework();

		// OpenAL initialization and shutdown
		ALboolean ALFWInitOpenAL(const int myDevice) 
			{ return ALFWInitOpenAL(myDevice, NULL); };
		ALboolean ALFWInitOpenAL(const int myDevice, const ALint attrs[]);
		ALboolean ALFWShutdownOpenAL();

		static Framework* getInstance() {
			if( !framework ) {
				framework = new Framework();
			}
			return framework;
		}

		static void deleteInstance() {
			if(framework) {
				delete framework;
				framework = NULL;
			}
		}

		ALCdevice* GetDevice() { return (ALCdevice*)ptrDevice; }
		int GetCurrentDevice()	{return currentDevice; }

		class ALDeviceList* ALFWGetDeviceList() { return pDeviceList; }

		// Extension Queries 
		ALboolean ALFWIsEFXSupported();
	protected:
		class ALDeviceList *pDeviceList;
		void *ptrContext;
		void *ptrDevice;
		
		int currentDevice;

		static Framework* framework;
};

// EFX Extension function pointer variables

// Effect objects
extern LPALGENEFFECTS alGenEffects;
extern LPALDELETEEFFECTS alDeleteEffects;
extern LPALISEFFECT alIsEffect;
extern LPALEFFECTI alEffecti;
extern LPALEFFECTIV alEffectiv;
extern LPALEFFECTF alEffectf;
extern LPALEFFECTFV alEffectfv;
extern LPALGETEFFECTI alGetEffecti;
extern LPALGETEFFECTIV alGetEffectiv;
extern LPALGETEFFECTF alGetEffectf;
extern LPALGETEFFECTFV alGetEffectfv;

// Filter objects
extern LPALGENFILTERS alGenFilters;
extern LPALDELETEFILTERS alDeleteFilters;
extern LPALISFILTER alIsFilter;
extern LPALFILTERI alFilteri;
extern LPALFILTERIV alFilteriv;
extern LPALFILTERF alFilterf;
extern LPALFILTERFV alFilterfv;
extern LPALGETFILTERI alGetFilteri;
extern LPALGETFILTERIV alGetFilteriv;
extern LPALGETFILTERF alGetFilterf;
extern LPALGETFILTERFV alGetFilterfv;

// Auxiliary slot object
extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

// XRAM Extension function pointer variables and enum values

typedef ALboolean (__cdecl *LPEAXSETBUFFERMODE)(ALsizei n, ALuint *buffers, ALint value);
typedef ALenum    (__cdecl *LPEAXGETBUFFERMODE)(ALuint buffer, ALint *value);

extern LPEAXSETBUFFERMODE eaxSetBufferMode;
extern LPEAXGETBUFFERMODE eaxGetBufferMode;

// X-RAM Enum values
extern ALenum eXRAMSize, eXRAMFree;
extern ALenum eXRAMAuto, eXRAMHardware, eXRAMAccessible;

#endif _FRAMEWORK_H_