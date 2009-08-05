#include <windows.h>
#include "ConfigFile.h"
#include <stdio.h>

#define WM_WA_IPC WM_USER
#define IPC_GETINIFILE 334
#define DEFAULT_SIZE 64

namespace WinampOpenALOut {

	char ConfigFile::INI_FILE[MAX_PATH];

	static char app_name[] = "Wumpus OpenAL Output";
	static char global_name[] = "Winamp";

	void ConfigFile::ReadString(char *name,char *data, int mlen)
	{
		char buf[DEFAULT_SIZE];
		strcpy_s(buf,DEFAULT_SIZE,data);
		GetPrivateProfileString(app_name,name,buf,data,mlen,INI_FILE);
	}

	void ConfigFile::ReadGlobalString(char *name,char *data, int mlen)
	{
		char buf[DEFAULT_SIZE];
		strcpy_s(buf,DEFAULT_SIZE,data);
		GetPrivateProfileString(global_name,name,buf,data,mlen,INI_FILE);
	}

	int ConfigFile::ReadInteger(char *name)
	{
		char buf[DEFAULT_SIZE] = {'\0'};
		ConfigFile::ReadString(name, buf, DEFAULT_SIZE);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	float ConfigFile::ReadFloat(char *name)
	{
		char buf[10] = {'\0'};
		ConfigFile::ReadString(name, buf, DEFAULT_SIZE);
		
		float retVal = 0.0f;
		if(strlen(buf)>0) {
			retVal = (float)atof(buf);
		}
		return retVal;
	}

	void ConfigFile::WriteFloat(char* name, float f)
	{
		char buf[10] = {'\0'};
		sprintf_s(buf,10,"%3.2f",f);
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}

	int ConfigFile::ReadGlobalInteger(char *name)
	{
		char buf[10] = {'\0'};
		ConfigFile::ReadGlobalString(name, buf, DEFAULT_SIZE);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	bool ConfigFile::ReadBoolean(char *name)
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
		char *p;
		if (hMainWindow 
		   && (p = (char *)SendMessage(hMainWindow, WM_WA_IPC, 0, IPC_GETINIFILE))
		   &&  p!= (char *)1){
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

	void ConfigFile::WriteString(char *name, char *data)
	{
		WritePrivateProfileString(app_name,name,data,INI_FILE);
	}

	void ConfigFile::WriteInteger(char *name, int i)
	{
		char buf[DEFAULT_SIZE] = {'\0'};
		/*_itoa_s(i, buf, DEFAULT_SIZE);*/
		sprintf_s(buf, DEFAULT_SIZE,"%d\0",i);
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}

	void ConfigFile::WriteBoolean(char *name, bool v)
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
