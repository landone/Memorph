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

void CSGO_WallHack::drawCrosshair() {

	glm::vec2 dim = DX::GetWindowDim();
	glm::vec2 center = dim / 2.0f;
	float* punchAng = (float*)((*localPlayerPtr) + CSGO::m_aimPunchAngle);
	center.x -= dim.x / 90.0f * punchAng[1];
	center.y += dim.y / 90.0f * punchAng[0];


	DX::DrawLine(center + glm::vec2(-crosshairLength, 0), center + glm::vec2(crosshairLength, 0), crosshairWidth, crosshairColor);
	DX::DrawLine(center + glm::vec2(0, -crosshairLength), center + glm::vec2(0, crosshairLength), crosshairWidth, crosshairColor);

}

void CSGO_WallHack::aimAt(unsigned long entAdr) {

	unsigned long clientState = *(unsigned long*)(engineBase + CSGO::dwClientState);
	float* viewAng = (float*)(clientState + CSGO::dwClientState_ViewAngles);
	unsigned long boneMat = *((unsigned long*)(entAdr + CSGO::m_dwBoneMatrix));
	glm::vec4 enemPos = glm::vec4(getBonePos(boneMat, 8), 1);
	glm::vec4 myPos = glm::vec4(getBonePos(myBoneMat, 8), 1);

	float flatDist = sqrtf(pow(enemPos[0] - myPos[0], 2) + pow(enemPos[1] - myPos[1], 2));
	viewAng[0] = -atan2f(enemPos[2] - myPos[2], flatDist) * 180.0f / 3.14159f;
	viewAng[1] = atan2f(enemPos[1] - myPos[1], enemPos[0] - myPos[0]) * 180.0f / 3.14159f;

}

glm::vec3 CSGO_WallHack::getBonePos(unsigned long boneMat, int bone) {

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
		myBoneMat = (*localPlayerPtr) + CSGO::m_dwBoneMatrix;

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

		unsigned long boneMat = *((unsigned long*)(targets[i] + CSGO::m_dwBoneMatrix));
		glm::vec2 lastPos;
		if (!DX::WorldToScreen(getBonePos(boneMat, CSGO::BoneOrder[0]), viewMat, lastPos)) {
			continue;
		}
		
		float health = *((int*)(targets[i] + CSGO::m_iHealth)) / 100.0f;
		glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

		for (int j = 1; j < CSGO::BoneOrderSize; j++) {

			/* Adjust for bone hopping */
			int changeLast = -1;
			switch (j - 1) {
			case CSGO::Bone_LeftArmEnd:
			case CSGO::Bone_RightArmEnd:
				changeLast = CSGO::Bone_ProfileEnd;
				break;
			case CSGO::Bone_LeftLegEnd:
				changeLast = CSGO::Bone_TorsoEnd;
				break;
			}
			if (changeLast != -1) {
				DX::WorldToScreen(getBonePos(boneMat, CSGO::BoneOrder[changeLast]), viewMat, lastPos);
			}

			glm::vec2 bonePos;
			DX::WorldToScreen(getBonePos(boneMat, CSGO::BoneOrder[j]), viewMat, bonePos);
			DX::DrawLine(lastPos, bonePos, 2, color);
			lastPos = bonePos;

		}

	}
	targetMutex.unlock();

	drawCrosshair();

}