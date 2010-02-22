#include "Winamp.h"
#include "wa_ipc.h"

HWND Winamp::theMainWindow = 0;

void Winamp::Initialise(const HWND hMainWindow)
{
	theMainWindow = hMainWindow;
}

int Winamp::GetTrackLength()
{
	LRESULT len = SendMessage(
		theMainWindow,
		WM_WA_IPC,
		2,
		IPC_GETOUTPUTTIME );

	return (int)len;
}

void Winamp::Next()
{
	PostMessage(theMainWindow, WM_COMMAND,40048,0);
}

void Winamp::Stop()
{
	PostMessage(theMainWindow, WM_COMMAND,40047,0);
}