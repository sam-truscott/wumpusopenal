#ifndef OUT_RENDERER_H
#define OUT_RENDERER_H

#include "al.h"
#include "Constants.h"
#include "Framework\Framework.h"
#include "Clock.h"

namespace WinampOpenALOut
{
	public class Output_Renderer
	{
		typedef struct
		{
			ALuint buffer_id;
			bool available;
			unsigned int size;
			void* data;
		} buffer_type;

	public:
		Output_Renderer(
			unsigned int buffer_len,
			unsigned char a_channel,
			class Output_Effects* the_effects);
		~Output_Renderer();
		int	Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
		void Close();
		int Write(char *buf, int len);
		int CanWrite();
		int IsPlaying();
		int Pause(int pause);
		void SetPan(int newPan);
		void Flush(int newTimeMs);

		void SwitchOutputDevice(int device);	
		inline bool IsStreamOpen()						{ return streamOpen; }
		void SetXRAMEnabled( bool enabled );

		inline unsigned int GetBufferFree(void)
		{
			return buffer_free;
		}

		void CheckProcessedBuffers();
		void CheckPlayState();
		void SetVolumeInternal(ALfloat newVolume);
		
		inline unsigned long long GetPlayedTime()
		{
			return played;
		}

		inline void SetPlayedTime(unsigned long long t)
		{
			played = t;
		}

		inline ALint GetPosition()
		{
			ALint deltaBytes;
			alGetSourcei(uiSource, AL_BYTE_OFFSET, &deltaBytes);
			return deltaBytes;
		}

		void SetMatrix ( speaker_T speaker );

	protected:

		inline void onError();

			// semaphore for the right access to buffers/open_al api
		CRITICAL_SECTION criticalSection;

		unsigned int	c_bufferLength;

		bool			xram_enabled;

		// boolean to store internal playing state
		bool			isPlaying;
		// boolean to store if the file steam is open and
		// thread is running
		bool			streamOpen;

		unsigned int	buffer_free;
		unsigned int	buffers_free;

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

		// the open al buffers themselves
		buffer_type	    uiBuffers[MAX_NO_BUFFERS];
		// integer used to reference the open al source
		ALuint		    uiSource;

		unsigned int	bufferSize;

		Time_Type		lastCheckBuffers;
		Time_Type		lastCheckDelay;

		class Output_Effects	*effects;

	private:

		unsigned char channel;
		unsigned long long played;

		void log_debug_msg(char* msg, char* file = __FILE__, int line = __LINE__);
		inline void fmemcpy(char* dest, int destPos, char* src, int srcPos, int size);
	};

}

#endif