#include "TF2_WallHack.h"

#include <iostream>

#include "DX.h"
#include "Memorph.h"
#include "TF2.h"

/* 45.0f added to Z axis to approximate center without relying on bones */
static const float PLAYER_HALF_HEIGHT = 45.0f;
//Value TF2 uses to represent an attack
static const int ATTACK_VALUE = 5;
//How many frames attack input should last
static const int ATTACK_FRAMES = 5;
//Dimension ratio of player representative rectangle (skeleton substitution)
//Width is 1/3 of height.
static const float RECT_DIM_RATIO = 1 / 3.0f;

void TF2_WallHack::OnStart() {

	//Initialize TF2 variables
	TF2::Initialize();
	//Listen for M3 press
	Memorph::addMenuText("MOUSE3 - Shoot at closest enemy head");
	Memorph::addButton(VK_MBUTTON);

}

void TF2_WallHack::OnEnd() {

	

}

void TF2_WallHack::OnKey(unsigned char key, bool pressed) {

	//Nothing if released
	if (!pressed) {
		return;
	}

	if (key == VK_MBUTTON) {
		middleMousePressed = true;
	}

}

/*
* Draw a player's skeleton on screen, using an entity address and a view matrix.
* Nothing drawn upon error.
*/
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

	//Get health percentage
	float health = *((int*)(ent + TF2::m_iHealth)) / (float)(*((int*)(ent + TF2::m_iMaxHealth)));
	//Choose skeleton color based on health (red = low, green = high)
	glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

	//Last skeleton bone position (screen & world)
	glm::vec2 lastScrPos = glm::vec2(0,0);
	glm::vec3 lastPos = glm::vec3(0,0,0);

	//Loop through bone indices to draw skeleton
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

		//If bone was adjusted, update last positions accordingly
		if (changeLast != -1) {
			lastPos = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)entClass][changeLast], TF2::BoneVector::Position);
			DX::WorldToScreen(lastPos, viewMat, lastScrPos);
		}

		glm::vec3 bonePos = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)entClass][j], TF2::BoneVector::Position);
		glm::vec2 boneScrPos;
		//If legitimate bone position, draw line from last bone to this bone
		if (bonePos != glm::vec3(0, 0, 0) && lastPos != glm::vec3(0, 0, 0)) {
			if (DX::WorldToScreen(bonePos, viewMat, boneScrPos)) {
				DX::DrawLine(lastScrPos, boneScrPos, 2, color);
			}
		}
		else {
			DX::WorldToScreen(bonePos, viewMat, boneScrPos);
		}

		//Update last positions
		lastScrPos = boneScrPos;
		lastPos = bonePos;

	}

}

