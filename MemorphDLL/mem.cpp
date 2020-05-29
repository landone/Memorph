#include "stdafx.h"
#include "mem.h"

static const int NOT_FUNC = 0x90;

void mem::PatchEx(void* dest, void* src, unsigned int size, HANDLE hProc) {

	DWORD oldprotect;
	VirtualProtectEx(hProc, dest, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProc, dest, src, size, nullptr);
	VirtualProtectEx(hProc, dest, size, oldprotect, &oldprotect);

}

void mem::NopEx(void* dest, unsigned int size, HANDLE hProc) {

	void* nopArray = new BYTE[size];
	memset(nopArray, NOT_FUNC, size);

	PatchEx(dest, nopArray, size, hProc);
	delete[] nopArray;

}

uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {

	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {

		ReadProcessMemory(hProc, (LPCVOID)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];

	}

	return addr;

}

void mem::Patch(void* dest, void* src, unsigned int size) {

	DWORD oldprotect;
	VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dest, src, size);
	VirtualProtect(dest, size, oldprotect, &oldprotect);

}

void mem::Nop(void* dest, unsigned int size) {

	DWORD oldprotect;
	VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dest, NOT_FUNC, size);
	VirtualProtect(dest, size, oldprotect, &oldprotect);

}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets) {

	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++) {

		addr = *(uintptr_t*)addr;
		addr += offsets[i];

	}

	return addr;

}