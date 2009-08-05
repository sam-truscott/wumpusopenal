#include "Out_Wumpus.h"
#include "ConfigStatusForm.h"

namespace WinampOpenALOut {

	speaker_matrix_T matrix;

	delegate void UpdateData();

	Config::Config(Output_Wumpus *aPtrOw)
		{
		InitializeComponent();

		ptrOw = aPtrOw;
		ptrForm = this;

		overRide = false;

		currentDevice = ConfigFile::ReadInteger(CONF_DEVICE);

		// get a list of all the devices and add them to the combo
		ALDeviceList *pDeviceList = Framework::getInstance()->ALFWGetDeviceList();
		if ((pDeviceList) && (pDeviceList->GetNumDevices())) {
			for (ALint i = 0; i < pDeviceList->GetNumDevices(); i++) {
				if( i == pDeviceList->GetDefaultDevice()) {
					comboBoxDevices->Items->Add("Default: " + gcnew String(pDeviceList->GetDeviceName(i)));
				}else{
					comboBoxDevices->Items->Add(gcnew String(pDeviceList->GetDeviceName(i)));
				}
			}
		}

		// if the current device is not set, use the default device
		if(currentDevice == -1) {
			comboBoxDevices->SelectedIndex = pDeviceList->GetDefaultDevice();
			currentDevice = pDeviceList->GetDefaultDevice();
		}else{
			comboBoxDevices->SelectedIndex = currentDevice;
		}

		int buffer = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);
		if ( buffer >= 250
			|| buffer <= 4000)
		{
			trackBufferLength->Value = buffer;
		}

		// update their captions with values
		labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";

		checkBoxExpandMono->Checked = ptrOw->IsMonoExpanded();
		checkBoxExpandStereo->Checked = ptrOw->IsStereoExpanded();
		checkBoxXRAM->Checked = ptrOw->IsXRAMEnabled();

		int i;
		for ( i = 0 ; i < NO_OF_EFFECTS ; i++ )
		{
			this->comboBoxEffect->Items->Add( gcnew String( REVERB_NAMES_TABLE[i] ));
		}

		int efx_env = ConfigFile::ReadInteger(CONF_EFX_ENV);

		if ( efx_env < EFX_REVERB_PRESET_GENERIC ||
			 efx_env > EFX_REVERB_PRESET_SMALLWATERROOM )
		{
			efx_env = EFX_REVERB_PRESET_GENERIC;
		}
		this->comboBoxEffect->SelectedIndex = efx_env;

		this->checkBoxEfxEnabled->Checked = ConfigFile::ReadBoolean(CONF_EFX_ENABLED);

		this->checkBoxSplit->Checked = ptrOw->IsSplit();

		speaker_matrix_T matrix = ptrOw->GetMatrix();
		numFLx->Value = (System::Decimal)matrix.speakers[0].x;
		numFLy->Value = (System::Decimal)matrix.speakers[0].y;
		numFLz->Value = (System::Decimal)matrix.speakers[0].z;
		numFRx->Value = (System::Decimal)matrix.speakers[1].x;
		numFRy->Value = (System::Decimal)matrix.speakers[1].y;
		numFRz->Value = (System::Decimal)matrix.speakers[1].z;
		numRLx->Value = (System::Decimal)matrix.speakers[2].x;
		numRLy->Value = (System::Decimal)matrix.speakers[2].y;
		numRLz->Value = (System::Decimal)matrix.speakers[2].z;
		numRRx->Value = (System::Decimal)matrix.speakers[3].x;
		numRRy->Value = (System::Decimal)matrix.speakers[3].y;
		numRRz->Value = (System::Decimal)matrix.speakers[3].z;
		numCx->Value = (System::Decimal)matrix.speakers[4].x;
		numCy->Value = (System::Decimal)matrix.speakers[4].y;
		numCz->Value = (System::Decimal)matrix.speakers[4].z;
		numSx->Value = (System::Decimal)matrix.speakers[5].x;
		numSy->Value = (System::Decimal)matrix.speakers[5].y;
		numSz->Value = (System::Decimal)matrix.speakers[5].z;

