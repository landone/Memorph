#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <string>

//https://www.youtube.com/watch?v=PWhv0C0jD2k
//https://github.com/frk1/hazedumper/blob/master/csgo.cs

HANDLE hProc = NULL;
DWORD pid;

HMODULE GetModule(std::string name)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			TCHAR szModName[MAX_PATH];
			if (GetModuleBaseName(hProc, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				std::wstring wstr(szModName);
				std::string str(wstr.begin(), wstr.end());
				if (str == name) {
					return hMods[i];
				}
			}
		}
	}

	return NULL;
}

void writeMem(const char* data, int len, DWORD address) {

	WriteProcessMemory(hProc, (LPVOID)address, data, len, 0);

}

void readMem(const char* buf, int len, DWORD address) {

	ReadProcessMemory(hProc, (LPCVOID)address, (LPVOID)buf, len, 0);

}

bool attachProc(const char* procName) {

	PROCESSENTRY32 procEnt;
	procEnt.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcSnap == INVALID_HANDLE_VALUE) {
		std::cout << "Failed snapshot" << std::endl;
		return false;
	}

	while (Process32Next(hProcSnap, &procEnt)) {

		char name[260];
		char DefChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, procEnt.szExeFile, -1, name, 260, &DefChar, NULL);

		if (!strcmp(procName, name)) {
			std::cout << "Process found (" << procEnt.th32ProcessID << ")" << std::endl;
			pid = procEnt.th32ProcessID;
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProc == NULL) {
				std::cout << "Failed opening process" << std::endl;
				CloseHandle(hProcSnap);
				return false;
			}
			CloseHandle(hProcSnap);
			return true;
		}

	}

	std::cout << "Unable to find process" << std::endl;
	CloseHandle(hProcSnap);
	return false;

}

int main() {

	DWORD localPlayerOffs = 0xD2FB94;
	DWORD flashOffs = 0xA410;
	DWORD healthOffs = 0x100;
	DWORD localPlayer = NULL;
	std::string procName = "csgo.exe";

	if (attachProc(procName.c_str())) {

		HMODULE mod = GetModule("client_panorama.dll");
		readMem((char*)&localPlayer, sizeof(localPlayer), (DWORD)mod + localPlayerOffs);
		if (localPlayer == NULL) {
			std::cout << "Local player non-existant" << std::endl;
		}
		else {
			while (true) {
				int flashDur = 0;
				readMem((char*)&flashDur, sizeof(flashDur), localPlayer + flashOffs);
				if (flashDur > 0) {
					flashDur = 0;
					writeMem((char*)&flashDur, sizeof(flashDur), localPlayer + flashOffs);
				}
				/*int health = -1;
				readMem((char*)&health, sizeof(health), localPlayer + healthOffs);
				std::cout << "Health: " << health << std::endl;*/
				Sleep(50);
				system("cls");

			}
		}
	}

	system("pause");

	return 0;

}