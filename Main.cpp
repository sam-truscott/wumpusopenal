#include "Main.h"
#include "Out_Openal.h"
#include "Out_Wumpus.h"
#include "Version.h"

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

#ifndef SSE_BUILD
WinampOpenALOut::Output_Wumpus *outputWumpus = 0;
#else
Output_Wumpus* outputWumpus = 0;
#endif

/*
	point to the correct procedures
*/
Out_Module out = {
	OUT_VER,
	TITLE, 
	800005,
	0, // hmainwindow
	0, // hdllinstance
	_Config,
	_About,
	_Initialise,
	_Quit,
	_Open,
	_Close,
	_Write,
	_CanWrite,
	_IsPlaying,
	_Pause,
	_SetVolume,
	_SetPan,
	_Flush,
	_GetOutputTime,
	_GetWrittenTime
};

void _Config(HWND hwndParent) {
	outputWumpus->Config(hwndParent);
}

void _About(HWND hwndParent) {
	outputWumpus->About(hwndParent);
}

void _Initialise() {

	/*_CrtSetBreakAlloc(228);
	_CrtSetBreakAlloc(203);
	_CrtSetBreakAlloc(194);*/

	if(outputWumpus) {
		outputWumpus->Quit();
		delete outputWumpus;
		outputWumpus = 0;
	}
#ifndef SSE_BUILD
	outputWumpus = new WinampOpenALOut::Output_Wumpus();
#else
	outputWumpus = new Output_Wumpus();
#endif
	if(outputWumpus) {
		outputWumpus->Initialise(out.hMainWindow);
	}
}

void _Quit() {
	if(outputWumpus) {
		outputWumpus->Quit();
		delete outputWumpus;
		outputWumpus = NULL;
	}

#ifdef _DEBUG
	if(_CrtDumpMemoryLeaks()) {
		_ASSERTE( _CrtCheckMemory( ) );
	}
#endif
}

int _Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms) {
	if(outputWumpus) {
		return outputWumpus->Open(samplerate, numchannels, bitspersamp, bufferlenms, prebufferms);
	}else{
		return 0;
	}
}

void _Close() {
	if(outputWumpus) {
		outputWumpus->Close();
	}
}

int _Write(char *buf, int len) {
	if(outputWumpus) {
		return outputWumpus->Write(buf, len);
	}else{
		return -1;
	}
}
	
int _CanWrite() {
	if(outputWumpus) {
		return outputWumpus->CanWrite();
	}else{
		return 0;
	}
}

int _IsPlaying() {
	if(outputWumpus) {
		return outputWumpus->IsPlaying();
	}else{
		return 0;
	}
}

int _Pause(int pause) {
	if(outputWumpus) {
		return outputWumpus->Pause(pause);
	}else{
		return pause;
	}
}


void _SetVolume(int volume) {
	if(outputWumpus) {
		outputWumpus->SetVolume(volume);
	}
}

void _SetPan(int pan) {
	if(outputWumpus) {
		outputWumpus->SetPan(pan);
	}
}

void _Flush(int t) {
	if(outputWumpus) {
		outputWumpus->Flush(t);
	}
}

int _GetOutputTime() {
	if(outputWumpus) {
		return outputWumpus->GetOutputTime();
	}else{
		return 0;
	}
}

int _GetWrittenTime() {
	if(outputWumpus) {
		return outputWumpus->GetWrittenTime();
	}
	return 0;
}

extern "C" {

	__declspec( dllexport ) Out_Module * winampGetOutModule()
	{
		return &out;
	}

}