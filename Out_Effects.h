#include "al.h"
#include "alc.h"

#define MAX_AUX_EFFECTS_SLOTS 4
#define MAX_EFFECTS 2
#define MAX_FILTERS 2

namespace WinampOpenALOut {
	class EffectsModule {
		public:
			EffectsModule(class Output_Wumpus *aPtrOw);
			~EffectsModule();

			inline bool GetEffectsEnabled()					{ return effectsEnabled; }
			inline bool GetEffectsSupported()				{ return effectsSupported; }

			inline void	SetEffectsSupported(bool b)			{ 
				effectsSupported = b;
				// if effects aren't support, they can't be enabled either
				if(!b) { SetEffectsEnabled(false); }
			}

			void SourceDetermined(ALuint src);
			void SourceRemoved();

			void SetEffectsEnabled(bool b);

		protected:

			class Output_Wumpus *ptrOw;
			ALuint uiSource;

			ALCint iSends;

			ALuint uiEffectSlot[MAX_AUX_EFFECTS_SLOTS];
			ALuint uiEffect[MAX_EFFECTS];
			ALuint uiFilter[MAX_FILTERS];

			bool effectsEnabled;
			bool effectsSupported;

		private:
	};
}