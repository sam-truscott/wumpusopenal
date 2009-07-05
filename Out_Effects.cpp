#include "Out_Effects.h"
#include "Framework\Framework.h"

static const EAXREVERBPROPERTIES REVERB_LOOKUP_TABLE[NO_OF_EFFECTS] = 
{
	REVERB_PRESET_GENERIC,
	REVERB_PRESET_PADDEDCELL,
	REVERB_PRESET_ROOM,
	REVERB_PRESET_BATHROOM,
	REVERB_PRESET_LIVINGROOM,
	REVERB_PRESET_STONEROOM,
	REVERB_PRESET_AUDITORIUM,
	REVERB_PRESET_CONCERTHALL,
	REVERB_PRESET_CAVE,
	REVERB_PRESET_ARENA,
	REVERB_PRESET_HANGAR,
	REVERB_PRESET_CARPETTEDHALLWAY,
	REVERB_PRESET_HALLWAY,
	REVERB_PRESET_STONECORRIDOR,
	REVERB_PRESET_ALLEY,
	REVERB_PRESET_FOREST,
	REVERB_PRESET_CITY,
	REVERB_PRESET_MOUNTAINS,
	REVERB_PRESET_QUARRY,
	REVERB_PRESET_PLAIN,
	REVERB_PRESET_PARKINGLOT,
	REVERB_PRESET_SEWERPIPE,
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
	REVERB_PRESET_PSYCHOTIC,
	REVERB_PRESET_OUTDOORS_BACKYARD,
	REVERB_PRESET_OUTDOORS_ROLLINGPLAINS,
	REVERB_PRESET_OUTDOORS_DEEPCANYON,
	REVERB_PRESET_OUTDOORS_CREEK,
	REVERB_PRESET_OUTDOORS_VALLEY,
	REVERB_PRESET_MOOD_HEAVEN,
	REVERB_PRESET_MOOD_HELL,
	REVERB_PRESET_MOOD_MEMORY,
	REVERB_PRESET_DRIVING_INCAR_RACER,
	REVERB_PRESET_DRIVING_INCAR_LUXURY,
	REVERB_PRESET_DRIVING_TUNNEL,
	REVERB_PRESET_CITY_STREETS,
	REVERB_PRESET_CITY_SUBWAY,
	REVERB_PRESET_CITY_MUSEUM,
	REVERB_PRESET_CITY_LIBRARY,
	REVERB_PRESET_CITY_UNDERPASS,
	REVERB_PRESET_CITY_ABANDONED,
	REVERB_PRESET_DUSTYROOM,
	REVERB_PRESET_CHAPEL,
	REVERB_PRESET_SMALLWATERROOM
};

namespace WinampOpenALOut {

	Output_Effects::Output_Effects()
	{
		is_on = false;
		is_loaded = false;

		effect = EFX_REVERB_PRESET_GENERIC;
		uiSource = 0;
		uiEffectSlot = 0;
		uiEffect = 0;
		memset(
			&efxReverb,
			0,
			sizeof(efxReverb));
	}

	Output_Effects::~Output_Effects()
	{
		if ( is_loaded == true )
		{
			this->on_close();
		}
	}

