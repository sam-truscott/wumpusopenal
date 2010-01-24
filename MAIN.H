#include <windows.h>

inline void WAOP_Config(HWND hwndParent);
inline void WAOP_About(HWND hwndParent);
inline void WAOP_Initialise();
inline void WAOP_Quit();
inline int WAOP_Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
inline void WAOP_Close();
inline int WAOP_Write(char *buf, int len);
inline int WAOP_CanWrite();
inline int WAOP_IsPlaying();
inline int WAOP_Pause(int pause);
inline void WAOP_SetVolume(int volume);
inline void WAOP_SetPan(int pan);
inline void WAOP_Flush(int t);
inline int WAOP_GetOutputTime();
inline int WAOP_GetWrittenTime();