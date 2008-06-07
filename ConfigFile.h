#ifndef SSE_BUILD
namespace WinampOpenALOut {
#endif

#define CONF_TRUE "true"
#define CONF_FALSE "false"

#define CONF_DEVICE "DefaultDevice"
#define DEFC_DEVICE 0
#define CONF_EFX "EffectsEnabled"
#define CONF_BUFFER_LENGTH "BufferLength"
#define DEFC_BUFFER_LENGTH 2000

#ifndef SSE_BUILD
	public class ConfigFile {
#else
	class ConfigFile {
#endif

	private:
		static char INI_FILE[MAX_PATH];
	public:
		static void Initialise();

		static void ReadString(char *name,char *data, int mlen);
		static void WriteString(char *name,char *data);
		
		static int ReadInteger(char *name);
		static void WriteInteger(char *name, int i);

		static bool ReadBoolean(char *name);
		static void WriteBoolean(char* name, bool v);
	};
#ifndef SSE_BUILD
}
#endif