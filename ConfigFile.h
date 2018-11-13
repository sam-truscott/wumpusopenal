#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

namespace WinampOpenALOut {

#define CONF_TRUE "true"
#define CONF_FALSE "false"

/*
 * The string variables present in the configuration file
 */
#define CONF_DEVICE "DefaultDevice"
#define CONF_BUFFER_LENGTH "BufferLength"
#define CONF_MONO_EXPAND "MonoExpansion"
#define CONF_STEREO_EXPAND "StereoExpansion"
#define CONF_XRAM_ENABLED "UseXRAM"
#define CONF_VOLUME "volume"
#define CONF_PITCH "OpenALPitch"
#define CONF_SPLIT "Enable3D"

#ifndef NATIVE
	public class ConfigFile
#else
	class ConfigFile
#endif
	{
	private:
		static char INI_FILE[MAX_PATH];
	public:

		/* initialise the configuration file */
		static void Initialise(const HWND hMainWindow);

		static void ReadString(const char *name,char *data, const int mlen);
		static void WriteString(const char *name, const char *data);
		
		static int ReadInteger(const char *name);
		static void WriteInteger(const char *name, const int i);

		static float ReadFloat(const char *name, bool* valid);
		static void WriteFloat(const char* name, const float f);

		static bool ReadBoolean(const char *name);
		static void WriteBoolean(const char* name, const bool v);

		static void ReadGlobalString(const char *name,char *data, const int mlen);
		static int ReadGlobalInteger(const char *name);
	};
}

#endif