#pragma once

#include <vector>

#include "Hack.h"

class TF2_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;

private:

	float* viewMatrix = nullptr;
	std::vector<float*> targets;
	unsigned long clientBase;
	unsigned long engineBase;
	unsigned long entList;
	unsigned long* localPlayerPtr;
	int* myTeam;
	int* myClass;

};