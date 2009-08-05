#include "Out_Renderer.h"
#include "Out_Effects.h"
#include "ConfigFile.h"

#define DEBUG_BUFFER_SIZE 255

#ifdef _DEBUG
	#include <crtdbg.h>
#endif

#define SYNC_START EnterCriticalSection(&criticalSection)
#define SYNC_END LeaveCriticalSection(&criticalSection)

namespace WinampOpenALOut
{

	Output_Renderer::Output_Renderer(
		unsigned int buffer_len,
		unsigned char a_channel,
		Output_Effects* the_effects)
	{
		// make sure all the pointers are set to zero
		c_bufferLength = buffer_len;
		effects = the_effects;
		this->channel = a_channel;
		played = 0;
		uiSource = 0;
		lastPause = 0;
	}

	Output_Renderer::~Output_Renderer()
	{
		//ensure everything in memory is deleted
	}

	void Output_Renderer::fmemcpy(char* dest, int destPos, char* src, int srcPos, int size)
	{
		// get a pointer to the memory data with an offset
		void* ptr = (void*)&dest[destPos];
		void* ptrSrc = (void*)&src[srcPos];

		// now copy from this position onwards
		memcpy_s(ptr, MAXIMUM_BUFFER_SIZE, ptrSrc, size);
	}

	void Output_Renderer::onError()
	{
		this->Close();
	}

	void Output_Renderer::CheckProcessedBuffers() 
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
				buffer_free += uiBuffers[selectedBuffer].size;
				played += uiBuffers[selectedBuffer].size;
				uiBuffers[selectedBuffer].size = 0;

			} else { 
				MessageBoxA(NULL, "Error in Monitor Thread - Out of Range", "Error Monitoring", MB_OK);
				this->onError();
			}
		}
	}

	void Output_Renderer::CheckPlayState() {

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

			if(streamOpen && 
				(iState == AL_STOPPED))
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

	void Output_Renderer::log_debug_msg(char* msg, char* file, int line)
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
		open

		this procedure is invoked by winamp when it opens a file
		we store the sample rate, numchannels, bitsPerSample

		the open al buffers are created and an open al source
	*/
	int Output_Renderer::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms) {
		
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

		// determine the format to output in
		switch(numberOfChannels)
		{
			case 1:
				ulFormat = bitsPerSample == 8 ? alGetEnumValue("AL_FORMAT_MONO8") : alGetEnumValue("AL_FORMAT_MONO16");
				break;
			case 2:
				ulFormat = bitsPerSample == 8 ? alGetEnumValue("AL_FORMAT_STEREO8") : alGetEnumValue("AL_FORMAT_STEREO16");
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
		lastPause = 0;

		// determine the size of the buffer
		bytesPerSampleChannel = ((bitsPerSample >> SHIFT_BITS_TO_BYTES)*numberOfChannels);

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

#ifdef _DEBUG
		sprintf_s(
			dbg,
			DEBUG_BUFFER_SIZE,
			"-> Using {%d} buffers for total size of {%d}", 
			noBuffers,
			bufferSize);
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
			if ( xram_enabled == true &&
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

#ifdef _DEBUG
		if ( xram_enabled == true )
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
		
		/* Effects */
		if ( effects != NULL )
		{
			effects->add_source(uiSource);
		}

		// set the volume for the source
		this->SetVolumeInternal(volume);

		// we're not playing yet because we're prebuffering
		isPlaying = false;
		// the stream is open and ready for the main thread
		streamOpen = true;

		SYNC_END;

		return c_bufferLength;
	}

	/*
		close

		this procedure is invoked by winamp when the file is closed
	*/
	void Output_Renderer::Close() 
	{
		SYNC_START;
		streamOpen = false;

		if ( effects != NULL )
		{
			effects->on_close();
		}

		// stop the source
		alSourceStop(uiSource);

		played = 0;

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
		SYNC_END;

	}

	/*
		write

		this procedure is invoked by winamp when it attempts to write
		data to the plugin
	*/
	int Output_Renderer::Write(char *buf, int len)
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

#ifdef _DEBUG
			char dbg[DEBUG_BUFFER_SIZE] = {'\0'};

			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing data {0x%08X} with length {%d}",buf, len);
			log_debug_msg(dbg, __FILE__, __LINE__);
#endif
			
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
#ifdef _DEBUG
			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing using buffer {%d}", selectedBuffer);
			log_debug_msg(dbg, __FILE__, __LINE__);
#endif
			if(selectedBuffer == UNKNOWN_BUFFER)
			{
				SYNC_END;
				return -1;
			}

			uiBuffers[selectedBuffer].data = buf;

			buffer_free -= len;
			uiBuffers[selectedBuffer].size = len;

#ifdef _DEBUG
			sprintf_s(
				dbg,
				DEBUG_BUFFER_SIZE,
				"Writing to OpenAL buffer,{%d} bytes", len);
			log_debug_msg(dbg, __FILE__, __LINE__);
#endif
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
		}

		SYNC_END;

		return 0;
	}

	/*
		canwrite

		this procedure is invoked by winamp when it determining
		if more data can be written to the plugin
	*/
	int Output_Renderer::CanWrite() {
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
	int Output_Renderer::IsPlaying() {
		SYNC_START;

		this->CheckProcessedBuffers();

		int r = isPlaying && streamOpen ? IS_PLAYING : IS_NOT_PLAYING;
		SYNC_END;
		return r;
	}

	/*
		pause

		returns the previous pause state
	*/
	int Output_Renderer::Pause(int pause) {
		SYNC_START;
		lastPause = pause;
		
		// clear the error state
		alGetError();

		// if this is un-paused during pre-buffering
		// we may start to play data before it's ready
		// and cause and under-run
		if(streamOpen)
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
		set volume (internal) - used to check the volume range
		and store the volume for later use
	*/
	void Output_Renderer::SetVolumeInternal(ALfloat newVolume)
	{
		if(newVolume <= VOLUME_MAX && newVolume >= VOLUME_MIN)
		{
			volume = newVolume;
			alSourcef(uiSource,AL_GAIN, newVolume);
		}
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Renderer::Flush(int tMs)
	{
		SYNC_START;

		// make sure we've stopped playing
		alSourceStop(uiSource);

		SYNC_END;
	}

	void Output_Renderer::SetXRAMEnabled( bool enabled )
	{
		/* Windows Vista or higher doesnt support XRAM yet - it seems */
		if ( WINVER > 0x0600 )
		{
			xram_enabled = false;
		}
		else
		{
			xram_enabled = enabled;
		}
	}

	Output_Effects* Output_Renderer::get_effects()
	{
		return this->effects;
	}

	void Output_Renderer::SetMatrix ( speaker_T speaker )
	{
		ALfloat x = ((float)speaker.x) / 255.0f;
		ALfloat y = ((float)speaker.y) / 255.0f;
		ALfloat z = ((float)speaker.z) / 255.0f;
		alSource3f(
			this->uiSource,
			AL_POSITION,
			x,
			y,
			z);
	}

}