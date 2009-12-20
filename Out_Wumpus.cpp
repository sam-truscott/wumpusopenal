#include "Out_Wumpus.h"
#include "Version.h"
#include "Out_Effects.h"
#include "ConfigStatusForm.h"
#include "Out_Renderer.h"
#include "Winamp.h"

#define DEBUG_BUFFER_SIZE 255

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

#define SYNC_START EnterCriticalSection(&criticalSection)
#define SYNC_END LeaveCriticalSection(&criticalSection)

namespace WinampOpenALOut {

	Output_Wumpus::Output_Wumpus() {

		// make sure all the pointers are set to zero
		lastOutputTime = ZERO_TIME;
		lastWrittenTime = ZERO_TIME;
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		currentOutputTime = ZERO_TIME;
		currentWrittenTime = ZERO_TIME;
		effects = NULL;

		no_renderers = 0;
		for ( char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			renderers[rend] = NULL;
		}

		isPlaying = false;
		streamOpen = false;
		preBuffer = false;
		preBufferNumber = 0;
		xram_detected = false;
		xram_enabled = false;

		sampleRate = 0;
		numberOfChannels = 0;
		bitsPerSample = 0;
		noBuffers = 0;
		bytesPerSampleChannel = 0;
		lastPause = 0;
		volume = 0;

		temp_size = 0;
		memset(
			temp,
			0,
			sizeof(temp));

		c_bufferLength = 0;
		monoExpand = false;
		stereoExpand = false;
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

	int Output_Wumpus::get_current_output_time(__int64 currentOutputTime, unsigned int sampleRate)//this ignores high 32bits of total_written
	{
		return MulDiv( (int)(currentOutputTime & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sampleRate);
	}

	int Output_Wumpus::get_current_written_time(__int64 currentWrittenTime, unsigned int sampleRate)//this ignores high 32bits of total_written
	{
		return MulDiv( (int)(currentWrittenTime & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sampleRate);
	}

	void Output_Wumpus::fmemcpy(char* dest, int destPos, char* src, int srcPos, int size) {
		
		// get a pointer to the memory data with an offset
		void* ptrDst = (void*)&dest[destPos];
		void* ptrSrc = (void*)&src[srcPos];

		// now copy from this position onwards
		memcpy_s(ptrDst, MAXIMUM_BUFFER_SIZE, ptrSrc, size);
	}

	void Output_Wumpus::onError()
	{
		this->Close();
	}

	void Output_Wumpus::SwitchOutputDevice(int device)
	{
		SYNC_START;

		SwitchOutputDevice(device, split_out);

		SYNC_END;
	}

	void Output_Wumpus::SwitchOutputDevice(int device, bool isSplit)
	{
		SYNC_START;

		/* stop the source so we dont hear anthing else */
		this->Relocate(device, GetOutputTime(),isSplit);

		SYNC_END;
	}

	void Output_Wumpus::Relocate(int device, int currentPosition, bool isSplit) {

		SYNC_START;

		// grab some temporary data for where we are now
		// and the format of the data
		const unsigned int tempSampleRate = sampleRate;
		const unsigned int tempBitsPerSample = bitsPerSample;
		const unsigned int tempNumberOfChannels = originalNumberOfChannels;

		// shut down the thread and wait for it to shutdown
		const bool tempStreamOpen = streamOpen;
		const int tempPause = lastPause;
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

		split_out = isSplit;
		
		if(tempStreamOpen) 
		{
			// re-initialise everything
			this->Open(
				tempSampleRate,
				tempNumberOfChannels,
				tempBitsPerSample,
				ZERO_TIME,ZERO_TIME);
						
			// reset back to the current position
			this->SetBufferTime(currentPosition);

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
		this->isPlaying = false;

		/*
		 * ask all of the renderers if they're playing
		 */
		for ( char rend=0 ; rend < no_renderers ; rend++ )
		{
			if ( this->renderers[rend] )
			{
				this->renderers[rend]->CheckPlayState();
				this->isPlaying |= this->renderers[rend]->IsPlaying();
			}
		}
	}

	/*
		config

		this procedure is invoked by winamp when the configure button is clicked
	*/
	void Output_Wumpus::Config(HWND hwnd) {

		WinampOpenALOut::Config^ config = WinampOpenALOut::Config::GetInstance(this);
		config->Load();
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
	void Output_Wumpus::About(HWND hwnd)
	{
		MessageBoxA(hwnd,"Wumpus OpenAL Output Plug-in "
			PI_VER "\nCompiled on " __DATE__ " - " __TIME__
			"\n(c) 2009 Sam Truscott\n"
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
	void Output_Wumpus::Initialise(HWND window)
	{
		InitializeCriticalSection(&criticalSection);

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
		streamOpen = false;
		isPlaying = false;
		split_out = false;

		noBuffers = NO_BUFFERS;
		sampleRate = NO_SAMPLE_RATE;
		bitsPerSample = NO_BITS_PER_SAMPLE;
		numberOfChannels = NO_NUMBER_OF_CHANNELS;
		currentOutputTime = ZERO_TIME;
		currentWrittenTime = ZERO_TIME;
		total_played = ZERO_TIME;
		total_written = ZERO_TIME;
		lastOutputTime = ZERO_TIME;
		lastWrittenTime = ZERO_TIME;

		// load the config up
		ConfigFile::Initialise(window);

		/*
		 *	initialise the winamp interface to get info
		 */
		Winamp::Initialise(window);

		/*
		 *	get the buffer length from the config file
		 */
		c_bufferLength = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);
		if(c_bufferLength == ERROR_BUFFER || 
			c_bufferLength > CONF_BUFFER_LENGTH_MAX || 
			c_bufferLength < CONF_BUFFER_LENGTH_MIN)
		{
			c_bufferLength = DEFC_BUFFER_LENGTH;
			ConfigFile::WriteInteger(CONF_BUFFER_LENGTH, c_bufferLength);
		}

		// read in the current device to use
		int currentDevice = ConfigFile::ReadInteger(CONF_DEVICE);
		if(currentDevice == -1)
		{
			currentDevice = DEFC_DEVICE;
			ConfigFile::WriteInteger(CONF_DEVICE, currentDevice);
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
		effects_list efx_env = EFX_REVERB_PRESET_GENERIC;
			
		efx_env = (effects_list)ConfigFile::ReadInteger(CONF_EFX_ENV);

		/*
		 *	range check the setting value
		 */
		if ( efx_env < EFX_REVERB_PRESET_GENERIC ||
			 efx_env > EFX_REVERB_PRESET_SMALLWATERROOM )
		{
			efx_env = EFX_REVERB_PRESET_GENERIC;
		}

		effects->set_enabled(efx_enabled);
		effects->set_current_effect(efx_env);

		/*
			initialise openal itself - this has been modified
			and will also select the default sound card
		*/
		if (!Framework::getInstance()->ALFWInitOpenAL(currentDevice))
		{
			MessageBoxA(NULL, "Could not initialise OpenAL", "Error", MB_OK);
		}

#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Using Device {%d} with buffer of {%d}",
			currentDevice,
			c_bufferLength);
		this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		this->monoExpand = ConfigFile::ReadBoolean(CONF_MONO_EXPAND);
		this->stereoExpand = ConfigFile::ReadBoolean(CONF_STEREO_EXPAND);
		this->xram_enabled = ConfigFile::ReadBoolean(CONF_XRAM_ENABLED);

#ifdef _DEBUGGING
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Mono expansion {%d}, Stereo expansion {%d}, Use XRAM? {%d}",
			this->monoExpand,
			this->stereoExpand,
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

		if ( Framework::getInstance()->ALFWIsXRAMSupported() == AL_TRUE )
		{
#ifdef _DEBUGGING
			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"-> Detect XRAM, Size {%d}MB, Free {%d}MB",
				alGetInteger(eXRAMSize) / (1024 * 1024),
				alGetInteger(eXRAMFree) / (1024 * 1024) );
			this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif
			this->xram_detected = true;
		}

		/*
		 *	fairly horrible bit of code to read in the speaker
		 * matrix (all the real numbers for where the speakers are)
		 */
		char setting[MATRIX_BUFFER_SIZE];
		memset(setting, '\0', MATRIX_BUFFER_SIZE);
		//pump out the default format for the string
		// the _x_ is the x/y/z axis position
		// the _y is the renderer number
		strcpy_s(setting,MATRIX_BUFFER_SIZE,"matrix_x_y\0");
		for (char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			this->renderers[rend] = NULL;

			// offset the ascii value
			setting[MATRIX_RENDERER_POSITION] = rend + '0';
			bool valid = false;
			float tempf;
			setting[MATRIX_AXIS_POSITION] = 'x';
			tempf = ConfigFile::ReadFloat(setting, &valid);
			if ( valid )
			{
				speaker_matrix.speakers[rend].x = tempf;
			} else {
				speaker_matrix.speakers[rend].x = DEFAULT_MATRIX[rend][0];
			}
			setting[MATRIX_AXIS_POSITION] = 'y';
			tempf = ConfigFile::ReadFloat(setting, &valid);
			if ( valid )
			{
				speaker_matrix.speakers[rend].y = tempf;
			} else {
				speaker_matrix.speakers[rend].y = DEFAULT_MATRIX[rend][1];
			}
			setting[MATRIX_AXIS_POSITION] = 'z';
			tempf = ConfigFile::ReadFloat(setting, &valid);
			if ( valid )
			{
				speaker_matrix.speakers[rend].z = tempf;
			} else {
				speaker_matrix.speakers[rend].z = DEFAULT_MATRIX[rend][2];
			}
		}

		SYNC_END;

	}

	void Output_Wumpus::SetMatrix( speaker_matrix_T m )
	{
		speaker_matrix = m;

		// if it's a mono source or single stereo
		// then leave it in the middle otherwise it'll be
		// wherever the user configured the left speaker to be
		if ( no_renderers > 1 )
		{
			for( char i=0 ; i < no_renderers ; i++ )
			{
				if ( renderers[i] != NULL )
				{
					renderers[i]->SetMatrix( m.speakers[i] );
				}
			}
		}

		char setting[MATRIX_BUFFER_SIZE];
		memset(setting, '\0', MATRIX_BUFFER_SIZE);
		strcpy_s(setting,11,"matrix_x_y\0");
		for (char rend=0 ; rend < MAX_RENDERERS ; rend++ )
		{
			setting[MATRIX_RENDERER_POSITION] = rend + '0';

			setting[MATRIX_AXIS_POSITION] = 'x';
				ConfigFile::WriteFloat( setting, speaker_matrix.speakers[rend].x );
			setting[MATRIX_AXIS_POSITION] = 'y';	
				ConfigFile::WriteFloat( setting, speaker_matrix.speakers[rend].y );
			setting[MATRIX_AXIS_POSITION] = 'z';	
				ConfigFile::WriteFloat( setting, speaker_matrix.speakers[rend].z );
		}
	}

	/*
		quit

		this procedure is invoked by winamp when it tells the plugin to quit
		this is usually done when winamp is closed
	*/
	void Output_Wumpus::Quit() {

		SYNC_START;

		// if a steam is open, close it
		if(streamOpen)
		{
			this->Close();
			streamOpen = false;
		}

		delete effects;
		effects = NULL;

		ConfigFile::WriteInteger(CONF_VOLUME, (int)(volume * VOLUME_DIVISOR) );

		// shutdown openal
		Framework::getInstance()->ALFWShutdownOpenAL();
		Framework::deleteInstance();

		SYNC_END;

		DeleteCriticalSection(&criticalSection);
	}

	/*
		open

		this procedure is invoked by winamp when it opens a file
		we store the sample rate, numchannels, bitsPerSample

		the open al buffers are created and an open al source
	*/
	int Output_Wumpus::Open(
		int samplerate,
		int numchannels,
		int bitspersamp, 
		int bufferlenms,
		int prebufferms) {
		
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
		sampleRate = samplerate;
		numberOfChannels = numchannels;
		bitsPerSample = bitspersamp;
	
		// reset the play position back to zero
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		lastPause = 0;

		temp_size = 0;
		memset(temp, 0, sizeof(temp));

		// determine the size of the buffer
		bytesPerSampleChannel = ((bitsPerSample >> SHIFT_BITS_TO_BYTES)*numberOfChannels);

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
			effects->setup();
		}

		/* stereo and mono expansion 
		 *	we need to store the original number of channels
		 *	incase we need to expand them out and need to work
		 *	out how much "real" data is in the stream
		 */
		originalNumberOfChannels = this->numberOfChannels;
		if ( this->stereoExpand && this->numberOfChannels == 2)
		{
			this->numberOfChannels += 2;
		}
		else if ( this->monoExpand && this->numberOfChannels == 1)
		{
			this->numberOfChannels += 3;
		}

		no_renderers = 0;
		if ( split_out == true )
		{
			for ( unsigned char rend=0 ; rend < numberOfChannels ; rend++ )
			{
				renderers[rend] = new Output_Renderer(c_bufferLength, rend, effects);
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
			renderers[0] = new Output_Renderer(c_bufferLength, 0, effects);
			renderers[0]->SetXRAMEnabled(use_xram);
			renderers[0]->Open(samplerate,this->numberOfChannels,bitspersamp,0,0);
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
		isPlaying = false;
		// the stream is open and ready for the main thread
		streamOpen = true;
		// start prebuffering
		preBuffer = true;

		preBufferNumber = NO_BUFFERS_PROCESSED;

		// reload the speaker positions
		SetMatrix(speaker_matrix);

		SYNC_END;

		return c_bufferLength;
	}

	/*
		close

		this procedure is invoked by winamp when the file is closed
	*/
	void Output_Wumpus::Close() 
	{
		SYNC_START;
		streamOpen = false;

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
			effects->on_close();
		}

		// just incase the thread has exitted, assume playing has stopped
		isPlaying = IS_NOT_PLAYING;

		noBuffers = NO_BUFFERS;
		sampleRate = NO_SAMPLE_RATE;
		bitsPerSample = NO_BITS_PER_SAMPLE;
		numberOfChannels = NO_NUMBER_OF_CHANNELS;
		currentOutputTime = ZERO_TIME;
		currentWrittenTime = ZERO_TIME;
		total_played = ZERO_TIME;
		total_written = ZERO_TIME;
		lastOutputTime = ZERO_TIME;
		lastWrittenTime = ZERO_TIME;
		
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
				memset(temp, 0, TEMP_BUFFER_SIZE);
			}

			// ############## MONO EXPANSION
#pragma region MONO_EXPAND 
			if(monoExpand && originalNumberOfChannels == 1) {
				// we're writing out four as much data so
				// increase this value by three more times
				const unsigned int new_len = (len*4);

				// expand buffer here
				char* newBuffer = new char[new_len];
#ifdef _DEBUGGING
				memset(newBuffer, 0, new_len);
#endif

				// set relative value to zero
				int nPos = 0;

				const unsigned char sampleSize = 
					((bitsPerSample == 8) ? ONE_BYTE_SAMPLE : TWO_BYTE_SAMPLE);
				
				/* expand the samples out */
				for(int pos=0; pos < len; pos++) {

					if ( sampleSize == ONE_BYTE_SAMPLE)
					{
						newBuffer[nPos++] = buf[pos];
						newBuffer[nPos++] = buf[pos];
						newBuffer[nPos++] = buf[pos];
						newBuffer[nPos++] = buf[pos];
					}
					else
					{
						const short* src = (short*)(buf + pos);
						short* dst_a = (short*)(newBuffer + nPos);
						short* dst_b = (short*)(newBuffer + nPos + TWO_BYTE_SAMPLE);
						short* dst_c = (short*)(newBuffer + nPos + (TWO_BYTE_SAMPLE * 2));
						short* dst_d = (short*)(newBuffer + nPos + (TWO_BYTE_SAMPLE * 4));

						(*dst_a) = (*src);
						(*dst_b) = (*src);
						(*dst_c) = (*src);
						(*dst_d) = (*src);

						nPos += (sampleSize * 4);
						pos += sampleSize;
					}
				}

				delete buf;
				buf = newBuffer;
				len = new_len;
			}
#pragma endregion MONO_EXPAND
			// ############## END MONO EXPANSION

			// ############## STEREO EXPANSION
#pragma region STEREO_EXPAND
			if(stereoExpand && originalNumberOfChannels == 2) {

				// we're writing out twice as much data so
				// increase this value again
				const unsigned int old_length = len;
				const unsigned int new_len = old_length * 2;

				// expand buffer here
				char* newBuffer = new char[new_len];
#ifdef _DEBUGGING
				memset(newBuffer, 0, new_len);
#endif
				const unsigned char sampleSize = 
					((bitsPerSample == 8) ? TWO_BYTE_SAMPLE : FOUR_BYTE_SAMPLE);
				
				/* expand the samples out */
				int nPos = 0;
				for(unsigned int pos=0; pos < old_length; )
				{
					if ( sampleSize == TWO_BYTE_SAMPLE )
					{
						memcpy_s(newBuffer + nPos, new_len, buf + pos, TWO_BYTE_SAMPLE);
						memcpy_s(newBuffer + nPos + TWO_BYTE_SAMPLE, new_len, buf + pos, TWO_BYTE_SAMPLE);
					}
					else
					{
						void* dst1 = newBuffer + nPos;
						void* dst2 = newBuffer + nPos + FOUR_BYTE_SAMPLE;
						const char* src = buf + pos;

						const size_t red_len = new_len - nPos;

						memcpy_s(dst1, red_len,src, FOUR_BYTE_SAMPLE);
						memcpy_s(dst2, red_len,src, FOUR_BYTE_SAMPLE);	
					}

					nPos += (sampleSize * 2);
					pos += sampleSize;
				}

				delete buf;
				len = new_len;
				buf = newBuffer;
			}
#pragma endregion STEREO_EXPAND

			total_written += len;

#pragma region SPLIT_OUTPUT
			if ( split_out == true )
			{
				// create a table of pointers to each channels buffer
				char* buffers[MAX_RENDERERS];
				memset(buffers,0, sizeof(char*) * MAX_RENDERERS);
				const unsigned int csize = len / no_renderers;

				for ( char rend=0 ; rend < no_renderers ; rend++ )
				{
					// alloc a buffer to create it
					buffers[rend] = new char[csize];
					memset(buffers[rend],0,csize);
				
					if ( bitsPerSample == EIGHT_BIT_PER_SAMPLE )
					{
						/* create buffers for src/dst*/
						char *dst = (char*)buffers[rend];
						const char *src = (char*)buf;
						/* offset the channel number */
						src+=rend;

						/* work out top range */
						const char* to = dst + (csize);

						/* copy over */
						while(dst < to)
						{
							*dst = *src;
							dst++;
							src += no_renderers;
						}
					}
					else
					{
						/* create buffers for src/dst*/
						short *dst = (short*)buffers[rend];
						const short *src = (short*)buf;
						/* offset the channel number */
						src+=(rend * 2);	// * (times) by 2 coz its 16-bit

						/* work out top range */
						const short* to = dst + (csize / 2); // 2 coz 2 bytes per sample (16bit)

						/* copy over */
						while(dst < to)
						{
							*dst = *src;
							dst++;
							src += no_renderers;
						}
					}
				}

				/* we write in a sepeate loop to ensure that they're close
				 * together, if they're in the loop above the audio may drift */
				for ( char rend=0; rend < no_renderers ; rend++ )
				{
					renderers[rend]->Write(buffers[rend],csize);
				}

				delete buf;
			}
#pragma endregion SPLIT_OUTPUT
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
			if(!preBuffer)
			{
				this->CheckPlayState();
			}

			if(preBuffer)
			{
				if(++preBufferNumber == PREBUFFER_LIMIT)
				{
					preBuffer = false;
				}
			}
		}

		SYNC_END;

		return 0;
	}

	/*
		canwrite

		this procedure is invoked by winamp when it determining
		if more data can be written to the plugin
	*/
	int Output_Wumpus::CanWrite() {
		SYNC_START;

		int r = EMPTY_THE_BUFFER;
		if(streamOpen)
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
	int Output_Wumpus::IsPlaying() {
		SYNC_START;

		if ( streamOpen )
		{
			this->CheckProcessedBuffers();

			if(!preBuffer)
			{
				this->CheckPlayState();
			}
		}

		int r = isPlaying && streamOpen ? IS_PLAYING : IS_NOT_PLAYING;
		SYNC_END;
		return r;
	}

	/*
		pause

		returns the previous pause state
	*/
	int Output_Wumpus::Pause(int pause) {
		SYNC_START;
		lastPause = pause;
		
		// clear the error state
		alGetError();

		// if this is un-paused during pre-buffering
		// we may start to play data before it's ready
		// and cause and under-run
		
		if ( !this->preBuffer )
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
		return lastPause;
	}

	/*
		setvolume

		this procedure is invoked by winamp to set the volume
	*/
	void Output_Wumpus::SetVolume(int newVolume) {
		/*
			work out the % volume (as a float) and set it
		*/
		SYNC_START;

		if(streamOpen)
		{
			// calculate the volume to use (0.0 to 1.0)
			ALfloat aVolume = (ALfloat)newVolume / (ALfloat)VOLUME_DIVISOR;
			this->SetVolumeInternal(aVolume);
		}
		
		SYNC_END;
	}

	/*
		set volume (internal) - used to check the volume range
		and store the volume for later use
	*/
	void Output_Wumpus::SetVolumeInternal(ALfloat newVolume)
	{
		if(newVolume <= VOLUME_MAX && newVolume >= VOLUME_MIN)
		{
			volume = newVolume;
			
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
	void Output_Wumpus::SetPan(int newPan)
	{
		ALfloat f = ((ALfloat)newPan)/255.0f; 
		alListener3f(AL_POSITION, -f ,0.0,0.0);
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Wumpus::Flush(int newTimeMs)
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
			"Jump to %d, max length is %d\n", newTimeMs, Winamp::GetTrackLength());
		log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		bool closing = false;

		/*
		 * this checks that we can only flush (skip) when the steam is open
		 * but also make sure that we're skipping within the track length.
		 * if this isn't done, winamp will continue to skip past the end of
		 * the file.
		 */
		if ( streamOpen && (newTimeMs < (Winamp::GetTrackLength()-c_bufferLength)) )
		{
			// calculate the number of bytes that will have been
			// this will relocate to the current device at a set time
			this->Relocate(Framework::getInstance()->GetCurrentDevice(), newTimeMs, split_out);

			CheckPlayState();
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

	int Output_Wumpus::SetBufferTime(int tMs) {
		// calculate the number of bytes that will have been
		// processed after (t)ms
		int calcTime;

		SYNC_START;
		
		calcTime = 
			((((sampleRate / ONE_SECOND_IN_MS) *
			(bitsPerSample >> SHIFT_BITS_TO_BYTES)) *
			numberOfChannels) * tMs);

#ifdef _DEBUGGING
		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Calc to %dms, calc'd to be %d bytes", tMs, calcTime);
		log_debug_msg(dbg, __FILE__, __LINE__);
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Using sample rate %d, bps %d channels %d", sampleRate, bitsPerSample, numberOfChannels);
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
		lastOutputTime = tMs;
		lastWrittenTime = tMs;
		currentOutputTime = tMs;
		currentWrittenTime = tMs;

		SYNC_END;

		return calcTime;
	}

	/*
		getWrittenTime

		returns the time in ms of current position
	*/
	int Output_Wumpus::GetWrittenTime()
	{
		//SYNC_START;
		
		if(streamOpen)
		{
			currentWrittenTime = total_written;
			currentWrittenTime = get_current_written_time(currentWrittenTime, sampleRate) / bytesPerSampleChannel;

			/*
			 * we need to return the correct information. if we've expanded the data
			 * we need to only return half those values
			 */
			if ( stereoExpand && originalNumberOfChannels == 2)
			{
				currentWrittenTime /= 2;
			}

			if ( monoExpand && originalNumberOfChannels == 1)
			{
				currentWrittenTime /= 4;
			}

		}else{
			currentWrittenTime = ZERO_TIME;
		}
		
		//SYNC_END;
		
		// make sure we only use the first 32bits of the 64bit value
		lastWrittenTime = (int)(currentWrittenTime & THIRTY_TWO_BIT_BIT_MASK);
		return lastWrittenTime;
	}

	/*
		getOutputTime

		returns the time in ms of current position
	*/
	int Output_Wumpus::GetOutputTime()
	{
		//SYNC_START;

		if(streamOpen)
		{
			//SYNC_START;
			//CheckProcessedBuffers();
			//SYNC_END;

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
			currentOutputTime = total_played;
			// this converts bytes to ms and does 32bit part
			currentOutputTime =
				get_current_output_time(currentOutputTime, sampleRate)
				/ bytesPerSampleChannel;

			if ( stereoExpand && originalNumberOfChannels == 2)
			{
				currentOutputTime /= 2;
			}

			if ( monoExpand && originalNumberOfChannels == 1)
			{
				currentOutputTime /= 4;
			}

		}else{
			currentOutputTime = ZERO_TIME;
		}

		//SYNC_END;

		lastOutputTime = (int)(currentOutputTime & THIRTY_TWO_BIT_BIT_MASK);
		return lastOutputTime;
	}

	void Output_Wumpus::SetMonoExpanded(bool expanded)
	{
		monoExpand = expanded;
		ConfigFile::WriteBoolean(CONF_MONO_EXPAND,monoExpand);
	}

	void Output_Wumpus::SetStereoExpanded(bool expanded)
	{ 
		stereoExpand = expanded;
		ConfigFile::WriteBoolean(CONF_STEREO_EXPAND,stereoExpand);
	}

	void Output_Wumpus::SetSplit ( bool split )
	{
		ConfigFile::WriteBoolean(CONF_SPLIT,split);
		SwitchOutputDevice(Framework::getInstance()->GetCurrentDevice(),split);
	}

	void Output_Wumpus::SetXRAMEnabled( bool enabled )
	{
		xram_enabled = enabled;
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED,xram_enabled);
		SwitchOutputDevice(Framework::getInstance()->GetCurrentDevice(),split_out);
	}

	Output_Effects* Output_Wumpus::get_effects()
	{
		return this->effects;
	}

}