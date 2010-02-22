#ifndef WINAMP_H
#define WINAMP_H


#include <windows.h>

class Winamp
{
public:
	static void Initialise(const HWND hMainWindow);
	static int GetTrackLength(void);
	static void Stop(void);
	static void Next(void);
private:
	static HWND theMainWindow;
};

#endif