	void Output_Effects::set_source(ALuint the_source)
	{
		uiSource = the_source;

		if ( this->is_on == true )
		{
			if ( this->is_loaded == true )
			{
				this->on_close();
			}

			if ( Framework::getInstance()->ALFWIsEFXSupported() == AL_TRUE )
			{
				if (CreateAuxEffectSlot(&uiEffectSlot) == AL_TRUE )
				{
					if (CreateEffect(&uiEffect, AL_EFFECT_EAXREVERB))
					{
						EAXREVERBPROPERTIES eaxEffect = REVERB_LOOKUP_TABLE[effect];
						EFXEAXREVERBPROPERTIES efxReverb;
						ConvertReverbParameters(&eaxEffect, &efxReverb);

						if (SetEFXEAXReverbProperties(&efxReverb, uiEffect))
						{
							alAuxiliaryEffectSloti(uiEffectSlot, AL_EFFECTSLOT_EFFECT, uiEffect);

							alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, uiEffectSlot, 0, AL_FILTER_NULL);

							this->is_loaded = true;
						}
					}
				}
			}
		}
	}

	void Output_Effects::on_close(void)
	{
		if ( this->is_loaded == true )
		{
			alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
			alAuxiliaryEffectSloti(uiEffectSlot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
			alDeleteEffects(1, &uiEffect);
			alDeleteAuxiliaryEffectSlots(1, &uiEffectSlot);

			this->is_loaded = false;
		}
	}

	effects_list Output_Effects::get_current_effect(void)
	{
		return effect;
	}

	void Output_Effects::set_current_effect(effects_list an_effect)
	{
		this->on_close();
		effect = an_effect;
		this->set_source(uiSource);
	}

	bool Output_Effects::is_enabled(void)
	{
		return is_on;
	}

	void Output_Effects::set_enabled(bool enable)
	{
		if ( enable == false &&
			is_loaded == true )
		{
			this->on_close();
		}

		if ( enable != is_on )
		{
			this->on_close();
			is_on = enable;
			this->set_source(uiSource);
		}
	}

	ALboolean Output_Effects::CreateAuxEffectSlot(ALuint *puiAuxEffectSlot)
	{
		ALboolean bReturn = AL_FALSE;

		// Clear AL Error state
		alGetError();

		// Generate an Auxiliary Effect Slot
		alGenAuxiliaryEffectSlots(1, puiAuxEffectSlot);
		if (alGetError() == AL_NO_ERROR)
			bReturn = AL_TRUE;

		return bReturn;
	}

	ALboolean Output_Effects::CreateEffect(ALuint *puiEffect, ALenum eEffectType)
	{
		ALboolean bReturn = AL_FALSE;

		if (puiEffect)
		{
			// Clear AL Error State
			alGetError();

			// Generate an Effect
			alGenEffects(1, puiEffect);
			if (alGetError() == AL_NO_ERROR)
			{
				// Set the Effect Type
				alEffecti(*puiEffect, AL_EFFECT_TYPE, eEffectType);
				if (alGetError() == AL_NO_ERROR)
					bReturn = AL_TRUE;
				else
					alDeleteEffects(1, puiEffect);
			}
		}

		return bReturn;
	}

	ALboolean Output_Effects::SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect)
	{
		ALboolean bReturn = AL_FALSE;

		if (pEFXEAXReverb)
		{
			// Clear AL Error code
			alGetError();

			alEffectf(uiEffect, AL_EAXREVERB_DENSITY, pEFXEAXReverb->flDensity);
			alEffectf(uiEffect, AL_EAXREVERB_DIFFUSION, pEFXEAXReverb->flDiffusion);
			alEffectf(uiEffect, AL_EAXREVERB_GAIN, pEFXEAXReverb->flGain);
			alEffectf(uiEffect, AL_EAXREVERB_GAINHF, pEFXEAXReverb->flGainHF);
			alEffectf(uiEffect, AL_EAXREVERB_GAINLF, pEFXEAXReverb->flGainLF);
			alEffectf(uiEffect, AL_EAXREVERB_DECAY_TIME, pEFXEAXReverb->flDecayTime);
			alEffectf(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, pEFXEAXReverb->flDecayHFRatio);
			alEffectf(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, pEFXEAXReverb->flDecayLFRatio);
			alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, pEFXEAXReverb->flReflectionsGain);
			alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, pEFXEAXReverb->flReflectionsDelay);
			alEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, pEFXEAXReverb->flReflectionsPan);
			alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, pEFXEAXReverb->flLateReverbGain);
			alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, pEFXEAXReverb->flLateReverbDelay);
			alEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, pEFXEAXReverb->flLateReverbPan);
			alEffectf(uiEffect, AL_EAXREVERB_ECHO_TIME, pEFXEAXReverb->flEchoTime);
			alEffectf(uiEffect, AL_EAXREVERB_ECHO_DEPTH, pEFXEAXReverb->flEchoDepth);
			alEffectf(uiEffect, AL_EAXREVERB_MODULATION_TIME, pEFXEAXReverb->flModulationTime);
			alEffectf(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, pEFXEAXReverb->flModulationDepth);
			alEffectf(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, pEFXEAXReverb->flAirAbsorptionGainHF);
			alEffectf(uiEffect, AL_EAXREVERB_HFREFERENCE, pEFXEAXReverb->flHFReference);
			alEffectf(uiEffect, AL_EAXREVERB_LFREFERENCE, pEFXEAXReverb->flLFReference);
			alEffectf(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, pEFXEAXReverb->flRoomRolloffFactor);
			alEffecti(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, pEFXEAXReverb->iDecayHFLimit);

			if (alGetError() == AL_NO_ERROR)
				bReturn = AL_TRUE;
		}

		return bReturn;
	}

}