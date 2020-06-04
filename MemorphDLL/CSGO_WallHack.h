#pragma once

#include <vector>
#include <mutex>
#include <glm/glm.hpp>

#include "Hack.h"

class CSGO_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;

private:

	void aimAt(unsigned long ent);
	glm::vec3 getBonePos(unsigned long boneMat, int bone);
	void drawCrosshair();

	int crosshairLength = 4;
	int crosshairWidth = 2;
	glm::vec4 crosshairColor = glm::vec4(255, 255, 255, 255);
	std::vector<unsigned long> targets;
	std::mutex targetMutex;
	bool inGame = false;

	float* viewMatrix = nullptr;
	float* myPos = nullptr;
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	int* myTeam;
	int* myObserveMode;
	unsigned long myBoneMat;

};