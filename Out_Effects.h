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
	public class Output_Effects
	{
		public:
			Output_Effects();
			~Output_Effects();

			void set_source(ALuint the_source);
			void on_close(void);

			effects_list get_current_effect(void);
			void set_current_effect(effects_list an_effect);

			bool is_enabled(void);
			void set_enabled(bool enable);

		private:

			bool is_on;
			bool is_loaded;

			effects_list effect;

			ALuint		uiSource;
			ALuint		uiEffectSlot;
			ALuint		uiEffect;
			EFXEAXREVERBPROPERTIES efxReverb;

			ALboolean CreateAuxEffectSlot(ALuint *puiAuxEffectSlot);
			ALboolean CreateEffect(ALuint *puiEffect, ALenum eEffectType);
			ALboolean SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect);
	};
};

#endif