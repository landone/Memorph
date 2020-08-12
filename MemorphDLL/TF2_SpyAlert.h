#pragma once

#include "Hack.h"
#include "DX.h"

#include <glm/glm.hpp>
#include <string>

class TF2_SpyAlert : public Hack {
public:

	void OnStart() override;
	void OnThink() override;
	void OnDraw() override;

private:

	const float ALERT_DIST = 400.0f;
	const glm::vec4 ALERT_COLOR = glm::vec4(255, 255, 0, 255);
	const std::string ALERT_TEXT = "SPY ALERT";

	bool doAlert = false;
	DX::Font font = { nullptr, 0 };
	glm::vec2 fontPos;

};