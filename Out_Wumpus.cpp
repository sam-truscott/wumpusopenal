#include "Out_Wumpus.h"
#include "Version.h"
#include "Out_Effects.h"
#ifndef NATIVE
#include "ConfigStatusForm.h"
#endif
#include "ConfigFile.h"
#include "Out_Renderer.h"
#include "Winamp.h"

#define DEBUG_BUFFER_SIZE 255

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

#define SYNC_START EnterCriticalSection(&critical_section)
#define SYNC_END LeaveCriticalSection(&critical_section)

namespace WinampOpenALOut {

	Output_Wumpus::Output_Wumpus() {

		// make sure all the pointers are set to zero
		last_output_time = ZERO_TIME;
		last_written_time = ZERO_TIME;
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		current_output_time = ZERO_TIME;
		current_written_time = ZERO_TIME;
		effects = NULL;

		no_renderers = 0;
		for ( char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			renderers[rend] = NULL;
		}

		is_playing = false;
		stream_open = false;
		pre_buffer = false;
		pre_buffer_number = 0;
		xram_detected = false;
		xram_enabled = false;

		sample_rate = 0;
		number_of_channels = 0;
		bits_per_sample = 0;
		no_buffers = 0;
		bytes_per_sample_channel = 0;
		last_pause = 0;
		volume = 0;

		temp_size = 0;
		memset(
			temp,
			0,
			sizeof(temp));

		conf_buffer_length = 0;
		is_mono_expanded = false;
		is_stereo_expanded = false;
		split_out = false;
	}

	Output_Wumpus::~Output_Wumpus()
	{
		//ensure everything in memory is deleted
	}

	/*
		_DllMainCRTStartup

		This is called when the DLL is loaded
	*/
	BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
	{
		if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		{
			// optimisation where threads are no longer attached/detached - I think
			DisableThreadLibraryCalls((HMODULE)hInst);
		}
		return TRUE;
	}

