#include "TF2_Camera.h"
#include "TF2.h"
#include "Memorph.h"

#include <glm/glm.hpp>

void TF2_Camera::OnStart() {

	TF2::Initialize();
	Memorph::addMenuText("Camera alteration");

}

void TF2_Camera::OnEnd() {



}

void TF2_Camera::OnDraw() {

	unsigned long localPlayer = (*TF2::localPlayerPtr);

	if (!localPlayer) {
		return;
	}

	float* viewOffs = (float*)(localPlayer + TF2::m_vecViewOffset);
	viewOffs[2] = TF2::getPosition(localPlayer).z + 300.0f;

}