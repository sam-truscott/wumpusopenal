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
		Config(class Output_Wumpus *an_output_plugin);

		static Config^ GetInstance(Output_Wumpus *output)
		{
			if(!loaded)
			{
				loaded = true;
				ptrForm = gcnew Config(output);
				ptrForm->Load();			
				ptrForm->Visible = false;
			}
			return ptrForm;
		}

		void ShowDeviceDetails();

		void Load();

	protected:

	private: void ApplyChanges();

		static void ThreadProcedure();
		void DoUpdate();

		Int32 current_device;

		static class Output_Wumpus *output_plugin;
		static Config^ ptrForm;
		static bool loaded = false;

		static bool overRide = false;

		static UInt32 buffer_percent_full;
		static UInt64 written_bytes;
		static UInt64 played_bytes;
		static UInt32 written_ms;
		static UInt32 played_ms;


















private: System::Windows::Forms::ToolTip^  toolTipWarning;
private: System::Windows::Forms::ToolTip^  toolTipInfo;


















private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Button^  buttonOk;
private: System::Windows::Forms::Button^  buttonCancel;
private: System::Windows::Forms::Button^  buttonApply;
	private: System::Windows::Forms::TabPage^  tabPageStatistics;
	private: System::Windows::Forms::Label^  labelPlayedB;
	private: System::Windows::Forms::Label^  labelWrittenB;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  labelLatency;
	private: System::Windows::Forms::Label^  labelPlayedMs;
	private: System::Windows::Forms::Label^  labelWrittenMs;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TabPage^  tabPageConfig;
	private: System::Windows::Forms::CheckBox^  checkBoxXRAM;
	private: System::Windows::Forms::CheckBox^  checkBoxExpandStereo;
	private: System::Windows::Forms::CheckBox^  checkBoxExpandMono;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  labelBufferLength;
	private: System::Windows::Forms::TrackBar^  trackBufferLength;
	private: System::Windows::Forms::ListBox^  listBoxExtensions;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::ComboBox^  comboBoxDevices;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TabControl^  tabConfiguration;




































































































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
			this->toolTipWarning = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTipInfo = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->buttonReset = (gcnew System::Windows::Forms::Button());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->tabPageStatistics = (gcnew System::Windows::Forms::TabPage());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->labelWrittenMs = (gcnew System::Windows::Forms::Label());
			this->labelPlayedMs = (gcnew System::Windows::Forms::Label());
			this->labelLatency = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->labelWrittenB = (gcnew System::Windows::Forms::Label());
			this->labelPlayedB = (gcnew System::Windows::Forms::Label());
			this->tabPageConfig = (gcnew System::Windows::Forms::TabPage());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->comboBoxDevices = (gcnew System::Windows::Forms::ComboBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->listBoxExtensions = (gcnew System::Windows::Forms::ListBox());
			this->trackBufferLength = (gcnew System::Windows::Forms::TrackBar());
			this->labelBufferLength = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->checkBoxExpandMono = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandStereo = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxXRAM = (gcnew System::Windows::Forms::CheckBox());
			this->tabConfiguration = (gcnew System::Windows::Forms::TabControl());
			this->tabPageStatistics->SuspendLayout();
			this->tabPageConfig->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBufferLength))->BeginInit();
			this->tabConfiguration->SuspendLayout();
			this->SuspendLayout();
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
			// buttonReset
			// 
			this->buttonReset->Location = System::Drawing::Point(67, 379);
			this->buttonReset->Name = L"buttonReset";
			this->buttonReset->Size = System::Drawing::Size(114, 23);
			this->buttonReset->TabIndex = 16;
			this->buttonReset->Text = L"Restore to Defaults";
			this->buttonReset->UseVisualStyleBackColor = true;
			this->buttonReset->Click += gcnew System::EventHandler(this, &Config::buttonReset_Click_1);
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(268, 379);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(75, 23);
			this->buttonOk->TabIndex = 15;
			this->buttonOk->Text = L"Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &Config::buttonOk_Click_1);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Location = System::Drawing::Point(349, 379);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 14;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			this->buttonCancel->Click += gcnew System::EventHandler(this, &Config::buttonCancel_Click_1);
			// 
			// buttonApply
			// 
			this->buttonApply->Location = System::Drawing::Point(187, 379);
			this->buttonApply->Name = L"buttonApply";
			this->buttonApply->Size = System::Drawing::Size(75, 23);
			this->buttonApply->TabIndex = 13;
			this->buttonApply->Text = L"Apply";
			this->buttonApply->UseVisualStyleBackColor = true;
			this->buttonApply->Click += gcnew System::EventHandler(this, &Config::buttonApply_Click_1);
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
			this->tabPageStatistics->Location = System::Drawing::Point(4, 22);
			this->tabPageStatistics->Name = L"tabPageStatistics";
			this->tabPageStatistics->Padding = System::Windows::Forms::Padding(3);
			this->tabPageStatistics->Size = System::Drawing::Size(401, 335);
			this->tabPageStatistics->TabIndex = 1;
			this->tabPageStatistics->Text = L"Statistics";
			this->tabPageStatistics->UseVisualStyleBackColor = true;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 13);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(70, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"Written (Ms): ";
			this->toolTipInfo->SetToolTip(this->label5, L"Amount of data written (Milliseconds)");
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 35);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(68, 13);
			this->label6->TabIndex = 3;
			this->label6->Text = L"Played (Ms): ";
			this->toolTipInfo->SetToolTip(this->label6, L"Amount of data played (Milliseconds)");
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(6, 58);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(74, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"Latency (Ms): ";
			this->toolTipInfo->SetToolTip(this->label7, L"Latency between Winamp writing the data and hearing it.");
			// 
			// labelWrittenMs
			// 
			this->labelWrittenMs->AutoSize = true;
			this->labelWrittenMs->Location = System::Drawing::Point(86, 13);
			this->labelWrittenMs->Name = L"labelWrittenMs";
			this->labelWrittenMs->Size = System::Drawing::Size(13, 13);
			this->labelWrittenMs->TabIndex = 5;
			this->labelWrittenMs->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelWrittenMs, L"Amount of data written (Milliseconds)");
			// 
			// labelPlayedMs
			// 
			this->labelPlayedMs->AutoSize = true;
			this->labelPlayedMs->Location = System::Drawing::Point(86, 35);
			this->labelPlayedMs->Name = L"labelPlayedMs";
			this->labelPlayedMs->Size = System::Drawing::Size(13, 13);
			this->labelPlayedMs->TabIndex = 6;
			this->labelPlayedMs->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelPlayedMs, L"Amount of data played (Milliseconds)");
			// 
			// labelLatency
			// 
			this->labelLatency->AutoSize = true;
			this->labelLatency->Location = System::Drawing::Point(86, 58);
			this->labelLatency->Name = L"labelLatency";
			this->labelLatency->Size = System::Drawing::Size(13, 13);
			this->labelLatency->TabIndex = 7;
			this->labelLatency->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelLatency, L"Latency between Winamp writing the data and hearing it.");
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(184, 13);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(60, 13);
			this->label10->TabIndex = 8;
			this->label10->Text = L"Written (B):";
			this->toolTipInfo->SetToolTip(this->label10, L"Amount of data written (Bytes)");
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(184, 35);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(61, 13);
			this->label8->TabIndex = 9;
			this->label8->Text = L"Played (B): ";
			this->toolTipInfo->SetToolTip(this->label8, L"Amount of data played (Bytes)");
			// 
			// labelWrittenB
			// 
			this->labelWrittenB->AutoSize = true;
			this->labelWrittenB->Location = System::Drawing::Point(250, 13);
			this->labelWrittenB->Name = L"labelWrittenB";
			this->labelWrittenB->Size = System::Drawing::Size(13, 13);
			this->labelWrittenB->TabIndex = 10;
			this->labelWrittenB->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelWrittenB, L"Amount of data written (Bytes)");
			// 
			// labelPlayedB
			// 
			this->labelPlayedB->AutoSize = true;
			this->labelPlayedB->Location = System::Drawing::Point(251, 35);
			this->labelPlayedB->Name = L"labelPlayedB";
			this->labelPlayedB->Size = System::Drawing::Size(13, 13);
			this->labelPlayedB->TabIndex = 11;
			this->labelPlayedB->Text = L"\?";
			this->toolTipInfo->SetToolTip(this->labelPlayedB, L"Amount of data played (Bytes)");
			// 
			// tabPageConfig
			// 
			this->tabPageConfig->Controls->Add(this->checkBoxXRAM);
			this->tabPageConfig->Controls->Add(this->checkBoxExpandStereo);
			this->tabPageConfig->Controls->Add(this->checkBoxExpandMono);
			this->tabPageConfig->Controls->Add(this->label4);
			this->tabPageConfig->Controls->Add(this->labelBufferLength);
			this->tabPageConfig->Controls->Add(this->trackBufferLength);
			this->tabPageConfig->Controls->Add(this->listBoxExtensions);
			this->tabPageConfig->Controls->Add(this->label16);
			this->tabPageConfig->Controls->Add(this->comboBoxDevices);
			this->tabPageConfig->Controls->Add(this->label1);
			this->tabPageConfig->Location = System::Drawing::Point(4, 22);
			this->tabPageConfig->Name = L"tabPageConfig";
			this->tabPageConfig->Padding = System::Windows::Forms::Padding(3);
			this->tabPageConfig->Size = System::Drawing::Size(401, 335);
			this->tabPageConfig->TabIndex = 0;
			this->tabPageConfig->Text = L"Configuration";
			this->tabPageConfig->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 13);
			this->label1->TabIndex = 0;
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
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(6, 34);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(0, 13);
			this->label16->TabIndex = 2;
			// 
			// listBoxExtensions
			// 
			this->listBoxExtensions->FormattingEnabled = true;
			this->listBoxExtensions->Location = System::Drawing::Point(15, 101);
			this->listBoxExtensions->Name = L"listBoxExtensions";
			this->listBoxExtensions->Size = System::Drawing::Size(380, 147);
			this->listBoxExtensions->TabIndex = 3;
			this->toolTipInfo->SetToolTip(this->listBoxExtensions, L"A list of supported OpenAL Extensions the card supports. Not all are shown.");
			this->listBoxExtensions->SelectedIndexChanged += gcnew System::EventHandler(this, &Config::listBoxExtensions_SelectedIndexChanged);
			// 
			// trackBufferLength
			// 
			this->trackBufferLength->LargeChange = 500;
			this->trackBufferLength->Location = System::Drawing::Point(9, 50);
			this->trackBufferLength->Maximum = 6000;
			this->trackBufferLength->Minimum = 250;
			this->trackBufferLength->Name = L"trackBufferLength";
			this->trackBufferLength->Size = System::Drawing::Size(386, 45);
			this->trackBufferLength->SmallChange = 250;
			this->trackBufferLength->TabIndex = 10;
			this->trackBufferLength->TickFrequency = 256;
			this->toolTipInfo->SetToolTip(this->trackBufferLength, L"Control the maximum amount of data for OpenAL to buffer in RAM");
			this->trackBufferLength->Value = 2000;
			this->trackBufferLength->Scroll += gcnew System::EventHandler(this, &Config::trackBufferLength_Scroll);
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
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(12, 9);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(0, 13);
			this->label4->TabIndex = 13;
			// 
			// checkBoxExpandMono
			// 
			this->checkBoxExpandMono->AutoSize = true;
			this->checkBoxExpandMono->Location = System::Drawing::Point(6, 279);
			this->checkBoxExpandMono->Name = L"checkBoxExpandMono";
			this->checkBoxExpandMono->Size = System::Drawing::Size(230, 17);
			this->checkBoxExpandMono->TabIndex = 14;
			this->checkBoxExpandMono->Text = L"Expand Mono to 4.0 (small performance hit)";
			this->toolTipWarning->SetToolTip(this->checkBoxExpandMono, L"This expands Mono (1.0) sound out to Quad (4.0) sound. Data has to be copied in R"
				L"AM so there\'s a performance cost.");
			this->checkBoxExpandMono->UseVisualStyleBackColor = true;
			// 
			// checkBoxExpandStereo
			// 
			this->checkBoxExpandStereo->AutoSize = true;
			this->checkBoxExpandStereo->Location = System::Drawing::Point(6, 302);
			this->checkBoxExpandStereo->Name = L"checkBoxExpandStereo";
			this->checkBoxExpandStereo->Size = System::Drawing::Size(234, 17);
			this->checkBoxExpandStereo->TabIndex = 15;
			this->checkBoxExpandStereo->Text = L"Expand Stereo to 4.0 (small performance hit)";
			this->toolTipWarning->SetToolTip(this->checkBoxExpandStereo, L"This expands Stereo (2.0) sound out to Quad (4.0) sound. Data has to be copied in"
				L" RAM so there\'s a performance cost.");
			this->checkBoxExpandStereo->UseVisualStyleBackColor = true;
			// 
			// checkBoxXRAM
			// 
			this->checkBoxXRAM->AutoSize = true;
			this->checkBoxXRAM->Location = System::Drawing::Point(6, 256);
			this->checkBoxXRAM->Name = L"checkBoxXRAM";
			this->checkBoxXRAM->Size = System::Drawing::Size(266, 17);
			this->checkBoxXRAM->TabIndex = 16;
			this->checkBoxXRAM->Text = L"Use XRAM (use 6000ms buffer for maximum effect)";
			this->toolTipWarning->SetToolTip(this->checkBoxXRAM, L"XRAM isn\'t designed for streaming as there\'s a CPU hit constantly uploading small"
				L" bits of data. Therefore use the largest buffer size as small buffer sizes will "
				L"cause issues!");
			this->checkBoxXRAM->UseVisualStyleBackColor = true;
			// 
			// tabConfiguration
			// 
			this->tabConfiguration->Controls->Add(this->tabPageConfig);
			this->tabConfiguration->Controls->Add(this->tabPageStatistics);
			this->tabConfiguration->Location = System::Drawing::Point(12, 12);
			this->tabConfiguration->Name = L"tabConfiguration";
			this->tabConfiguration->SelectedIndex = 0;
			this->tabConfiguration->Size = System::Drawing::Size(409, 361);
			this->tabConfiguration->TabIndex = 0;
			// 
			// Config
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(433, 414);
			this->Controls->Add(this->buttonReset);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonApply);
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
			this->tabPageStatistics->ResumeLayout(false);
			this->tabPageStatistics->PerformLayout();
			this->tabPageConfig->ResumeLayout(false);
			this->tabPageConfig->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBufferLength))->EndInit();
			this->tabConfiguration->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void buttonCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
			 ptrForm->Close();
		 }

private: System::Void trackBufferLength_Scroll(System::Object^  sender, System::EventArgs^  e) {
			 labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
		 }
private: System::Void buttonApply_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 ApplyChanges();
		 }
private: System::Void buttonOk_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 // call the apply button	
			this->buttonApply_Click_1(sender, e);

			// close the form
			overRide = true;
			loaded = false;
			ptrForm->Close();
		 }
private: System::Void buttonCancel_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
			 ptrForm->Close();
		 }
private: System::Void buttonReset_Click_1(System::Object^  sender, System::EventArgs^  e) {
			 trackBufferLength->Value = DEFC_BUFFER_LENGTH;
			 labelBufferLength->Text = "Buffer Length (" + trackBufferLength->Value + "ms)";
			 checkBoxXRAM->Checked = false;
			 checkBoxExpandMono->Checked = false;
			 checkBoxExpandStereo->Checked = false;
		 }
private: System::Void Config_Unload(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
		 }

private: System::Void listBoxExtensions_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
}
};
}

#endif /*__CONFIG_STATUS_FORM*/