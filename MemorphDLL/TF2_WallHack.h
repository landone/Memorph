#pragma once

#include <vector>
#include <mutex>
#include <glm/glm.hpp>

#include "Hack.h"

class TF2_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;

private:

	glm::vec3 getBonePos(unsigned long boneMat, int bone);
	void drawBones(unsigned long ent, const glm::mat4& viewMat);

	float* viewMatrix = nullptr;
	float* myPos = nullptr;
	std::vector<unsigned long> targets;
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	bool inGame = false;
	int* myTeam;
	int* myClass;
	int* myObserveMode;
	bool attacked = false;
	std::mutex targetMutex;
	std::mutex attackMutex;
	unsigned long closestTarget = NULL;

};