#include "CSGO_WallHack.h"

#include <glm/glm.hpp>
#include <iostream>

#include "DX.h"
#include "MemProc.h"
#include "CSGO.h"

void CSGO_WallHack::OnStart() {

	clientBase = MemProc::getCurrentModule(CSGO::clientDllName);
	engineBase = MemProc::getCurrentModule(CSGO::engineDllName);
	entList = clientBase + CSGO::dwEntityList;
	localPlayerPtr = (unsigned long*)(clientBase + CSGO::dwLocalPlayer);
	viewMatrix = (float*)(clientBase + CSGO::dwViewMatrix);

}

void CSGO_WallHack::OnEnd() {



}

void CSGO_WallHack::OnThink() {

	targetMutex.lock();
	targets.clear();
	targetMutex.unlock();

	if ((*localPlayerPtr) == NULL) {
		inGame = false;
		return;
	}

	if (!inGame) {

		inGame = true;

		myPos = ((float*)((*localPlayerPtr) + CSGO::m_vecOrigin));
		myObserveMode = ((int*)((*localPlayerPtr) + CSGO::m_iObserverMode));
		myTeam = ((int*)((*localPlayerPtr) + CSGO::m_iTeamNum));

	}

	for (int i = 1; i <= CSGO::MAX_PLAYERS; i++) {

		unsigned long& entAdr = *((unsigned long*)(entList + i * CSGO::entityRefSize));
		if (entAdr != NULL && entAdr != (*localPlayerPtr)) {

			int& observerMode = *((int*)(entAdr + CSGO::m_iObserverMode));
			if (observerMode != 0) {
				/* Only living players */
				continue;
			}
			int& team = *((int*)(entAdr + CSGO::m_iTeamNum));
			if (team > 1 && team != (*myTeam)) {

				targetMutex.lock();
				targets.push_back((float*)(entAdr + CSGO::m_vecOrigin));
				targetMutex.unlock();

			}

		}

	}

}

void CSGO_WallHack::OnDraw() {

	if (!inGame) {
		return;
	}

	glm::mat4 viewMat;
	memcpy(&viewMat[0][0], viewMatrix, sizeof(float) * 16);
	targetMutex.lock();
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
	targetMutex.unlock();

}