#include "al.h"
#include "efx.h"
#include "EFX-Util.h"

#ifndef OUT_EFFECTS
#define OUT_EFFECTS

typedef enum
{
	EFX_REVERB_PRESET_GENERIC = 0,
	EFX_REVERB_PRESET_PADDEDCELL = 1,
	EFX_REVERB_PRESET_ROOM = 2,
	EFX_REVERB_PRESET_BATHROOM = 3,
	EFX_REVERB_PRESET_LIVINGROOM = 4,
	EFX_REVERB_PRESET_STONEROOM = 5,
	EFX_REVERB_PRESET_AUDITORIUM = 6,
	EFX_REVERB_PRESET_CONCERTHALL = 7,
	EFX_REVERB_PRESET_CAVE = 8,
	EFX_REVERB_PRESET_ARENA = 9,
	EFX_REVERB_PRESET_HANGAR = 10,
	EFX_REVERB_PRESET_CARPETTEDHALLWAY = 11,
	EFX_REVERB_PRESET_HALLWAY = 12,
	EFX_REVERB_PRESET_STONECORRIDOR = 13,
	EFX_REVERB_PRESET_ALLEY = 14,
	EFX_REVERB_PRESET_FOREST = 15,
	EFX_REVERB_PRESET_CITY = 16,
	EFX_REVERB_PRESET_MOUNTAINS = 17,
	EFX_REVERB_PRESET_QUARRY = 18,
	EFX_REVERB_PRESET_PLAIN = 19,
	EFX_REVERB_PRESET_PARKINGLOT = 20,
	EFX_REVERB_PRESET_SEWERPIPE = 21,
	EFX_REVERB_PRESET_UNDERWATER = 22,
	EFX_REVERB_PRESET_DRUGGED = 23,
	EFX_REVERB_PRESET_DIZZY = 24,
	EFX_REVERB_PRESET_PSYCHOTIC = 25,
	EFX_REVERB_PRESET_OUTDOORS_BACKYARD = 26,
	EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS = 27,
	EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON = 28,
	EFX_REVERB_PRESET_OUTDOORS_CREEK = 29,
	EFX_REVERB_PRESET_OUTDOORS_VALLEY = 30,
	EFX_REVERB_PRESET_MOOD_HEAVEN = 31,
	EFX_REVERB_PRESET_MOOD_HELL = 32,
	EFX_REVERB_PRESET_MOOD_MEMORY = 33,
	EFX_REVERB_PRESET_DRIVING_INCAR_RACER = 34,
	EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY = 35,
	EFX_REVERB_PRESET_DRIVING_TUNNEL = 36,
	EFX_REVERB_PRESET_CITY_STREETS = 37,
	EFX_REVERB_PRESET_CITY_SUBWAY = 38,
	EFX_REVERB_PRESET_CITY_MUSEUM = 39,
	EFX_REVERB_PRESET_CITY_LIBRARY = 40,
	EFX_REVERB_PRESET_CITY_UNDERPASS = 41,
	EFX_REVERB_PRESET_CITY_ABANDONED = 42,
	EFX_REVERB_PRESET_DUSTYROOM = 43,
	EFX_REVERB_PRESET_CHAPEL = 44,
	EFX_REVERB_PRESET_SMALLWATERROOM = 45
} effects_list;

#define NO_OF_EFFECTS EFX_REVERB_PRESET_SMALLWATERROOM + 1

#define MAX_NO_CHANNELS 8

static const char* REVERB_NAMES_TABLE[NO_OF_EFFECTS] = 
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
			void AddSource(ALuint the_source);
			void OnClose(void);

			effects_list GetCurrentEffect(void);
			void SetCurrentEffect(effects_list an_effect);

			bool IsEnabled(void);
			bool Enable(bool enable);

		private:

			void CleanUp(eax_load_status cleanup_reason);

			bool is_on;
			bool is_loaded;

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