	int Output_Wumpus::get_current_output_time(
		const __int64 the_current_output_time, 
		const unsigned int sample_rate)
	{
		return MulDiv( (int)(the_current_output_time & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sample_rate);
	}

	int Output_Wumpus::get_current_written_time(
		const __int64 the_current_written_time,
		const unsigned int sample_rate)
	{
		return MulDiv( (int)(the_current_written_time & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sample_rate);
	}

	void Output_Wumpus::OnError()
	{
		this->Close();
	}

	void Output_Wumpus::SwitchOutputDevice(const int device)
	{
		SYNC_START;

		SwitchOutputDevice(device, split_out);

		SYNC_END;
	}

	void Output_Wumpus::SwitchOutputDevice(const int device, const bool is_split)
	{
		SYNC_START;

		/* stop the source so we dont hear anthing else */
		this->Relocate(device, GetOutputTime(),is_split);

		SYNC_END;
	}

	void Output_Wumpus::Relocate(
		const int device, 
		const int current_position, 
		const bool is_split) {

		SYNC_START;

		// grab some temporary data for where we are now
		// and the format of the data
		const unsigned int tempSampleRate = sample_rate;
		const unsigned int tempBitsPerSample = bits_per_sample;
		const unsigned int tempNumberOfChannels = original_number_of_channels;

		// shut down the thread and wait for it to shutdown
		const bool tempStreamOpen = stream_open;
		const int tempPause = last_pause;
		const float tempVolume = volume;

		/* stop playing */
		this->Close();
		
		// only switch devices if we have to
		if(device != Framework::getInstance()->GetCurrentDevice())
		{
			// shutdown openal
			Framework::getInstance()->ALFWShutdownOpenAL();
			// re-initialise openal
			Framework::getInstance()->ALFWInitOpenAL(device);
		}

		split_out = is_split;
		
		if(tempStreamOpen) 
		{
			// re-initialise everything
			this->Open(
				tempSampleRate,
				tempNumberOfChannels,
				tempBitsPerSample,
				ZERO_TIME,ZERO_TIME);
						
			// reset back to the current position
			this->SetBufferTime(current_position);

			// reset the volume
			this->SetVolumeInternal(tempVolume);

			// if it was paused, re-pause
			if (tempPause)
			{
				this->Pause(tempPause);
			}
		}
		SYNC_END;
	}

	void Output_Wumpus::CheckProcessedBuffers() 
	{
		/*
		 * ask each of the renderers to check where they are
		 */
		for ( char rend=0 ; rend < no_renderers ; rend++ )
		{
			this->renderers[rend]->CheckProcessedBuffers();
		}
	}

	void Output_Wumpus::CheckPlayState()
	{
		this->is_playing = false;

		HANDLE threads[MAX_RENDERERS];
		int threadCount = 0;
		memset(threads,0,sizeof(HANDLE)*MAX_RENDERERS);

		/*
		 * ask all of the renderers if they're playing
		 */
		for ( char rend=0 ; rend < no_renderers ; rend++ )
		{
			if ( this->renderers[rend] )
			{
				threads[threadCount++] = this->renderers[rend]->CheckPlayState();
				this->is_playing |= this->renderers[rend]->IsPlaying();
			}
		}

		for ( char rend=0 ; rend < threadCount ; rend++ )
		{
			ResumeThread(threads[rend]);
		}
	}

	/*
		config

		this procedure is invoked by winamp when the configure button is clicked
	*/
	void Output_Wumpus::Config(const HWND hwnd)
	{
#ifndef NATIVE
		WinampOpenALOut::Config^ config = WinampOpenALOut::Config::GetInstance(this);
		config->Load();
#endif
	}

	void Output_Wumpus::log_debug_msg(char* msg, char* file, int line)
	{
		/* basic logging to file - only if we're in debug mode */
#ifdef _DEBUG
		FILE *debug_file = NULL;
		fopen_s(&debug_file, "out_openal.log", "a+");
		fprintf_s(debug_file,"%s, %d - %s\n", file, line, msg);
		fclose(debug_file);
#endif
	}

	/*
		about

		this procedure is invoked by winamp when the about button is clicked
	*/
	void Output_Wumpus::About(const HWND hwnd)
	{
		MessageBoxA(hwnd,"Wumpus OpenAL Output Plug-in "
			PI_VER "\nCompiled on " __DATE__ " - " __TIME__
			"\n(c) 2013 Sam Truscott\n"
			"\n"
			"Download latest version and source code (GPL)\n"
			"https://sourceforge.net/projects/winampopenalout/\n"
			"\n"
			"Thanks to:\n"
			"\tTinuva\n"
			"\tsoddit112\n"
			"\tGoujon\n"
			"\n"
			"In memory of Alex Cicciu, 2009\n"
			,"About",MB_OK);
	}

	/*
		init

		this procedure is called when the plugin is initialised
	*/
	void Output_Wumpus::Initialise(const HWND window)
	{
		InitializeCriticalSection(&critical_section);

		SYNC_START;

		effects = new Output_Effects();

		/*
		 * empty the speaker matrix (values of where the speakers are)
		 * by zeroing them all
		 */
		memset(
			&speaker_matrix,
			0,
			sizeof(speaker_matrix_T));

		/*
		 *	set up the variables to a default state
		 */
		stream_open = false;
		is_playing = false;
		split_out = false;

		no_buffers = NO_BUFFERS;
		sample_rate = NO_SAMPLE_RATE;
		bits_per_sample = NO_BITS_PER_SAMPLE;
		number_of_channels = NO_NUMBER_OF_CHANNELS;
		current_output_time = ZERO_TIME;
		current_written_time = ZERO_TIME;
		total_played = ZERO_TIME;
		total_written = ZERO_TIME;
		last_output_time = ZERO_TIME;
		last_written_time = ZERO_TIME;

		// load the config up
		ConfigFile::Initialise(window);

		/*
		 *	initialise the winamp interface to get info
		 */
		Winamp::Initialise(window);

		/*
		 *	get the buffer length from the config file
		 */
		conf_buffer_length = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);
		if(conf_buffer_length == ERROR_BUFFER || 
			conf_buffer_length > CONF_BUFFER_LENGTH_MAX || 
			conf_buffer_length < CONF_BUFFER_LENGTH_MIN)
		{
			conf_buffer_length = DEFC_BUFFER_LENGTH;
			ConfigFile::WriteInteger(CONF_BUFFER_LENGTH, conf_buffer_length);
		}

		// read in the current device to use
		int current_device = ConfigFile::ReadInteger(CONF_DEVICE);
		if(current_device == -1)
		{
			current_device = DEFC_DEVICE;
			ConfigFile::WriteInteger(CONF_DEVICE, current_device);
		}

		/*
		 *	get the last used volume we used to with openal
		 */
		int last_volume = ConfigFile::ReadGlobalInteger(CONF_VOLUME);
		if ( last_volume != -1)
		{
			volume = (last_volume / (ALfloat)VOLUME_DIVISOR);
		}
		else
		{
			volume = 1.0f;
		}

		/*
		 *	get the 3D mode (split out) and effects settings
		 */
		split_out = ConfigFile::ReadBoolean(CONF_SPLIT);

		bool efx_enabled = ConfigFile::ReadBoolean(CONF_EFX_ENABLED);
		effects_list efx_env = REVERB_PRESET_GENERIC;
			
		efx_env = (effects_list)ConfigFile::ReadInteger(CONF_EFX_ENV);

		/*
		 *	range check the setting value
		 */
		if ( efx_env < REVERB_PRESET_GENERIC ||
			 efx_env > REVERB_PRESET_SMALLWATERROOM )
		{
			efx_env = REVERB_PRESET_GENERIC;
		}

		/*
			initialise openal itself - this has been modified
			and will also select the default sound card
		*/
		if (!Framework::getInstance()->ALFWInitOpenAL(current_device))
		{
			MessageBoxA(NULL, "Could not initialise OpenAL", "Error", MB_OK);
		}

		effects->Enable(efx_enabled);
		effects->SetCurrentEffect(efx_env);

#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Using Device {%d} with buffer of {%d}",
			current_device,
			conf_buffer_length);
		this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		this->is_mono_expanded = ConfigFile::ReadBoolean(CONF_MONO_EXPAND);
		this->is_stereo_expanded = ConfigFile::ReadBoolean(CONF_STEREO_EXPAND);
		this->xram_enabled = ConfigFile::ReadBoolean(CONF_XRAM_ENABLED);

#ifdef _DEBUGGING
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Mono expansion {%d}, Stereo expansion {%d}, Use XRAM? {%d}",
			this->is_mono_expanded,
			this->is_stereo_expanded,
			this->xram_enabled);
		this->log_debug_msg(dbg, __FILE__, __LINE__);
		this->log_debug_msg("Looking for XRAM, all values need to be larger than Zero");

