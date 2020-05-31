#pragma once

#include <vector>
#include <mutex>

#include "Hack.h"

class TF2_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;

private:

	float* viewMatrix = nullptr;
	float* myPos = nullptr;
	std::vector<float*> targets;
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	int* gameStatus;
	bool inGame = false;
	int* myTeam;
	int* myClass;
	int* myObserveMode;
	bool attacked = false;
	std::mutex targetMutex;
	std::mutex attackMutex;

};