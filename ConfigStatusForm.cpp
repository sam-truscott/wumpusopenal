#include "Out_Wumpus.h"
#include "ConfigStatusForm.h"

namespace WinampOpenALOut {

	delegate void UpdateData();

	Config::Config(Output_Wumpus *an_output_plugin)
		{
		InitializeComponent();

		output_plugin = an_output_plugin;
		ptrForm = this;

		overRide = false;

		current_device = ConfigFile::ReadInteger(CONF_DEVICE);

		// get a list of all the devices and add them to the combo
		ALDeviceList *device_list = Framework::getInstance()->ALFWGetDeviceList();
		if ((device_list) && (device_list->GetNumDevices())) {
			for (ALint i = 0; i < device_list->GetNumDevices(); i++) {
				if( i == device_list->GetDefaultDevice()) {
					comboBoxDevices->Items->Add("Default: " + gcnew String(device_list->GetDeviceName(i)));
				}else{
					comboBoxDevices->Items->Add(gcnew String(device_list->GetDeviceName(i)));
				}
			}
		}

		// if the current device is not set, use the default device
		if(current_device == -1) {
			comboBoxDevices->SelectedIndex = device_list->GetDefaultDevice();
			current_device = device_list->GetDefaultDevice();
		}else{
			comboBoxDevices->SelectedIndex = current_device;
		}

		int buffer = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);
		if ( buffer >= 250
			|| buffer <= 4000)
		{
			trackBufferLength->Value = buffer;
		}

		// update their captions with values
		labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";

		checkBoxExpandMono->Checked = output_plugin->IsMonoExpanded();
		checkBoxExpandStereo->Checked = output_plugin->IsStereoExpanded();
		checkBoxXRAM->Checked = output_plugin->IsXRAMEnabled();

		ShowDeviceDetails();
	}

	void Config::Load()
	{
		Show();
		checkBoxExpandMono->Checked = output_plugin->IsMonoExpanded();
		checkBoxExpandStereo->Checked = output_plugin->IsStereoExpanded();
		if(output_plugin->IsXRAMPresent() == true)
		{
			checkBoxXRAM->Enabled = true;
			checkBoxXRAM->Checked = output_plugin->IsXRAMEnabled();
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
		Int32 new_device = comboBoxDevices->SelectedIndex;

		if(new_device != current_device && new_device != -1) {
			// if we've changed device switch device and save
			ConfigFile::WriteInteger(CONF_DEVICE, new_device);
			current_device = new_device;
			output_plugin->SwitchOutputDevice(current_device);
		}
		if(output_plugin->GetConfBufferLength() != trackBufferLength->Value) {
			// if we've changed the buffer length stop playback and change it
			output_plugin->SetConfBufferLength(trackBufferLength->Value);
			ConfigFile::WriteInteger(CONF_BUFFER_LENGTH, trackBufferLength->Value);
			output_plugin->SwitchOutputDevice(current_device);
		}
		
		// store settings regarding effects
		if(output_plugin->IsMonoExpanded() != checkBoxExpandMono->Checked
			|| output_plugin->IsStereoExpanded() != checkBoxExpandStereo->Checked) {
				
				output_plugin->SetStereoExpanded(checkBoxExpandStereo->Checked);
				output_plugin->SetMonoExpanded(checkBoxExpandMono->Checked);
				output_plugin->SwitchOutputDevice(current_device);
		}

		if(output_plugin->IsXRAMEnabled() != checkBoxXRAM->Checked)
		{
			output_plugin->SetXRAMEnabled(checkBoxXRAM->Checked);
		}

		ConfigFile::WriteBoolean(CONF_MONO_EXPAND, checkBoxExpandMono->Checked);
		ConfigFile::WriteBoolean(CONF_STEREO_EXPAND, checkBoxExpandStereo->Checked);
		ConfigFile::WriteBoolean(CONF_XRAM_ENABLED, checkBoxXRAM->Checked);
		
		ShowDeviceDetails();
	}

	void Config::ThreadProcedure() {
		// only execute this thread while the form is visible to save cpu
		while(ptrForm->Visible && !overRide) {

			// get all the dynamic values
			written_ms			= output_plugin->GetLastWrittenTime();
			played_ms			= output_plugin->GetLastOutputTime();
			written_bytes		= output_plugin->GetWrittenBytes();
			played_bytes		= output_plugin->GetPlayedBytes();

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

		listBoxExtensions->Items->Add( "Device: " + comboBoxDevices->Items[current_device]->ToString());

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATION_EXT");
		} else {
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATION_EXT");
		}

		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT")) {
			listBoxExtensions->Items->Add("Present: ALC_ENUMERATE_ALL_EXT");
		} else {
			listBoxExtensions->Items->Add("Absent: ALC_ENUMERATE_ALL_EXT");
		}

		if (alcIsExtensionPresent(NULL, "AL_EXT_float32")) {
			listBoxExtensions->Items->Add("Present: AL_EXT_float32");
		} else {
			listBoxExtensions->Items->Add("Absent: AL_EXT_float32");
		}
		if (alcIsExtensionPresent(NULL, "AL_EXT_MCFORMATS")) {
			listBoxExtensions->Items->Add("Present: AL_EXT_MCFORMATS");
		} else {
			listBoxExtensions->Items->Add("Absent: AL_EXT_MCFORMATS");
		}

		// get the device
		ALCdevice* pDevice = alcGetContextsDevice(alcGetCurrentContext());
	

		if (alcIsExtensionPresent(pDevice, "ALC_EXT_EFX") == AL_TRUE) {
			listBoxExtensions->Items->Add("Present: ALC_EXT_EFX");
		}else{
			listBoxExtensions->Items->Add("Absent: ALC_EXT_EFX");
		}
	}
}
