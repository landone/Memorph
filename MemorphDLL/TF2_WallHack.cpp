#include "TF2_WallHack.h"

#include <glm/glm.hpp>
#include <iostream>

#include "DX.h"
#include "MemProc.h"
#include "TF2.h"

void TF2_WallHack::OnStart() {

	TF2::Initialize();

	MemProc proc;

	unsigned long clientSz = 0;
	unsigned long engineSz = 0;
	clientBase = proc.getModule(TF2::clientDllName, &clientSz);
	engineBase = proc.getModule(TF2::engineDllName, &engineSz);
	entList = clientBase + TF2::dwEntityList;
	localPlayerPtr = (unsigned long*)(clientBase + TF2::dwLocalPlayer);
	viewMatrixPtr = (float*)(engineBase + TF2::dwViewMatrix);

	proc.detach();

}

void TF2_WallHack::OnEnd() {

	

}

void TF2_WallHack::aimAtHead(unsigned long target) {

	if (target == NULL) {
		return;
	}

	unsigned long boneMat = *((unsigned long*)(target + TF2::dwBoneMatrix));
	if (boneMat == NULL) {
		return;
	}

	int targClass = *((int*)(target + TF2::m_iClass));
	if (targClass == TF2::Class_Spy) {
		/* Aim at disguise head */
		int disguise = *((int*)(target + TF2::m_nDisguiseClass));
		if (disguise > 0) {
			targClass = disguise;
		}
	}

	glm::vec3 headPos = getBonePos(boneMat, TF2::BoneOrder[targClass][1]);
	float* myViewOffs = (float*)((*localPlayerPtr) + TF2::m_vecViewOffset);
	glm::vec3 myHead = glm::vec3(myPos[0] + myViewOffs[0], myPos[1] + myViewOffs[1], myPos[2] + myViewOffs[2]);
	float* viewAng = (float*)(engineBase + TF2::dwViewAngles);

	float flatDist = sqrtf(pow(headPos[0] - myHead[0], 2) + pow(headPos[1] - myHead[1], 2));
	viewAng[0] = -atan2f(headPos[2] - myHead[2], flatDist) * 180.0f / 3.14159f;
	viewAng[1] = atan2f(headPos[1] - myHead[1], headPos[0] - myHead[0]) * 180.0f / 3.14159f;

}

