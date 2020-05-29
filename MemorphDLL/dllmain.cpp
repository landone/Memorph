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
	MemProc proc(TF2::processName);
	unsigned long clientBase = proc.getModule(TF2::clientDllName);
	unsigned long engineBase = proc.getModule(TF2::engineDllName);
	unsigned long entList = clientBase + TF2::dwEntityList;
	unsigned long& localPlayer = *((unsigned long*)(clientBase + TF2::dwLocalPlayer));
	int& myTeam = *((int*)(localPlayer + TF2::m_iTeam));
	int& myClass = *((int*)(localPlayer + TF2::m_iClass));

	while (true) {

		if (GetAsyncKeyState(VK_END) & 1) {
			std::cout << "END pressed\n";
			break;
		}
		
		if (myClass == TF2::Class_Spy) {

			int& activeWep = *((int*)(localPlayer + TF2::m_hActiveWeapon));
			/* Melee weapon is hMyWeapons[1] */
			int& meleeWep = *((int*)(localPlayer + TF2::m_hMyWeapons + TF2::handleSize));
			if (activeWep == meleeWep) {

				unsigned long& wepBase = *((unsigned long*)(entList + (activeWep & 0xFFF) * TF2::entityRefSize));
				bool& ready = *((bool*)(wepBase + TF2::m_bReadyToBackstab));
				if (ready) {

					int& attack = *((int*)(clientBase + TF2::dwAttack));
					attack = 5;

				}

			}

		}

		//system("cls");
		float* viewAngles = ((float*)(engineBase + TF2::dwViewAngles));
		float* myPos = ((float*)(localPlayer + TF2::m_vecOrigin));
		for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

			unsigned long& entAdr = *((unsigned long*)(entList + i * TF2::entityRefSize));
			if (entAdr != NULL && entAdr != localPlayer) {

				int& team = *((int*)(entAdr + TF2::m_iTeam));
				if (team > 1 && team != myTeam) {

					float* pos = ((float*)(entAdr + TF2::m_vecOrigin));
					float angle = atan2f(pos[1] - myPos[1], pos[0] - myPos[0]) * 180.0f / 3.14159f;
					viewAngles[1] = angle;
					break;

				}

			}

		}

		Sleep(12);

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