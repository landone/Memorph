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



}

MemProc::MemProc(std::string exeFile) : MemProc() {

	attach(exeFile);

}

unsigned long MemProc::FindAddress(unsigned long mod, unsigned long modSize, const unsigned char* sig, const char* mask, ScanType def, unsigned long extra) {

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (hProc == NULL) {
		std::cout << "Failed opening process for signatures" << std::endl;
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

	CloseHandle(hProc);
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

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (hProc == NULL) {
		std::cout << "Failed opening process for signatures" << std::endl;
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
					CloseHandle(hProc);
					return (unsigned long)mbi.BaseAddress + i;

				}

			}

			delete[] buffer;

		}

		offset += mbi.RegionSize;

	}

	CloseHandle(hProc);
	return NULL;

}

bool MemProc::write(const void* src, int len, unsigned long addr) {

	return WriteProcessMemory(handle, (LPVOID)addr, src, len, 0);

}

bool MemProc::read(const void* dest, int len, unsigned long addr) {

	return ReadProcessMemory(handle, (LPCVOID)addr, (LPVOID)dest, len, 0);

}

unsigned long MemProc::getCurrentModule(std::string name, unsigned long* sizeBuf) {

	return GetModuleBaseAddress(GetCurrentProcessId(), name, sizeBuf);

}

unsigned long MemProc::getModule(std::string name) {

	if (!isAttached()) {
		return NULL;
	}

	return GetModuleBaseAddress(pid, name.c_str());

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

unsigned long MemProc::GetProcId(std::string rawString)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring str = converter.from_bytes(rawString);
	const wchar_t* procName = str.c_str();
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	return procId;
}

unsigned long MemProc::GetModuleBaseAddress(unsigned long procId, std::string rawString, unsigned long* sizeBuf)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring str = converter.from_bytes(rawString);
	const wchar_t* modName = str.c_str();
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
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