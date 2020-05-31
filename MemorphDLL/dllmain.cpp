#include <vector>
#include <iostream>

#include "Hack.h"
#include "TF2.h"
#include "TF2_WallHack.h"
#include "CSGO_WallHack.h"
#include "DX.h"

CSGO_WallHack wallHack;
std::vector<Hack*> hacks = {
	&wallHack
};

void hkEndScene() {
	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnDraw();
	}
}

DWORD WINAPI HackThread(HMODULE hModule) {

	DX::HookEndScene(hkEndScene);

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Memorph: Successfully injected" << std::endl;

	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnStart();
	}

	while (true) {

		if (GetAsyncKeyState(VK_END) & 1) {
			std::cout << "END pressed\n";
			break;
		}

		for (int i = 0; i < hacks.size(); i++) {
			hacks[i]->OnThink();
		}

		Sleep(12);

	}

	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnEnd();
	}

	DX::UnhookEndScene();

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