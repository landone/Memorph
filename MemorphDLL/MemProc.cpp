#include "MemProc.h"

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <locale>
#include <codecvt>

MemProc::MemProc() {

	attachSelf();

}

MemProc::MemProc(std::string exeFile) : MemProc() {

	attach(exeFile);

}

unsigned long MemProc::FindAddress(unsigned long mod, unsigned long modSize, const unsigned char* sig, const char* mask, ScanType def, unsigned long extra) {

	if (!isAttached()) {
		return NULL;
	}

	unsigned long result = FindSignature(mod, modSize, sig, mask);

	if (def == ScanType::READ) {

		ReadProcessMemory(hProc, (LPCVOID)(result + extra), (LPVOID)&result, sizeof(result), NULL);
		result -= mod;

	}
	else if (def == ScanType::SUBTRACT) {

		result -= mod;

	}

	return result;

}

bool MemProc::DataCompare(unsigned char* data, const unsigned char* sign, const char* mask) {

	for (; *mask; mask++, sign++, data++) {

		if (*mask == 'x' && *data != *sign) {
			return false;
		}

	}

	return true;

}

unsigned long MemProc::FindSignature(unsigned long base, unsigned long size, const unsigned char* sign, const char* mask) {

	if (!isAttached()) {
		return NULL;
	}

	MEMORY_BASIC_INFORMATION mbi = { 0 };
	DWORD offset = 0;
	while (offset < size) {

		VirtualQueryEx(hProc, (LPCVOID)(base + offset), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (mbi.State != MEM_FREE) {

			unsigned char* buffer = new unsigned char[mbi.RegionSize];
			ReadProcessMemory(hProc, mbi.BaseAddress, buffer, mbi.RegionSize, NULL);
			for (int i = 0; i < mbi.RegionSize; i++) {

				if (DataCompare(buffer + i, sign, mask)) {

					delete[] buffer;
					return (unsigned long)mbi.BaseAddress + i;

				}

			}

			delete[] buffer;

		}

		offset += mbi.RegionSize;

	}

	return NULL;

}

bool MemProc::write(const void* src, int len, unsigned long addr) {

	return WriteProcessMemory(hProc, (LPVOID)addr, src, len, 0);

}

bool MemProc::read(const void* dest, int len, unsigned long addr) {

	return ReadProcessMemory(hProc, (LPCVOID)addr, (LPVOID)dest, len, 0);

}

bool MemProc::attachSelf() {

	detach();
	return openProc(GetCurrentProcessId());

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
			CloseHandle(hProcSnap);
			return openProc(procEnt.th32ProcessID);
		}

	}

	std::cout << "Unable to find process:" << exeFile << std::endl;
	CloseHandle(hProcSnap);
	return false;

}

bool MemProc::openProc(unsigned long pid) {
	
	this->pid = pid;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProc == NULL) {

		std::cout << "Failed opening process" << std::endl;
		return false;

	}

	return true;

}

bool MemProc::isAttached() {

	if (hProc == nullptr || pid <= 0) {
		return false;
	}

	DWORD exitCode;
	GetExitCodeProcess(hProc, &exitCode);
	if (exitCode != STILL_ACTIVE) {
		detach();
		return false;
	}

	return true;

}

void MemProc::detach() {

	if (hProc != nullptr) {
		CloseHandle(hProc);
		hProc = nullptr;
		pid = 0;
	}

}

unsigned long MemProc::getModule(std::string rawString, unsigned long* sizeBuf)
{

	if (!isAttached()) {
		return NULL;
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring str = converter.from_bytes(rawString);
	const wchar_t* modName = str.c_str();
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					if (sizeBuf != nullptr) {
						(*sizeBuf) = modEntry.modBaseSize;
					}
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

void MemProc::printModules()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				std::string str = converter.to_bytes(modEntry.szModule);
				std::cout << str << std::endl;
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
}