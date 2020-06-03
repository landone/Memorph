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

void CSGO_WallHack::aimAt(unsigned long entAdr) {

	unsigned long clientState = *(unsigned long*)(engineBase + CSGO::dwClientState);
	float* viewAng = (float*)(clientState + CSGO::dwClientState_ViewAngles);
	glm::vec4 enemPos = glm::vec4(getBonePos(entAdr, 8), 1);
	glm::vec4 myPos = glm::vec4(getBonePos((*localPlayerPtr), 8), 1);

	float flatDist = sqrtf(pow(enemPos[0] - myPos[0], 2) + pow(enemPos[1] - myPos[1], 2));
	viewAng[0] = -atan2f(enemPos[2] - myPos[2], flatDist) * 180.0f / 3.14159f;
	viewAng[1] = atan2f(enemPos[1] - myPos[1], enemPos[0] - myPos[0]) * 180.0f / 3.14159f;

}

glm::vec3 CSGO_WallHack::getBonePos(unsigned long ent, int bone) {

	unsigned long boneMat = *((unsigned long*)(ent + CSGO::m_dwBoneMatrix));
	glm::vec3 bonePos;
	bonePos.x = *(float*)(boneMat + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(boneMat + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(boneMat + 0x30 * bone + 0x2C);
	return bonePos;

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
				targets.push_back(entAdr);
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

		aimAt(targets[i]);
		break;
		/*glm::vec4 headPos = glm::vec4(getBonePos(targets[i], 8), 1);

		glm::vec2 output;
		if (DX::WorldToScreen(headPos, viewMat, output)) {

			glm::vec2 output2;
			DX::WorldToScreen(headPos + glm::vec4(0, 0, 10, 0), viewMat, output2);
			int size = output.y - output2.y;
			DX::DrawFillRect(output - glm::vec2(size / 2, size), glm::vec2(size, size), glm::vec4(255, 0, 0, 255));

		}*/
	}
	targetMutex.unlock();

}