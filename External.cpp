#include <windows.h>
#include <stdio.h>
#include "Out_Openal.h"

typedef Out_Module* (__cdecl *getInstanceType)();

int main(int argc, char* argv[]) {

	Out_Module *m = GetPluginInstance();

	if(m == NULL) {
		printf("Failed to get instance - exit\n");
		return -3;
	}

	printf("Created handle OK!\n");

	m->Init();

	printf("Initialised OK\n");

	m->About(NULL);

	int maxLatency = m->Open(22050,2,16,0,0);

	m->SetVolume(255);

	if(maxLatency == -1) {
		printf("Unable to open Plug-in\n");
		return -4;
	}

	printf("Opened OK: Max Latency=%d\n", maxLatency);

	FILE *file;
	if(fopen_s(&file, "c:\\windows\\media\\tada.wav","r+b")!=0) {
		printf("Couldn't open tada.wav!\n");
	}

	char* buffer = new char[4096];
	memset(buffer, 0, 4096);
	// should read the header but this is quick demo
	// so no need - will cause a pop
	int bytes = (int)fread(buffer, sizeof(char), 4096, file);
	//printf("Wrote %d bytes\n", bytes);

	if(bytes > 0) {
		int writeResult = m->Write(buffer, bytes);
		if(writeResult != 0) {
			printf("First write failed - exit\n");
		}else{

			int writeSize = 4096;

			while(true) {

				writeSize = m->CanWrite();
				//printf("We can write %d bytes\n", writeSize);
				if(writeSize > 0) {
					delete buffer;
					buffer = NULL;
					buffer = new char[writeSize];
					//printf("Trying to read %d bytes\n", writeSize);
					bytes = (int)fread(buffer, sizeof(char), writeSize, file);
					if(bytes <= 0) { 
						//printf("Ran outta data\n");
					}else{
						//printf("Wrote %d bytes\n", bytes);
						m->Write(buffer, bytes);
					}
				}

				int out = m->GetOutputTime();
				int written = m->GetWrittenTime();

				printf("written: %dms, played: %dms\n", written, out);

				if(m->IsPlaying() == 0) {
					printf("Stopped playing\n");
					break;
				}

				Sleep(100);
			}
		}
	}else{
		printf("Failed to read tada!\n");
	}

	delete buffer;
	buffer = NULL;

	m->Close();

	fclose(file);

	m->Quit();

	printf("Quit OK\n");

	printf("PRESS ANY KEY TO EXIT\n");
	getchar();

//	FreeLibrary(hDLL);

	return 0;
}