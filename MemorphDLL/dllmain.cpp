#include "stdafx.h"
#include "mem.h"
#include "proc.h"
#include "MemProc.h"
#include "TF2.h"
#include "Includes.h"
#include <glm/glm.hpp>

void* d3d9Device[119];
BYTE EndSceneByte[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;

float* viewMatrix = nullptr;
std::vector<float*> targets;

bool WorldToScreen(const glm::vec4& pos, const glm::mat4& matrix, glm::vec2& output) {

	glm::vec4 product = pos * matrix;
	if (product.w < 0.1) {
		return false;
	}

	glm::vec3 NDC = glm::vec3(product) / product.w;
	output.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	output.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	return true;

}

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {

	if (!pDevice) {
		pDevice = o_pDevice;
	}

	/* Draw Here */
	if (viewMatrix) {
		glm::mat4 viewMat;
		memcpy(&viewMat[0][0], viewMatrix, sizeof(float) * 16);
		for (int i = 0; i < targets.size(); i++) {
			glm::vec4 targetPos;
			targetPos.x = targets[i][0];
			targetPos.y = targets[i][1];
			targetPos.z = targets[i][2];
			targetPos.w = 1;

			glm::vec2 output;
			if (WorldToScreen(targetPos, viewMat, output)) {

				glm::vec2 output2;
				WorldToScreen(targetPos + glm::vec4(0, 0, 45, 0), viewMat, output2);
				int size = output.y - output2.y;
				DrawFillRect(output.x - size / 6, output.y - size, size / 3, size, D3DCOLOR_ARGB(255, 0, 0, 255));

			}
		}
	}

	oEndScene(pDevice);

}

DWORD WINAPI HackThread(HMODULE hModule) {

	/* Hook EndScene() for drawing */
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {

		/* Grab EndScene() */
		memcpy(EndSceneByte, (char*)d3d9Device[42], 7);
		/* Store original EndScene */
		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);

	}

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
		viewMatrix = (float*)(engineBase + TF2::dwViewMatrix);
		targets.clear();

		float* viewAngles = ((float*)(engineBase + TF2::dwViewAngles));
		float* myPos = ((float*)(localPlayer + TF2::m_vecOrigin));
		for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

			unsigned long& entAdr = *((unsigned long*)(entList + i * TF2::entityRefSize));
			if (entAdr != NULL && entAdr != localPlayer) {

				int& team = *((int*)(entAdr + TF2::m_iTeam));
				if (team > 1 && team != myTeam) {

					targets.push_back((float*)(entAdr + TF2::m_vecOrigin));

					/*float angle = atan2f(pos[1] - myPos[1], pos[0] - myPos[0]) * 180.0f / 3.14159f;
					viewAngles[1] = angle;*/
					
				}

			}

		}

		Sleep(12);

	}

	/* Unhook EndScene() */
	Patch((BYTE*)d3d9Device[42], EndSceneByte, 7);

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