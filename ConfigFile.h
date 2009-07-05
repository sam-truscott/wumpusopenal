namespace WinampOpenALOut {

#define CONF_TRUE "true"
#define CONF_FALSE "false"

#define CONF_DEVICE "DefaultDevice"
#define CONF_BUFFER_LENGTH "BufferLength"
#define CONF_MONO_EXPAND "MonoExpansion"
#define CONF_STEREO_EXPAND "StereoExpansion"
#define CONF_XRAM_ENABLED "UseXRAM"
#define CONF_VOLUME "volume"
#define CONF_EFX_ENABLED "EffectsEnabled"
#define CONF_EFX_ENV "EffectEnvironment"
#define CONF_PITCH "OpenALPitch"

	public class ConfigFile {

	private:
		static char INI_FILE[MAX_PATH];
	public:
		static void Initialise(HWND hMainWindow);

		static void ReadString(char *name,char *data, int mlen);
		static void WriteString(char *name,char *data);
		
		static int ReadInteger(char *name);
		static void WriteInteger(char *name, int i);

		static bool ReadBoolean(char *name);
		static void WriteBoolean(char* name, bool v);

		static void ReadGlobalString(char *name,char *data, int mlen);
		static int ReadGlobalInteger(char *name);
	};
}