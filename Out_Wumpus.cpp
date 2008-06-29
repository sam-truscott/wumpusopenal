#include "Out_Wumpus.h"
#include "Version.h"

#ifndef SSE_BUILD
#include "ConfigStatusForm.h"
#include "Out_Effects.h"
#else
#include "ConfigFile.h"
#endif

#ifdef _DEBUG
	#include <windows.h>
	#include <crtdbg.h>
#endif

#define SYNC_START EnterCriticalSection(&criticalSection)
#define SYNC_END LeaveCriticalSection(&criticalSection)

#ifndef SSE_BUILD
namespace WinampOpenALOut {
#endif

	Output_Wumpus::Output_Wumpus() {

		// make sure all the pointers are set to zero
		lastOutputTime = ZERO_TIME;
		lastWrittenTime = ZERO_TIME;
		total_written = ZERO_TIME;
		total_played = ZERO_TIME;
		currentOutputTime = ZERO_TIME;
		currentWrittenTime = ZERO_TIME;

#ifdef _DEBUG
		fopen_s(&file, "out_openal.txt", "w+");
#endif

		// create an instance of the effects module
#ifndef SSE_BUILD
		effectsModule = new EffectsModule(this);
#endif

	}

	Output_Wumpus::~Output_Wumpus() {
#ifdef _DEBUG
		fclose(file);
#endif

		//ensure everything in memory is deleted
#ifndef SSE_BUILD
		delete effectsModule;
		effectsModule = NULL;
#endif
	}

	/*
		_DllMainCRTStartup

		This is called when the DLL is loaded
	*/
	BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
	{
		if (ul_reason_for_call == DLL_PROCESS_ATTACH) {

			// optimisation where threads are no longer attached/detached - I think
			DisableThreadLibraryCalls((HMODULE)hInst);
		}
		return TRUE;
	}

	inline void Output_Wumpus::fmemcpy(char* dest, int destPos, char* src, int size) {
		
		// get a pointer to the memory data with an offset
		void* ptr = (void*)&dest[destPos];

		// now copy from this position onwards
		memcpy_s(ptr, MAXIMUM_BUFFER_SIZE, src, size);
	}

	inline void Output_Wumpus::fmemcpy(char* dest, int destPos, char* src, int srcPos, int size) {
		
		// get a pointer to the memory data with an offset
		void* ptr = (void*)&dest[destPos];
		void* ptrSrc = (void*)&src[srcPos];

		// now copy from this position onwards
		memcpy_s(ptr, MAXIMUM_BUFFER_SIZE, ptrSrc, size);
	}

