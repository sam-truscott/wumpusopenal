#ifndef OUT_RENDERER_H
#define OUT_RENDERER_H

#include "al.h"
#include "Constants.h"
#include "Framework\Framework.h"

namespace WinampOpenALOut
{
#ifndef NATIVE
	public class Output_Renderer
#else
	class Output_Renderer
#endif
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

		int	Open(
			const int samplerate, 
			const int numchannels, 
			const int bitspersamp, 
			const int bufferlenms, 
			const int prebufferms);
		void Close();
		void Write(
			const char *buf,
			const int len);
		int CanWrite();
		bool IsPlaying();
		int Pause(const int pause);
		void Flush();

		inline bool IsStreamOpen()						{ return stream_open; }
		void SetXRAMEnabled( const bool enabled );

		inline unsigned int GetBufferFree(void)
		{
			return buffer_size_free;
		}

		void CheckProcessedBuffers();
		inline void Play();
		HANDLE CheckPlayState();
		void SetVolumeInternal(const ALfloat new_volume);
		
		inline unsigned long long GetPlayedTime()
		{
			ALint delta_bytes = 0;
			alGetSourcei(source, AL_BYTE_OFFSET, &delta_bytes);
			return played + delta_bytes;
		}

		inline void SetPlayedTime(const unsigned long long t)
		{
			played = t;
		}

		void SetMatrix ( const speaker_T speaker );

	protected:

		inline void onError();

		unsigned int	conf_buffer_length;

		bool			xram_enabled;

		// boolean to store internal playing state
		bool			is_playing;
		// boolean to store if the file steam is open and
		// thread is running
		bool			stream_open;

		unsigned int	buffer_size_free;
		unsigned int	number_buffers_free;

		// integer to store the sample rate
		unsigned int	sample_rate;
		// integer to store the number of channels
		unsigned int	number_of_channels;
		// integer to store the bits per second
		unsigned int	bits_per_sample;
		// integer to store the number of buffers we'll use
		unsigned int	number_of_buffers;
		// integer to store bytes per sample (optimisation
		unsigned int	bytes_per_sample_channel;
		// integer to store the last pause state
		int				last_pause;

		// floating point value to store the volume
		ALfloat			volume;

		// store the format
		unsigned long	format;

		// the open al buffers themselves
		buffer_type	    buffers[MAX_NO_BUFFERS];
		ALuint			next_buffer_index;
		// integer used to reference the open al source
		ALuint		    source;

		unsigned int	calculated_buffer_size;

		class Output_Effects	*effects;

	private:

		unsigned char channel;
		unsigned long long played;

		void log_debug_msg(char* msg, char* file = __FILE__, int line = __LINE__);
	};

}

#endif