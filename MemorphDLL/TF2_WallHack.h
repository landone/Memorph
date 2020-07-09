#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Hack.h"

class TF2_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;
	void OnKey(unsigned char, bool) override;

private:

	void drawBones(unsigned long ent, const glm::mat4& viewMat);

	/* Amount of frames to wait before resetting attack values */
	int attackFrames = 0;

	std::vector<unsigned long> targets;
	glm::vec3 headList[65];

	glm::mat4 viewMat = glm::mat4(1.0f);
	unsigned long closestTarget = NULL;
	bool middleMousePressed = false;

};