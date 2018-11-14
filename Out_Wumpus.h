#ifndef OUT_WUMPUS_H
#define OUT_WUMPUS_H

#include "Constants.h"
#include "Framework\Framework.h"

namespace WinampOpenALOut
{
#ifndef NATIVE
	public class Output_Wumpus
#else
	class Output_Wumpus
#endif
	{

	public:
		Output_Wumpus();
		~Output_Wumpus();
		void Config(const HWND hwnd);
		void About(const HWND hwnd);
		void Initialise(const HWND window);
		void Quit();
		int	Open(
			const int samplerate, 
			const int numchannels, 
			const int bitspersamp, 
			const int bufferlenms, 
			const int prebufferms);
		void Close();
		int Write(char *buf, int len);
		int CanWrite();
		int IsPlaying();
		int Pause(const int pause);
		void SetVolume(const int new_volume);
		void SetPan(const int new_pan);
		void Flush(const int new_time_in_ms);
		int inline	GetOutputTime();
		int GetWrittenTime();

		void SwitchOutputDevice(const int device);
		
		inline bool IsStreamOpen()						{ return stream_open; }
		inline unsigned int	GetSampleRate()				{ return sample_rate; }
		inline unsigned int GetBitsPerSample()			{ return bits_per_sample; }
		inline unsigned int GetNumberOfChannels()		{ return number_of_channels; }
		inline int GetLastOutputTime()					{ return last_output_time; }
		inline int GetLastWrittenTime()					{ return last_written_time; }
		inline __int64 GetWrittenBytes()				{ return total_written; }
		inline __int64 GetPlayedBytes()					{ return total_played; }

		inline int GetConfBufferLength()				{ return conf_buffer_length;}
		inline void SetConfBufferLength( const unsigned int i) 
			{ conf_buffer_length = i;}

		inline bool IsStereoExpanded() { return is_stereo_expanded;}
		void SetStereoExpanded(const bool expanded);

		inline bool IsMonoExpanded() { return is_mono_expanded;}
		void SetMonoExpanded(const bool expanded);

		inline bool IsXRAMEnabled() { return xram_enabled; }
		void SetXRAMEnabled( const bool enabled );

		inline bool IsSplit() { return split_out; }
		void SetSplit ( const bool split );

		inline bool	IsXRAMPresent() { return xram_detected; }

	protected:

		inline void OnError();

		void SwitchOutputDevice(const int device, const bool is_split);

		void Relocate(
			const int device, 
			const int current_position, 
			const bool is_split);

		void CheckProcessedBuffers();
		void CheckPlayState();

		void ExpandMonoToQuad(char ** pbuf, int * plen);
		void ExpandStereoToQuad(char ** pbuf, int * plen);
		void SplitAudioToMonoChannels(const char * buf, const int len);

		int SetBufferTime(const int new_ms);

		void SetVolumeInternal(const ALfloat new_volume);

			// semaphore for the right access to buffers/open_al api
		CRITICAL_SECTION critical_section;

		int inline get_current_output_time(
			const __int64 the_current_output_time, 
			const unsigned int sample_rate);
		int inline get_current_written_time(
			const __int64 the_current_written_time, 
			const unsigned int sample_rate);

		// boolean to store internal playing state
		bool			is_playing;
		// boolean to store if the file steam is open and
		// thread is running
		bool			stream_open;
		// boolean for prebuffering state at the start to get as much data
		// as possible
		bool			pre_buffer;
		unsigned int	pre_buffer_number;

		bool			xram_detected;
		bool			xram_enabled;

		// integer to store the sample rate
		unsigned int	sample_rate;
		// integer to store the number of channels
		unsigned int	number_of_channels;
		unsigned int	original_number_of_channels;
		// integer to store the bits per second
		unsigned int	bits_per_sample;
		// integer to store the number of buffers we'll use
		unsigned int	no_buffers;
		// integer to store bytes per sample (optimisation
		unsigned int	bytes_per_sample_channel;
		// integer to store the last pause state
		int				last_pause;

		// floating point value to store the volume
		ALfloat			volume;

		// store the format
		unsigned long	format;

		// long int to store time written
		__int64		total_written;
		// long int to store time played
		__int64		total_played;
		// integer to store the where we are right now
		__int64		current_output_time;
		// integer to store the where we've buffered to
		__int64		current_written_time;

		int			last_output_time;
		int			last_written_time;
		char		temp[TEMP_BUFFER_SIZE];
		int			temp_size;

		// used to store the configuration buffer length
		int	conf_buffer_length;

		bool			is_stereo_expanded;
		bool			is_mono_expanded;

		class Output_Renderer* renderers[MAX_RENDERERS];
		char			no_renderers;

		bool split_out;

	private:

		void log_debug_msg(char* msg, char* file = __FILE__, int line = __LINE__);
	};

}

#endif