		ShowDeviceDetails();
	}

	void Config::Load()
	{
		Show();
		checkBoxExpandMono->Checked = ptrOw->IsMonoExpanded();
		checkBoxExpandStereo->Checked = ptrOw->IsStereoExpanded();
		if(ptrOw->IsXRAMPresent() == true)
		{
			checkBoxXRAM->Enabled = true;
			checkBoxXRAM->Checked = ptrOw->IsXRAMEnabled();
		}
		else
		{
			checkBoxXRAM->Enabled = false;
			checkBoxXRAM->Checked = false;
		}
		this->Visible = true;
		Thread^ thread = gcnew Thread(gcnew ThreadStart(ThreadProcedure));
        thread->Start();
	}

	void Config::ApplyChanges()
	{
		Int32 newDevice = comboBoxDevices->SelectedIndex;
		if(newDevice != currentDevice && newDevice != -1) {
			// if we've changed device switch device and save
			ConfigFile::WriteInteger(CONF_DEVICE, newDevice);
			currentDevice = newDevice;
			ptrOw->SwitchOutputDevice(currentDevice);
		}
		if(ptrOw->GetConfBufferLength() != trackBufferLength->Value) {
			// if we've changed the buffer length stop playback and change it
			ptrOw->SetConfBufferLength(trackBufferLength->Value);
			ConfigFile::WriteInteger(CONF_BUFFER_LENGTH, trackBufferLength->Value);
			ptrOw->SwitchOutputDevice(currentDevice);
		}
		
		// store settings regarding effects
		if(ptrOw->IsMonoExpanded() != checkBoxExpandMono->Checked
			|| ptrOw->IsStereoExpanded() != checkBoxExpandStereo->Checked) {
				
				ptrOw->SetStereoExpanded(checkBoxExpandStereo->Checked);
				ptrOw->SetMonoExpanded(checkBoxExpandMono->Checked);
				ptrOw->SwitchOutputDevice(currentDevice);
		}

		if(ptrOw->IsXRAMEnabled() != checkBoxXRAM->Checked)
		{
			ptrOw->SetXRAMEnabled(checkBoxXRAM->Checked);
		}

		if(ptrOw->get_effects()->get_current_effect() != comboBoxEffect->SelectedIndex)
		{
			ptrOw->get_effects()->set_current_effect((effects_list)comboBoxEffect->SelectedIndex);
			ConfigFile::WriteInteger(CONF_EFX_ENV, comboBoxEffect->SelectedIndex);
		}

		if(ptrOw->get_effects()->is_enabled() != checkBoxEfxEnabled->Checked)
		{
			ptrOw->get_effects()->set_enabled(checkBoxEfxEnabled->Checked);
			ConfigFile::WriteBoolean(CONF_EFX_ENABLED, checkBoxEfxEnabled->Checked);
		}

		if ( ptrOw->IsSplit() != checkBoxSplit->Checked )
		{
			ptrOw->SetSplit( checkBoxSplit->Checked );
		}

		ptrOw->SetMatrix(matrix);

		ConfigFile::WriteBoolean(CONF_MONO_EXPAND, checkBoxExpandMono->Checked);
		ConfigFile::WriteBoolean(CONF_STEREO_EXPAND, checkBoxExpandStereo->Checked);
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED, checkBoxXRAM->Checked);
		
		ShowDeviceDetails();
	}

	void Config::ThreadProcedure() {
		// only execute this thread while the form is visible to save cpu
		while(ptrForm->Visible && !overRide) {

			// get all the dynamic values
			written_ms			= ptrOw->GetLastWrittenTime();
			played_ms			= ptrOw->GetLastOutputTime();
			written_bytes		= ptrOw->GetWrittenBytes();
			played_bytes		= ptrOw->GetPlayedBytes();

			if ( buffer_percent_full > 100 )
			{
				buffer_percent_full = 100;
			}

			/* make the gui thread update */
			try {
				UpdateData^ update = gcnew UpdateData(ptrForm, &DoUpdate);
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

	void Config::DoUpdate() {
		try {
			/* update all the text for times and sizes */
			labelWrittenMs ->Text = gcnew System::String("" + written_ms);
			labelPlayedMs->Text = gcnew System::String("" + played_ms);
			labelWrittenB->Text = gcnew System::String("" + written_bytes);
			labelPlayedB->Text = gcnew System::String("" + played_bytes);
			labelLatency->Text = gcnew System::String("" + (written_ms - played_ms) );
		}catch(...) {
			overRide = true;
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

		if (alcIsExtensionPresent(pDevice, "ALC_EXT_EFX") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: ALC_EXT_EFX");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_EXT_EFX");
		}

		/*if (alcIsExtensionPresent(pDevice, "EAX2.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 2.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 2.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX3.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 3.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 3.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX4.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 4.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 4.0");
		}


		if (alcIsExtensionPresent(pDevice, "EAX5.0") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: EAX 5.0");
		}else{
			listBoxExtensions->Items->Add("Absent: EAX 5.0");
		}

		if (alIsExtensionPresent( "AL_EXT_float32") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: AL_EXT_FLOAT32");
		}else{
			listBoxExtensions->Items->Add("Absent: AL_EXT_FLOAT32");
		}*/

	}

	void Config::UpdateMatrix(char speaker, char xyz, Decimal value)
	{
		switch (xyz)
		{
			case 0:
				matrix.speakers[speaker].x = (float)value;
				break;
			case 1:
				matrix.speakers[speaker].y = (float)value;
				break;
			case 2:
				matrix.speakers[speaker].z = (float)value;
				break;
			default:
				break;
		}
	}
}
