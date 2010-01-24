#include <windows.h>
#include "ConfigFile.h"
#include <stdio.h>

#define WM_WA_IPC WM_USER
#define IPC_GETINIFILE 334
#define DEFAULT_SIZE 64

namespace WinampOpenALOut {

	char ConfigFile::INI_FILE[MAX_PATH];

	const static char app_name[] = "Wumpus OpenAL Output";
	const static char global_name[] = "Winamp";

	void ConfigFile::ReadString(const char *name,char *data, const int mlen)
	{
		char buf[DEFAULT_SIZE];
		strcpy_s(buf,DEFAULT_SIZE,data);
		GetPrivateProfileString(app_name,name,buf,data,mlen,INI_FILE);
	}

	void ConfigFile::ReadGlobalString(const char *name,char *data, const int mlen)
	{
		char buf[DEFAULT_SIZE];
		strcpy_s(buf,DEFAULT_SIZE,data);
		GetPrivateProfileString(global_name,name,buf,data,mlen,INI_FILE);
	}

	int ConfigFile::ReadInteger(const char *name)
	{
		char buf[DEFAULT_SIZE] = {'\0'};
		ConfigFile::ReadString(name, buf, DEFAULT_SIZE);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	float ConfigFile::ReadFloat(const char *name, bool* valid)
	{
		char buf[10] = {'\0'};
		ConfigFile::ReadString(name, buf, DEFAULT_SIZE);
		
		float retVal = -0.0f;
		*valid = false;

		if(strlen(buf)>0) {
			*valid = true;
			retVal = (float)atof(buf);
		}
		return retVal;
	}

	void ConfigFile::WriteFloat(const char* name, const float f)
	{
		char buf[10] = {'\0'};
		sprintf_s(buf,10,"%3.2f",f);
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}

	int ConfigFile::ReadGlobalInteger(const char *name)
	{
		char buf[10] = {'\0'};
		ConfigFile::ReadGlobalString(name, buf, DEFAULT_SIZE);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	bool ConfigFile::ReadBoolean(const char *name)
	{
		char buf[6] = {'\0'};
		ConfigFile::ReadString(name, buf, 6);

		bool retVal = false;
		if(strcmp(buf, CONF_TRUE)==0) {
			retVal = true;
		}

		return retVal;
	}

	void ConfigFile::Initialise(HWND hMainWindow)
	{
		char *p = 0;
		if (hMainWindow 
		   && (p = (char *)SendMessage(hMainWindow, WM_WA_IPC, 0, IPC_GETINIFILE))
		   &&  (p!= (char *)1) ){
			strcpy_s(INI_FILE,MAX_PATH, p);
			strcpy_s(INI_FILE,MAX_PATH, p);
		}else{
			GetModuleFileName(NULL, INI_FILE, sizeof(INI_FILE));
			p = INI_FILE + strlen(INI_FILE);
			while (p >= INI_FILE && *p != '.') {
				p--;
			}
			strcpy_s(++p,MAX_PATH, "ini");
		}
	}

	void ConfigFile::WriteString(const char *name, const char *data)
	{
		WritePrivateProfileString(app_name,name,data,INI_FILE);
	}

	void ConfigFile::WriteInteger(const char *name, const int i)
	{
		char buf[DEFAULT_SIZE] = {'\0'};
		sprintf_s(buf, DEFAULT_SIZE,"%d\0",i);
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}

	void ConfigFile::WriteBoolean(const char *name, const bool v)
	{
		char buf[DEFAULT_SIZE] = {'\0'};
		if(v) {
			strcat_s(buf, CONF_TRUE);
		}else{
			strcat_s(buf, CONF_FALSE);
		}
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}
}
