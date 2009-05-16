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
		Config(class Output_Wumpus *aPtrOw)
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

			buffer = ConfigFile::ReadInteger(CONF_BUFFER_LATENCY);
			if ( buffer >= 250
				|| buffer <= 4000)
			{
				trackBarLatency->Value = buffer;
			}

			// update their captions with values
			labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
			labelBuffLatency->Text = "Buffer Latency (" + trackBarLatency->Value + "ms)";

			checkBoxExpandMono->Checked = ptrOw->IsMonoExpanded();
			checkBoxExpandStereo->Checked = ptrOw->IsStereoExpanded();
			checkBoxXRAM->Checked = ptrOw->IsXRAMEnabled();

			ShowDeviceDetails();
		}

		void Load()
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

		static Config^ GetInstance(Output_Wumpus *aPtrOw)
		{
			if(!loaded)
			{
				loaded = true;
				ptrForm = gcnew Config(aPtrOw);
				ptrForm->Load();			
				ptrForm->Visible = false;
			}
			return ptrForm;
		}

		void Config::ShowDeviceDetails();

	protected:

		static void ThreadProcedure();
		void DoUpdate();

		Int32 currentDevice;

		static class Output_Wumpus *ptrOw;
		static Config^ ptrForm;
		static bool loaded = false;

		static bool overRide = false;

		static UInt32 buffer_percent_full;
		static UInt64 written_bytes;
		static UInt64 played_bytes;
		static UInt32 written_ms;
		static UInt32 played_ms;

	private: System::Windows::Forms::TabPage^  tabPage3D;
	private: System::Windows::Forms::VScrollBar^  vScrollBarZ;
	private: System::Windows::Forms::HScrollBar^  hScrollBarX;


	private: System::Windows::Forms::VScrollBar^  vScrollBarY;
	private: System::Windows::Forms::Label^  label3;



private: System::Windows::Forms::TabPage^  tabPageConfig;
private: System::Windows::Forms::CheckBox^  checkBoxExpandStereo;
private: System::Windows::Forms::CheckBox^  checkBoxExpandMono;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Label^  labelBufferLength;
private: System::Windows::Forms::TrackBar^  trackBufferLength;