	int Output_Wumpus::get_current_output_time(__int64 currentOutputTime, unsigned int sampleRate)//this ignores high 32bits of total_written
	{
		return MulDiv( (int)(currentOutputTime & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sampleRate);
	}

	int Output_Wumpus::get_current_written_time(__int64 currentWrittenTime, unsigned int sampleRate)//this ignores high 32bits of total_written
	{
		return MulDiv( (int)(currentWrittenTime & THIRTY_TWO_BIT_BIT_MASK),ONE_SECOND_IN_MS,sampleRate);
	}

	void Output_Wumpus::onError() {
		this->Close();
	}

	void Output_Wumpus::SwitchOutputDevice(int device) {

		// grab some temporary data for where we are now
		// and the format of the data
		int currentPosition = GetOutputTime();
		unsigned int tempSampleRate = sampleRate;
		unsigned int tempBitsPerSample = bitsPerSample;
		unsigned int tempNumberOfChannels = numberOfChannels;

		// shut down the thread and wait for it to shutdown
		bool tempStreamOpen = streamOpen;
		this->Close();

		SYNC_START;
		
		// make sure the source is stopped
		alSourceStop(uiSource);

		// detach all the buffers from the source
		alSourcei(uiSource, AL_BUFFER, NO_BUFFERS);

		// delete all the buffers
		alDeleteBuffers( noBuffers, uiBuffers );

#ifndef SSE_BUILD
		effectsModule->SourceRemoved();
#endif

		// shutdown openal
		Framework::getInstance()->ALFWShutdownOpenAL();
		// re-initialise openal
		Framework::getInstance()->ALFWInitOpenAL(device);

		// make all the buffers available
		for(unsigned int i=0;i<MAX_NO_BUFFERS;i++) {
			avalBuffers[i] = true;
			bufferSizes[i] = EMPTY_THE_BUFFER;
		}
		
		if(tempStreamOpen) {
			// re-initialise everything
			this->Open(tempSampleRate, tempNumberOfChannels, tempBitsPerSample, 0,0);
			currentOutputTime = currentPosition;
			// reset back to the current position
			this->Flush(currentPosition);
			// reset the volume
			alSourcef(uiSource,AL_GAIN, volume);
		}
		SYNC_END;
	}

	void Output_Wumpus::CheckProcessedBuffers() {

		ALint			iBuffersProcessed;
		ALuint			uiNextBuffer;
#ifdef _DEBUG
		ALenum			err;
		bool			error;
#endif

		unsigned int selectedBuffer;

		iBuffersProcessed = NO_BUFFERS_PROCESSED;

		// ask open al how many buffers have been processed
		alGetError();
		alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
#ifdef _DEBUG
		err = alGetError();
		if(err != AL_NO_ERROR) {
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
#endif

		while (iBuffersProcessed) {
			uiNextBuffer = DEFAULT_BUFFER;
			
			// clear any open al errors
			alGetError();

			/* unqueue *any* processed buffer. the buffer that has been
				processed will be put into uiNextBuffer */
			alSourceUnqueueBuffers(uiSource, GET_ONE_BUFFER, &uiNextBuffer); 

#ifdef _DEBUG
			//check that no errors have occured
			err = alGetError();
			error = false;
			if(err != AL_NO_ERROR) { error = true;	}
#endif

			iBuffersProcessed--;

			// based on the bufferID determine the buffer index
			selectedBuffer = UNKNOWN_BUFFER;
			for(unsigned int i=0;i<noBuffers;i++) {
				if( uiBuffers[i] == uiNextBuffer ) {
					selectedBuffer = i;
					break;
				}
			}

#ifdef _DEBUG
			// check that the bufferID and index are in correct range
			if(uiNextBuffer == DEFAULT_BUFFER) {	error = true;	}
			if(selectedBuffer < DEFAULT_BUFFER || selectedBuffer > noBuffers) {	error = true; }

			/* if no errors have occured we can mark that
				buffer as available again */
			if(!error) {
#endif
				/* increase our played time position
				 (the delta for where we are in the current buffer is done
				 in get_output_time */
				total_played += bufferSizes[selectedBuffer];
#ifdef _DEBUG
				fprintf(file, "\tIndex->%d, OpenAL ID->%d\n", selectedBuffer, uiBuffers[selectedBuffer]);
#endif

				avalBuffers[selectedBuffer] = true;
				bufferSizes[selectedBuffer] = EMPTY_THE_BUFFER;
#ifdef _DEBUG
			} else { 
				MessageBoxA(NULL, "Error in Monitor Thread - Out of Range", "Error Monitoring", MB_OK);
				onError();
			}
#endif
		}
	}

	void Output_Wumpus::CheckPlayState() {

		ALint			iState;
		ALint			iQueuedBuffers;
		ALenum			err;

		/*
			can the current state of the source
		*/
		/* AL_SOURCE_STATE, AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED */
		alGetError();
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);

		if (iState == AL_PLAYING) {
			// if the source is playing then we record that
			isPlaying = true;
		}else{

			if(!preBuffer
				&& streamOpen
				&& (iState == AL_INITIAL || iState == AL_STOPPED)
				&& total_written == total_played) {

				isPlaying = false;

				return;
			}

			// if we're not playing check to see if any buffers are queued
			alGetError();
			alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
#ifdef _DEBUG
			err = alGetError();
			if(err != AL_NO_ERROR) {
				MessageBoxA(NULL, "Error playing", "Error Playing", MB_OK);
				onError();
			}
#endif

			if (iQueuedBuffers) {

#ifdef _DEBUG
				fprintf(file, "CheckPlayState - %d buffers queued, playing...\n", iQueuedBuffers);
#endif

				// if any buffers are queued and we're not playing - play!
				if(!lastPause) {
					alSourcePlay(uiSource);
					isPlaying = true;
				}
			} else {

#ifdef _DEBUG
				fprintf(file, "CheckPlayState - stopped, no more buffers to play\n");
#endif

				// otherwise we're finished
				isPlaying = false;
			}
		}
	}

	void Output_Wumpus::CheckAvailableBuffers() {
		// have a look at the openal buffers and see if
		// any of them are free
		canWrite = false;
		for(unsigned int i=0;i<noBuffers;i++) {
			if(avalBuffers[i]) {
				canWrite = true;
			}
		}
	}

	/*
		config

		this procedure is invoked by winamp when the configure button is clicked
	*/
	void Output_Wumpus::Config(HWND hwnd) {

#ifndef SSE_BUILD
		WinampOpenALOut::Config^ config = WinampOpenALOut::Config::GetInstance(this);
		if(!config->Visible) {
			Application::Run(config);
		}
#else
		MessageBoxA(hwnd, "Wumpus OpenAL Output Plugin (SSE2 version) has no configuration", "Configure", MB_OK);
#endif
	}

	/*
		about

		this procedure is invoked by winamp when the about button is clicked
	*/
	void Output_Wumpus::About(HWND hwnd) {
		MessageBoxA(hwnd,"Wumpus OpenAL Output Plug-in " PI_VER "\nCompiled on " __DATE__ " - " __TIME__"\n(c) 2008 Sam Truscott\nThanks to:\n\tTinuva\n","About",MB_OK);
	}

	/*
		init

		this procedure is called when the plugin is initialised
	*/
	void Output_Wumpus::Initialise(HWND window) {

		InitializeCriticalSection(&criticalSection);

		SYNC_START;

		Clock::Initialise();

		/*
			set up the variables to a default state
		*/
		streamOpen = false;
		canWrite = false;
		isPlaying = false;

		tmpBuffer = EMPTY_THE_BUFFER;
		tmpBufferSize = EMPTY_THE_BUFFER;

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

		// TODO used for 3D Position and Panning
		//posX = 1.0;
		//posY = 1.0;
		//posZ = 1.0;

		for(int i=0;i<MAX_NO_BUFFERS;i++) {
			bufferSizes[i] = EMPTY_THE_BUFFER;
		}

		// load the config up
		ConfigFile::Initialise(window);

		c_bufferLength = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);
		if(c_bufferLength == ERROR_BUFFER
			|| c_bufferLength > CONF_BUFFER_LENGTH_MAX
			|| c_bufferLength < CONF_BUFFER_LENGTH_MIN) {

			c_bufferLength = DEFC_BUFFER_LENGTH;
			ConfigFile::WriteInteger(CONF_BUFFER_LENGTH, c_bufferLength);
		}

		// read in the current device to use
		int currentDevice = ConfigFile::ReadInteger(CONF_DEVICE);
		if(currentDevice == -1) {
			currentDevice = DEFC_DEVICE;
			ConfigFile::WriteInteger(CONF_DEVICE, currentDevice);
		}

		/*
			initialise open al itself - this has been modified
			and will also select the default sound card
		*/
		if (!Framework::getInstance()->ALFWInitOpenAL(currentDevice)) {
			MessageBoxA(NULL, "Could not initialise OpenAL", "Error", MB_OK);
		}

#ifdef _DEBUG
		fprintf(file, "Initialise!\n\tBuffer Length: %d\n\tCurrent Device: %d\n", c_bufferLength, currentDevice);
#endif

		// find out if effects are supported
		this->effectsSupported = Framework::getInstance()->ALFWIsEFXSupported() == AL_TRUE ? true : false;
		// find out if effects are enabled
		this->effectsEnabled = ConfigFile::ReadBoolean(CONF_EFX);

		// if effects are enabled, but not supported, disable them
		if(!effectsSupported) {
			effectsEnabled = false;
			ConfigFile::WriteBoolean(CONF_EFX, false);
		}

		this->monoExpand = ConfigFile::ReadBoolean(CONF_MONO_EXPAND);
		this->stereoExpand = ConfigFile::ReadBoolean(CONF_STEREO_EXPAND);

		SYNC_END;

	}

