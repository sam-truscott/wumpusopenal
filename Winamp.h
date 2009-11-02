#ifndef WINAMP_H
#define WINAMP_H


#include <windows.h>

class Winamp
{
public:
	static void Initialise(HWND hMainWindow);
	static int GetTrackLength(void);
private:
	static HWND theMainWindow;
};

#endif