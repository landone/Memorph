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
	glm::vec3 getBonePos(unsigned long ent, int bone);

	float* viewMatrix = nullptr;
	float* myPos = nullptr;
	std::vector<unsigned long> targets;
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	bool inGame = false;
	int* myTeam;
	int* myObserveMode;
	std::mutex targetMutex;

};