	unsigned int Output_Wumpus::GetPlayingBuffer() {
		ALint tbuff = UNKNOWN_BUFFER;
		alGetSourcei(uiSource, AL_BUFFER, &tbuff);
		
		unsigned int cbuff = UNKNOWN_BUFFER;
		for(unsigned int i=0;i<noBuffers;i++) {
			if( tbuff == (ALint)uiBuffers[i] ) {
				cbuff = i;
				break;
			}
		}
		return cbuff;
	}

	/*
		quit

		this procedure is invoked by winamp when it tells the plugin to quit
		this is usually done when winamp is closed
	*/
	void Output_Wumpus::Quit() {

		// if a steam is open, close it
		if(streamOpen) {
			Close();
			streamOpen = false;
		}

		SYNC_START;

#ifndef SSE_BUILD
		delete effectsModule;
		effectsModule = 0;
#endif

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

		if(bitspersamp > SIXTEEN_BIT_PER_SAMPLE) {
			canWrite = false;
			MessageBoxA(NULL, "This Plug-In only supports 8 and 16bit audio, please disable 24bit audio in Winamp", "Whoops", MB_OK);
			SYNC_END;
			return -1;
		}
		
		//record the format of the data we're getting
		sampleRate = samplerate;
		numberOfChannels = numchannels;
		bitsPerSample = bitspersamp;

		bool tmpStereoExpand = stereoExpand;
		bool tmpMonoExpand = monoExpand;

		stereoExpand = false;
		monoExpand = false;

		// determine the format to output in
		switch(numberOfChannels) {
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

		// determine the size of the buffer
		bytesPerSampleChannel = ((bitsPerSample >> SHIFT_BITS_TO_BYTES)*numberOfChannels);

		if (stereoExpand) { bytesPerSampleChannel *= 2; }
		if (monoExpand) { bytesPerSampleChannel *=4;}

		unsigned int bufferSize = (bytesPerSampleChannel * (sampleRate / ONE_SECOND_IN_MS)) * c_bufferLength;

		//noBuffers = MAX_NO_BUFFERS;
		noBuffers = (bufferSize / MAXIMUM_BUFFER_SIZE);// + MAXIMUM_BUFFER_OFFSET;

		tmpBuffer = new char[MAXIMUM_BUFFER_SIZE];
		tmpBufferSize = EMPTY_THE_BUFFER;

		// if we have no buffers just assume we can use at least (MINIMUM_BUFFERS)
		// we need more than one so we can listen to one and write t'other
		// if that doesnt happen it will error anyway
		if(noBuffers < MINIMUM_BUFFERS) {
			noBuffers = MINIMUM_BUFFERS;
		}

#ifdef _DEBUG
		fprintf(file, "Open\n");
		fprintf(file, "\tSample Rate: %d\n\tChannels: %d\n\tBits Per Sample: %d\n", sampleRate, numberOfChannels, bitspersamp);
		fprintf(file, "Using:\n");
		fprintf(file, "\tTotal Buffer Size: %d bytes\n\tSingle Buffer Size: %d\n\tNumber of Buffers: %d\n", bufferSize, MAXIMUM_BUFFER_SIZE, noBuffers);
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

		// allocate some buffers
		alGetError();
		alGenBuffers( noBuffers, uiBuffers );
		ALenum err = alGetError();
		if( err == AL_INVALID_VALUE || err == AL_OUT_OF_MEMORY) {

#ifdef _DEBUG
			if( err != AL_NO_ERROR) {
				
				if(err == AL_OUT_OF_MEMORY) {
					MessageBoxA(NULL, "Error generating Buffer - Out of Memory", "Error", MB_OK);
				} else if(err == AL_INVALID_VALUE) {
					MessageBoxA(NULL, "Error generating  Buffer - Invalid Value", "Error", MB_OK);
				} else {
					MessageBoxA(NULL, "Error generating  Buffer - Unknown Error", "Error", MB_OK);
				}
				onError();
			}
#endif
			
			while(err != AL_NO_ERROR && noBuffers > 1) {
				alGetError();
				noBuffers--;
				alGenBuffers( noBuffers, uiBuffers );
				err = alGetError();
			}
		}

		// allocate a source
		alGetError();
		alGenSources( 1, &uiSource );
		err = alGetError();
#ifdef _DEBUG
		if( err != AL_NO_ERROR) {
			
			if(err == AL_OUT_OF_MEMORY) {
				MessageBoxA(NULL, "Error generating Source - Out of Memory", "Error", MB_OK);
			} else if(err == AL_INVALID_VALUE) {
				MessageBoxA(NULL, "Error generating Source - Invalid Value", "Error", MB_OK);
			} else if(err == AL_INVALID_OPERATION) {
				MessageBoxA(NULL, "Error generating Source - Invalid Operation", "Error", MB_OK);
			}  else {
				MessageBoxA(NULL, "Error generating Source - Unknown Error", "Error", MB_OK);
			}
			onError();
		}
#endif

		alSourcei(uiSource, AL_LOOPING, AL_FALSE);

		/* enable effects (this returns instantly if
							effects are not supported
							or enabled */
#ifndef SSE_BUILD
		effectsModule->SourceDetermined(uiSource);
#endif

		// set the volume for the source
		alSourcef(uiSource,AL_GAIN, volume);

		// we can write to the buffers now(pre-buffer)
		canWrite = true;
		// we're not playing yet because we're prebuffering
		isPlaying = false;
		// the stream is open and ready for the main thread
		streamOpen = true;
		// start prebuffering
		preBuffer = true;

		// make all buffers available to write data into
		for(unsigned int i=0;i<noBuffers;i++) {
			avalBuffers[i] = true;
			bufferSizes[i] = EMPTY_THE_BUFFER;
		}

		SYNC_END;
		return c_bufferLength;
	}

	/*
		close

		this procedure is invoked by winamp when the file is closed
	*/
	void Output_Wumpus::Close() {

#ifdef _DEBUG
		fprintf(file, "Close\n");
#endif

		SYNC_START;
		streamOpen = false;

		delete tmpBuffer;
		tmpBuffer = EMPTY_THE_BUFFER;
		tmpBufferSize = EMPTY_THE_BUFFER;

		// mark the stream as closed
		canWrite = false;

		// stop the source
		alSourceStop(uiSource);

		// remove all buffers
		alSourcei(uiSource, AL_BUFFER, 0);

		// delete the source
		alDeleteSources( 1, &uiSource );
		// delete all the buffers
		alDeleteBuffers( noBuffers, uiBuffers );

#ifndef SSE_BUILD
		effectsModule->SourceRemoved();
#endif

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
	int Output_Wumpus::Write(char *buf, int len) {

		SYNC_START;

		ALenum err;

		// if we cannot write exit now (non-blocking op)
		if(!canWrite || !streamOpen) {
			SYNC_END;
#ifdef _DEBUG
			fprintf(file, "\tWrite - Cannot write, locked out\n");
#endif
			return -1;
		}

		bool dataToBuffer = false;

		if(tmpBufferSize + len == MAXIMUM_BUFFER_SIZE) {

			fmemcpy(tmpBuffer, tmpBufferSize, buf, len);
			tmpBufferSize += len;
			
		}else if(tmpBufferSize + len > MAXIMUM_BUFFER_SIZE) {

			dataToBuffer = true;

		}else {

			fmemcpy(tmpBuffer, tmpBufferSize, buf, len);
			tmpBufferSize += len;

			SYNC_END;

			return 0;
		}

		// if the buffer is valid (non-NULL)
		if (buf) {

			total_written += tmpBufferSize;
			
			// set it to ERROR'd so we can detect errors
			ALuint uiNextBuffer = UNKNOWN_BUFFER;
			unsigned int selectedBuffer = UNKNOWN_BUFFER;

			/*	go through the buffers and find an available one

				if we're here (critical section, canWrite=true) then
				we assume that one is! */
			for(ALuint i=0;i<noBuffers;i++) {
				if(avalBuffers[i]) {
						// find an available buffer
						uiNextBuffer = uiBuffers[i];
						selectedBuffer = i;
						avalBuffers[i] = false;
						break;
				}
			}
			if(selectedBuffer == UNKNOWN_BUFFER){
#ifdef _DEBUG
				fprintf(file, "\t### CANNOT FIND A FREE BUFFER ###\n");
#endif
				SYNC_END;
				return -1;
			}

			// ############## MONO EXPANSION

			if(monoExpand) {
				// we're writing out four as much data so
				// increase this value by three more times
				total_written += (tmpBufferSize*3);

				// expand buffer here
				char* newBuffer = new char[MAXIMUM_BUFFER_SIZE * 4];

				unsigned int nPos = 0;
				
				/* expand the samples out */
				for(unsigned int pos=0; pos < tmpBufferSize; pos++) {
					newBuffer[nPos++] = tmpBuffer[pos];
					newBuffer[nPos++] = tmpBuffer[pos];
					newBuffer[nPos++] = tmpBuffer[pos];
					newBuffer[nPos++] = tmpBuffer[pos];
				}

#ifdef _DEBUG
				if(nPos != tmpBufferSize * 2) {
					fprintf(file, "\tWriting abnormal amount for stereo expansion\n");
				}
#endif

				// delete old buffer
				delete tmpBuffer;
				// switch em over
				tmpBuffer = newBuffer;
				// increase the size by four
				tmpBufferSize *= 4;
			}

			// ############## END MONO EXPANSION

			// ############## STEREO EXPANSION

			if(stereoExpand) {

				// we're writing out twice as much data so
				// increase this value again
				total_written += tmpBufferSize;

				// expand buffer here
				char* newBuffer = new char[MAXIMUM_BUFFER_SIZE * 2];

				unsigned int nPos = 0;
				unsigned char sampleSize = bitsPerSample == 8 ? TWO_BYTE_SAMPLE : FOUR_BYTE_SAMPLE;
				
				/* expand the samples out */
				for(unsigned int pos=0; pos < tmpBufferSize;) {
					// copy the front left and right
					fmemcpy(/*dst	*/	newBuffer,
							/*dstPos*/	nPos,
							/*src	*/	tmpBuffer, 
							/*srcPos*/	pos, 
							/*size	*/	sampleSize);
					nPos+=sampleSize;

					// copy the rear left and right
					fmemcpy(/*dst	*/	newBuffer,
							/*dstPos*/	nPos,
							/*src	*/	tmpBuffer,
							/*srcPos*/	pos,
							/*size	*/	sampleSize);
					nPos+=sampleSize;
					pos+=sampleSize;
				}

#ifdef _DEBUG
				if(nPos != tmpBufferSize * 2) {
					fprintf(file, "\tWriting abnormal amount for stereo expansion\n");
				}
#endif

				// delete old buffer
				delete tmpBuffer;
				// switch em over
				tmpBuffer = newBuffer;
				// increase the size by two
				tmpBufferSize *= 2;
			}

			// ############## END STEREO EXPANSION

#ifdef _DEBUG
			fprintf(file, "\tWriting to OpenALBuffer\n");
			fprintf(file, "\t\tBuffer: Index->%d, OpenAL ID->%d\n", selectedBuffer, uiBuffers[selectedBuffer]);
			fprintf(file, "\t\tSize: %d bytes\n", len);
#endif

			bufferSizes[selectedBuffer] = tmpBufferSize;

			// buffer the data with the correct format
			alGetError();
			alBufferData(uiNextBuffer, ulFormat, tmpBuffer, tmpBufferSize, sampleRate);
#ifdef _DEBUG
			err = alGetError();
			if( err != AL_NO_ERROR) {
				
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
				onError();
			}
#endif

			// add the buffer to the source queue
			alGetError();
			alSourceQueueBuffers(uiSource, 1, &uiNextBuffer);
#ifdef _DEBUG
			err = alGetError();
			if( err != AL_NO_ERROR) {
				
				if(err == AL_INVALID_NAME) {
					MessageBoxA(NULL, "Error Queuing - Invalid Name", "Error", MB_OK);
				} else if(err == AL_INVALID_OPERATION) {
					MessageBoxA(NULL, "Error Queuing - Invalid Operation", "Error", MB_OK);
				} else {
					MessageBoxA(NULL, "Error Queuing - Unknown Error", "Error", MB_OK);
				}
				onError();
			}
#endif

			/* now that there is data in the buffers check the play
			state. if nothing is playing then either a buffer under-run
			has occured or this is the first time the file has been written.
			it could also be a small file that the monitor thread might not see
			*/
			if(!preBuffer) {
				CheckPlayState();
			}

			if(preBuffer) {
				preBuffer = false;
			}
		}

		tmpBufferSize = EMPTY_THE_BUFFER;
		if(dataToBuffer) {
			fmemcpy(tmpBuffer, tmpBufferSize, buf, len);
			tmpBufferSize += len;
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

			CheckProcessedBuffers();
			CheckAvailableBuffers();

			// determine how many openal buffers we can use
			unsigned int size = 0;
			for(unsigned int i=0;i<noBuffers;i++) {
				if(avalBuffers[i]) {
					size += MAXIMUM_BUFFER_SIZE;
				}
			}

			r = canWrite ? size : EMPTY_THE_BUFFER;
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

		CheckProcessedBuffers();
		CheckAvailableBuffers();
		CheckPlayState();

		int r = isPlaying && streamOpen ? 1 : 0;
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
		
		if(streamOpen) {
			if(pause) {
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
	void Output_Wumpus::SetVolume(int aVolume) {
		/*
			work out the % volume (as a float) and set it
		*/
		SYNC_START;

		// calculate the volume to use (0.0 to 1.0)
		volume = (ALfloat)aVolume / (ALfloat)255;
		alGetError();
		alSourcef(uiSource,AL_GAIN, volume);
		
		SYNC_END;
	}

	/*
		setpan - INCOMPLETE

		this procedure is invoked by winamp to set the pan
	*/
	void Output_Wumpus::SetPan(int pan) {
	}

	/*
		flush

		this procedure is invoked by winamp to flush the buffers
		and start playing from time (t)ms.
	*/
	void Output_Wumpus::Flush(int t) {
		SYNC_START;

		// make sure we've stopped playing
		alSourceStop(uiSource);

		// remove all buffers
		alSourcei(uiSource, AL_BUFFER, 0);

		// delete all the buffers
		alDeleteBuffers( noBuffers, uiBuffers );

#ifndef SSE_BUILD
		effectsModule->SourceRemoved();
#endif

		// re-generate buffers
		alGenBuffers( noBuffers, uiBuffers );

		// calculate the number of bytes that will have been
		// processed after (t)ms
		int calcTime = ((sampleRate / ONE_SECOND_IN_MS) * (bitsPerSample >> SHIFT_BITS_TO_BYTES) * numberOfChannels) * t;

		// reset played pointers
		total_written = calcTime;
		total_played = calcTime;
		lastOutputTime = t;
		lastWrittenTime = t;
		currentOutputTime = t;
		currentWrittenTime = t;

		// make all the buffers available
		for(unsigned int i=0;i<noBuffers;i++) {
			avalBuffers[i] = true;
			bufferSizes[i] = EMPTY_THE_BUFFER;
		}

		// allow writing
		canWrite = true;
		// make sure we prebuffer again
		preBuffer = true;

		SYNC_END;
	}

	/*
		getWrittenTime

		returns the time in ms of current position
	*/
	int Output_Wumpus::GetWrittenTime() {

		SYNC_START;
		
		if(streamOpen) {
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
	int Output_Wumpus::GetOutputTime() {
		
		SYNC_START;

		if(streamOpen) {

			CheckAvailableBuffers();

			//alSource3f(uiSource, AL_POSITION, posX,posY,posZ);

			// only check the state of the openal buffers
			// every so often, no need to keep checking them
			// if we know they're x'ms long
			Time_Type currentTime = Clock::GetTime();
			if(currentTime > lastCheckBuffers + lastCheckDelay) {
				CheckProcessedBuffers();
				lastCheckBuffers = currentTime;
			}
			//CheckPlayState();

			ALint deltaBytes;
			alGetError();
			alGetSourcei(uiSource, AL_BYTE_OFFSET, &deltaBytes);
			ALenum err = alGetError();

			// this works it out
			currentOutputTime = total_played + deltaBytes;

/*#ifdef _DEBUG
			char dbg[2048] = {'\0'};
			int delta = deltaBytes;
			sprintf(dbg, "GetOutputTime Total Bytes=%016u, Delta=%08d, Total=%016u\n", (int)total_played, delta, (int)currentOutputTime);
			OutputDebugStringA(dbg);
#endif*/


			// this converts bytes to ms and does 32bit part
			currentOutputTime = get_current_output_time(currentOutputTime, sampleRate) / bytesPerSampleChannel;

		}else{
			currentOutputTime = ZERO_TIME;
		}

		SYNC_END;

		lastOutputTime = (int)(currentOutputTime & THIRTY_TWO_BIT_BIT_MASK);
		return lastOutputTime;
	}

	bool Output_Wumpus::GetEffectsEnabled() {
#ifndef SSE_BUILD
		return effectsModule->GetEffectsEnabled();
#else
		return false;
#endif
	}
	
	bool Output_Wumpus::GetEffectsSupported() {
#ifndef SSE_BUILD
		return effectsModule->GetEffectsSupported();
#else
		return false;
#endif
	}

	void Output_Wumpus::SetEffectsEnabled(bool b) {
		
		SYNC_START;

		bool tempWrite = canWrite;
		canWrite = false;
#ifndef SSE_BUILD
		effectsModule->SetEffectsEnabled(b);
#endif
		canWrite = tempWrite;

		SYNC_END;
	}

	void Output_Wumpus::SetEffectsSupported(bool b) {
		SYNC_START;
#ifndef SSE_BUILD
		effectsModule->SetEffectsSupported(b);
#endif
		SYNC_END;
	}
#ifndef SSE_BUILD
}
#endif