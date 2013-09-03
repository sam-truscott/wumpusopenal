#ifndef OUT_EFFECTS
#define OUT_EFFECTS

#include "al.h"
#include "efx.h"
#include "efx-presets.h"

typedef enum
{
	REVERB_PRESET_GENERIC = 0,
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
} effects_list;

#define NO_OF_EFFECTS (REVERB_PRESET_SMALLWATERROOM + 1)

#define MAX_NO_CHANNELS 8

static const char* REVERB_NAMES_TABLE[] = 
{
	"Generic Reverb",
	"Padded Cell",
	"Room",
	"Bathroom",
	"Living Room",
	"Stone Room",
	"Auditorium",
	"Concert Hall",
	"Cave",
	"Arena",
	"Hanger",
	"Carpetted Hallway",
	"Hallway",
	"Stone Corridor",
	"Alley",
	"Forest",
	"City",
	"Mountains",
	"Quarry",
	"Plain",
	"Car Park [Parking Lot]",
	"Drains [Sewer Pipe]",
	"Underwater",
	"Drugged",
	"Dizzy",
	"Psychotic",
	"Outdoors - Backyard",
	"Outdoors - Rolling Plains",
	"Outdoors - Deep Canyon",
	"Outdoors - Creek",
	"Outdoors - Valley",
	"Mood - Heaven",
	"Mood - Hell",
	"Mood - Memory",
	"Driving - Racing",
	"Driving - Luxury",
	"Driving - Tunnel",
	"City - Streets",
	"City - Tube [Subway]",
	"City - Museum",
	"City - Library",
	"City - Underpass",
	"City - Abandoned",
	"Dusty Room",
	"Chapel",
	"Small Water Room"
};

namespace WinampOpenALOut
{

	typedef enum
	{
		EAX_LOADED_OK,
		EAX_FAIL_PROPERTIES,
		EAX_FAIL_CREATE_EFFECT,
		EAX_FAIL_EFFECT_SLOT,
		EAX_NOT_SUPPORTED,
		EAX_NOT_LOADED
	} eax_load_status;

	/*
	 * This class abstracts the [reverb] effects for multiple
	 * sources.
	 */
#ifndef NATIVE
	public class Output_Effects
#else
	class Output_Effects
#endif
	{
		public:
			Output_Effects();
			~Output_Effects();

			eax_load_status Setup(void);
			void AddSource(const ALuint the_source);
			void OnClose(void);

			effects_list GetCurrentEffect(void);
			void SetCurrentEffect(const effects_list an_effect);

			bool IsEnabled(void);
			bool Enable(const bool enable);

		private:

			void CleanUp(const eax_load_status cleanup_reason);

			bool is_on;

			effects_list effect;

			eax_load_status status;

			ALuint		source[MAX_NO_CHANNELS];
			unsigned char channels;
			ALuint		effect_slot;
			ALuint		the_effect;
			EFXEAXREVERBPROPERTIES efx_reverb;

			ALboolean CreateAuxEffectSlot(ALuint *aux_effect_slot);
			ALboolean CreateEffect(ALuint *effect, ALenum effect_type);
			ALboolean SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *efx_eax_reverb, ALuint effect);
	};
};

#endif