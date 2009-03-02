#ifndef __CONFIG_STATUS_FORM
#define __CONFIG_STATUS_FORM

#include "Constants.h"
#include "Framework\aldlist.h"
#include "ConfigFile.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;

namespace WinampOpenALOut {

	/// <summary>
	/// Summary for Config
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Config : public System::Windows::Forms::Form
	{
	public:
		Config(class Output_Wumpus *aPtrOw) {
			InitializeComponent();

			ptrOw = aPtrOw;
			ptrForm = this;

			currentBuffer = 0;

			overRide = false;
			Load();

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

			trackBufferLength->Value = ConfigFile::ReadInteger(CONF_BUFFER_LENGTH);

			// update their captions with values
			labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";

			checkBoxExpandMono->Checked = ptrOw->IsMonoExpanded();
			checkBoxExpandStereo->Checked = ptrOw->IsStereoExpanded();

			ShowDeviceDetails();
		}

		void Load() {
			Show();
			Thread^ thread = gcnew Thread(gcnew ThreadStart(ThreadProcedure));
            thread->Start();
		}

		static Config^ GetInstance(Output_Wumpus *aPtrOw) {
			if(!loaded) {
				loaded = true;
				ptrForm = gcnew Config(aPtrOw);
			}
			return ptrForm;
		}

		void Config::ShowDeviceDetails();

	protected:

		static void ThreadProcedure();

		void DoUpdate();

		int currentDevice;

		static class Output_Wumpus *ptrOw;
		static Config^ ptrForm;
		static bool loaded = false;

		static bool overRide = false;

		static int writtenTime;
		static int playedTime;
		static unsigned int bufferSize;
		static unsigned int currentBuffer;
		static unsigned int numberOfBuffers;
		static unsigned int numberOfChannels;
		static unsigned int bitsPerSample;
		static unsigned int sampleRate;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ListBox^  listBoxBufferSizes;
	private: System::Windows::Forms::TabPage^  tabPageConfig;
	private: System::Windows::Forms::Button^  buttonOk;
private: System::Windows::Forms::Button^  buttonCancel;


private: System::Windows::Forms::Button^  buttonApply;

	private: System::Windows::Forms::ListBox^  listBoxExtensions;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::ComboBox^  comboBoxDevices;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::CheckBox^  checkBoxEffectsEnabled;
	private: System::Windows::Forms::Label^  labelBufferLength;

	private: System::Windows::Forms::TrackBar^  trackBufferLength;




private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::CheckBox^  checkBoxExpandStereo;

private: System::Windows::Forms::CheckBox^  checkBoxExpandMono;
private: System::Windows::Forms::TabPage^  tabPage3D;
private: System::Windows::Forms::VScrollBar^  vScrollBarZ;
private: System::Windows::Forms::HScrollBar^  hScrollBarX;


private: System::Windows::Forms::VScrollBar^  vScrollBarY;
private: System::Windows::Forms::Label^  label3;
	protected: 

			 static System::Windows::Forms::Label^ ptrLblSampleRate;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Config()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tabControl1;
	protected: 

	private: System::Windows::Forms::TabPage^  tabPageStatus;




	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  lblWrittenTime;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  lblPlayedTime;

	private: System::Windows::Forms::Label^  label6;









	private: System::Windows::Forms::Label^  lblNumberOfChannels;

	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  lblBitsPerSecond;

	private: System::Windows::Forms::Label^  lblSampleRate;

	private: System::Windows::Forms::Label^  label11;
private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPageConfig = (gcnew System::Windows::Forms::TabPage());
			this->checkBoxExpandStereo = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandMono = (gcnew System::Windows::Forms::CheckBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->buttonReset = (gcnew System::Windows::Forms::Button());
			this->labelBufferLength = (gcnew System::Windows::Forms::Label());
			this->trackBufferLength = (gcnew System::Windows::Forms::TrackBar());
			this->checkBoxEffectsEnabled = (gcnew System::Windows::Forms::CheckBox());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->listBoxExtensions = (gcnew System::Windows::Forms::ListBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->comboBoxDevices = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPage3D = (gcnew System::Windows::Forms::TabPage());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->vScrollBarZ = (gcnew System::Windows::Forms::VScrollBar());
			this->hScrollBarX = (gcnew System::Windows::Forms::HScrollBar());
			this->vScrollBarY = (gcnew System::Windows::Forms::VScrollBar());
			this->tabPageStatus = (gcnew System::Windows::Forms::TabPage());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->listBoxBufferSizes = (gcnew System::Windows::Forms::ListBox());
			this->lblNumberOfChannels = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->lblBitsPerSecond = (gcnew System::Windows::Forms::Label());
			this->lblSampleRate = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->lblWrittenTime = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->lblPlayedTime = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tabControl1->SuspendLayout();
			this->tabPageConfig->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->BeginInit();
			this->tabPage3D->SuspendLayout();
			this->tabPageStatus->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPageConfig);
			// disable 3d menu
			//this->tabControl1->Controls->Add(this->tabPage3D);
			this->tabControl1->Controls->Add(this->tabPageStatus);
			this->tabControl1->Location = System::Drawing::Point(12, 12);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(409, 390);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPageConfig
			// 
			this->tabPageConfig->Controls->Add(this->checkBoxExpandStereo);
			this->tabPageConfig->Controls->Add(this->checkBoxExpandMono);
			this->tabPageConfig->Controls->Add(this->label4);
			this->tabPageConfig->Controls->Add(this->buttonReset);
			this->tabPageConfig->Controls->Add(this->labelBufferLength);
			this->tabPageConfig->Controls->Add(this->trackBufferLength);
			this->tabPageConfig->Controls->Add(this->checkBoxEffectsEnabled);
			this->tabPageConfig->Controls->Add(this->buttonOk);
			this->tabPageConfig->Controls->Add(this->buttonCancel);
			this->tabPageConfig->Controls->Add(this->buttonApply);
			this->tabPageConfig->Controls->Add(this->listBoxExtensions);
			this->tabPageConfig->Controls->Add(this->label16);
			this->tabPageConfig->Controls->Add(this->comboBoxDevices);
			this->tabPageConfig->Controls->Add(this->label1);
			this->tabPageConfig->Location = System::Drawing::Point(4, 22);
			this->tabPageConfig->Name = L"tabPageConfig";
			this->tabPageConfig->Padding = System::Windows::Forms::Padding(3);
			this->tabPageConfig->Size = System::Drawing::Size(401, 364);
			this->tabPageConfig->TabIndex = 0;
			this->tabPageConfig->Text = L"Configuration";
			this->tabPageConfig->UseVisualStyleBackColor = true;
			// 
			// checkBoxExpandStereo
			// 
			this->checkBoxExpandStereo->AutoSize = true;
			this->checkBoxExpandStereo->Location = System::Drawing::Point(15, 212);
			this->checkBoxExpandStereo->Name = L"checkBoxExpandStereo";
			this->checkBoxExpandStereo->Size = System::Drawing::Size(126, 17);
			this->checkBoxExpandStereo->TabIndex = 15;
			this->checkBoxExpandStereo->Text = L"Expand Stereo to 4.0";
			this->checkBoxExpandStereo->UseVisualStyleBackColor = true;
			// 
			// checkBoxExpandMono
			// 
			this->checkBoxExpandMono->AutoSize = true;
			this->checkBoxExpandMono->Location = System::Drawing::Point(15, 189);
			this->checkBoxExpandMono->Name = L"checkBoxExpandMono";
			this->checkBoxExpandMono->Size = System::Drawing::Size(122, 17);
			this->checkBoxExpandMono->TabIndex = 14;
			this->checkBoxExpandMono->Text = L"Expand Mono to 4.0";
			this->checkBoxExpandMono->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(12, 9);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(44, 13);
			this->label4->TabIndex = 13;
			this->label4->Text = L"Device:";
			// 
			// buttonReset
			// 
			this->buttonReset->Location = System::Drawing::Point(38, 335);
			this->buttonReset->Name = L"buttonReset";
			this->buttonReset->Size = System::Drawing::Size(114, 23);
			this->buttonReset->TabIndex = 12;
			this->buttonReset->Text = L"Restore to Defaults";
			this->buttonReset->UseVisualStyleBackColor = true;
			this->buttonReset->Click += gcnew System::EventHandler(this, &Config::buttonReset_Click);
			// 
			// labelBufferLength
			// 
			this->labelBufferLength->AutoSize = true;
			this->labelBufferLength->Location = System::Drawing::Point(12, 34);
			this->labelBufferLength->Name = L"labelBufferLength";
			this->labelBufferLength->Size = System::Drawing::Size(71, 13);
			this->labelBufferLength->TabIndex = 11;
			this->labelBufferLength->Text = L"Buffer Length";
			// 
			// trackBufferLength
			// 
			this->trackBufferLength->LargeChange = 500;
			this->trackBufferLength->Location = System::Drawing::Point(15, 50);
			this->trackBufferLength->Maximum = CONF_BUFFER_LENGTH_MAX;
			this->trackBufferLength->Minimum = CONF_BUFFER_LENGTH_MIN;
			this->trackBufferLength->Name = L"trackBufferLength";
			this->trackBufferLength->Size = System::Drawing::Size(369, 45);
			this->trackBufferLength->SmallChange = 100;
			this->trackBufferLength->TabIndex = 10;
			this->trackBufferLength->TickFrequency = 256;
			this->trackBufferLength->Value = DEFC_BUFFER_LENGTH;
			this->trackBufferLength->Scroll += gcnew System::EventHandler(this, &Config::trackBufferLength_Scroll);
			// 
			// checkBoxEffectsEnabled
			// 
			this->checkBoxEffectsEnabled->AutoSize = true;
			this->checkBoxEffectsEnabled->Enabled = false;
			this->checkBoxEffectsEnabled->Location = System::Drawing::Point(289, 34);
			this->checkBoxEffectsEnabled->Name = L"checkBoxEffectsEnabled";
			this->checkBoxEffectsEnabled->Size = System::Drawing::Size(95, 17);
			this->checkBoxEffectsEnabled->TabIndex = 7;
			this->checkBoxEffectsEnabled->Text = L"Enable Effects";
			this->checkBoxEffectsEnabled->UseVisualStyleBackColor = true;
			//TODO fix effects?
			this->checkBoxEffectsEnabled->Visible = false;
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(239, 335);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 6;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &Config::buttonOk_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Location = System::Drawing::Point(320, 335);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 5;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			this->buttonCancel->Click += gcnew System::EventHandler(this, &Config::buttonCancel_Click);
			// 
			// buttonApply
			// 
			this->buttonApply->Location = System::Drawing::Point(158, 335);
			this->buttonApply->Name = L"buttonApply";
			this->buttonApply->Size = System::Drawing::Size(75, 23);
			this->buttonApply->TabIndex = 4;
			this->buttonApply->Text = L"Apply";
			this->buttonApply->UseVisualStyleBackColor = true;
			this->buttonApply->Click += gcnew System::EventHandler(this, &Config::buttonApply_Click);
			// 
			// listBoxExtensions
			// 
			this->listBoxExtensions->FormattingEnabled = true;
			this->listBoxExtensions->Location = System::Drawing::Point(15, 101);
			this->listBoxExtensions->Name = L"listBoxExtensions";
			this->listBoxExtensions->Size = System::Drawing::Size(360, 82);
			this->listBoxExtensions->TabIndex = 3;
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(6, 34);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(0, 13);
			this->label16->TabIndex = 2;
			// 
			// comboBoxDevices
			// 
			this->comboBoxDevices->FormattingEnabled = true;
			this->comboBoxDevices->Location = System::Drawing::Point(86, 6);
			this->comboBoxDevices->Name = L"comboBoxDevices";
			this->comboBoxDevices->Size = System::Drawing::Size(309, 21);
			this->comboBoxDevices->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 13);
			this->label1->TabIndex = 0;
			// 
			// tabPage3D
			// 
			this->tabPage3D->Controls->Add(this->label3);
			this->tabPage3D->Controls->Add(this->vScrollBarZ);
			this->tabPage3D->Controls->Add(this->hScrollBarX);
			this->tabPage3D->Controls->Add(this->vScrollBarY);
			this->tabPage3D->Location = System::Drawing::Point(4, 22);
			this->tabPage3D->Name = L"tabPage3D";
			this->tabPage3D->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3D->Size = System::Drawing::Size(401, 364);
			this->tabPage3D->TabIndex = 2;
			this->tabPage3D->Text = L"3D";
			this->tabPage3D->UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 3);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(25, 13);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Info";
			// 
			// vScrollBarZ
			// 
			this->vScrollBarZ->Location = System::Drawing::Point(180, 3);
			this->vScrollBarZ->Maximum = 255;
			this->vScrollBarZ->Minimum = -255;
			this->vScrollBarZ->Name = L"vScrollBarZ";
			this->vScrollBarZ->Size = System::Drawing::Size(28, 330);
			this->vScrollBarZ->TabIndex = 2;
			this->vScrollBarZ->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Config::vScrollBarZ_Scroll);
			// 
			// hScrollBarX
			// 
			this->hScrollBarX->Location = System::Drawing::Point(3, 333);
			this->hScrollBarX->Maximum = 255;
			this->hScrollBarX->Minimum = -255;
			this->hScrollBarX->Name = L"hScrollBarX";
			this->hScrollBarX->Size = System::Drawing::Size(373, 28);
			this->hScrollBarX->TabIndex = 1;
			this->hScrollBarX->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Config::hScrollBarX_Scroll);
			// 
			// vScrollBarY
			// 
			this->vScrollBarY->Location = System::Drawing::Point(375, 3);
			this->vScrollBarY->Maximum = 255;
			this->vScrollBarY->Minimum = -255;
			this->vScrollBarY->Name = L"vScrollBarY";
			this->vScrollBarY->Size = System::Drawing::Size(23, 330);
			this->vScrollBarY->TabIndex = 0;
			this->vScrollBarY->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Config::vScrollBarY_Scroll);
			// 
			// tabPageStatus
			// 
			this->tabPageStatus->Controls->Add(this->label2);
			this->tabPageStatus->Controls->Add(this->listBoxBufferSizes);
			this->tabPageStatus->Controls->Add(this->lblNumberOfChannels);
			this->tabPageStatus->Controls->Add(this->label14);
			this->tabPageStatus->Controls->Add(this->lblBitsPerSecond);
			this->tabPageStatus->Controls->Add(this->lblSampleRate);
			this->tabPageStatus->Controls->Add(this->label11);
			this->tabPageStatus->Controls->Add(this->label10);
			this->tabPageStatus->Controls->Add(this->lblWrittenTime);
			this->tabPageStatus->Controls->Add(this->label8);
			this->tabPageStatus->Controls->Add(this->lblPlayedTime);
			this->tabPageStatus->Controls->Add(this->label6);
			this->tabPageStatus->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabPageStatus->Location = System::Drawing::Point(4, 22);
			this->tabPageStatus->Name = L"tabPageStatus";
			this->tabPageStatus->Padding = System::Windows::Forms::Padding(3);
			this->tabPageStatus->Size = System::Drawing::Size(401, 364);
			this->tabPageStatus->TabIndex = 1;
			this->tabPageStatus->Text = L"Status";
			this->tabPageStatus->UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 54);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(68, 13);
			this->label2->TabIndex = 15;
			this->label2->Text = L"Buffer Status";
			// 
			// listBoxBufferSizes
			// 
			this->listBoxBufferSizes->FormattingEnabled = true;
			this->listBoxBufferSizes->Location = System::Drawing::Point(9, 70);
			this->listBoxBufferSizes->Name = L"listBoxBufferSizes";
			this->listBoxBufferSizes->Size = System::Drawing::Size(386, 290);
			this->listBoxBufferSizes->TabIndex = 14;
			// 
			// lblNumberOfChannels
			// 
			this->lblNumberOfChannels->AutoSize = true;
			this->lblNumberOfChannels->Location = System::Drawing::Point(126, 29);
			this->lblNumberOfChannels->Name = L"lblNumberOfChannels";
			this->lblNumberOfChannels->Size = System::Drawing::Size(13, 13);
			this->lblNumberOfChannels->TabIndex = 13;
			this->lblNumberOfChannels->Text = L"0";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(17, 29);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(103, 13);
			this->label14->TabIndex = 12;
			this->label14->Text = L"Number of Channels";
			// 
			// lblBitsPerSecond
			// 
			this->lblBitsPerSecond->AutoSize = true;
			this->lblBitsPerSecond->Location = System::Drawing::Point(126, 16);
			this->lblBitsPerSecond->Name = L"lblBitsPerSecond";
			this->lblBitsPerSecond->Size = System::Drawing::Size(13, 13);
			this->lblBitsPerSecond->TabIndex = 11;
			this->lblBitsPerSecond->Text = L"0";
			// 
			// lblSampleRate
			// 
			this->lblSampleRate->AutoSize = true;
			this->lblSampleRate->Location = System::Drawing::Point(126, 3);
			this->lblSampleRate->Name = L"lblSampleRate";
			this->lblSampleRate->Size = System::Drawing::Size(13, 13);
			this->lblSampleRate->TabIndex = 10;
			this->lblSampleRate->Text = L"0";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(36, 16);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(81, 13);
			this->label11->TabIndex = 9;
			this->label11->Text = L"Bits Per Sample";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(52, 3);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(68, 13);
			this->label10->TabIndex = 8;
			this->label10->Text = L"Sample Rate";
			// 
			// lblWrittenTime
			// 
			this->lblWrittenTime->AutoSize = true;
			this->lblWrittenTime->Location = System::Drawing::Point(270, 16);
			this->lblWrittenTime->Name = L"lblWrittenTime";
			this->lblWrittenTime->Size = System::Drawing::Size(13, 13);
			this->lblWrittenTime->TabIndex = 7;
			this->lblWrittenTime->Text = L"0";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(197, 16);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(67, 13);
			this->label8->TabIndex = 6;
			this->label8->Text = L"Written Time";
			// 
			// lblPlayedTime
			// 
			this->lblPlayedTime->AutoSize = true;
			this->lblPlayedTime->Location = System::Drawing::Point(270, 3);
			this->lblPlayedTime->Name = L"lblPlayedTime";
			this->lblPlayedTime->Size = System::Drawing::Size(13, 13);
			this->lblPlayedTime->TabIndex = 5;
			this->lblPlayedTime->Text = L"0";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(199, 3);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(65, 13);
			this->label6->TabIndex = 4;
			this->label6->Text = L"Played Time";
			// 
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(433, 414);
			this->Controls->Add(this->tabControl1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"Config";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Wumpus OpenAL Output Plug-in";
			this->TopMost = true;
			this->Disposed += gcnew System::EventHandler(this, &Config::Config_Unload);
			this->tabControl1->ResumeLayout(false);
			this->tabPageConfig->ResumeLayout(false);
			this->tabPageConfig->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->EndInit();
			this->tabPage3D->ResumeLayout(false);
			this->tabPage3D->PerformLayout();
			this->tabPageStatus->ResumeLayout(false);
			this->tabPageStatus->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void buttonApply_Click(System::Object^  sender, System::EventArgs^  e) {
				int newDevice = comboBoxDevices->SelectedIndex;
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

				ConfigFile::WriteBoolean(CONF_MONO_EXPAND, checkBoxExpandMono->Checked);
				ConfigFile::WriteBoolean(CONF_STEREO_EXPAND, checkBoxExpandStereo->Checked);
				
				ShowDeviceDetails();
			 }
private: System::Void buttonCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
			 ptrForm->Close();
		 }
