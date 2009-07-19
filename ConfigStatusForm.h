#ifndef __CONFIG_STATUS_FORM
#define __CONFIG_STATUS_FORM

#include "Constants.h"
#include "Framework\aldlist.h"
#include "ConfigFile.h"
#include "Out_Effects.h"

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
		Config(class Output_Wumpus *aPtrOw);

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

		void ShowDeviceDetails();

		void Load();

	protected:

	private: void ApplyChanges();

		static void ThreadProcedure();
		void DoUpdate();
		void UpdateMatrix(char speaker, char xyz, Decimal value); 

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


private: System::Windows::Forms::TabPage^  tabPageConfig;
private: System::Windows::Forms::CheckBox^  checkBoxExpandStereo;
private: System::Windows::Forms::CheckBox^  checkBoxExpandMono;
private: System::Windows::Forms::Label^  label4;

private: System::Windows::Forms::Label^  labelBufferLength;
private: System::Windows::Forms::TrackBar^  trackBufferLength;




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


private: System::Windows::Forms::Label^  labelLatency;
private: System::Windows::Forms::Label^  labelPlayedMs;
private: System::Windows::Forms::Label^  labelWrittenMs;
private: System::Windows::Forms::Label^  labelPlayedB;
private: System::Windows::Forms::Label^  labelWrittenB;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TabPage^  tabPageEffects;
private: System::Windows::Forms::CheckBox^  checkBoxEfxEnabled;
private: System::Windows::Forms::ComboBox^  comboBoxEffect;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::CheckBox^  checkBoxSplit;
private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Button^  buttonOk;
private: System::Windows::Forms::Button^  buttonCancel;
private: System::Windows::Forms::Button^  buttonApply;
private: System::Windows::Forms::NumericUpDown^  numFLy;


private: System::Windows::Forms::NumericUpDown^  numFLx;
private: System::Windows::Forms::NumericUpDown^  numRLz;


private: System::Windows::Forms::NumericUpDown^  numFLz;
private: System::Windows::Forms::NumericUpDown^  numRLy;


private: System::Windows::Forms::NumericUpDown^  numRLx;
private: System::Windows::Forms::NumericUpDown^  numSz;


private: System::Windows::Forms::NumericUpDown^  numSy;

private: System::Windows::Forms::NumericUpDown^  numSx;


private: System::Windows::Forms::NumericUpDown^  numCz;

private: System::Windows::Forms::NumericUpDown^  numCy;

private: System::Windows::Forms::NumericUpDown^  numCx;
private: System::Windows::Forms::NumericUpDown^  numRRz;



private: System::Windows::Forms::NumericUpDown^  numRRy;

private: System::Windows::Forms::NumericUpDown^  numRRx;

private: System::Windows::Forms::NumericUpDown^  numFRz;

private: System::Windows::Forms::NumericUpDown^  numFRy;

private: System::Windows::Forms::NumericUpDown^  numFRx;

