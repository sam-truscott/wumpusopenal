#include <windows.h>
#include "ConfigFile.h"

#define WM_WA_IPC WM_USER
#define IPC_GETINIFILE 334

namespace WinampOpenALOut {

	char ConfigFile::INI_FILE[MAX_PATH];

	static char app_name[] = "Wumpus OpenAL Output";
	static char global_name[] = "Winamp";

	void ConfigFile::ReadString(char *name,char *data, int mlen) {
		char buf[2048];
		strcpy_s(buf,2048,data);
		GetPrivateProfileString(app_name,name,buf,data,mlen,INI_FILE);
	}

	void ConfigFile::ReadGlobalString(char *name,char *data, int mlen) {
		char buf[2048];
		strcpy_s(buf,2048,data);
		GetPrivateProfileString(global_name,name,buf,data,mlen,INI_FILE);
	}

	int ConfigFile::ReadInteger(char *name) {
		char buf[10] = {'\0'};
		ConfigFile::ReadString(name, buf, 2048);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	int ConfigFile::ReadGlobalInteger(char *name) {
		char buf[10] = {'\0'};
		ConfigFile::ReadGlobalString(name, buf, 2048);
		
		int retVal = -1;
		if(strlen(buf)>0) {
			retVal = atoi(buf);
		}
		return retVal;
	}

	bool ConfigFile::ReadBoolean(char *name) {
		char buf[6] = {'\0'};
		ConfigFile::ReadString(name, buf, 6);

		bool retVal = false;
		if(strcmp(buf, CONF_TRUE)==0) {
			retVal = true;
		}

		return retVal;
	}

	void ConfigFile::Initialise(HWND hMainWindow){
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

	void ConfigFile::WriteString(char *name, char *data) {
		WritePrivateProfileString(app_name,name,data,INI_FILE);
	}

	void ConfigFile::WriteInteger(char *name, int i) {
		char buf[10] = {'\0'};
		_itoa_s(i, buf, 10);
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}

	void ConfigFile::WriteBoolean(char *name, bool v) {
		char buf[128] = {'\0'};
		if(v) {
			strcat_s(buf, CONF_TRUE);
		}else{
			strcat_s(buf, CONF_FALSE);
		}
		WritePrivateProfileString(app_name,name,buf,INI_FILE);
	}
}