private: System::Windows::Forms::Button^  buttonOk;
private: System::Windows::Forms::Button^  buttonCancel;
private: System::Windows::Forms::Button^  buttonApply;
private: System::Windows::Forms::ListBox^  listBoxExtensions;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::ComboBox^  comboBoxDevices;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::CheckBox^  checkBoxXRAM;
private: System::Windows::Forms::ToolTip^  toolTipWarning;
private: System::Windows::Forms::ToolTip^  toolTipInfo;
private: System::Windows::Forms::TabPage^  tabPageStatistics;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::ProgressBar^  progressBar1;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  labelLatency;
private: System::Windows::Forms::Label^  labelPlayedMs;
private: System::Windows::Forms::Label^  labelWrittenMs;
private: System::Windows::Forms::Label^  labelPlayedB;
private: System::Windows::Forms::Label^  labelWrittenB;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TrackBar^  trackBarLatency;
private: System::Windows::Forms::Label^  labelBuffLatency;






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
private: System::Windows::Forms::TabControl^  tabConfiguration;
protected: 

	protected: 






























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
			this->components = (gcnew System::ComponentModel::Container());
			this->tabConfiguration = (gcnew System::Windows::Forms::TabControl());
			this->tabPageConfig = (gcnew System::Windows::Forms::TabPage());
			this->labelBuffLatency = (gcnew System::Windows::Forms::Label());
			this->trackBarLatency = (gcnew System::Windows::Forms::TrackBar());
			this->checkBoxXRAM = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandStereo = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandMono = (gcnew System::Windows::Forms::CheckBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->buttonReset = (gcnew System::Windows::Forms::Button());
			this->labelBufferLength = (gcnew System::Windows::Forms::Label());
			this->trackBufferLength = (gcnew System::Windows::Forms::TrackBar());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->listBoxExtensions = (gcnew System::Windows::Forms::ListBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->comboBoxDevices = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPageStatistics = (gcnew System::Windows::Forms::TabPage());
			this->labelPlayedB = (gcnew System::Windows::Forms::Label());
			this->labelWrittenB = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->labelLatency = (gcnew System::Windows::Forms::Label());
			this->labelPlayedMs = (gcnew System::Windows::Forms::Label());
			this->labelWrittenMs = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tabPage3D = (gcnew System::Windows::Forms::TabPage());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->vScrollBarZ = (gcnew System::Windows::Forms::VScrollBar());
			this->hScrollBarX = (gcnew System::Windows::Forms::HScrollBar());
			this->vScrollBarY = (gcnew System::Windows::Forms::VScrollBar());
			this->toolTipWarning = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTipInfo = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->tabConfiguration->SuspendLayout();
			this->tabPageConfig->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarLatency))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->BeginInit();
			this->tabPageStatistics->SuspendLayout();
			this->tabPage3D->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabConfiguration
			// 
			this->tabConfiguration->Controls->Add(this->tabPageConfig);
			this->tabConfiguration->Controls->Add(this->tabPageStatistics);
			this->tabConfiguration->Location = System::Drawing::Point(12, 12);
			this->tabConfiguration->Name = L"tabConfiguration";
			this->tabConfiguration->SelectedIndex = 0;
			this->tabConfiguration->Size = System::Drawing::Size(409, 390);
			this->tabConfiguration->TabIndex = 0;
			// 
			// tabPageConfig
			// 
			this->tabPageConfig->Controls->Add(this->labelBuffLatency);
			this->tabPageConfig->Controls->Add(this->trackBarLatency);
			this->tabPageConfig->Controls->Add(this->checkBoxXRAM);
			this->tabPageConfig->Controls->Add(this->checkBoxExpandStereo);
			this->tabPageConfig->Controls->Add(this->checkBoxExpandMono);
			this->tabPageConfig->Controls->Add(this->label4);
			this->tabPageConfig->Controls->Add(this->buttonReset);
			this->tabPageConfig->Controls->Add(this->labelBufferLength);
			this->tabPageConfig->Controls->Add(this->trackBufferLength);
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
			// labelBuffLatency
			// 
			this->labelBuffLatency->AutoSize = true;
			this->labelBuffLatency->Location = System::Drawing::Point(198, 34);
			this->labelBuffLatency->Name = L"labelBuffLatency";
			this->labelBuffLatency->Size = System::Drawing::Size(28, 13);
			this->labelBuffLatency->TabIndex = 18;
			this->labelBuffLatency->Text = L"###";
			// 
			// trackBarLatency
			// 
			this->trackBarLatency->LargeChange = 500;
			this->trackBarLatency->Location = System::Drawing::Point(199, 50);
			this->trackBarLatency->Maximum = 2000;
			this->trackBarLatency->Minimum = 400;
			this->trackBarLatency->Name = L"trackBarLatency";
			this->trackBarLatency->Size = System::Drawing::Size(196, 45);
			this->trackBarLatency->SmallChange = 250;
			this->trackBarLatency->TabIndex = 17;
			this->trackBarLatency->TickFrequency = 256;
			this->toolTipWarning->SetToolTip(this->trackBarLatency, L"The Latency reported to Winamp. It\'s advised that this is not changed.");
			this->trackBarLatency->Value = 2000;
			this->trackBarLatency->Scroll += gcnew System::EventHandler(this, &Config::trackBarLatency_Scroll);
			// 
			// checkBoxXRAM
			// 
			this->checkBoxXRAM->AutoSize = true;
			this->checkBoxXRAM->Location = System::Drawing::Point(6, 256);
			this->checkBoxXRAM->Name = L"checkBoxXRAM";
			this->checkBoxXRAM->Size = System::Drawing::Size(187, 17);
			this->checkBoxXRAM->TabIndex = 16;
			this->checkBoxXRAM->Text = L"Use XRAM (small performance hit)";
			this->toolTipWarning->SetToolTip(this->checkBoxXRAM, L"XRAM isn\'t designed for streaming as there\'s a CPU hit constantly uploading small" 
				L" bits of data. You can use XRAM but it isn\'t recommended.");
			this->checkBoxXRAM->UseVisualStyleBackColor = true;
			// 
			// checkBoxExpandStereo
			// 
			this->checkBoxExpandStereo->AutoSize = true;
			this->checkBoxExpandStereo->Location = System::Drawing::Point(6, 302);
			this->checkBoxExpandStereo->Name = L"checkBoxExpandStereo";
			this->checkBoxExpandStereo->Size = System::Drawing::Size(276, 17);
			this->checkBoxExpandStereo->TabIndex = 15;
			this->checkBoxExpandStereo->Text = L"Expand Stereo to 4.0 (also has small performance hit)";
			this->toolTipWarning->SetToolTip(this->checkBoxExpandStereo, L"This expands Stereo (2.0) sound out to Quad (4.0) sound. Data has to be copied in" 
				L" RAM so there\'s a performance cost.");
			this->checkBoxExpandStereo->UseVisualStyleBackColor = true;
			// 
			// checkBoxExpandMono
			// 
			this->checkBoxExpandMono->AutoSize = true;
			this->checkBoxExpandMono->Location = System::Drawing::Point(6, 279);
			this->checkBoxExpandMono->Name = L"checkBoxExpandMono";
			this->checkBoxExpandMono->Size = System::Drawing::Size(262, 17);
			this->checkBoxExpandMono->TabIndex = 14;
			this->checkBoxExpandMono->Text = L"Expand Mono to 4.0 (again, small performance hit)";
			this->toolTipWarning->SetToolTip(this->checkBoxExpandMono, L"This expands Mono (1.0) sound out to Quad (4.0) sound. Data has to be copied in R" 
				L"AM so there\'s a performance cost.");
			this->checkBoxExpandMono->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(12, 9);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(0, 13);
			this->label4->TabIndex = 13;
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
			this->labelBufferLength->Size = System::Drawing::Size(28, 13);
			this->labelBufferLength->TabIndex = 11;
			this->labelBufferLength->Text = L"###";
			// 
			// trackBufferLength
			// 
			this->trackBufferLength->LargeChange = 500;
			this->trackBufferLength->Location = System::Drawing::Point(9, 50);
			this->trackBufferLength->Maximum = 4000;
			this->trackBufferLength->Minimum = 250;
			this->trackBufferLength->Name = L"trackBufferLength";
			this->trackBufferLength->Size = System::Drawing::Size(184, 45);
			this->trackBufferLength->SmallChange = 250;
			this->trackBufferLength->TabIndex = 10;
			this->trackBufferLength->TickFrequency = 256;
			this->toolTipInfo->SetToolTip(this->trackBufferLength, L"Control the maximum amount of data for OpenAL to buffer in RAM");
			this->trackBufferLength->Value = 2000;
			this->trackBufferLength->Scroll += gcnew System::EventHandler(this, &Config::trackBufferLength_Scroll);
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
			this->listBoxExtensions->Size = System::Drawing::Size(360, 147);
			this->listBoxExtensions->TabIndex = 3;
			this->toolTipInfo->SetToolTip(this->listBoxExtensions, L"A list of supported OpenAL Extensions the card supports. Not all are shown.");
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
			this->toolTipInfo->SetToolTip(this->comboBoxDevices, L"Select the Device you want OpenAL to use.");
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 13);
			this->label1->TabIndex = 0;
			// 
			// tabPageStatistics
			// 
			this->tabPageStatistics->Controls->Add(this->labelPlayedB);
			this->tabPageStatistics->Controls->Add(this->labelWrittenB);
			this->tabPageStatistics->Controls->Add(this->label8);
			this->tabPageStatistics->Controls->Add(this->label10);
			this->tabPageStatistics->Controls->Add(this->labelLatency);
			this->tabPageStatistics->Controls->Add(this->labelPlayedMs);
			this->tabPageStatistics->Controls->Add(this->labelWrittenMs);
			this->tabPageStatistics->Controls->Add(this->label7);
			this->tabPageStatistics->Controls->Add(this->label6);
			this->tabPageStatistics->Controls->Add(this->label5);
			this->tabPageStatistics->Controls->Add(this->progressBar1);
			this->tabPageStatistics->Controls->Add(this->label2);
			this->tabPageStatistics->Location = System::Drawing::Point(4, 22);
			this->tabPageStatistics->Name = L"tabPageStatistics";
			this->tabPageStatistics->Padding = System::Windows::Forms::Padding(3);
			this->tabPageStatistics->Size = System::Drawing::Size(401, 364);
			this->tabPageStatistics->TabIndex = 1;
			this->tabPageStatistics->Text = L"Statistics";
			this->tabPageStatistics->UseVisualStyleBackColor = true;
			// 
			// labelPlayedB
			// 
			this->labelPlayedB->AutoSize = true;
			this->labelPlayedB->Location = System::Drawing::Point(251, 65);
			this->labelPlayedB->Name = L"labelPlayedB";
			this->labelPlayedB->Size = System::Drawing::Size(13, 13);
			this->labelPlayedB->TabIndex = 11;
			this->labelPlayedB->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelPlayedB, L"Amount of data played (Bytes)");
			// 
			// labelWrittenB
			// 
			this->labelWrittenB->AutoSize = true;
			this->labelWrittenB->Location = System::Drawing::Point(250, 43);
			this->labelWrittenB->Name = L"labelWrittenB";
			this->labelWrittenB->Size = System::Drawing::Size(13, 13);
			this->labelWrittenB->TabIndex = 10;
			this->labelWrittenB->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelWrittenB, L"Amount of data written (Bytes)");
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(184, 65);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(61, 13);
			this->label8->TabIndex = 9;
			this->label8->Text = L"Played (B): ";
			this->toolTipInfo->SetToolTip(this->label8, L"Amount of data played (Bytes)");
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(184, 43);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(60, 13);
			this->label10->TabIndex = 8;
			this->label10->Text = L"Written (B):";
			this->toolTipInfo->SetToolTip(this->label10, L"Amount of data written (Bytes)");
			// 
			// labelLatency
			// 
			this->labelLatency->AutoSize = true;
			this->labelLatency->Location = System::Drawing::Point(86, 88);
			this->labelLatency->Name = L"labelLatency";
			this->labelLatency->Size = System::Drawing::Size(13, 13);
			this->labelLatency->TabIndex = 7;
			this->labelLatency->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelLatency, L"Latency between Winamp writing the data and hearing it.");
			// 
			// labelPlayedMs
			// 
			this->labelPlayedMs->AutoSize = true;
			this->labelPlayedMs->Location = System::Drawing::Point(86, 65);
			this->labelPlayedMs->Name = L"labelPlayedMs";
			this->labelPlayedMs->Size = System::Drawing::Size(13, 13);
			this->labelPlayedMs->TabIndex = 6;
			this->labelPlayedMs->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelPlayedMs, L"Amount of data played (Milliseconds)");
			// 
			// labelWrittenMs
			// 
			this->labelWrittenMs->AutoSize = true;
			this->labelWrittenMs->Location = System::Drawing::Point(86, 43);
			this->labelWrittenMs->Name = L"labelWrittenMs";
			this->labelWrittenMs->Size = System::Drawing::Size(13, 13);
			this->labelWrittenMs->TabIndex = 5;
			this->labelWrittenMs->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelWrittenMs, L"Amount of data written (Milliseconds)");
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(6, 88);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(74, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"Latency (Ms): ";
			this->toolTipInfo->SetToolTip(this->label7, L"Latency between Winamp writing the data and hearing it.");
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 65);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(68, 13);
			this->label6->TabIndex = 3;
			this->label6->Text = L"Played (Ms): ";
			this->toolTipInfo->SetToolTip(this->label6, L"Amount of data played (Milliseconds)");
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 43);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(70, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"Written (Ms): ";
			this->toolTipInfo->SetToolTip(this->label5, L"Amount of data written (Milliseconds)");
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(70, 12);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(325, 22);
			this->progressBar1->TabIndex = 1;
			this->toolTipInfo->SetToolTip(this->progressBar1, L"The amount of data in OpenALs buffers. Won\'t always be full because Winamp doesn\'" 
				L"t write full packets.");
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 12);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(58, 13);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Buffer Size";
			this->toolTipInfo->SetToolTip(this->label2, L"The amount of data in OpenALs buffers. Won\'t always be full because Winamp doesn\'" 
				L"t write full packets.");
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
			this->vScrollBarZ->Location = System::Drawing::Point(0, 0);
			this->vScrollBarZ->Name = L"vScrollBarZ";
			this->vScrollBarZ->Size = System::Drawing::Size(17, 80);
			this->vScrollBarZ->TabIndex = 4;
			// 
			// hScrollBarX
			// 
			this->hScrollBarX->Location = System::Drawing::Point(0, 0);
			this->hScrollBarX->Name = L"hScrollBarX";
			this->hScrollBarX->Size = System::Drawing::Size(80, 17);
			this->hScrollBarX->TabIndex = 5;
			// 
			// vScrollBarY
			// 
			this->vScrollBarY->Location = System::Drawing::Point(0, 0);
			this->vScrollBarY->Name = L"vScrollBarY";
			this->vScrollBarY->Size = System::Drawing::Size(17, 80);
			this->vScrollBarY->TabIndex = 6;
			// 
			// toolTipWarning
			// 
			this->toolTipWarning->ToolTipIcon = System::Windows::Forms::ToolTipIcon::Warning;
			this->toolTipWarning->ToolTipTitle = L"Warning";
			// 
			// toolTipInfo
			// 
			this->toolTipInfo->ToolTipIcon = System::Windows::Forms::ToolTipIcon::Info;
			this->toolTipInfo->ToolTipTitle = L"Information";
			// 
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(433, 414);
			this->Controls->Add(this->tabConfiguration);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"Config";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Wumpus OpenAL Output Plug-in";
			this->TopMost = true;
			this->Disposed += gcnew System::EventHandler(this, &Config::Config_Unload);
			this->tabConfiguration->ResumeLayout(false);
			this->tabPageConfig->ResumeLayout(false);
			this->tabPageConfig->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarLatency))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->EndInit();
			this->tabPageStatistics->ResumeLayout(false);
			this->tabPageStatistics->PerformLayout();
			this->tabPage3D->ResumeLayout(false);
			this->tabPage3D->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void buttonApply_Click(System::Object^  sender, System::EventArgs^  e) {
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
				if(ptrOw->GetConfBufferLatency() != trackBarLatency->Value) {
					// if we've changed the buffer length stop playback and change it
					ptrOw->SetConfBufferLatency(trackBarLatency->Value);
					ConfigFile::WriteInteger(CONF_BUFFER_LATENCY, trackBarLatency->Value);
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

				ConfigFile::WriteBoolean(CONF_MONO_EXPAND, checkBoxExpandMono->Checked);
				ConfigFile::WriteBoolean(CONF_STEREO_EXPAND, checkBoxExpandStereo->Checked);
				ConfigFile::WriteBoolean(CONF_XRAM_ENABLED, checkBoxXRAM->Checked);
				
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
			 trackBarLatency->Value = DEFC_BUFFER_LATENCY;
			 labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
			 labelBuffLatency->Text = "Buffer Latency (" + trackBarLatency->Value + "ms)";
			 checkBoxXRAM->Checked = false;
			 checkBoxExpandMono->Checked = false;
			 checkBoxExpandStereo->Checked = false;
		 }
private: System::Void trackBarLatency_Scroll(System::Object^  sender, System::EventArgs^  e) {
			 labelBuffLatency->Text = "Buffer Latency (" + trackBarLatency->Value + "ms)";
		 }
};
}

#endif /*__CONFIG_STATUS_FORM*/