private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label33;

















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
			this->checkBoxXRAM = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandStereo = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExpandMono = (gcnew System::Windows::Forms::CheckBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->labelBufferLength = (gcnew System::Windows::Forms::Label());
			this->trackBufferLength = (gcnew System::Windows::Forms::TrackBar());
			this->listBoxExtensions = (gcnew System::Windows::Forms::ListBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->comboBoxDevices = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPageEffects = (gcnew System::Windows::Forms::TabPage());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numSz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numSy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numSx = (gcnew System::Windows::Forms::NumericUpDown());
			this->numCz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numCy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numCx = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRRz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRRy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRRx = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFRz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFRy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFRx = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRLz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFLz = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRLy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numRLx = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFLy = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFLx = (gcnew System::Windows::Forms::NumericUpDown());
			this->checkBoxSplit = (gcnew System::Windows::Forms::CheckBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->checkBoxEfxEnabled = (gcnew System::Windows::Forms::CheckBox());
			this->comboBoxEffect = (gcnew System::Windows::Forms::ComboBox());
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
			this->toolTipWarning = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTipInfo = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->buttonReset = (gcnew System::Windows::Forms::Button());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->tabConfiguration->SuspendLayout();
			this->tabPageConfig->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->BeginInit();
			this->tabPageEffects->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSx))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCx))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRx))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRx))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLz))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLx))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLx))->BeginInit();
			this->tabPageStatistics->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabConfiguration
			// 
			this->tabConfiguration->Controls->Add(this->tabPageConfig);
			this->tabConfiguration->Controls->Add(this->tabPageEffects);
			this->tabConfiguration->Controls->Add(this->tabPageStatistics);
			this->tabConfiguration->Location = System::Drawing::Point(12, 12);
			this->tabConfiguration->Name = L"tabConfiguration";
			this->tabConfiguration->SelectedIndex = 0;
			this->tabConfiguration->Size = System::Drawing::Size(409, 361);
			this->tabConfiguration->TabIndex = 0;
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
			this->trackBufferLength->Size = System::Drawing::Size(386, 45);
			this->trackBufferLength->SmallChange = 250;
			this->trackBufferLength->TabIndex = 10;
			this->trackBufferLength->TickFrequency = 256;
			this->toolTipInfo->SetToolTip(this->trackBufferLength, L"Control the maximum amount of data for OpenAL to buffer in RAM");
			this->trackBufferLength->Value = 2000;
			this->trackBufferLength->Scroll += gcnew System::EventHandler(this, &Config::trackBufferLength_Scroll);
			// 
			// listBoxExtensions
			// 
			this->listBoxExtensions->FormattingEnabled = true;
			this->listBoxExtensions->Location = System::Drawing::Point(15, 101);
			this->listBoxExtensions->Name = L"listBoxExtensions";
			this->listBoxExtensions->Size = System::Drawing::Size(380, 147);
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
			// tabPageEffects
			// 
			this->tabPageEffects->Controls->Add(this->label28);
			this->tabPageEffects->Controls->Add(this->label29);
			this->tabPageEffects->Controls->Add(this->label30);
			this->tabPageEffects->Controls->Add(this->label31);
			this->tabPageEffects->Controls->Add(this->label32);
			this->tabPageEffects->Controls->Add(this->label33);
			this->tabPageEffects->Controls->Add(this->label22);
			this->tabPageEffects->Controls->Add(this->label23);
			this->tabPageEffects->Controls->Add(this->label24);
			this->tabPageEffects->Controls->Add(this->label25);
			this->tabPageEffects->Controls->Add(this->label26);
			this->tabPageEffects->Controls->Add(this->label27);
			this->tabPageEffects->Controls->Add(this->label19);
			this->tabPageEffects->Controls->Add(this->label20);
			this->tabPageEffects->Controls->Add(this->label21);
			this->tabPageEffects->Controls->Add(this->label18);
			this->tabPageEffects->Controls->Add(this->label17);
			this->tabPageEffects->Controls->Add(this->label15);
			this->tabPageEffects->Controls->Add(this->label14);
			this->tabPageEffects->Controls->Add(this->label13);
			this->tabPageEffects->Controls->Add(this->label12);
			this->tabPageEffects->Controls->Add(this->label11);
			this->tabPageEffects->Controls->Add(this->label9);
			this->tabPageEffects->Controls->Add(this->label2);
			this->tabPageEffects->Controls->Add(this->numSz);
			this->tabPageEffects->Controls->Add(this->numSy);
			this->tabPageEffects->Controls->Add(this->numSx);
			this->tabPageEffects->Controls->Add(this->numCz);
			this->tabPageEffects->Controls->Add(this->numCy);
			this->tabPageEffects->Controls->Add(this->numCx);
			this->tabPageEffects->Controls->Add(this->numRRz);
			this->tabPageEffects->Controls->Add(this->numRRy);
			this->tabPageEffects->Controls->Add(this->numRRx);
			this->tabPageEffects->Controls->Add(this->numFRz);
			this->tabPageEffects->Controls->Add(this->numFRy);
			this->tabPageEffects->Controls->Add(this->numFRx);
			this->tabPageEffects->Controls->Add(this->numRLz);
			this->tabPageEffects->Controls->Add(this->numFLz);
			this->tabPageEffects->Controls->Add(this->numRLy);
			this->tabPageEffects->Controls->Add(this->numRLx);
			this->tabPageEffects->Controls->Add(this->numFLy);
			this->tabPageEffects->Controls->Add(this->numFLx);
			this->tabPageEffects->Controls->Add(this->checkBoxSplit);
			this->tabPageEffects->Controls->Add(this->label3);
			this->tabPageEffects->Controls->Add(this->checkBoxEfxEnabled);
			this->tabPageEffects->Controls->Add(this->comboBoxEffect);
			this->tabPageEffects->Location = System::Drawing::Point(4, 22);
			this->tabPageEffects->Name = L"tabPageEffects";
			this->tabPageEffects->Padding = System::Windows::Forms::Padding(3);
			this->tabPageEffects->Size = System::Drawing::Size(401, 335);
			this->tabPageEffects->TabIndex = 2;
			this->tabPageEffects->Text = L"3D/Effects";
			this->tabPageEffects->UseVisualStyleBackColor = true;
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Location = System::Drawing::Point(306, 236);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(12, 13);
			this->label28->TabIndex = 47;
			this->label28->Text = L"z";
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Location = System::Drawing::Point(306, 210);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(12, 13);
			this->label29->TabIndex = 46;
			this->label29->Text = L"y";
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->Location = System::Drawing::Point(306, 186);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(12, 13);
			this->label30->TabIndex = 45;
			this->label30->Text = L"x";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->Location = System::Drawing::Point(306, 136);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(12, 13);
			this->label31->TabIndex = 44;
			this->label31->Text = L"z";
			// 
			// label32
			// 
			this->label32->AutoSize = true;
			this->label32->Location = System::Drawing::Point(306, 110);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(12, 13);
			this->label32->TabIndex = 43;
			this->label32->Text = L"y";
			// 
			// label33
			// 
			this->label33->AutoSize = true;
			this->label33->Location = System::Drawing::Point(306, 86);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(12, 13);
			this->label33->TabIndex = 42;
			this->label33->Text = L"x";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(9, 236);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(12, 13);
			this->label22->TabIndex = 41;
			this->label22->Text = L"z";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(9, 210);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(12, 13);
			this->label23->TabIndex = 40;
			this->label23->Text = L"y";
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(9, 186);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(12, 13);
			this->label24->TabIndex = 39;
			this->label24->Text = L"x";
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(9, 136);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(12, 13);
			this->label25->TabIndex = 38;
			this->label25->Text = L"z";
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Location = System::Drawing::Point(9, 110);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(12, 13);
			this->label26->TabIndex = 37;
			this->label26->Text = L"y";
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(9, 86);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(12, 13);
			this->label27->TabIndex = 36;
			this->label27->Text = L"x";
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(153, 238);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(12, 13);
			this->label19->TabIndex = 35;
			this->label19->Text = L"z";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(153, 212);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(12, 13);
			this->label20->TabIndex = 34;
			this->label20->Text = L"y";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(153, 188);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(12, 13);
			this->label21->TabIndex = 33;
			this->label21->Text = L"x";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(153, 138);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(12, 13);
			this->label18->TabIndex = 32;
			this->label18->Text = L"z";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(153, 112);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(12, 13);
			this->label17->TabIndex = 31;
			this->label17->Text = L"y";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(153, 88);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(12, 13);
			this->label15->TabIndex = 30;
			this->label15->Text = L"x";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(321, 172);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(58, 13);
			this->label14->TabIndex = 29;
			this->label14->Text = L"Rear Right";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(27, 172);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(51, 13);
			this->label13->TabIndex = 28;
			this->label13->Text = L"Rear Left";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(168, 172);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(61, 13);
			this->label12->TabIndex = 27;
			this->label12->Text = L"Sub woofer";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(321, 70);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(59, 13);
			this->label11->TabIndex = 26;
			this->label11->Text = L"Front Right";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(168, 70);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(38, 13);
			this->label9->TabIndex = 25;
			this->label9->Text = L"Centre";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(27, 70);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 13);
			this->label2->TabIndex = 24;
			this->label2->Text = L"Front Left";
			// 
			// numSz
			// 
			this->numSz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numSz->Location = System::Drawing::Point(171, 240);
			this->numSz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numSz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numSz->Name = L"numSz";
			this->numSz->Size = System::Drawing::Size(67, 20);
			this->numSz->TabIndex = 23;
			this->numSz->ValueChanged += gcnew System::EventHandler(this, &Config::numSz_ValueChanged);
			// 
			// numSy
			// 
			this->numSy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numSy->Location = System::Drawing::Point(171, 214);
			this->numSy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numSy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numSy->Name = L"numSy";
			this->numSy->Size = System::Drawing::Size(67, 20);
			this->numSy->TabIndex = 22;
			this->numSy->ValueChanged += gcnew System::EventHandler(this, &Config::numSy_ValueChanged);
			// 
			// numSx
			// 
			this->numSx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numSx->Location = System::Drawing::Point(171, 188);
			this->numSx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numSx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numSx->Name = L"numSx";
			this->numSx->Size = System::Drawing::Size(67, 20);
			this->numSx->TabIndex = 21;
			this->numSx->ValueChanged += gcnew System::EventHandler(this, &Config::numSx_ValueChanged);
			// 
			// numCz
			// 
			this->numCz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numCz->Location = System::Drawing::Point(171, 138);
			this->numCz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numCz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numCz->Name = L"numCz";
			this->numCz->Size = System::Drawing::Size(67, 20);
			this->numCz->TabIndex = 20;
			this->numCz->ValueChanged += gcnew System::EventHandler(this, &Config::numCz_ValueChanged);
			// 
			// numCy
			// 
			this->numCy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numCy->Location = System::Drawing::Point(171, 112);
			this->numCy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numCy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numCy->Name = L"numCy";
			this->numCy->Size = System::Drawing::Size(67, 20);
			this->numCy->TabIndex = 19;
			this->numCy->ValueChanged += gcnew System::EventHandler(this, &Config::numCy_ValueChanged);
			// 
			// numCx
			// 
			this->numCx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numCx->Location = System::Drawing::Point(171, 86);
			this->numCx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numCx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numCx->Name = L"numCx";
			this->numCx->Size = System::Drawing::Size(67, 20);
			this->numCx->TabIndex = 18;
			this->numCx->ValueChanged += gcnew System::EventHandler(this, &Config::numCx_ValueChanged);
			// 
			// numRRz
			// 
			this->numRRz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRRz->Location = System::Drawing::Point(324, 240);
			this->numRRz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRRz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRRz->Name = L"numRRz";
			this->numRRz->Size = System::Drawing::Size(67, 20);
			this->numRRz->TabIndex = 17;
			this->numRRz->ValueChanged += gcnew System::EventHandler(this, &Config::numRRz_ValueChanged);
			// 
			// numRRy
			// 
			this->numRRy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRRy->Location = System::Drawing::Point(324, 214);
			this->numRRy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRRy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRRy->Name = L"numRRy";
			this->numRRy->Size = System::Drawing::Size(67, 20);
			this->numRRy->TabIndex = 16;
			this->numRRy->ValueChanged += gcnew System::EventHandler(this, &Config::numRRy_ValueChanged);
			// 
			// numRRx
			// 
			this->numRRx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRRx->Location = System::Drawing::Point(324, 188);
			this->numRRx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRRx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRRx->Name = L"numRRx";
			this->numRRx->Size = System::Drawing::Size(67, 20);
			this->numRRx->TabIndex = 15;
			this->numRRx->ValueChanged += gcnew System::EventHandler(this, &Config::numRRx_ValueChanged);
			// 
			// numFRz
			// 
			this->numFRz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFRz->Location = System::Drawing::Point(324, 138);
			this->numFRz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFRz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFRz->Name = L"numFRz";
			this->numFRz->Size = System::Drawing::Size(67, 20);
			this->numFRz->TabIndex = 14;
			this->numFRz->ValueChanged += gcnew System::EventHandler(this, &Config::numFRz_ValueChanged);
			// 
			// numFRy
			// 
			this->numFRy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFRy->Location = System::Drawing::Point(324, 112);
			this->numFRy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFRy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFRy->Name = L"numFRy";
			this->numFRy->Size = System::Drawing::Size(67, 20);
			this->numFRy->TabIndex = 13;
			this->numFRy->ValueChanged += gcnew System::EventHandler(this, &Config::numFRy_ValueChanged);
			// 
			// numFRx
			// 
			this->numFRx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFRx->Location = System::Drawing::Point(324, 86);
			this->numFRx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFRx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFRx->Name = L"numFRx";
			this->numFRx->Size = System::Drawing::Size(67, 20);
			this->numFRx->TabIndex = 12;
			this->numFRx->ValueChanged += gcnew System::EventHandler(this, &Config::numFRx_ValueChanged);
			// 
			// numRLz
			// 
			this->numRLz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRLz->Location = System::Drawing::Point(27, 240);
			this->numRLz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRLz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRLz->Name = L"numRLz";
			this->numRLz->Size = System::Drawing::Size(67, 20);
			this->numRLz->TabIndex = 11;
			this->numRLz->ValueChanged += gcnew System::EventHandler(this, &Config::numRLz_ValueChanged);
			// 
			// numFLz
			// 
			this->numFLz->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFLz->Location = System::Drawing::Point(27, 138);
			this->numFLz->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFLz->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFLz->Name = L"numFLz";
			this->numFLz->Size = System::Drawing::Size(67, 20);
			this->numFLz->TabIndex = 10;
			this->numFLz->ValueChanged += gcnew System::EventHandler(this, &Config::numFLz_ValueChanged);
			// 
			// numRLy
			// 
			this->numRLy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRLy->Location = System::Drawing::Point(27, 214);
			this->numRLy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRLy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRLy->Name = L"numRLy";
			this->numRLy->Size = System::Drawing::Size(67, 20);
			this->numRLy->TabIndex = 9;
			this->numRLy->ValueChanged += gcnew System::EventHandler(this, &Config::numRLy_ValueChanged);
			// 
			// numRLx
			// 
			this->numRLx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numRLx->Location = System::Drawing::Point(27, 188);
			this->numRLx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numRLx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numRLx->Name = L"numRLx";
			this->numRLx->Size = System::Drawing::Size(67, 20);
			this->numRLx->TabIndex = 8;
			this->numRLx->ValueChanged += gcnew System::EventHandler(this, &Config::numRLx_ValueChanged);
			// 
			// numFLy
			// 
			this->numFLy->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFLy->Location = System::Drawing::Point(27, 112);
			this->numFLy->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFLy->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFLy->Name = L"numFLy";
			this->numFLy->Size = System::Drawing::Size(67, 20);
			this->numFLy->TabIndex = 5;
			this->numFLy->ValueChanged += gcnew System::EventHandler(this, &Config::numFLy_ValueChanged);
			// 
			// numFLx
			// 
			this->numFLx->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 65536});
			this->numFLx->Location = System::Drawing::Point(27, 86);
			this->numFLx->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numFLx->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
			this->numFLx->Name = L"numFLx";
			this->numFLx->Size = System::Drawing::Size(67, 20);
			this->numFLx->TabIndex = 4;
			this->numFLx->ValueChanged += gcnew System::EventHandler(this, &Config::numFLx_ValueChanged);
			// 
			// checkBoxSplit
			// 
			this->checkBoxSplit->AutoSize = true;
			this->checkBoxSplit->Location = System::Drawing::Point(6, 40);
			this->checkBoxSplit->Name = L"checkBoxSplit";
			this->checkBoxSplit->Size = System::Drawing::Size(133, 17);
			this->checkBoxSplit->TabIndex = 3;
			this->checkBoxSplit->Text = L"Enable 3D and Effects";
			this->toolTipWarning->SetToolTip(this->checkBoxSplit, L"This requires the plug-in to seperate the data into Mono streams, CPU usage will " 
				L"increase.");
			this->checkBoxSplit->UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(6, 3);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(389, 34);
			this->label3->TabIndex = 2;
			this->label3->Text = L"OpenAL supports a wide variety of effects. The list below is some built in reverb" 
				L" effects.";
			// 
			// checkBoxEfxEnabled
			// 
			this->checkBoxEfxEnabled->AutoSize = true;
			this->checkBoxEfxEnabled->Location = System::Drawing::Point(6, 273);
			this->checkBoxEfxEnabled->Name = L"checkBoxEfxEnabled";
			this->checkBoxEfxEnabled->Size = System::Drawing::Size(101, 17);
			this->checkBoxEfxEnabled->TabIndex = 1;
			this->checkBoxEfxEnabled->Text = L"Effects Enabled";
			this->checkBoxEfxEnabled->UseVisualStyleBackColor = true;
			// 
			// comboBoxEffect
			// 
			this->comboBoxEffect->FormattingEnabled = true;
			this->comboBoxEffect->Location = System::Drawing::Point(6, 305);
			this->comboBoxEffect->Name = L"comboBoxEffect";
			this->comboBoxEffect->Size = System::Drawing::Size(389, 21);
			this->comboBoxEffect->TabIndex = 0;
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
			this->tabConfiguration->ResumeLayout(false);
			this->tabPageConfig->ResumeLayout(false);
			this->tabPageConfig->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBufferLength))->EndInit();
			this->tabPageEffects->ResumeLayout(false);
			this->tabPageEffects->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSx))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numCx))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRRx))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFRx))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLz))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numRLx))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numFLx))->EndInit();
			this->tabPageStatistics->ResumeLayout(false);
			this->tabPageStatistics->PerformLayout();
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
			 checkBoxSplit->Checked = false;
		 }
