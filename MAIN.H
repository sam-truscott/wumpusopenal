#include <windows.h>
extern int config_whichdevice;
extern unsigned char config_panrev;
extern unsigned char config_altvolset;
extern unsigned char config_iot,config_volumeenabled;
extern unsigned char config_priority;

void _Config(HWND hwndParent);
void _About(HWND hwndParent);
void _Initialise();
void _Quit();
int _Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
void _Close();
int _Write(char *buf, int len);
int _CanWrite();
int _IsPlaying();
int _Pause(int pause);
void _SetVolume(int volume);
void _SetPan(int pan);
void _Flush(int t);
int _GetOutputTime();
int _GetWrittenTime();