#pragma once

#include "Constants.h"
#include "Framework\Framework.h"
#include "Clock.h"

#ifndef SSE_BUILD
namespace WinampOpenALOut {
	public class Output_Wumpus {
#else
	class Output_Wumpus {
#endif
	public:
		Output_Wumpus();
		~Output_Wumpus();
		void Config(HWND hwnd);
		void About(HWND hwnd);
		void Initialise(HWND window);
		void Quit();
		int	Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
		void Close();
		int Write(char *buf, int len);
		int CanWrite();
		int IsPlaying();
		int Pause(int pause);
		void SetVolume(int newVolume);
		void SetPan(int newPan);
		void Flush(int newTimeMs);
		int inline	GetOutputTime();
		int GetWrittenTime();

		void SwitchOutputDevice(int device);
		void Relocate(int device, int currentPosition);

		inline bool IsStreamOpen()						{ return streamOpen; }

		unsigned int GetPlayingBuffer();

		inline unsigned int	GetSampleRate()				{ return sampleRate; }
		inline unsigned int GetBitsPerSample()			{ return bitsPerSample; }
		inline unsigned int GetNumberOfChannels()		{ return numberOfChannels; }
		inline int GetLastOutputTime()					{ return lastOutputTime; }
		inline int GetLastWrittenTime()					{ return lastWrittenTime; }

		inline unsigned int GetNumberOfBuffers()		{ return noBuffers; }
		inline unsigned int GetBufferSize(int buffer)	{ return bufferSizes[buffer];}

		inline bool GetEffectsEnabled();
		inline bool GetEffectsSupported();

		inline void	SetEffectsEnabled(bool b);
		inline void	SetEffectsSupported(bool b);

		inline int GetConfBufferLength() { return c_bufferLength;}
		inline void SetConfBufferLength( unsigned int i) { c_bufferLength = i;}

		inline bool IsStereoExpanded() { return stereoExpand;}
		inline void SetStereoExpanded(bool expanded) { stereoExpand = expanded;}

		inline bool IsMonoExpanded() { return monoExpand;}
		inline void SetMonoExpanded(bool expanded) {monoExpand = expanded;}

		inline ALuint GetSource() { return uiSource; }

	protected:

		inline void onError();

		void CheckProcessedBuffers();
		void CheckPlayState();
		void inline CheckAvailableBuffers();
		int SetBufferTime(int tMs);

#ifndef SSE_BUILD
		class EffectsModule* effectsModule;
#endif

			// semaphore for the right access to buffers/open_al api
		CRITICAL_SECTION criticalSection;

		int inline get_current_output_time(__int64 currentOutputTime, unsigned int sampleRate);
		int inline get_current_written_time(__int64 currentOutputTime, unsigned int sampleRate);

		// boolean to store if winamp is allowed to write more data
		bool			canWrite;
		// boolean to store internal playing state
		bool			isPlaying;
		// boolean to store if the file steam is open and
		// thread is running
		bool			streamOpen;
		// boolean for prebuffering state at the start to get as much data
		// as possible
		bool			preBuffer;

		char*			tmpBuffer;
		unsigned int	tmpBufferSize;

		 // boolean to store if effects are supported
		bool			effectsSupported;
		bool			effectsEnabled;

		// integer to store the sample rate
		unsigned int	sampleRate;
		// integer to store the number of channels
		unsigned int	numberOfChannels;
		// integer to store the bits per second
		unsigned int	bitsPerSample;
		// integer to store the number of buffers we'll use
		unsigned int	noBuffers;
		// integer to store bytes per sample (optimisation
		unsigned int	bytesPerSampleChannel;
		// integer to store the last pause state
		int				lastPause;

		// floating point value to store the volume
		ALfloat			volume;

		// store the format
		unsigned long	ulFormat;

		// long int to store time written
		__int64		total_written;
		// long int to store time played
		__int64		total_played;
		// integer to store the where we are right now
		__int64		currentOutputTime;
		// integer to store the where we've buffered to
		__int64		currentWrittenTime;

		int			lastOutputTime;
		int			lastWrittenTime;

		// the open al buffers themselves
		ALuint		    uiBuffers[MAX_NO_BUFFERS];
		// boolean array used to store whether buffers are usable/available
		bool			avalBuffers[MAX_NO_BUFFERS];
		// integer array to store the sizes of the buffers from winamp
		// this is used so we only update the time written when its done
		unsigned int	bufferSizes[MAX_NO_BUFFERS];
		// integer used to reference the open al source
		ALuint		    uiSource;

		Time_Type		lastCheckBuffers;
		Time_Type		lastCheckDelay;

		// used to store the configuration buffer length
		unsigned int	c_bufferLength;

		bool			stereoExpand;
		bool			monoExpand;

		// used for 3D Position and Panning
		//ALfloat posX;
		//ALfloat posY;
		//ALfloat posZ;
		ALfloat listenerPos[3];
		ALfloat listenerVel[3];
		ALfloat	listenerOri[6];

#ifdef _DEBUG
		FILE *file;
#endif

	private:
		inline void fmemcpy(char* dest, int destPos, char* src, int size);
		inline void fmemcpy(char* dest, int destPos, char* src, int srcPos, int size);
	};
#ifndef SSE_BUILD
}
#endif