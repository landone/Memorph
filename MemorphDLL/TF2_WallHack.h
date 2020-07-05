#pragma once

#include <vector>
#include <map>
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

	void aimAtHead(unsigned long target);
	unsigned long headTrack = NULL;

	float* viewMatrixPtr = nullptr;
	float* myPos = nullptr;
	std::vector<unsigned long> targets;
	glm::vec3 headList[65];

	glm::mat4 viewMat = glm::mat4(1.0f);
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	bool inGame = false;
	int* myTeam;
	int* myClass;
	unsigned long closestTarget = NULL;

};