		ALboolean xram_ext = alIsExtensionPresent("EAX-RAM");
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"XRAM Extension: {=%d}, SetMode {0x%08X}, GetMode {0x%08X}, Size {%d}, Free {%d}, Auto {%d}, Hardware {%d}, Accessible {%d}",
			xram_ext,
			alGetProcAddress("EAXSetBufferMode"),
			alGetProcAddress("EAXGetBufferMode"),
			alGetEnumValue("AL_EAX_RAM_SIZE"),
			alGetEnumValue("AL_EAX_RAM_FREE"),
			alGetEnumValue("AL_STORAGE_AUTOMATIC"),
			alGetEnumValue("AL_STORAGE_HARDWARE"),
			alGetEnumValue("AL_STORAGE_ACCESSIBLE"));
		this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		/*
		 *	fairly horrible bit of code to read in the speaker
		 * matrix (all the real numbers for where the speakers are)
		 */
		char setting[MATRIX_BUFFER_SIZE];
		memset(setting, '\0', MATRIX_BUFFER_SIZE);
		//pump out the default format for the string
		// the _a_ is the x/y/z axis position
		// the _r is the renderer number
		strcpy_s(setting,MATRIX_BUFFER_SIZE,"matrix_a_r\0");
		for (char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			this->renderers[rend] = NULL;

			LoadSpeakerValues(
				&speaker_matrix.speakers[rend],
				&DEFAULT_MATRIX.speakers[rend],
				setting,
				rend);
		}

		LoadSpeakerValues(
			&speaker_matrix.position,
			&DEFAULT_MATRIX.position,
			setting,
			'P' - '0');

		LoadSpeakerValues(
			&speaker_matrix.direction,
			&DEFAULT_MATRIX.direction,
			setting,
			'D' - '0');

		SYNC_END;

	}

	void Output_Wumpus::LoadSpeakerValues(
		speaker_T * speaker, 
		const speaker_T * default_speaker, 
		const char * setting, 
		const int offset)
	{

		if ( speaker && setting )
		{
			char name[11];
			memset(name, 0, 11);

			// copy the name over for manipulation
			memcpy_s(name, 11, setting, strlen(setting));

			// offset the ascii value
			name[MATRIX_RENDERER_POSITION] = (char)offset + '0';
			bool valid = false;
			float tempf = 0.0;
			name[MATRIX_AXIS_POSITION] = 'x';
			tempf = ConfigFile::ReadFloat(name, &valid);
			if ( valid )
			{
				speaker->x = tempf;
			} else {
				speaker->x = default_speaker->x;
			}
			name[MATRIX_AXIS_POSITION] = 'y';
			tempf = ConfigFile::ReadFloat(name, &valid);
			if ( valid )
			{
				speaker->y = tempf;
			} else {
				speaker->y = default_speaker->y;
			}
			name[MATRIX_AXIS_POSITION] = 'z';
			tempf = ConfigFile::ReadFloat(name, &valid);
			if ( valid )
			{
				speaker->z = tempf;
			} else {
				speaker->z = default_speaker->z;
			}
		}
	}

	void Output_Wumpus::SetMatrix( const speaker_matrix_T m )
	{
		speaker_matrix = m;

		// if it's a mono source or single stereo
		// then leave it in the middle otherwise it'll be
		// wherever the user configured the left speaker to be
		if ( no_renderers > 1 && split_out == true)
		{
			for( char rend=0 ; rend < no_renderers ; rend++ )
			{
				if ( renderers[rend] != NULL )
				{
					renderers[rend]->SetMatrix( m.speakers[rend] );
				}
			}

			alListener3f(
				AL_POSITION,
				speaker_matrix.position.x,
				speaker_matrix.position.y,
				speaker_matrix.position.z);

			alListener3f(
				AL_DIRECTION,
				speaker_matrix.direction.x,
				speaker_matrix.direction.y,
				speaker_matrix.direction.z);
		}

		char setting[MATRIX_BUFFER_SIZE];
		memset(setting, '\0', MATRIX_BUFFER_SIZE);
		strcpy_s(setting,11,"matrix_x_y\0");
		for (char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			SaveSpeakerValues(
				&speaker_matrix.speakers[rend],
				setting,
				rend);
		}

		SaveSpeakerValues(
			&speaker_matrix.position,
			setting,
			'P' - '0');

		SaveSpeakerValues(
			&speaker_matrix.direction,
			setting,
			'D' - '0');
	}

	void Output_Wumpus::SaveSpeakerValues(
			const speaker_T * speaker,
			const char * setting,
			const int offset)
	{
		char name[11];
		memset(name, 0, 11);

		memcpy_s(name, 11, setting, strlen(setting));

		name[MATRIX_RENDERER_POSITION] = (char)offset + '0';

		name[MATRIX_AXIS_POSITION] = 'x';
			ConfigFile::WriteFloat( name, speaker->x );
		name[MATRIX_AXIS_POSITION] = 'y';	
			ConfigFile::WriteFloat( name, speaker->y );
		name[MATRIX_AXIS_POSITION] = 'z';	
			ConfigFile::WriteFloat( name, speaker->z );
	}

	/*
		quit

		this procedure is invoked by winamp when it tells the plugin to quit
		this is usually done when winamp is closed
	*/
	void Output_Wumpus::Quit() {

		SYNC_START;

		// if a steam is open, close it
		if(stream_open)
		{
			this->Close();
			stream_open = false;
		}

		delete effects;
		effects = NULL;

		ConfigFile::WriteInteger(CONF_VOLUME, (int)(volume * VOLUME_DIVISOR) );

		// shutdown openal
		Framework::getInstance()->ALFWShutdownOpenAL();
		Framework::deleteInstance();

		SYNC_END;

		DeleteCriticalSection(&critical_section);
	}

	/*
		open

		this procedure is invoked by winamp when it opens a file
		we store the sample rate, numchannels, bitsPerSample

		the open al buffers are created and an open al source
	*/
	int Output_Wumpus::Open(
		const int samplerate,
		const int numchannels,
		const int bitspersamp, 
		const int bufferlenms,
		const int prebufferms) {
		
		SYNC_START;

		/*
		 * catch the case where someone tried 24-bit or higher
		 */
		if(bitspersamp > SIXTEEN_BIT_PER_SAMPLE)
		{
			MessageBoxA(NULL, "This Plug-In only supports 8 and 16bit audio, please disable 24bit audio in Winamp", "Whoops", MB_OK);
			SYNC_END;
			return -1;
		}
#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Open: Sample rate {%d}, Channels {%d}, Bits {%d}",
			samplerate,
			numchannels,
			bitspersamp);
		log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		//record the format of the data we're getting
		sample_rate = samplerate;
		number_of_channels = numchannels;
		bits_per_sample = bitspersamp;
	
		// reset the play position back to zero
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		last_pause = 0;

		temp_size = 0;
		memset(temp, 0, sizeof(temp));

		// determine the size of the buffer
		bytes_per_sample_channel = ((bits_per_sample >> SHIFT_BITS_TO_BYTES)*number_of_channels);

		/*
		 * just incase we haven't closed properly from last time,
		 * make sure by closing down the renderers
		 */
		for ( char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			if ( renderers[rend] )
			{
				renderers[rend]->Close();
				delete renderers[rend];
				renderers[rend] = NULL;
			}
		}

		bool use_xram = false;

		if ( xram_detected == true && 
				xram_enabled == true &&
				alGetEnumValue("AL_EAX_RAM_FREE") > 0)
		{
			use_xram = true;
		}

		if( effects!= NULL)
		{
			effects->Setup();
		}

		/* stereo and mono expansion 
		 *	we need to store the original number of channels
		 *	incase we need to expand them out and need to work
		 *	out how much "real" data is in the stream
		 */
		original_number_of_channels = this->number_of_channels;
		if ( this->is_stereo_expanded && this->number_of_channels == 2)
		{
			this->number_of_channels += 2;
		}
		else if ( this->is_mono_expanded && this->number_of_channels == 1)
		{
			this->number_of_channels += 3;
		}

		no_renderers = 0;
		if ( split_out == true )
		{
			for ( unsigned char rend=0 ; rend < number_of_channels ; rend++ )
			{
				renderers[rend] = new Output_Renderer(
					conf_buffer_length, 
					rend, 
					effects);
				renderers[rend]->SetXRAMEnabled(use_xram);
				// if we're splitting out, there will always be '1' channel
				// because we'll split multiple channels out to many single renderers
				renderers[rend]->Open(samplerate,1,bitspersamp,0,0);
				no_renderers++;
			}
		}
		else
		{
			/*
			 * otherwise, create one renderer and just use that to represent
			 * the whole stream
			 */
			renderers[0] = new Output_Renderer(
				conf_buffer_length,
				0,
				effects);
			renderers[0]->SetXRAMEnabled(use_xram);
			renderers[0]->Open(samplerate,this->number_of_channels,bitspersamp,0,0);
			no_renderers++;
		}