private: System::Void Config_Unload(System::Object^  sender, System::EventArgs^  e) {
			 overRide = true;
			 loaded = false;
		 }
private: System::Void numFLx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(0,0,numFLx->Value);
		 }
private: System::Void numFLy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(0,1,numFLy->Value);
		 }
private: System::Void numFLz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(0,2,numFLz->Value);
		 }
private: System::Void numFRx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(1,0,numFRx->Value);
		 }
private: System::Void numFRy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(1,1,numFRy->Value);
		 }
private: System::Void numFRz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(1,2,numFRz->Value);
		 }
private: System::Void numRLx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(2,0,numRLx->Value);
		 }
private: System::Void numRLy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(2,1,numRLy->Value);
		 }
private: System::Void numRLz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(2,2,numRLz->Value);
		 }
private: System::Void numRRx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(3,0,numRRx->Value);
		 }
private: System::Void numRRy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(3,1,numRRy->Value);
		 }
private: System::Void numRRz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(3,2,numRRz->Value);
		 }
private: System::Void numCx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(4,0,numCx->Value);
		 }
private: System::Void numCy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(4,1,numCy->Value);
		 }
private: System::Void numCz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(4,2,numCz->Value);
		 }
private: System::Void numSx_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(5,0,numSx->Value);
		 }
private: System::Void numSy_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(5,1,numSy->Value);
		 }
private: System::Void numSz_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 UpdateMatrix(5,2,numSz->Value);
		 }
};
}

#endif /*__CONFIG_STATUS_FORM*/