#include "TF2_WallHack.h"

#include <iostream>

#include "DX.h"
#include "Memorph.h"
#include "TF2.h"

void TF2_WallHack::OnStart() {

	TF2::Initialize();
	Memorph::addMenuText("MOUSE3 - Shoot at closest enemy head");
	Memorph::addButton(VK_MBUTTON);

}

void TF2_WallHack::OnEnd() {

	

}

void TF2_WallHack::OnKey(unsigned char key, bool pressed) {

	if (!pressed) {
		return;
	}

	if (key == VK_MBUTTON) {
		middleMousePressed = true;
	}

}

void TF2_WallHack::drawBones(unsigned long ent, const glm::mat4& viewMat) {

	unsigned long boneMat = TF2::getBoneMatrix(ent);
	if (boneMat == NULL) {
		return;
	}

	TF2::Class entClass = TF2::getClass(ent);
	if (entClass == TF2::Class::Spy) {
		/* Draw bones for disguise instead */
		TF2::Class disguise = *((TF2::Class*)(ent + TF2::m_nDisguiseClass));
		if (disguise != TF2::Class::None) {
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
			lastPos = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)entClass][changeLast], TF2::BoneVector::Position);
			DX::WorldToScreen(lastPos, viewMat, lastScrPos);
		}

		glm::vec3 bonePos = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)entClass][j], TF2::BoneVector::Position);
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

	if ((*TF2::localPlayerPtr) == NULL) {
		return;
	}


	float closestDist = INFINITY;
	closestTarget = NULL;
	TF2::Team myTeam = TF2::getTeam(*TF2::localPlayerPtr);
	for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

		unsigned long& entAdr = *((unsigned long*)(TF2::entityList + i * TF2::entityRefSize));
		if (entAdr != NULL && entAdr != (*TF2::localPlayerPtr)) {

			int& observerMode = *((int*)(entAdr + TF2::m_iObserverMode));
			if (observerMode != 0) {
				/* Only living players */
				continue;
			}
			TF2::Team team = TF2::getTeam(entAdr);
			if ((team == TF2::Team::RED || team == TF2::Team::BLU) && team != myTeam) {

				unsigned long boneMat = TF2::getBoneMatrix(entAdr);
				if (boneMat == NULL) {
					continue;
				}

				TF2::Class targClass = TF2::getClass(entAdr);
				glm::vec3 targHead = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)targClass][1], TF2::BoneVector::Position);
				if (targHead == headList[i]) {
					/* Probably a bugged/stationary skeleton */
					continue;
				}
				else {
					headList[i] = targHead;
				}

				targets.push_back(i);

				glm::vec3 entOrig = TF2::getPosition(entAdr);
				/* 45.0f added to Z axis to approximate center without relying on bones */
				entOrig.z += 45.0f;
				glm::vec2 targScreen;
				if (DX::WorldToScreen(entOrig, viewMat, targScreen)) {
					float dist = glm::distance(targScreen, DX::GetWindowDim() * 0.5f);
					if (dist < closestDist) {
						closestDist = dist;
						closestTarget = entAdr;
					}
				}

			}

		}

	}

	/* Necessary to avoid asynchronous trouble */
	if (middleMousePressed && closestTarget) {
		TF2::aimAtHead(closestTarget);
		int& attackValue = *(int*)(TF2::clientBase + TF2::dwAttack);
		attackValue = 5;
		attackFrames = 5;
	}
	middleMousePressed = false;

}

void TF2_WallHack::OnDraw() {

	if (!(*TF2::localPlayerPtr)) {
		return;
	}

	if (attackFrames > 0) {
		attackFrames--;
		if (attackFrames == 0) {
			int& attackValue = *(int*)(TF2::clientBase + TF2::dwAttack);
			attackValue = 4;
		}
	}

	memcpy(&viewMat[0][0], TF2::viewMatrixPtr, sizeof(float) * 16);
	for (int i = 0; i < targets.size(); i++) {
		
		unsigned long targ = *((unsigned long*)(TF2::entityList + targets[i] * TF2::entityRefSize));
		if (targ == NULL) {
			continue;
		}

		if (targ == closestTarget) {
			drawBones(closestTarget, viewMat);
		}
		else {

			float health = *((int*)(targ + TF2::m_iHealth)) / (float)(*((int*)(targ + TF2::m_iMaxHealth)));
			glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

			glm::vec3 targOrig = TF2::getPosition(targ);

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