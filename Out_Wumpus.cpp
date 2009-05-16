#include "Out_Wumpus.h"
#include "Version.h"

#include "ConfigStatusForm.h"

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
		/* stop the source so we dont hear anthing else */
		alSourceStop(uiSource);
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
			this->Pause(tempPause);
		}
		SYNC_END;
	}

	void Output_Wumpus::CheckProcessedBuffers() 
	{

		ALint			iBuffersProcessed;
		ALuint			uiNextBuffer;
		ALenum			err;

		unsigned int selectedBuffer;

		iBuffersProcessed = NO_BUFFERS_PROCESSED;

		// ask open al how many buffers have been processed
		alGetError();
		alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
		if( (err = alGetError()) != AL_NO_ERROR )
		{
			if(err == AL_INVALID_VALUE) {
				MessageBoxA(NULL, "Error with Source - Invalid Value", "Error With Source", MB_OK);
			} else if (err == AL_INVALID_ENUM) {
				MessageBoxA(NULL, "Error with Source - Invalid Enumeration", "Error With Source", MB_OK);
			} else if (err == AL_INVALID_NAME) {
				MessageBoxA(NULL, "Error with Source - Invalid Name", "Error With Source", MB_OK);
			} else if (err == AL_INVALID_OPERATION) {
				MessageBoxA(NULL, "Error with Source - Invalid Operation", "Error With Source", MB_OK);
			} else {
				MessageBoxA(NULL, "Error with Source - Unknown Error", "Error With Source", MB_OK);
			}
			onError();
		}

		while (iBuffersProcessed--)
		{
			uiNextBuffer = DEFAULT_BUFFER;
			
			// clear any open al errors
			alGetError();

			/* unqueue *any* processed buffer. the buffer that has been
				processed will be put into uiNextBuffer */
			alSourceUnqueueBuffers(
				uiSource,
				GET_ONE_BUFFER,
				&uiNextBuffer); 

			// based on the bufferID determine the buffer index
			selectedBuffer = UNKNOWN_BUFFER;
			for(unsigned int i=0;i<noBuffers;i++)
			{
				if( (!uiBuffers[i].available) && uiBuffers[i].buffer_id == uiNextBuffer )
				{
					selectedBuffer = i;
					uiBuffers[i].available = true;

					if ( uiBuffers[i].data != NULL )
					{
						delete uiBuffers[i].data;
						uiBuffers[i].data = NULL;
					}

					buffers_free++;
					break;
				}
			}

			/* if no errors have occured we can mark that
				buffer as available again */
			if(alGetError() == AL_NO_ERROR)
			{
				/* increase our played time position
				 (the delta for where we are in the current buffer is done
				 in get_output_time */
				total_played += uiBuffers[selectedBuffer].size;
				buffer_free += uiBuffers[selectedBuffer].size;
				uiBuffers[selectedBuffer].size = 0;

			} else { 
				MessageBoxA(NULL, "Error in Monitor Thread - Out of Range", "Error Monitoring", MB_OK);
				this->onError();
			}
		}
	}

	void Output_Wumpus::CheckPlayState() {

		ALint			iState;
		ALint			iQueuedBuffers;

		/*
			can the current state of the source
		*/
		/* AL_SOURCE_STATE, AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED */
		alGetError();
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
		
		if(alGetError() != AL_NO_ERROR)
		{
			this->onError();
		}

		if (iState == AL_PLAYING)
		{
			// if the source is playing then we record that
			isPlaying = true;
		}else{

			if(!preBuffer && 
				streamOpen && 
				(iState == AL_INITIAL || iState == AL_STOPPED) &&
				total_written == total_played)
			{
				isPlaying = false;
				return;
			}

			// if we're not playing check to see if any buffers are queued
			alGetError();
			alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);

			if(alGetError() != AL_NO_ERROR)
			{
				MessageBoxA(NULL, "Error playing", "Error Playing", MB_OK);
				this->onError();
			}

			if (iQueuedBuffers)
			{
				// if any buffers are queued and we're not playing - play!
				if(!lastPause)
				{
					alSourcePlay(uiSource);
					isPlaying = true;
				}
			} else {
				// otherwise we're finished
				isPlaying = false;
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

		Clock::Initialise();

		/*
			set up the variables to a default state
		*/
		streamOpen = false;
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

		c_bufferLatency = ConfigFile::ReadInteger(CONF_BUFFER_LATENCY);
		if(c_bufferLatency == ERROR_BUFFER || 
			c_bufferLatency > CONF_BUFFER_LATENCY_MAX || 
			c_bufferLatency < CONF_BUFFER_LATENCY_MIN)
		{
			c_bufferLatency = DEFC_BUFFER_LATENCY;
			ConfigFile::WriteInteger(CONF_BUFFER_LATENCY, c_bufferLatency);
		}

		// read in the current device to use
		int currentDevice = ConfigFile::ReadInteger(CONF_DEVICE);
		if(currentDevice == -1)
		{
			currentDevice = DEFC_DEVICE;
			ConfigFile::WriteInteger(CONF_DEVICE, currentDevice);
		}

		int last_volume = ConfigFile::ReadGlobalInteger(CONF_VOLUME);
		volume = (last_volume / (ALfloat)VOLUME_DIVISOR);

		/*
			initialise openal itself - this has been modified
			and will also select the default sound card
		*/
		if (!Framework::getInstance()->ALFWInitOpenAL(currentDevice))
		{
			MessageBoxA(NULL, "Could not initialise OpenAL", "Error", MB_OK);
		}

		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Using Device {%d} with buffer of {%d}",
			currentDevice,
			c_bufferLength);
		this->log_debug_msg(dbg, __FILE__, __LINE__);

		this->monoExpand = ConfigFile::ReadBoolean(CONF_MONO_EXPAND);
		this->stereoExpand = ConfigFile::ReadBoolean(CONF_STEREO_EXPAND);
		this->xram_enabled = ConfigFile::ReadBoolean(CONF_XRAM_ENABLED);

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

		if ( Framework::getInstance()->ALFWIsXRAMSupported() == AL_TRUE )
		{
			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"-> Detect XRAM, Size {%d}MB, Free {%d}MB",
				alGetEnumValue("AL_EAX_RAM_SIZE") / (1024 * 1024),
				alGetEnumValue("AL_EAX_RAM_FREE") / (1024 * 1024) );
			this->log_debug_msg(dbg, __FILE__, __LINE__);
			xram_detected = true;
		}

		SYNC_END;

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

		char dbg[DEBUG_BUFFER_SIZE] = {'\0'};
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"Open: Sample rate {%d}, Channels {%d}, Bits {%d}",
			samplerate,
			numchannels,
			bitspersamp);
		log_debug_msg(dbg, __FILE__, __LINE__);

		//record the format of the data we're getting
		sampleRate = samplerate;
		numberOfChannels = numchannels;
		bitsPerSample = bitspersamp;

		bool tmpStereoExpand = stereoExpand;
		bool tmpMonoExpand = monoExpand;

		stereoExpand = false;
		monoExpand = false;

		// determine the format to output in
		switch(numberOfChannels)
		{
			case 1:
				if(tmpMonoExpand) {
					ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
					monoExpand = true;
				}else{
					ulFormat = bitsPerSample == 8 ? alGetEnumValue("AL_FORMAT_MONO8") : alGetEnumValue("AL_FORMAT_MONO16");
				}
				break;
			case 2:
				if(tmpStereoExpand) {
					ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
					stereoExpand = true;
				}else{
					ulFormat = bitsPerSample == 8 ? alGetEnumValue("AL_FORMAT_STEREO8") : alGetEnumValue("AL_FORMAT_STEREO16");
				}
				break;
			case 4:
				ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
				break;
			case 6:
				ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
				break;
			case 7:
				ulFormat = alGetEnumValue("AL_FORMAT_61CHN16");
				break;
			case 8:
				ulFormat = alGetEnumValue("AL_FORMAT_71CHN16");
				break;
		};

		// reset the play position back to zero
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		lastPause = 0;

		temp_size = 0;

		// determine the size of the buffer
		bytesPerSampleChannel = ((bitsPerSample >> SHIFT_BITS_TO_BYTES)*numberOfChannels);

		if (stereoExpand) { bytesPerSampleChannel *= 2; }
		if (monoExpand) { bytesPerSampleChannel *=4; }

		bufferSize = (bytesPerSampleChannel * (sampleRate / ONE_SECOND_IN_MS)) * c_bufferLength;

		//noBuffers = MAX_NO_BUFFERS;
		noBuffers = (bufferSize / MAXIMUM_BUFFER_SIZE);// + MAXIMUM_BUFFER_OFFSET;

		// if we have no buffers just assume we can use at least (MINIMUM_BUFFERS)
		// we need more than one so we can listen to one and write t'other
		// if that doesnt happen it will error anyway
		if(noBuffers < MINIMUM_BUFFERS)
		{
			noBuffers = MINIMUM_BUFFERS;
		}

		this->buffer_free = noBuffers * MAXIMUM_BUFFER_SIZE;
		buffers_free = noBuffers;

		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"-> Using {%d} buffers for total size of {%d}", 
			noBuffers,
			bufferSize);
		this->log_debug_msg(dbg, __FILE__, __LINE__);

		/*
			set up the various timers
		*/
		// reset the clock
		Clock::Initialise();
		// get the current time (small optimisation)
		Time_Type currentTime = Clock::GetTime();
		// reset the time we last checked the state of the playing buffers
		lastCheckBuffers = currentTime;
		// determine the delay to check the state of the openal buffers
		lastCheckDelay = Clock::Milliseconds(c_bufferLength / noBuffers);

		unsigned int in_xram_ok = 0;

		// allocate some buffers
		alGetError();
		for(unsigned int i=0;i<MAX_NO_BUFFERS;i++)
		{
			uiBuffers[i].size = 0;
			uiBuffers[i].available = false;
			uiBuffers[i].data = NULL;
		}

		for(unsigned int i=0;i<noBuffers;i++)
		{
			uiBuffers[i].size = 0;
			uiBuffers[i].available = true;
			uiBuffers[i].data = NULL;
			alGenBuffers( 1, &uiBuffers[i].buffer_id );

			/* if xram is enabled write the buffer to XRAM */
			if ( xram_detected == true && 
				xram_enabled == true &&
				alGetEnumValue("AL_EAX_RAM_FREE") > 0)
			{
				ALboolean inhw = eaxSetBufferMode(
					1,
					&uiBuffers[i].buffer_id,
					eXRAMHardware);

				if ( inhw == AL_FALSE )
				{
					log_debug_msg("Failed to set buffer as XRAM", __FILE__, __LINE__);
				}
				else
				{
					in_xram_ok++;
				}	
			}
		}

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

		ALenum err = alGetError();
		if( err == AL_INVALID_VALUE || err == AL_OUT_OF_MEMORY)
		{
			if( err != AL_NO_ERROR) 
			{
				if(err == AL_OUT_OF_MEMORY) {
					MessageBoxA(NULL, "Error generating Buffer - Out of Memory", "Error", MB_OK);
				} else if(err == AL_INVALID_VALUE) {
					MessageBoxA(NULL, "Error generating  Buffer - Invalid Value", "Error", MB_OK);
				} else {
					MessageBoxA(NULL, "Error generating  Buffer - Unknown Error", "Error", MB_OK);
				}
				this->onError();
			}
			
			/* if we've created too many buffers try and
		     * use less */
			while(err != AL_NO_ERROR && noBuffers > 1)
			{
				alGetError();
				noBuffers--;
				for(unsigned int i=0;i<noBuffers;i++)
				{
					alGenBuffers( noBuffers, &uiBuffers[i].buffer_id );
				}
				err = alGetError();
			}
		}

		// allocate a source
		alGetError();
		alGenSources( 1, &uiSource );

		if( (err = alGetError()) != AL_NO_ERROR)
		{
			if(err == AL_OUT_OF_MEMORY) {
				MessageBoxA(NULL, "Error generating Source - Out of Memory", "Error", MB_OK);
			} else if(err == AL_INVALID_VALUE) {
				MessageBoxA(NULL, "Error generating Source - Invalid Value", "Error", MB_OK);
			} else if(err == AL_INVALID_OPERATION) {
				MessageBoxA(NULL, "Error generating Source - Invalid Operation", "Error", MB_OK);
			}  else {
				MessageBoxA(NULL, "Error generating Source - Unknown Error", "Error", MB_OK);
			}
			this->onError();
		}
		
		alSourcei(uiSource, AL_LOOPING, AL_FALSE);

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

		return c_bufferLatency;
	}

	/*
		close

		this procedure is invoked by winamp when the file is closed
	*/
	void Output_Wumpus::Close() 
	{
		SYNC_START;
		streamOpen = false;

		// stop the source
		alSourceStop(uiSource);

		ALint iState;
		unsigned int timeout = 0;
		// query the state of the source and wait for it to stop
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
		while(iState != AL_STOPPED && iState != AL_INITIAL)
		{
			Sleep(10);
			if(timeout++ > CLOSE_TIMEOUT_COUNT) {
				break;
			}
		}

		// remove all buffers
		alSourcei(uiSource, AL_BUFFER, 0);

		// delete the source
		alDeleteSources( 1, &uiSource );
		// delete all the buffers
		for(unsigned int i=0;i<noBuffers;i++)
		{
			if ( uiBuffers[i].data != NULL )
			{
				delete uiBuffers[i].data;
				uiBuffers[i].data = NULL;
			}
			alDeleteBuffers( noBuffers, &uiBuffers[i].buffer_id );
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

		ALenum err;

		// if we cannot write exit now (non-blocking op)
		if(buffers_free == 0 || !streamOpen)
		{
			SYNC_END;
			return -1;
		}

		// if the buffer is valid (non-NULL)
		if (buf) {

			char dbg[DEBUG_BUFFER_SIZE] = {'\0'};

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing data {0x%08X} with length {%d}",buf, len);
			log_debug_msg(dbg, __FILE__, __LINE__);

			if ( len + temp_size < MINIMUM_BUFFER_SIZE ) 
			{
				fmemcpy(
					temp,
					temp_size,
					buf,
					0,
					len);

				temp_size += len;

				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"Temp buffer increased by {%d} to {%d}",len, temp_size);
				log_debug_msg(dbg, __FILE__, __LINE__);

				SYNC_END;
				return 0;
			}
			
			// set it to ERROR'd so we can detect errors
			ALuint uiNextBuffer = UNKNOWN_BUFFER;
			unsigned int selectedBuffer = UNKNOWN_BUFFER;

			/*	go through the buffers and find an available one

				if we're here (critical section, canWrite=true) then
				we assume that one is! 
			*/
			for(ALuint i=0;i<noBuffers;i++)
			{
				// find an available buffer
				if ( uiBuffers[i].available == true )
				{
					uiNextBuffer = uiBuffers[i].buffer_id;
					uiBuffers[i].available = false;
					selectedBuffer = i;
					buffers_free--;
					break;
				}
			}

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing using buffer {%d}", selectedBuffer);
			log_debug_msg(dbg, __FILE__, __LINE__);

			if(selectedBuffer == UNKNOWN_BUFFER)
			{
				SYNC_END;
				return -1;
			}

			uiBuffers[selectedBuffer].data = NULL;

			if ( temp_size > 0 )
			{
				uiBuffers[selectedBuffer].data = new char[MAXIMUM_BUFFER_SIZE];

				/* copy the first buffer in */
				memcpy_s(
					uiBuffers[selectedBuffer].data,
					MAXIMUM_BUFFER_SIZE,
					temp,
					temp_size);

				sprintf_s(
					dbg,
					DEBUG_BUFFER_SIZE,
					"Copying {%d} bytes from temp", temp_size);
				log_debug_msg(dbg, __FILE__, __LINE__);

				unsigned int overflow = 0;
				if ((temp_size + len) > MAXIMUM_BUFFER_SIZE)
				{
					overflow = (temp_size + len) - MAXIMUM_BUFFER_SIZE;
				}

				const unsigned int remains = len - overflow;

				/* copy what we can of the second */
				fmemcpy(
					(char*)uiBuffers[selectedBuffer].data,
					temp_size,
					buf,
					0,
					remains);

				buf = (char*)uiBuffers[selectedBuffer].data;
				len = temp_size + remains;

				if ( overflow > 0 )
				{

					/* get the rest ready for next time */
					memcpy_s(
						temp,
						TEMP_BUFFER_SIZE,
						(char*)(buf + remains),
						overflow);

					sprintf_s(
						dbg,
						DEBUG_BUFFER_SIZE,
						"Storing {%d} bytes for next time", overflow);
					log_debug_msg(dbg, __FILE__, __LINE__);

					temp_size = overflow; 
				}
				else
				{
					temp_size = 0; 
				}
			}

			buffer_free -= len;
			total_written += len;
			uiBuffers[selectedBuffer].size = len;

			// ############## MONO EXPANSION

			if(monoExpand) {
				// we're writing out four as much data so
				// increase this value by three more times
				unsigned int new_len = (len*4);

				// expand buffer here
				char* newBuffer = new char[new_len];
				memset(newBuffer, 0, new_len);

				// set relative value to zero
				unsigned int nPos = 0;
				
				/* expand the samples out */
				for(unsigned int pos=0; pos < len; pos++) {
					newBuffer[nPos++] = buf[pos];
					newBuffer[nPos++] = buf[pos];
					newBuffer[nPos++] = buf[pos];
					newBuffer[nPos++] = buf[pos];
				}

				buf = newBuffer;
				len = new_len;
				uiBuffers[selectedBuffer].data = buf;
			}

			// ############## END MONO EXPANSION

			// ############## STEREO EXPANSION

			if(stereoExpand) {

				// we're writing out twice as much data so
				// increase this value again
				unsigned int new_len = len * 2;

				// expand buffer here
				char* newBuffer = new char[new_len];
				memset(newBuffer, 0, new_len);

				unsigned int nPos = 0;
				unsigned char sampleSize = bitsPerSample == 8 ? TWO_BYTE_SAMPLE : FOUR_BYTE_SAMPLE;
				
				/* expand the samples out */
				for(unsigned int pos=0; pos < len;) {
					// copy the front left and right
					fmemcpy(/*dst	*/	newBuffer,
							/*dstPos*/	nPos,
							/*src	*/	buf, 
							/*srcPos*/	pos, 
							/*size	*/	sampleSize);
					nPos+=sampleSize;

					// copy the rear left and right
					fmemcpy(/*dst	*/	newBuffer,
							/*dstPos*/	nPos,
							/*src	*/	buf,
							/*srcPos*/	pos,
							/*size	*/	sampleSize);
					nPos+=sampleSize;
					pos+=sampleSize;
				}

				len = new_len;
				buf = newBuffer;
				uiBuffers[selectedBuffer].data = buf;
			}

			// ############## END STEREO EXPANSION

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing to OpenAL buffer,{%d} bytes", len);
			log_debug_msg(dbg, __FILE__, __LINE__);

			// buffer the data with the correct format
			alGetError();
			alBufferData(uiNextBuffer, ulFormat, buf, len, sampleRate);
			if( (err = alGetError()) != AL_NO_ERROR)
			{
				if(err == AL_OUT_OF_MEMORY) {
					MessageBoxA(NULL, "Error Buffering - Out of Memory", "Error", MB_OK);
				} else if(err == AL_INVALID_VALUE) {
					MessageBoxA(NULL, "Error Buffering - Invalid Value", "Error", MB_OK);
				} else if(err == AL_INVALID_ENUM) {
					MessageBoxA(NULL, "Error Buffering - Invalid Enumeration", "Error", MB_OK);
				} else if( err == AL_INVALID_NAME) {
					MessageBoxA(NULL, "Error Buffering - Invalid Name", "Error", MB_OK);
				} else {
					char txt[200] = {'\0'};
					sprintf_s(txt, "Error Buffering - Unknown Error [%d]", err);
					MessageBoxA(NULL, txt, "Error", MB_OK);
				}
				this->onError();
			}

			// add the buffer to the source queue
			alGetError();
			alSourceQueueBuffers(uiSource, 1, &uiNextBuffer);
			if( (err = alGetError()) != AL_NO_ERROR)
			{
				if(err == AL_INVALID_NAME) {
					MessageBoxA(NULL, "Error Queuing - Invalid Name", "Error", MB_OK);
				} else if(err == AL_INVALID_OPERATION) {
					MessageBoxA(NULL, "Error Queuing - Invalid Operation", "Error", MB_OK);
				} else {
					MessageBoxA(NULL, "Error Queuing - Unknown Error", "Error", MB_OK);
				}
				this->onError();
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
		int r;
		if(streamOpen) {

			this->CheckProcessedBuffers();

			r = ((buffers_free > 0) ? buffer_free : 0);
		}else{
			r = EMPTY_THE_BUFFER;
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
		if(streamOpen && !preBuffer)
		{
			if(pause)
			{
				alSourcePause(uiSource);
			}else{
				alSourcePlay(uiSource);
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
		if(newVolume <= VOLUME_MAX && newVolume >= VOLUME_MIN) {
			volume = newVolume;
			alSourcef(uiSource,AL_GAIN, newVolume);
			ConfigFile::WriteInteger(CONF_VOLUME, volume * VOLUME_DIVISOR);
		}
	}

	/*
		setpan - INCOMPLETE

		this procedure is invoked by winamp to set the pan
	*/
	void Output_Wumpus::SetPan(int pan)
	{
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Wumpus::Flush(int tMs) {
		SYNC_START;

		// make sure we've stopped playing
		alSourceStop(uiSource);

		// calculate the number of bytes that will have been
		// this will relocate to the current device at a set time
		this->Relocate(Framework::getInstance()->GetCurrentDevice(), tMs);

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
			if(currentTime > lastCheckBuffers + lastCheckDelay)
			{
				CheckProcessedBuffers();
				lastCheckBuffers = currentTime;
			}

			ALint deltaBytes;
			// get the position in the currently playing buffer
			alGetSourcei(uiSource, AL_BYTE_OFFSET, &deltaBytes);

			// this works it out how many bytes it is
			currentOutputTime = total_played + deltaBytes;

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

	void Output_Wumpus::SetXRAMEnabled( bool enabled )
	{
		xram_enabled = enabled;
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED,xram_enabled);
	}

}