private: System::Void buttonOk_Click(System::Object^  sender, System::EventArgs^  e) {	 	
			// call the apply button	
			this->buttonApply_Click(sender, e);

			// close the form
			overRide = true;
			loaded = false;
			ptrForm->Close();
		 }
private: System::Void Config_Unload(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
		 }
private: System::Void trackBufferLength_Scroll(System::Object^  sender, System::EventArgs^  e) {
			 labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
		 }
private: System::Void buttonReset_Click(System::Object^  sender, System::EventArgs^  e) {
			 trackBufferLength->Value = DEFC_BUFFER_LENGTH;
			 labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
		 }
private: System::Void hScrollBarX_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) {
			// why do none of these work?
			 //alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
			//alSource3f(ptrOw->GetSource(), AL_POSITION, hScrollBarX->Value, vScrollBarY->Value, vScrollBarZ->Value);
			alListener3f(AL_POSITION, (ALfloat)hScrollBarX->Value, (ALfloat)vScrollBarY->Value, (ALfloat)vScrollBarZ->Value);
			//alSource3f(ptrOw->GetSource(), AL_POSITION, 0.0, 0.0, 0.0);
		 }
private: System::Void vScrollBarY_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) {
			alListener3f(AL_POSITION, (ALfloat)hScrollBarX->Value, (ALfloat)vScrollBarY->Value, (ALfloat)vScrollBarZ->Value);
			//alSource3f(ptrOw->GetSource(), AL_POSITION, 0.0, 0.0, 0.0);
		 }
private: System::Void vScrollBarZ_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) {
			alListener3f(AL_POSITION, (ALfloat)hScrollBarX->Value, (ALfloat)vScrollBarY->Value, (ALfloat)vScrollBarZ->Value);
			//alSource3f(ptrOw->GetSource(), AL_POSITION, 0.0, 0.0, 0.0);
		 }
};
}

#endif /*__CONFIG_STATUS_FORM*/