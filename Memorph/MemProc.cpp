#include "MemProc.h"

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

MemProc::MemProc() {



}

MemProc::MemProc(std::string exeFile) : MemProc() {

	attach(exeFile);

}

bool MemProc::write(const void* src, int len, unsigned long addr) {

	return WriteProcessMemory(handle, (LPVOID)addr, src, len, 0);

}

bool MemProc::read(const void* dest, int len, unsigned long addr) {

	return ReadProcessMemory(handle, (LPCVOID)addr, (LPVOID)dest, len, 0);

}

unsigned long MemProc::getModule(std::string name) {

	if (!isAttached()) {
		return NULL;
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(handle, hMods, sizeof(hMods), &cbNeeded)) {
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			TCHAR szModName[MAX_PATH];
			if (GetModuleBaseName(handle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				std::wstring wstr(szModName);
				std::string str(wstr.begin(), wstr.end());
				if (str == name) {
					return (unsigned long)hMods[i];
				}
			}
		}
	}

	return NULL;

}

bool MemProc::attach(std::string exeFile) {

	detach();

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

		if (!strcmp(exeFile.c_str(), name)) {
			pid = procEnt.th32ProcessID;
			handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (handle == NULL) {
				std::cout << "Failed opening process" << std::endl;
				CloseHandle(hProcSnap);
				return false;
			}
			CloseHandle(hProcSnap);
			return true;
		}

	}

	std::cout << "Unable to find process:" << exeFile << std::endl;
	CloseHandle(hProcSnap);
	return false;

}

bool MemProc::isAttached() {

	if (handle == nullptr || pid <= 0) {
		return false;
	}

	DWORD exitCode;
	GetExitCodeProcess(handle, &exitCode);
	if (exitCode != STILL_ACTIVE) {
		detach();
		return false;
	}

	return true;

}

void MemProc::detach() {

	if (handle != nullptr) {
		CloseHandle(handle);
		handle = nullptr;
		pid = 0;
	}

}