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
	localPlayerPtr = (unsigned long*)(clientBase + TF2::dwLocalPlayer);
	viewMatrixPtr = (float*)(engineBase + TF2::dwViewMatrix);

}

void TF2_WallHack::OnEnd() {

	

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

	if ((*myClass) == TF2::Class_Spy) {

		int& activeWep = *((int*)((*localPlayerPtr) + TF2::m_hActiveWeapon));
		int& meleeWep = *((int*)((*localPlayerPtr) + TF2::m_hMyWeapons + TF2::handleSize));
		if (activeWep == meleeWep && activeWep != NULL) {

			unsigned long& wepBase = *((unsigned long*)(entList + (activeWep & 0xFFF) * TF2::entityRefSize));
			if (wepBase != NULL) {
				bool& ready = *((bool*)(wepBase + TF2::m_bReadyToBackstab));
				if (ready) {

					int& attack = *((int*)(clientBase + TF2::dwAttack));
					attack = 5;
					attacked = true;

				}
			}

		}

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

				unsigned long boneMat = *((unsigned long*)(entAdr + TF2::dwBoneMatrix));
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
		if (targ == closestTarget) {
			drawBones(closestTarget, viewMat);
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

	if (attacked) {
		int& attack = *((int*)(clientBase + TF2::dwAttack));
		attack = 4;
		attacked = false;
	}

}