glm::vec3 TF2_WallHack::getBonePos(unsigned long boneMat, int bone) {

	glm::vec3 bonePos;
	bonePos.x = *(float*)(boneMat + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(boneMat + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(boneMat + 0x30 * bone + 0x2C);
	return bonePos;

}

void TF2_WallHack::drawBones(unsigned long ent, const glm::mat4& viewMat) {

	unsigned long boneMat = *((unsigned long*)(ent + TF2::dwBoneMatrix));
	if (boneMat == NULL) {
		return;
	}

	int entClass = *((int*)(ent + TF2::m_iClass));
	if (entClass == TF2::Class_Spy) {
		/* Draw bones for disguise instead */
		int disguise = *((int*)(ent + TF2::m_nDisguiseClass));
		if (disguise > 0) {
			entClass = disguise;
		}
	}

	float health = *((int*)(ent + TF2::m_iHealth)) / (float)(*((int*)(ent + TF2::m_iMaxHealth)));
	glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

	glm::vec2 lastScrPos = glm::vec2(0,0);
	glm::vec3 lastPos = glm::vec3(0,0,0);

	for (int j = 0; j < TF2::BoneOrderSize; j++) {

		/* Adjust for bone index hopping */
		int changeLast = -1;
		switch (j - 1) {
		case TF2::Bone_LeftArmEnd:
		case TF2::Bone_RightArmEnd:
			changeLast = TF2::Bone_ProfileEnd;
			break;
		case TF2::Bone_LeftLegEnd:
			changeLast = TF2::Bone_TorsoEnd;
			break;
		}
		if (changeLast != -1) {
			lastPos = getBonePos(boneMat, TF2::BoneOrder[entClass][changeLast]);
			DX::WorldToScreen(lastPos, viewMat, lastScrPos);
		}

		glm::vec3 bonePos = getBonePos(boneMat, TF2::BoneOrder[entClass][j]);
		glm::vec2 boneScrPos;
		if (bonePos != glm::vec3(0, 0, 0) && lastPos != glm::vec3(0, 0, 0)) {
			if (DX::WorldToScreen(bonePos, viewMat, boneScrPos)) {
				DX::DrawLine(lastScrPos, boneScrPos, 2, color);
			}
		}
		else {
			DX::WorldToScreen(bonePos, viewMat, boneScrPos);
		}

		lastScrPos = boneScrPos;
		lastPos = bonePos;

	}

}

void TF2_WallHack::OnThink() {

	targets.clear();

	if ((*localPlayerPtr) == NULL) {
		inGame = false;
		return;
	}

	if (!inGame) {
		
		inGame = true;

		myPos = ((float*)((*localPlayerPtr) + TF2::m_vecOrigin));
		myTeam = ((int*)((*localPlayerPtr) + TF2::m_iTeam));
		myClass = ((int*)((*localPlayerPtr) + TF2::m_iClass));

	}

	float closestDist = INFINITY;
	closestTarget = NULL;
	for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

		unsigned long& entAdr = *((unsigned long*)(entList + i * TF2::entityRefSize));
		if (entAdr != NULL && entAdr != (*localPlayerPtr)) {

			int& observerMode = *((int*)(entAdr + TF2::m_iObserverMode));
			if (observerMode != 0) {
				/* Only living players */
				continue;
			}
			int& team = *((int*)(entAdr + TF2::m_iTeam));
			if (team > 1 && team != (*myTeam)) {

				targets.push_back(i);

				float* entOrig = ((float*)(entAdr + TF2::m_vecOrigin));
				/* 45.0f added to Z axis to approximate center without relying on bones */
				glm::vec3 entPos(entOrig[0], entOrig[1], entOrig[2] + 45.0f);
				glm::vec2 targScreen;
				if (DX::WorldToScreen(entPos, viewMat, targScreen)) {
					float dist = glm::distance(targScreen, DX::GetWindowDim() * 0.5f);
					if (dist < closestDist) {
						closestDist = dist;
						closestTarget = entAdr;
					}
				}

			}

		}

	}

	static bool middleMouse = false;
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
		/* On middle mouse button press */
		if (!middleMouse) {
			if (headTrack != closestTarget) {
				aimAtHead(closestTarget);
				headTrack = closestTarget;
			}
			else {
				headTrack = NULL;
			}
		}
		middleMouse = true;

	}
	else {
		middleMouse = false;
	}

}

void TF2_WallHack::OnDraw() {

	if (!inGame || !(*localPlayerPtr)) {
		return;
	}

	memcpy(&viewMat[0][0], viewMatrixPtr, sizeof(float) * 16);
	for (int i = 0; i < targets.size(); i++) {
		
		unsigned long targ = *((unsigned long*)(entList + targets[i] * TF2::entityRefSize));
		if (targ == NULL) {
			continue;
		}

		unsigned long boneMat = *((unsigned long*)(targ + TF2::dwBoneMatrix));
		if (!boneMat) {
			continue;
		}

		int targClass = *((int*)(targ + TF2::m_iClass));
		glm::vec3 targHead = getBonePos(boneMat, TF2::BoneOrder[targClass][1]);
		if (targHead == headList[targets[i]]) {
			/* Probably a bugged/stationary skeleton */
			continue;
		}
		else {
			headList[targets[i]] = targHead;
		}

		if (targ == closestTarget) {
			drawBones(closestTarget, viewMat);
			if (headTrack == closestTarget) {
				aimAtHead(headTrack);
			}
			else {
				headTrack = NULL;
			}
		}
		else {

			float health = *((int*)(targ + TF2::m_iHealth)) / (float)(*((int*)(targ + TF2::m_iMaxHealth)));
			glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

			float* targOrig = ((float*)(targ + TF2::m_vecOrigin));

			glm::vec2 targPos;
			if (DX::WorldToScreen(glm::vec3(targOrig[0], targOrig[1], targOrig[2]), viewMat, targPos)) {

				glm::vec2 targPos2;
				DX::WorldToScreen(glm::vec3(targOrig[0], targOrig[1], targOrig[2] + 45.0f), viewMat, targPos2);
				float height = abs(targPos.y - targPos2.y);
				DX::DrawFillRect(targPos - glm::vec2(height / 6.0f, height), glm::vec2(height / 3.0f, height), color);

			}

		}

	}

}