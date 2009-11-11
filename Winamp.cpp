#include "Winamp.h"
#include "wa_ipc.h"

HWND Winamp::theMainWindow = 0;

void Winamp::Initialise(HWND hMainWindow)
{
	theMainWindow = hMainWindow;
}

int Winamp::GetTrackLength()
{
	//basicFileInfoStruct info;
	//memset(&info, 0, sizeof(info));
	//SendMessage(theMainWindow, WM_WA_IPC, (WPARAM)&info, IPC_GET_BASIC_FILE_INFO );

	LRESULT len = SendMessage(
		theMainWindow,
		WM_WA_IPC,
		2,
		IPC_GETOUTPUTTIME );

	return (int)len;
}