//Called when hack is given time to process
void TF2_WallHack::OnThink() {

	//Forget previous targets
	targets.clear();

	//Local player must exist. (In-game)
	if ((*TF2::localPlayerPtr) == NULL) {
		return;
	}

	unsigned long localPlayer = TF2::getLocalPlayer();
	//Closest distance to center screen (not world position)
	float closestDist = INFINITY;
	closestTarget = NULL;
	TF2::Team myTeam = TF2::getTeam(localPlayer);
	//Loop through all players
	for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

		unsigned long& entAdr = *((unsigned long*)(TF2::entityList + i * TF2::entityRefSize));
		//Player must exist and not be local player
		if (entAdr != NULL && entAdr != localPlayer) {

			int& observerMode = *((int*)(entAdr + TF2::m_iObserverMode));
			/* Only living players */
			if (observerMode != 0) {
				continue;
			}

			TF2::Team team = TF2::getTeam(entAdr);
			//Must be on enemy team
			if ((team == TF2::Team::RED || team == TF2::Team::BLU) && team != myTeam) {

				unsigned long boneMat = TF2::getBoneMatrix(entAdr);
				if (boneMat == NULL) {
					continue;
				}

				TF2::Class targClass = TF2::getClass(entAdr);
				//Index [1] is head bone in skeleton
				glm::vec3 targHead = TF2::getBoneVector(boneMat, TF2::BoneOrder[(int)targClass][1], TF2::BoneVector::Position);

				//If head in same position as last calculation
				if (targHead == headList[i]) {
					/* Probably a bugged/stationary skeleton, so skip */
					continue;
				}
				else {
					headList[i] = targHead;
				}

				//Viable target, so remember
				targets.push_back(i);

				glm::vec3 entOrig = TF2::getPosition(entAdr);
				
				entOrig.z += PLAYER_HALF_HEIGHT;
				//Target's position on screen coords
				glm::vec2 targScreen;
				if (DX::WorldToScreen(entOrig, viewMat, targScreen)) {
					float dist = glm::distance(targScreen, DX::GetWindowDim() * 0.5f);
					//If dist to center screen is less, then new closest target
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

		//Set local player to attack for some frames
		int& attackValue = *(int*)(TF2::clientBase + TF2::dwAttack);
		attackValue = ATTACK_VALUE;
		attackFrames = ATTACK_FRAMES;

		//Attempt to direct hit with rocket launcher
		if (TF2::getClass(localPlayer) == TF2::Class::Soldier) {

			unsigned long wep[2];
			wep[0] = *((unsigned long*)(localPlayer + TF2::m_hActiveWeapon));
			wep[1] = *((unsigned long*)(localPlayer + TF2::m_hMyWeapons));

			//If primary weapon active (rocket launcher)
			if (wep[0] == wep[1]) {

				glm::vec3 vel = TF2::getVelocity(closestTarget);
				float speed = TF2::ROCKET_SPEED;
				//Player position
				glm::vec3 plpos = TF2::getPosition(closestTarget);
				//Local player's eye position
				glm::vec3 eyepos = TF2::getEyePosition();

				/* Quadratic formula time: determine interception location */
				float a = glm::dot(vel, vel) - speed * speed;
				//Unsolvable if 2a == 0
				if (2 * a != 0.0f) {

					float b = 2.0f * (glm::dot(vel, plpos) - glm::dot(vel, eyepos));
					float c = glm::dot(plpos, plpos) + glm::dot(eyepos, eyepos) - 2 * (glm::dot(eyepos, plpos));
					//Calculate sqrt term once to avoid unnecessary processing
					float sqrtTerm = sqrtf(b * b - 4.0f * a * c);

					float t = -1;
					//If negative, try alternative solution
					if ((t = (-b + sqrtTerm) / (2.0f * a)) < 0.0f) {
						t = (-b - sqrtTerm) / (2.0f * a);
					}

					//Only positive time allowed (can not shoot backwards in time)
					if (t > 0.0f) {

						//Aim at intercept position
						TF2::aimAt(plpos + vel * t);

					}
						
				}

			}
		}
		else {
			//Regular aimbot, aim at head
			TF2::aimAtHead(closestTarget);
		}

	}
	middleMousePressed = false;

}

void TF2_WallHack::OnDraw() {

	if (!(*TF2::localPlayerPtr)) {
		return;
	}

	//Reset attack value once frames have passed
	if (attackFrames > 0) {
		attackFrames--;
		if (attackFrames == 0) {
			int& attackValue = *(int*)(TF2::clientBase + TF2::dwAttack);
			attackValue = 4;
		}
	}

	//Retrieve view matrix
	memcpy(&viewMat[0][0], TF2::viewMatrixPtr, sizeof(float) * 16);
	//Loop through list of targets
	for (int i = 0; i < targets.size(); i++) {
		
		unsigned long targ = *((unsigned long*)(TF2::entityList + targets[i] * TF2::entityRefSize));
		if (targ == NULL) {
			continue;
		}

		//Draw full skeleton of closest target
		if (targ == closestTarget) {
			drawBones(closestTarget, viewMat);
		}
		else {
			//Only draw rectangle for other targets
			
			//Get health percentage
			float health = *((int*)(targ + TF2::m_iHealth)) / (float)(*((int*)(targ + TF2::m_iMaxHealth)));
			//Choose rectangle color based on health (red = low, green = high)
			glm::vec4 color = glm::vec4(255 * (1 - health), 255 * (health), 0, 255);

			glm::vec3 targOrig = TF2::getPosition(targ);

			//Target's position on screen
			glm::vec2 targPos;
			if (DX::WorldToScreen(glm::vec3(targOrig[0], targOrig[1], targOrig[2]), viewMat, targPos)) {

				//Get approximate height position of player
				glm::vec2 targPos2;
				DX::WorldToScreen(glm::vec3(targOrig[0], targOrig[1], targOrig[2] + PLAYER_HALF_HEIGHT), viewMat, targPos2);
				float height = abs(targPos.y - targPos2.y);
				//Draw representative rectangle centered based on rectangle height
				DX::DrawFillRect(targPos - glm::vec2(height * 0.5f * RECT_DIM_RATIO, height), 
								glm::vec2(height * RECT_DIM_RATIO, height),
								color);

			}

		}

	}

}