#ifdef _DEBUGGING
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"-> Using {%d} renderers", 
			no_renderers);
		this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		// allocate some buffers
		alGetError();

		// set the volume for the source
		this->SetVolumeInternal(volume);

		// we're not playing yet because we're prebuffering
		is_playing = false;
		// the stream is open and ready for the main thread
		stream_open = true;
		// start prebuffering
		pre_buffer = true;

		pre_buffer_number = NO_BUFFERS_PROCESSED;

		// reload the speaker positions
		SetMatrix(speaker_matrix);

		SYNC_END;

		return conf_buffer_length;
	}

	/*
		close

		this procedure is invoked by winamp when the file is closed
	*/
	void Output_Wumpus::Close() 
	{
		SYNC_START;
		stream_open = false;

		/*
		 * loop through each renderer we're using and close each one down,
		 * also, delete the renderer and reclaim any memory etc.
		 */
		for ( char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			if ( renderers[rend] )
			{
				renderers[rend]->Close();
				delete renderers[rend];
				renderers[rend] = NULL;
			}
		}

		/*
		 * if we're using effects we can remove any sources
		 * that are attached
		 */
		if ( this->effects )
		{
			effects->OnClose();
		}

		// just incase the thread has exitted, assume playing has stopped
		is_playing = IS_NOT_PLAYING;

		no_buffers = NO_BUFFERS;
		sample_rate = NO_SAMPLE_RATE;
		bits_per_sample = NO_BITS_PER_SAMPLE;
		number_of_channels = NO_NUMBER_OF_CHANNELS;
		current_output_time = ZERO_TIME;
		current_written_time = ZERO_TIME;
		total_played = ZERO_TIME;
		total_written = ZERO_TIME;
		last_output_time = ZERO_TIME;
		last_written_time = ZERO_TIME;
		
		temp_size = 0;
		memset(temp, 0, sizeof(temp));

		SYNC_END;

	}

	/*
		write

		this procedure is invoked by winamp when it attempts to write
		data to the plugin
	*/
	int Output_Wumpus::Write(char *buf, int len)
	{
		SYNC_START;

		// if the buffer is valid (non-NULL)
		if (buf) {

#ifdef _DEBUGGING
			char dbg[DEBUG_BUFFER_SIZE] = {'\0'};

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing data {0x%08X} with length {%d}",buf, len);
			log_debug_msg(dbg, __FILE__, __LINE__);
#endif

			/*
			 * if our internal buffer isn't full, copy the new data
			 * to the internal buffer and quit out
			 */
			if ( len + temp_size < MINIMUM_BUFFER_SIZE ) 
			{
				memcpy_s(
					temp + temp_size, 
					sizeof(temp),
					buf,
					len);

				temp_size = temp_size + len;
#ifdef _DEBUGGING
				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"Temp buffer increased by {%d} to {%d}",len, temp_size);
				log_debug_msg(dbg, __FILE__, __LINE__);
#endif
				SYNC_END;
				return 0;
			}

			char * to_write = NULL;

			if ( temp_size > 0 )
			{
				const unsigned int to_write_size = len + temp_size;
				to_write = new char[to_write_size];

#ifdef _DEBUGGING
				/* this isnt required, all the data *should* be over-written */
				memset(to_write, 0x00, to_write_size);
#endif

				/* copy the first buffer in */
				memcpy_s(
					to_write,
					to_write_size,
					temp,
					temp_size);
#ifdef _DEBUGGING
				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"Copying {%d} bytes from temp", temp_size);
				log_debug_msg(dbg, __FILE__, __LINE__);
#endif
				/* copy what we can of the second */
				void * add_write = to_write + temp_size;
				const size_t add_write_size = to_write_size - temp_size; 
				memcpy_s(
					add_write,
					add_write_size,
					buf,
					len);

				buf = to_write;
				len += temp_size;
				temp_size = 0; 
				memset(temp, 0, sizeof(temp));
			}

			// ############## MONO EXPANSION
			if(is_mono_expanded && original_number_of_channels == 1)
			{
				ExpandMonoToQuad( &buf, &len);
			}
			// ############## STEREO EXPANSION
			if(is_stereo_expanded && original_number_of_channels == 2)
			{
				ExpandStereoToQuad( &buf, &len);	
			}

			total_written += len;

			if ( split_out == true )
			{
				SplitAudioToMonoChannels(buf, len);
			}
			else
			{
				/* if we're not splitting out, throw it to the renderers */
				for ( char rend=0; rend < no_renderers ; rend++ )
				{				
					renderers[rend]->Write(buf,len);
				}
			}

			/* now that there is data in the buffers check the play
			state. if nothing is playing then either a buffer under-run
			has occured or this is the first time the file has been written.
			it could also be a small file that the monitor thread might not see
			*/
			if(!pre_buffer)
			{
				this->CheckPlayState();
			}

			/*
			 * check the pre-buffer limit
			 */
			if(pre_buffer)
			{
				if(++pre_buffer_number == PREBUFFER_LIMIT)
				{
					pre_buffer = false;
				}
			}
		}

		SYNC_END;

		return 0;
	}

	void Output_Wumpus::ExpandMonoToQuad(char ** pbuf, int * plen)
	{
		int len = *plen;
		char * buf = *pbuf;

		// we're writing out four as much data so
		// increase this value by three more times
		const unsigned int new_len = (len*4);

		// expand buffer here
		char* new_buffer = new char[new_len];
#ifdef _DEBUGGING
		memset(new_buffer, 0, new_len);
#endif

		// set relative value to zero
		int nPos = 0;

		const unsigned char sample_size = 
			((bits_per_sample == 8) ? ONE_BYTE_SAMPLE : TWO_BYTE_SAMPLE);
		
		/* expand the samples out */
		for(int pos=0; pos < len; pos++) {

			if ( sample_size == ONE_BYTE_SAMPLE)
			{
				new_buffer[nPos++] = buf[pos];
				new_buffer[nPos++] = buf[pos];
				new_buffer[nPos++] = buf[pos];
				new_buffer[nPos++] = buf[pos];
			}
			else
			{
				const short* src = (short*)(buf + pos);
				short* dst_a = (short*)(new_buffer + nPos);
				short* dst_b = (short*)(new_buffer + nPos + TWO_BYTE_SAMPLE);
				short* dst_c = (short*)(new_buffer + nPos + (TWO_BYTE_SAMPLE * 2));
				short* dst_d = (short*)(new_buffer + nPos + (TWO_BYTE_SAMPLE * 4));

				(*dst_a) = (*src);
				(*dst_b) = (*src);
				(*dst_c) = (*src);
				(*dst_d) = (*src);

				nPos += (sample_size * 4);
				pos += sample_size;
			}
		}

		delete buf;
		buf = new_buffer;
		len = new_len;

		*plen = len;
		*pbuf = buf;
	}

	void Output_Wumpus::ExpandStereoToQuad(char ** pbuf, int * plen)
	{
		int len = *plen;
		char * buf = *pbuf;

		// we're writing out twice as much data so
		// increase this value again
		const unsigned int old_length = len;
		const unsigned int new_len = old_length * 2;

		// expand buffer here
		char* new_buffer = new char[new_len];
		const unsigned char sample_size = 
			((bits_per_sample == 8) ? TWO_BYTE_SAMPLE : FOUR_BYTE_SAMPLE);
		
		/* expand the samples out */
		int new_pos = 0;
		for(unsigned int pos=0; pos < old_length; )
		{
			if ( sample_size == TWO_BYTE_SAMPLE )
			{
				short* dst = (short*)(new_buffer + new_pos);
				const short* src = (const short*)(buf + pos);
				*dst++ = *src;
				*dst = *src;
			}
			else
			{
				int* dst = (int*)(new_buffer + new_pos);
				const int* src = (const int*)(buf + pos);
				*dst++ = *src;
				*dst = *src;
			}

			new_pos += (sample_size * 2);
			pos += sample_size;
		}

		delete buf;
		len = new_len;
		buf = new_buffer;

		*plen = len;
		*pbuf = buf;
	}

	void Output_Wumpus::SplitAudioToMonoChannels(const char * buf, const int len)
	{
		// create a table of pointers to each channels buffer
		char* buffers[MAX_RENDERERS];
		memset(buffers,0, sizeof(char*) * MAX_RENDERERS);
		const unsigned int renderer_size = len / no_renderers;

		if ( bits_per_sample == EIGHT_BIT_PER_SAMPLE )
		{
			/*
			 * create a new buffer for each renderer
			 */
			for ( char rend=0; rend < no_renderers ; rend++ )
			{
				buffers[rend] = new char[renderer_size];
				memset(buffers[rend],0, renderer_size);
			}
			/*
			 * treat each sample of the source as an array - one item for each channel.
			 * treat each sample of the destination as a pointer and add the sample
			 * index to it and copy the source to it.
			 */
			const char* src = (const char*)buf;
			for ( unsigned int sample = 0 ; sample < renderer_size ; sample++ )
			{
				for ( unsigned char rend=0; rend < no_renderers ; rend++ )
				{
					char* dst = (char*)buffers[rend];
					*(char*)(dst + sample) = (src[rend]);
				}
				src += number_of_channels;
			}
		} else {
			/*
			 * create a new buffer for each renderer
			 */
			for ( char rend=0; rend < no_renderers ; rend++ )
			{
				buffers[rend] = new char[renderer_size];
				memset(buffers[rend],0, renderer_size);
			}
			/*
			 * treat each sample of the source as an array - one item for each channel.
			 * treat each sample of the destination as a pointer and add the sample
			 * index to it and copy the source to it.
			 */
			const short* src = (const short*)buf;
			for ( unsigned int sample = 0 ; sample < (renderer_size/2) ; sample++ )
			{

				for ( unsigned char rend=0; rend < no_renderers ; rend++ )
				{
					*(short*)(((short*)buffers[rend]) + sample) = (src[rend]);
				}
				src += number_of_channels;
			}
		}

		/* we write in a sepeate loop to ensure that they're close
		 * together, if they're in the loop above the audio may drift */
		for ( char rend=0; rend < no_renderers ; rend++ )
		{
			renderers[rend]->Write(buffers[rend], renderer_size);
		}

		delete buf;
	}

	/*
		canwrite

		this procedure is invoked by winamp when it determining
		if more data can be written to the plugin
	*/
	int Output_Wumpus::CanWrite()
	{
		SYNC_START;

		int r = EMPTY_THE_BUFFER;
		if(stream_open)
		{
			/*
			 * find out if the first renderer can accept data,
			 * all the renderers should have the same amount of data
			 * free so we only need to ask one
			 */
			if ( no_renderers > 0 && renderers[0] )
			{
				r = renderers[0]->CanWrite();
			}

			/*
			 * don't attach the temp_size as it may cause overflows
			 * as we may tell winamp that we want more than we can handle
			 */
			if ( r >= temp_size)
			{
				r -= temp_size;
			}
		}

		SYNC_END;
		
		return r;
	}

	/*
		isplaying

		invoked by winamp to determine if there is still data in the buffers
		return 0 if empty
		return <0> if data present
	*/
	int Output_Wumpus::IsPlaying()
	{
		SYNC_START;

		if ( stream_open )
		{
			this->CheckProcessedBuffers();

			if(!pre_buffer)
			{
				this->CheckPlayState();
			}
		}

		int r = is_playing && stream_open ? IS_PLAYING : IS_NOT_PLAYING;
		SYNC_END;
		return r;
	}

	/*
		pause

		returns the previous pause state
	*/
	int Output_Wumpus::Pause(const int pause)
	{
		SYNC_START;
		last_pause = pause;
		
		// clear the error state
		alGetError();

		// if this is un-paused during pre-buffering
		// we may start to play data before it's ready
		// and cause and under-run
		
		if ( !this->pre_buffer || last_pause )
		{
			for ( char rend=0 ; rend < no_renderers ; rend++ )
			{
				if ( renderers[rend] )
				{
					renderers[rend]->Pause(pause);
				}
			}
		}

		SYNC_END;
		return last_pause;
	}

	/*
		setvolume

		this procedure is invoked by winamp to set the volume
	*/
	void Output_Wumpus::SetVolume(const int new_volume)
	{
		/*
			work out the % volume (as a float) and set it
		*/
		SYNC_START;

		if(stream_open)
		{
			// calculate the volume to use (0.0 to 1.0)
			ALfloat new_int_volume = (ALfloat)new_volume / (ALfloat)VOLUME_DIVISOR;
			this->SetVolumeInternal(new_int_volume);
		}
		
		SYNC_END;
	}

	/*
		set volume (internal) - used to check the volume range
		and store the volume for later use
	*/
	void Output_Wumpus::SetVolumeInternal(const ALfloat new_volume)
	{
		if(new_volume <= VOLUME_MAX && new_volume >= VOLUME_MIN)
		{
			volume = new_volume;
			
			for ( char rend=0 ; rend < no_renderers ; rend++ )
			{
				if ( renderers[rend] )
				{
					renderers[rend]->SetVolumeInternal(volume);
				}
			}

			ConfigFile::WriteInteger(CONF_VOLUME, (int)(volume * VOLUME_DIVISOR) );
		}
	}

	/*
		setpan

		this procedure is invoked by winamp to set the pan
	*/
	void Output_Wumpus::SetPan(const int new_pan)
	{
		ALfloat f = ((ALfloat)new_pan)/255.0f; 
		this->speaker_matrix.position.x = -f;
		//alListener3f(AL_POSITION, -f ,0.0,0.0);
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Wumpus::Flush(const int new_time_in_ms)
	{
		SYNC_START;

		for ( char rend=0 ; rend < no_renderers ; rend++ )
		{
			if ( renderers[rend] )
			{
				renderers[rend]->Flush();
			}
		}

#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Jump to %d, max length is %d\n", new_time_in_ms, Winamp::GetTrackLength());
		log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		bool closing = false;

		/*
		 * this checks that we can only flush (skip) when the steam is open
		 * but also make sure that we're skipping within the track length.
		 * if this isn't done, winamp will continue to skip past the end of
		 * the file.
		 */
		if ( stream_open && (new_time_in_ms < (Winamp::GetTrackLength()-conf_buffer_length)) )
		{
			// calculate the number of bytes that will have been
			// this will relocate to the current device at a set time
			this->Relocate(Framework::getInstance()->GetCurrentDevice(), new_time_in_ms, split_out);

			CheckPlayState();

			if ( last_pause )
			{
				for ( char rend=0 ; rend < no_renderers ; rend++ )
				{
					if ( renderers[rend] )
					{
						renderers[rend]->Pause(last_pause);
					}
				}
			}

		}else{
			this->Close();
			closing = true;
		}

		SYNC_END;

		if ( closing )
		{
			Winamp::Next();
		}
	}

	int Output_Wumpus::SetBufferTime(const int new_ms)
	{
		// calculate the number of bytes that will have been
		// processed after (t)ms
		int calcTime;

		SYNC_START;
		
		calcTime = 
			((((sample_rate / ONE_SECOND_IN_MS) *
			(bits_per_sample >> SHIFT_BITS_TO_BYTES)) *
			number_of_channels) * new_ms);

#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Calc to %dms, calc'd to be %d bytes", new_ms, calcTime);
		log_debug_msg(dbg, __FILE__, __LINE__);
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Using sample rate %d, bps %d channels %d", sample_rate, bits_per_sample, number_of_channels);
		log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		const unsigned __int64 new_time = calcTime / no_renderers;

		for ( char rend = 0 ; rend < no_renderers ; rend++ )
		{
			if ( renderers[rend] )
			{
				renderers[rend]->SetPlayedTime(new_time);
			}
		}

		// reset played pointers
		total_written = calcTime;
		total_played = calcTime;
		last_output_time = new_ms;
		last_written_time = new_ms;
		current_output_time = new_ms;
		current_written_time = new_ms;

		SYNC_END;

		return calcTime;
	}

	/*
		getWrittenTime

		returns the time in ms of current position
	*/
	int Output_Wumpus::GetWrittenTime()
	{	
		if(stream_open)
		{
			current_written_time = total_written;
			current_written_time = get_current_written_time(current_written_time, sample_rate) / bytes_per_sample_channel;

			/*
			 * we need to return the correct information. if we've expanded the data
			 * we need to only return half those values
			 */
			if ( is_stereo_expanded && original_number_of_channels == 2)
			{
				current_written_time /= 2;
			}

			if ( is_mono_expanded && original_number_of_channels == 1)
			{
				current_written_time /= 4;
			}

		}else{
			current_written_time = ZERO_TIME;
		}
		
		// make sure we only use the first 32bits of the 64bit value
		last_written_time = (int)(current_written_time & THIRTY_TWO_BIT_BIT_MASK);
		return last_written_time;
	}

	/*
		getOutputTime

		returns the time in ms of current position
	*/
	int Output_Wumpus::GetOutputTime()
	{
		if(stream_open)
		{
			__int64 new_total_played = 0;
			for( char rend=0; rend < no_renderers ; rend++ )
			{
				if ( renderers[rend] )
				{
					new_total_played += renderers[rend]->GetPlayedTime();
				}
			}

			total_played = new_total_played;

			// this works it out how many bytes it is
			current_output_time = total_played;
			// this converts bytes to ms and does 32bit part
			current_output_time =
				get_current_output_time(current_output_time, sample_rate)
				/ bytes_per_sample_channel;

			if ( is_stereo_expanded && original_number_of_channels == 2)
			{
				current_output_time /= 2;
			}

			if ( is_mono_expanded && original_number_of_channels == 1)
			{
				current_output_time /= 4;
			}

		}else{
			current_output_time = ZERO_TIME;
		}

		last_output_time = (int)(current_output_time & THIRTY_TWO_BIT_BIT_MASK);
		return last_output_time;
	}

	void Output_Wumpus::SetMonoExpanded(const bool expanded)
	{
		is_mono_expanded = expanded;
		ConfigFile::WriteBoolean(CONF_MONO_EXPAND,is_mono_expanded);
	}

	void Output_Wumpus::SetStereoExpanded(const bool expanded)
	{ 
		is_stereo_expanded = expanded;
		ConfigFile::WriteBoolean(CONF_STEREO_EXPAND,is_stereo_expanded);
	}

	void Output_Wumpus::SetSplit ( const bool split )
	{
		ConfigFile::WriteBoolean(CONF_SPLIT,split);
		SwitchOutputDevice(Framework::getInstance()->GetCurrentDevice(),split);
	}

	void Output_Wumpus::SetXRAMEnabled( const bool enabled )
	{
		xram_enabled = enabled;
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED,xram_enabled);
		SwitchOutputDevice(Framework::getInstance()->GetCurrentDevice(),split_out);
	}

	Output_Effects* Output_Wumpus::GetEffects()
	{
		return this->effects;
	}

}