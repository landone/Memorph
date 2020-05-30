#include <Windows.h>

#include "Hook.h"

void Hook::Patch(unsigned char* dest, unsigned char* src, unsigned int size) {

	DWORD oProc;
	VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oProc);
	memcpy(dest, src, size);
	VirtualProtect(dest, size, oProc, &oProc);

}

bool Hook::DefaultHook(char* src, char* dest, int len) {

	if (len < 5) {
		return false;
	}

	DWORD oProc;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oProc);
	memset(src, 0x90, len);
	uintptr_t relAddy = (uintptr_t)(dest - src - 5);
	*src = (char)0xE9;
	*(uintptr_t*)(src + 1) = (uintptr_t)relAddy;
	VirtualProtect(src, len, oProc, &oProc);

	return true;

}

char* Hook::TrampHook(char* src, char* dest, unsigned int len) {

	if (len < 5) {
		return nullptr;
	}

	char* gateway = (char*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (gateway == nullptr) {
		return nullptr;
	}
	memcpy(gateway, src, len);
	uintptr_t jumpAddy = (uintptr_t)(src - gateway - 5);
	*(gateway + len) = (char)0xE9;
	*(uintptr_t*)(gateway + len + 1) = jumpAddy;
	if (DefaultHook(src, dest, len)) {
		return gateway;
	}

	return nullptr;

}