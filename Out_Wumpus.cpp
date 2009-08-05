#include "Out_Wumpus.h"
#include "Version.h"
#include "Out_Effects.h"
#include "ConfigStatusForm.h"
#include "Out_Renderer.h"

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

		for ( char i=0 ; i < MAX_RENDERERS ; i++ )
		{
			renderers[i] = NULL;
		}
	}

	Output_Wumpus::~Output_Wumpus() {

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
		void* ptr = (void*)&dest[destPos];
		void* ptrSrc = (void*)&src[srcPos];

		// now copy from this position onwards
		memcpy_s(ptr, MAXIMUM_BUFFER_SIZE, ptrSrc, size);
	}

	void Output_Wumpus::onError()
	{
		this->Close();
	}

	void Output_Wumpus::SwitchOutputDevice(int device)
	{
		for ( char i=0 ; i<no_renderers ; i++)
		{
			this->renderers[i]->Close();
		}

		/* stop the source so we dont hear anthing else */
		this->Relocate(device, GetOutputTime());
	}

	void Output_Wumpus::Relocate(int device, int currentPosition) {

		SYNC_START;

		// grab some temporary data for where we are now
		// and the format of the data
		unsigned int tempSampleRate = sampleRate;
		unsigned int tempBitsPerSample = bitsPerSample;
		unsigned int tempNumberOfChannels = numberOfChannels;

		// shut down the thread and wait for it to shutdown
		bool tempStreamOpen = streamOpen;
		int tempPause = lastPause;
		float tempVolume = volume;

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
		
		if(tempStreamOpen) 
		{
			// re-initialise everything
			this->Open(
				tempSampleRate,
				tempNumberOfChannels,
				tempBitsPerSample,
				0,0);
						
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
		/* TODO */
		for ( char i=0 ; i < no_renderers ; i++ )
		{
			this->renderers[i]->CheckProcessedBuffers();
		}
	}

	void Output_Wumpus::CheckPlayState()
	{
		/* TODO */
		for ( char i=0 ; i < no_renderers ; i++ )
		{
			this->renderers[i]->CheckPlayState();
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
			"\n(c) 2008 Sam Truscott\n\n"
			"Download latest version and source code (GPL)\n"
			"https://sourceforge.net/projects/winampopenalout/\n\n"
			"Thanks to:\n"
			"\tTinuva\n"
			"\tsoddit112\n"
			"\tGoujon\n"
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

		memset(
			&speaker_matrix,
			0,
			sizeof(speaker_matrix_T));

		Clock::Initialise();

		/*
			set up the variables to a default state
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

		int last_volume = ConfigFile::ReadGlobalInteger(CONF_VOLUME);
		if ( last_volume != -1)
		{
			volume = (last_volume / (ALfloat)VOLUME_DIVISOR);
		}
		else
		{
			volume = 1.0f;
		}

		split_out = ConfigFile::ReadBoolean(CONF_SPLIT);

		bool efx_enabled = ConfigFile::ReadBoolean(CONF_EFX_ENABLED);
		effects_list efx_env = EFX_REVERB_PRESET_GENERIC;
			
		efx_env = (effects_list)ConfigFile::ReadInteger(CONF_EFX_ENV);

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

#ifdef _DEBUG
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

#ifdef _DEBUG
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
		if ( WINVER <= 0x0600 )
		{
			if ( Framework::getInstance()->ALFWIsXRAMSupported() == AL_TRUE )
			{
#ifdef _DEBUG
				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"-> Detect XRAM, Size {%d}MB, Free {%d}MB",
					alGetEnumValue("AL_EAX_RAM_SIZE") / (1024 * 1024),
					alGetEnumValue("AL_EAX_RAM_FREE") / (1024 * 1024) );
				this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif
				this->xram_detected = true;
			}
		}
		else
		{
			this->xram_detected = false;
			SetXRAMEnabled(false);
		}

		char setting[11];
		strcpy_s(setting,11,"matrix_x_y\0");
		for (char i=0 ; i < MAX_RENDERERS ; i++ )
		{
			setting[9] = i + '0';
			setting[7] = 'x';	speaker_matrix.speakers[i].x = ConfigFile::ReadFloat(setting);
			setting[7] = 'y';	speaker_matrix.speakers[i].y = ConfigFile::ReadFloat(setting);
			setting[7] = 'z';	speaker_matrix.speakers[i].z = ConfigFile::ReadFloat(setting);
		}

		SYNC_END;

	}

	void Output_Wumpus::SetMatrix( speaker_matrix_T m )
	{
		speaker_matrix = m;
		for( char i=0 ; i < no_renderers ; i++ )
		{
			if ( renderers[i] != NULL )
			{
				renderers[i]->SetMatrix( m.speakers[i] );
			}
		}

		char setting[11];
		strcpy_s(setting,11,"matrix_x_y\0");
		for (char i=0 ; i < MAX_RENDERERS ; i++ )
		{
			setting[9] = i + '0';
			setting[7] = 'x';	ConfigFile::WriteFloat( setting, speaker_matrix.speakers[i].x );
			setting[7] = 'y';	ConfigFile::WriteFloat( setting, speaker_matrix.speakers[i].y );
			setting[7] = 'z';	ConfigFile::WriteFloat( setting, speaker_matrix.speakers[i].z );
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

		ConfigFile::WriteInteger(CONF_VOLUME, volume * VOLUME_DIVISOR);

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
	int Output_Wumpus::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms) {
		
		SYNC_START;

		if(bitspersamp > SIXTEEN_BIT_PER_SAMPLE)
		{
			MessageBoxA(NULL, "This Plug-In only supports 8 and 16bit audio, please disable 24bit audio in Winamp", "Whoops", MB_OK);
			SYNC_END;
			return -1;
		}
#ifdef _DEBUG
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

		// determine the size of the buffer
		bytesPerSampleChannel = ((bitsPerSample >> SHIFT_BITS_TO_BYTES)*numberOfChannels);

		for ( char i=0 ; i < MAX_RENDERERS ; i++ )
		{
			if ( renderers[i] != NULL )
			{
				renderers[i]->Close();
				delete renderers[i];
				renderers[i] = NULL;
			}
		}

		bool use_xram = false;

		if ( xram_detected == true && 
				xram_enabled == true &&
				alGetEnumValue("AL_EAX_RAM_FREE") > 0)
		{
			use_xram = true;
		}

		no_renderers = 0;
		if ( split_out == true )
		{
			for ( unsigned int i=0 ; i < numberOfChannels ; i++ )
			{
				renderers[i] = new Output_Renderer(c_bufferLength, i);
				renderers[i]->SetXRAMEnabled(true);
				renderers[i]->Open(samplerate,1,bitspersamp,0,0);
				no_renderers++;
			}
		}
		else
		{
			renderers[0] = new Output_Renderer(c_bufferLength, 0);
			renderers[0]->SetXRAMEnabled(true);
			renderers[0]->Open(samplerate,numchannels,bitspersamp,0,0);
			no_renderers++;
		}

#ifdef _DEBUG
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"-> Using {%d} buffers", 
			noBuffers);
		this->log_debug_msg(dbg, __FILE__, __LINE__);
#endif

		/*
			set up the various timers
		*/
		// reset the clock
		Clock::Initialise();
		// get the current time (small optimisation)
		Time_Type currentTime = Clock::GetTime();
		// reset the time we last checked the state of the playing buffers
		lastCheckBuffers = currentTime;

		unsigned int in_xram_ok = 0;

		// allocate some buffers
		alGetError();

#ifdef _DEBUG
		if ( xram_detected == true && 
				xram_enabled == true )
		{
			sprintf_s(
				dbg, 
				DEBUG_BUFFER_SIZE, 
				"--> {%d} Buffers from {%d} were stored in XRAM OK", 
				in_xram_ok, 
				noBuffers);
			log_debug_msg(dbg, __FILE__, __LINE__);
		}
#endif

		// set the volume for the source
		this->SetVolumeInternal(volume);

		// we're not playing yet because we're prebuffering
		isPlaying = false;
		// the stream is open and ready for the main thread
		streamOpen = true;
		// start prebuffering
		preBuffer = true;

		preBufferNumber = NO_BUFFERS_PROCESSED;

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

		for ( char i=0 ; i < MAX_RENDERERS ; i++ )
		{
			if ( renderers[i] != NULL )
			{
				renderers[i]->Close();
				delete renderers[i];
				renderers[i] = NULL;
			}
		}

		// just incase the thread has exitted, assume playing has stopped
		isPlaying = false;

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

#ifdef _DEBUG
			char dbg[DEBUG_BUFFER_SIZE] = {'\0'};

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing data {0x%08X} with length {%d}",buf, len);
			log_debug_msg(dbg, __FILE__, __LINE__);
#endif

			if ( len + temp_size < MINIMUM_BUFFER_SIZE ) 
			{
				fmemcpy(
					temp,
					temp_size,
					buf,
					0,
					len);

				temp_size = temp_size + len;
#ifdef _DEBUG
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
				to_write = new char[len+temp_size];

				/* copy the first buffer in */
				fmemcpy(
					to_write,
					0,
					temp,
					0,
					temp_size);
#ifdef _DEBUG
				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"Copying {%d} bytes from temp", temp_size);
				log_debug_msg(dbg, __FILE__, __LINE__);
#endif
				/* copy what we can of the second */
				fmemcpy(
					to_write,
					temp_size,
					buf,
					0,
					len);

				buf = to_write;
				len += temp_size;
				temp_size = 0; 
			}
			
//			// ############## MONO EXPANSION
//
//			if(monoExpand) {
//				// we're writing out four as much data so
//				// increase this value by three more times
//				const unsigned int new_len = (len*4);
//
//				// expand buffer here
//				char* newBuffer = new char[new_len];
//#ifdef _DEBUG
//				memset(newBuffer, 0, new_len);
//#endif
//
//				// set relative value to zero
//				unsigned int nPos = 0;
//
//				const unsigned char sampleSize = 
//					((bitsPerSample == 8) ? ONE_BYTE_SAMPLE : TWO_BYTE_SAMPLE);
//				
//				/* expand the samples out */
//				for(unsigned int pos=0; pos < len; pos++) {
//
//					if ( sampleSize == ONE_BYTE_SAMPLE)
//					{
//						newBuffer[nPos++] = buf[pos];
//						newBuffer[nPos++] = buf[pos];
//						newBuffer[nPos++] = buf[pos];
//						newBuffer[nPos++] = buf[pos];
//					}
//					else
//					{
//						const short* src = (short*)(buf + pos);
//						short* dst_a = (short*)(newBuffer + nPos);
//						short* dst_b = (short*)(newBuffer + nPos + TWO_BYTE_SAMPLE);
//						short* dst_c = (short*)(newBuffer + nPos + (TWO_BYTE_SAMPLE * 2));
//						short* dst_d = (short*)(newBuffer + nPos + (TWO_BYTE_SAMPLE * 4));
//
//						(*dst_a) = (*src);
//						(*dst_b) = (*src);
//						(*dst_c) = (*src);
//						(*dst_d) = (*src);
//
//						nPos += (sampleSize * 4);
//						pos += sampleSize;
//					}
//				}
//
//				buf = newBuffer;
//				len = new_len;
//				/* we're using the internal buffer on the heap now */
//				if ( uiBuffers[selectedBuffer].data != NULL )
//				{
//					delete uiBuffers[selectedBuffer].data;
//					uiBuffers[selectedBuffer].data = NULL;
//				}
//				uiBuffers[selectedBuffer].data = buf;
//			}
//
//			// ############## END MONO EXPANSION
//
//			// ############## STEREO EXPANSION
//
//			if(stereoExpand) {
//
//				// we're writing out twice as much data so
//				// increase this value again
//				const unsigned int new_len = len * 2;
//
//				// expand buffer here
//				char* newBuffer = new char[new_len];
//#ifdef _DEBUG
//				memset(newBuffer, 0, new_len);
//#endif
//
//				unsigned int nPos = 0;
//				const unsigned char sampleSize = 
//					((bitsPerSample == 8) ? TWO_BYTE_SAMPLE : FOUR_BYTE_SAMPLE);
//				
//				/* expand the samples out */
//				for(unsigned int pos=0; pos < len;)
//				{
//					if ( sampleSize == TWO_BYTE_SAMPLE )
//					{
//						const short* src = (short*)(buf + pos);
//						short* dst_a = (short*)(newBuffer + nPos);
//						short* dst_b = (short*)(newBuffer + nPos + TWO_BYTE_SAMPLE);
//
//						(*dst_a) = (*src);
//						(*dst_b) = (*src);
//					}
//					else
//					{
//						const unsigned int* src = 
//							(const unsigned int*)(buf + pos);
//						unsigned int* dst_a = 
//							(unsigned int*)(newBuffer + nPos);
//						unsigned int* dst_b = 
//							(unsigned int*)(newBuffer + nPos + FOUR_BYTE_SAMPLE);
//
//						(*dst_a) = (*src);
//						(*dst_b) = (*src);
//					}
//
//					nPos += (sampleSize * 2);
//					pos += sampleSize;
//				}
//
//				len = new_len;
//				buf = newBuffer;
//				/* we're using the internal buffer on the heap now */
//				if ( uiBuffers[selectedBuffer].data != NULL )
//				{
//					delete uiBuffers[selectedBuffer].data;
//					uiBuffers[selectedBuffer].data = NULL;
//				}
//				uiBuffers[selectedBuffer].data = buf;
//			}

			total_written += len;

			if ( split_out)
			{
				char* buffers[MAX_RENDERERS];
				memset(buffers,0, sizeof(char*) * MAX_RENDERERS);

				for ( int i=0; i < no_renderers ; i++ )
				{
					const unsigned int csize = len / no_renderers;
					buffers[i] = new char[csize];
					memset(buffers[i],0,csize);
				
					if ( bitsPerSample == 8 )
					{
						/* TODO */
					}
					else
					{
						/* create buffers for src/dst*/
						short *dst = (short*)buffers[i], *src = (short*)buf;
						/* offset the channel number */
						src+=i;

						/* work out top range */
						const short* to = dst + (csize / 2);

						/* copy over */
						while(dst < to)
						{
							*dst = *src;
							dst++;
							src += no_renderers;
						}
					}

					renderers[i]->Write(buffers[i],csize);
				}
				
				delete buf;
			}
			else
			{
				for ( char i=0; i < no_renderers ; i++ )
				{				
					renderers[i]->Write(buf,len);
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

			this->CheckProcessedBuffers();

			if ( no_renderers > 0 )
			{
				r = renderers[0]->CanWrite() ? renderers[0]->GetBufferFree() : EMPTY_THE_BUFFER;
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

		this->CheckProcessedBuffers();

		if(!preBuffer)
		{
			this->CheckPlayState();
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
		
		for ( char i=0 ; i < no_renderers ; i++ )
		{
			renderers[i]->Pause(pause);
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
			
			for ( char i=0 ; i < no_renderers ; i++ )
			{
				renderers[i]->SetVolumeInternal(volume);
			}

			ConfigFile::WriteInteger(CONF_VOLUME, volume * VOLUME_DIVISOR);
		}
	}

	/*
		setpan - INCOMPLETE

		this procedure is invoked by winamp to set the pan
	*/
	void Output_Wumpus::SetPan(int pan)
	{
		ALfloat f = pan/255.0f; 
		alListener3f(AL_POSITION, -f ,1.0,1.0);
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Wumpus::Flush(int tMs) {
		SYNC_START;

		for ( char i=0 ; i < no_renderers ; i++ )
		{
			renderers[i]->Flush(tMs);
		}

		// calculate the number of bytes that will have been
		// this will relocate to the current device at a set time
		this->Relocate(Framework::getInstance()->GetCurrentDevice(), tMs);

		CheckPlayState();

		SYNC_END;
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

		for ( char i = 0 ; i < no_renderers ; i++ )
		{
			renderers[i]->SetPlayedTime(calcTime);
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
		SYNC_START;
		
		if(streamOpen)
		{
			currentWrittenTime = total_written;
			currentWrittenTime = get_current_written_time(currentWrittenTime, sampleRate) / bytesPerSampleChannel;
		}else{
			currentWrittenTime = ZERO_TIME;
		}
		
		SYNC_END;
		
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
		SYNC_START;

		if(streamOpen)
		{
			// only check the state of the openal buffers
			// every so often, no need to keep checking them
			// if we know they're x'ms long
			Time_Type currentTime = Clock::GetTime();

			CheckProcessedBuffers();
			lastCheckBuffers = currentTime;

			total_played = 0;
			for( char i=0; i < no_renderers ; i++ )
			{
				total_played += (renderers[i]->GetPlayedTime() + renderers[i]->GetPosition());
			}
			
			//total_played += (renderers[0]->GetPlayedTime() + renderers[0]->GetPosition());

			// this works it out how many bytes it is
			currentOutputTime = total_played;

			// this converts bytes to ms and does 32bit part
			currentOutputTime =
				get_current_output_time(currentOutputTime, sampleRate)
				/ bytesPerSampleChannel;

		}else{
			currentOutputTime = ZERO_TIME;
		}

		SYNC_END;

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
		split_out = split;
		ConfigFile::WriteBoolean(CONF_SPLIT,split_out);
	}

	void Output_Wumpus::SetXRAMEnabled( bool enabled )
	{
		/* Windows Vista or higher doesnt support XRAM yet - it seems */
		if ( WINVER > 0x0600 )
		{
			enabled = false;
		}

		xram_enabled = enabled;
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED,xram_enabled);
	}

	Output_Effects* Output_Wumpus::get_effects()
	{
		return this->effects;
	}

}