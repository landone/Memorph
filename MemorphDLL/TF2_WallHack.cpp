#include "TF2_WallHack.h"

#include <glm/glm.hpp>
#include <iostream>

#include "DX.h"
#include "MemProc.h"
#include "TF2.h"

void TF2_WallHack::OnStart() {

	clientBase = MemProc::getCurrentModule(TF2::clientDllName);
	engineBase = MemProc::getCurrentModule(TF2::engineDllName);
	entList = clientBase + TF2::dwEntityList;
	localPlayerPtr = ((unsigned long*)(clientBase + TF2::dwLocalPlayer));
	myTeam = ((int*)((*localPlayerPtr) + TF2::m_iTeam));
	myClass = ((int*)((*localPlayerPtr) + TF2::m_iClass));

}

void TF2_WallHack::OnEnd() {

	

}

void TF2_WallHack::OnThink() {

	if ((*myClass) == TF2::Class_Spy) {

		int& activeWep = *((int*)((*localPlayerPtr) + TF2::m_hActiveWeapon));
		int& meleeWep = *((int*)((*localPlayerPtr) + TF2::m_hMyWeapons + TF2::handleSize));
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
	float* myPos = ((float*)((*localPlayerPtr) + TF2::m_vecOrigin));
	for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

		unsigned long& entAdr = *((unsigned long*)(entList + i * TF2::entityRefSize));
		if (entAdr != NULL && entAdr != (*localPlayerPtr)) {

			int& team = *((int*)(entAdr + TF2::m_iTeam));
			if (team > 1 && team != (*myTeam)) {

				targets.push_back((float*)(entAdr + TF2::m_vecOrigin));

				//float angle = atan2f(pos[1] - myPos[1], pos[0] - myPos[0]) * 180.0f / 3.14159f;
				//viewAngles[1] = angle;

			}

		}

	}

}

void TF2_WallHack::OnDraw() {

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
			if (DX::WorldToScreen(targetPos, viewMat, output)) {

				glm::vec2 output2;
				DX::WorldToScreen(targetPos + glm::vec4(0, 0, 45, 0), viewMat, output2);
				int size = output.y - output2.y;
				DX::DrawFillRect(output - glm::vec2(size / 6, size), glm::vec2(size / 3, size), glm::vec4(255, 0, 0, 255));

			}
		}
	}

}