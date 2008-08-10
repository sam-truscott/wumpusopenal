#include "Out_Wumpus.h"
#include "ConfigStatusForm.h"

namespace WinampOpenALOut {

	delegate void UpdateData();

	void Config::ThreadProcedure() {
		// only execute this thread while the form is visible to save cpu
		while(ptrForm->Visible && !overRide) {

			// get all the dynamic values
			writtenTime			= ptrOw->GetLastWrittenTime();
			playedTime			= ptrOw->GetLastOutputTime();
			numberOfBuffers		= ptrOw->GetNumberOfBuffers();
			numberOfChannels	= ptrOw->GetNumberOfChannels();
			bitsPerSample		= ptrOw->GetBitsPerSample();
			sampleRate			= ptrOw->GetSampleRate();

			// if buffers exist collect size info
			if(ptrOw->GetNumberOfBuffers()!=0) {
				bufferSize = ptrOw->GetBufferSize(currentBuffer);
				if(currentBuffer == ptrOw->GetNumberOfBuffers() - 1) {
					currentBuffer = 0;
				}else{
					currentBuffer++;
				}
			// otherwise don't
			}else{
				currentBuffer = 0;
				bufferSize = 0;
			}

			/* make the gui thread update */
			try {
				UpdateData^ update = gcnew UpdateData(ptrForm, &Update);
				if(!ptrForm->Disposing && ptrForm->Visible && ptrForm->WindowState == FormWindowState::Normal) {
					ptrForm->Invoke(update);
				}
				delete update;
			}catch(...) {
				overRide = true;
				break;
			}

			// sleep for a nominal time
			Sleep(10);
		}
	}

	void Config::ShowDeviceDetails() {
		// clear down other items already in the list
		listBoxExtensions->Items->Clear();

		listBoxExtensions->Items->Add( "Device: " + comboBoxDevices->Items[currentDevice]->ToString());

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATION_EXT");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATION_EXT");
		}

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATE_ALL_EXT");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATE_ALL_EXT");
		}

		// get the device
		ALCdevice* pDevice = alcGetContextsDevice(alcGetCurrentContext());
		
		if (Framework::getInstance()->ALFWIsXRAMSupported() == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX-RAM");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX-RAM");
		}

		if (alcIsExtensionPresent(pDevice, "ALC_EXT_EFX")) {
			listBoxExtensions->Items->Add("Present: ALC_EXT_EFX");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_EXT_EFX");
		}

		ptrOw->SetEffectsSupported(Framework::getInstance()->ALFWIsEFXSupported() == AL_TRUE ? true : false);

		bool efxSupported = ptrOw->GetEffectsSupported();
		if(efxSupported) {
			checkBoxEffectsEnabled->Checked = ptrOw->GetEffectsEnabled();
			checkBoxEffectsEnabled->Enabled = true;
		}else{
			checkBoxEffectsEnabled->Checked = false;
			checkBoxEffectsEnabled->Enabled = false;
		}
	}

	void Config::Update() {
		try {
			/* update all the text for times and sizes */
			lblBitsPerSecond->Text = gcnew System::String("" + bitsPerSample);
			lblNumberOfChannels->Text = gcnew System::String("" + numberOfChannels);
			lblPlayedTime->Text = gcnew System::String("" + playedTime);
			lblWrittenTime->Text = gcnew System::String("" + writtenTime);
			lblSampleRate->Text = gcnew System::String("" + sampleRate);

			unsigned int cbuff = ptrOw->GetPlayingBuffer();
			
			// update the buffer status[es]
			int signedNumberOfBuffers = numberOfBuffers;

			while ( listBoxBufferSizes->Items->Count < signedNumberOfBuffers) {
				listBoxBufferSizes->Items->Add("Buffer " + listBoxBufferSizes->Items->Count + 1);
			}

			while( listBoxBufferSizes->Items->Count > signedNumberOfBuffers) {
				listBoxBufferSizes->Items->RemoveAt(listBoxBufferSizes->Items->Count-1);
			}

			if(numberOfBuffers!=0) {
				if(currentBuffer == cbuff) {
					listBoxBufferSizes->Items[currentBuffer] = "Buffer " + (currentBuffer) + " = " + bufferSize + " >> PLAYING";
				}else{
					listBoxBufferSizes->Items[currentBuffer] = "Buffer " + (currentBuffer) + " = " + bufferSize;
				}
			}

			ALfloat lx, ly, lz, sx, sy, sz = 0.0;

			alGetListener3f(AL_POSITION, &lx, &ly, &lz);
			alGetSource3f(ptrOw->GetSource(), AL_POSITION, &sx, &sy, &sz);

			label3->Text = "lx: " + lx + "\nly: " + ly + "\nlz: " + lz + "\nsx: " + sx + "\nsy: " + sy + "\nsz: " + sz;
		}catch(...) {
			overRide = true;
		}
	}
}