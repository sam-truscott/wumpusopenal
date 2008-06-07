#include "Out_Effects.h"
#include "Out_Wumpus.h"

namespace WinampOpenALOut {

	EffectsModule::EffectsModule(class Output_Wumpus *aPtrOw) {
		effectsEnabled = false;
		effectsSupported = false;
		ptrOw = aPtrOw;
	}

	EffectsModule::~EffectsModule() {
	}

	void EffectsModule::SetEffectsEnabled(bool effectsOn) {

		bool delta = effectsEnabled != effectsOn ? true : false;

		// backup state so we can re-enable auto later
		int currentPosition = ptrOw->GetOutputTime();
		unsigned int tempSampleRate = ptrOw->GetSampleRate();
		unsigned int tempBitsPerSample = ptrOw->GetBitsPerSample();;
		unsigned int tempNumberOfChannels = ptrOw->GetNumberOfChannels();
		bool tempStreamOpen = ptrOw->IsStreamOpen();

		if(delta) {
			// if there is a delta save playing position
			// make sure we're no longer playing
			ptrOw->Close();
		}

		if(!effectsEnabled && effectsOn) {
			// EFFECTS ARE BEING TURNED ON

			// shut down openal
			Framework::getInstance()->ALFWShutdownOpenAL();

			// re-initialise with effects enabled
			ALint attribs[4] = { 0 };
			attribs[0] = ALC_MAX_AUXILIARY_SENDS;
			attribs[1] = MAX_AUX_EFFECTS_SLOTS;
			Framework::getInstance()->ALFWInitOpenAL(
				Framework::getInstance()->getInstance()->GetCurrentDevice(),
				attribs);

			// EFFECT COMPATABILE SOURCE IS NOW READY
			// determine how many sends were allocated

			iSends = -1;
			ALCdevice* pDevice = alcGetContextsDevice(alcGetCurrentContext());
			alcGetIntegerv(pDevice, ALC_MAX_AUXILIARY_SENDS, 1, &iSends);

			if(iSends != -1) {
				this->effectsEnabled = true;
			}

		}else if(effectsEnabled && !effectsOn){
			// EFFECTS ARE BEING TURNED OFF

			// shut down openal
			Framework::getInstance()->ALFWShutdownOpenAL();

			// re-initialise openal without any slots
			Framework::getInstance()->ALFWInitOpenAL(
				Framework::getInstance()->GetCurrentDevice());

			this->effectsEnabled = false;
		}

		if(delta){
			// if there is a delta start re-playing from the position
			if(tempStreamOpen) {
				// re-initialise everything (two buffer times don't matter)
				ptrOw->Open(tempSampleRate, tempNumberOfChannels, tempBitsPerSample, 0,0);
				// reset back to the current position
				ptrOw->Flush(currentPosition);
			}
		}
	}

	void EffectsModule::SourceDetermined(ALuint src) {

		if(!effectsEnabled || !effectsSupported) {
			return;
		}

		return;

		ALenum err;
		this->uiSource = src;
		
		// clear the error buffer
		alGetError();

		// create the aux effects slots
		for(ALCint i = 0; i < iSends; i++) {
			alGenAuxiliaryEffectSlots(1, &uiEffectSlot[i]);
			err = alGetError();
			if(err != AL_NO_ERROR) {
				break;
			}
		}

		// create some effects
		for(ALuint i = 0; i < 2; i++) {
			alGenEffects(1, &uiEffect[i]);
			ALenum err = alGetError();
			if(err != AL_NO_ERROR) {
				break;
			}
		}

		// add a reverb effect
		if( alIsEffect(uiEffect[0])) {
			alEffecti(uiEffect[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);
			err = alGetError();
			if(err != AL_NO_ERROR) {
				return;
			}else{
				alGetError();
				alEffectf(uiEffect[0], AL_REVERB_DECAY_TIME, 5.0);
				err = alGetError();
				if(err != AL_NO_ERROR) {
					return;
				}
			}

			alGetError();
		}

		alEffectf(uiEffect[0], AL_REVERB_GAIN, 1.0);
		alEffectf(uiEffect[0], AL_REVERB_GAINHF, 1.0);

		// add a flanger effect
		alGetError();
		if( alIsEffect(uiEffect[1])) {
			alEffecti(uiEffect[1], AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
			err = alGetError();
			if(err != AL_NO_ERROR) {
				return;
			}else{
				alGetError();
				alEffecti(uiEffect[1], AL_FLANGER_PHASE, 180);
				err = alGetError();
				if(err != AL_NO_ERROR) {
					return;
				}
			}
		}

		// set the gain?
		alGetError();
		alAuxiliaryEffectSlotf(uiEffectSlot[0], AL_EFFECTSLOT_GAIN, 1.0);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		// set the gain?
		alGetError();
		alAuxiliaryEffectSlotf(uiEffectSlot[1], AL_EFFECTSLOT_GAIN, 1.0);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		alGetError();
		alAuxiliaryEffectSloti(uiEffectSlot[0], AL_EFFECTSLOT_EFFECT, uiEffect[0]);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		alGetError();
		alAuxiliaryEffectSloti(uiEffectSlot[1], AL_EFFECTSLOT_EFFECT, uiEffect[1]);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		alGetError();
		// attach effects slot 0 to send 0
		alSource3i(uiSource,AL_AUXILIARY_SEND_FILTER, uiEffectSlot[0],0, AL_FILTER_NULL);
		//alSource3i(uiSource,AL_AUXILIARY_SEND_FILTER, uiEffectSlot[0],1, uiFilter[0]);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		alGetError();
		// attach effects slot 1 to send 1
		//alSource3i(uiSource,AL_AUXILIARY_SEND_FILTER, uiEffectSlot[1],1, AL_FILTER_NULL);
		//alSource3i(uiSource,AL_AUXILIARY_SEND_FILTER, uiEffectSlot[1],1, uiFilter[0]);
		err = alGetError();
		if (err != AL_NO_ERROR) {
			printf("Failed to disable Source Send 1\n");
		}

		alSourcef(uiSource, AL_CONE_OUTER_GAINHF, 0.5f);
		alListenerf(AL_METERS_PER_UNIT, 0.3f);

	}

	void EffectsModule::SourceRemoved() {
		//TODO clean up - delete stuffs

		
	}
}