#include "Main.h"
#include "Out_Openal.h"
#include "Out_Wumpus.h"
#include "Version.h"

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

WinampOpenALOut::Output_Wumpus *outputWumpus = 0;

/*
	point to the correct procedures
*/
Out_Module out =
{
	OUT_VER,
	TITLE, 
	900009,
	0, // hmainwindow
	0, // hdllinstance
	WAOP_Config,
	WAOP_About,
	WAOP_Initialise,
	WAOP_Quit,
	WAOP_Open,
	WAOP_Close,
	WAOP_Write,
	WAOP_CanWrite,
	WAOP_IsPlaying,
	WAOP_Pause,
	WAOP_SetVolume,
	WAOP_SetPan,
	WAOP_Flush,
	WAOP_GetOutputTime,
	WAOP_GetWrittenTime
};

void WAOP_Config(HWND hwndParent)
{
	outputWumpus->Config(hwndParent);
}

void WAOP_About(HWND hwndParent)
{
	outputWumpus->About(hwndParent);
}

void WAOP_Initialise()
{
	//_CrtSetBreakAlloc(220);
	//_CrtSetBreakAlloc(202);

	if(outputWumpus)
	{
		outputWumpus->Quit();
		delete outputWumpus;
		outputWumpus = 0;
	}

	outputWumpus = new WinampOpenALOut::Output_Wumpus();

	if(outputWumpus)
	{
		outputWumpus->Initialise(out.hMainWindow);
	}
}

void WAOP_Quit()
{
	if(outputWumpus)
	{
		outputWumpus->Quit();
		delete outputWumpus;
		outputWumpus = NULL;
	}

#ifdef _DEBUG
	if(_CrtDumpMemoryLeaks())
	{
		_ASSERTE( _CrtCheckMemory( ) );
	}
#endif
}

int WAOP_Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	if(outputWumpus)
	{
		return outputWumpus->Open(samplerate, numchannels, bitspersamp, bufferlenms, prebufferms);
	}else{
		return 0;
	}
}

void WAOP_Close()
{
	if(outputWumpus)
	{
		outputWumpus->Close();
	}
}

int WAOP_Write(char *buf, int len)
{
	if(outputWumpus)
	{
		return outputWumpus->Write(buf, len);
	}else{
		return -1;
	}
}
	
int WAOP_CanWrite()
{
	if(outputWumpus)
	{
		return outputWumpus->CanWrite();
	}else{
		return 0;
	}
}

int WAOP_IsPlaying()
{
	if(outputWumpus)
	{
		return outputWumpus->IsPlaying();
	}else{
		return 0;
	}
}

int WAOP_Pause(int pause)
{
	if(outputWumpus)
	{
		return outputWumpus->Pause(pause);
	}else{
		return pause;
	}
}


void WAOP_SetVolume(int volume)
{
	if(outputWumpus)
	{
		outputWumpus->SetVolume(volume);
	}
}

void WAOP_SetPan(int pan)
{
	if(outputWumpus)
	{
		outputWumpus->SetPan(pan);
	}
}

void WAOP_Flush(int t)
{
	if(outputWumpus)
	{
		outputWumpus->Flush(t);
	}
}

int WAOP_GetOutputTime()
{
	if(outputWumpus)
	{
		return outputWumpus->GetOutputTime();
	}else{
		return 0;
	}
}

int WAOP_GetWrittenTime()
{
	if(outputWumpus)
	{
		return outputWumpus->GetWrittenTime();
	}
	return 0;
}

extern "C"
{
	__declspec( dllexport ) Out_Module * winampGetOutModule()
	{
		return &out;
	}
}