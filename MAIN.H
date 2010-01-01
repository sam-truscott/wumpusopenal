#include <windows.h>

void WAOP_Config(HWND hwndParent);
void WAOP_About(HWND hwndParent);
void WAOP_Initialise();
void WAOP_Quit();
int WAOP_Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
void WAOP_Close();
int WAOP_Write(char *buf, int len);
int WAOP_CanWrite();
int WAOP_IsPlaying();
int WAOP_Pause(int pause);
void WAOP_SetVolume(int volume);
void WAOP_SetPan(int pan);
void WAOP_Flush(int t);
int WAOP_GetOutputTime();
int WAOP_GetWrittenTime();