#include <Windows.h>
#include <iostream>

#include "Memorph.h"
#include "TF2_WallHack.h"
#include "CSGO_WallHack.h"

DWORD WINAPI HackThread(HMODULE hModule) {

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Successfully injected" << std::endl;

	TF2_WallHack hack;
	Memorph::addHack(hack);
	Memorph::run();
	Memorph::stop();

	if (f != 0) {
		fclose(f);
	}
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hand = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		if (hand != 0) {
			CloseHandle(hand);
		}
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;

}