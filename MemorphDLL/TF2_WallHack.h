#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Hack.h"

//Hack for seeing opposing players through walls
//Skeleton drawn for closest to view, colored to represent health
class TF2_WallHack : public Hack {
public:

	void OnStart() override;
	void OnEnd() override;
	void OnThink() override;
	void OnDraw() override;
	void OnKey(unsigned char, bool) override;

private:

	//Attempt to draw entire skeleton of a player's model to screen
	void drawBones(unsigned long ent, const glm::mat4& viewMat);

	/* Amount of frames to wait before resetting attack values */
	int attackFrames = 0;

	//List of viable targets to view
	std::vector<unsigned long> targets;
	//List of head positions for error approximation (reducing bugged skeleton draws)
	glm::vec3 headList[65];

	//View matrix
	glm::mat4 viewMat = glm::mat4(1.0f);
	//Closest target address
	unsigned long closestTarget = NULL;
	bool middleMousePressed = false;

};