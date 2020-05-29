#include "stdafx.h"
#include "mem.h"
#include "proc.h"
#include "MemProc.h"
#include "TF2.h"

DWORD WINAPI HackThread(HMODULE hModule) {

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Successfully injected" << std::endl;
	MemProc proc("hl2.exe");
	unsigned long modBase = proc.getModule(TF2::clientDllName);
	unsigned long entList = modBase + TF2::dwEntityList;
	unsigned long& localPlayer = *((unsigned long*)(modBase + TF2::dwLocalPlayer));

	while (true) {

		if (GetAsyncKeyState(VK_END) & 1) {
			std::cout << "END pressed\n";
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			std::cout << "NUMPAD1 pressed\n";
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			std::cout << "NUMPAD2 pressed\n";
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			std::cout << "NUMPAD3 pressed\n";
		}

		int& myTeam = *((int*)(localPlayer + TF2::m_iTeam));
		int& myClass = *((int*)(localPlayer + TF2::m_iClass));
		system("cls");
		std::cout << TF2::ClassToString(myClass) << " on " << TF2::TeamToString(myTeam) << std::endl;

		Sleep(100);

	}

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