#include "TF2_SpyAlert.h"
#include "TF2.h"
#include "Memorph.h"

void TF2_SpyAlert::OnStart() {

	TF2::Initialize();
	Memorph::addMenuText("Spy Alert Enabled");

}

void TF2_SpyAlert::OnThink() {

	if (!font.data) {
		font = DX::MakeFont(128, "Impact", true, false);
		fontPos = glm::vec2(DX::GetWindowDim().x / 2.0f, 0);
		fontPos -= glm::vec2(font.height * ALERT_TEXT.size() / 6.0f, 0.0f);
	}

	unsigned long localPlayer = TF2::getLocalPlayer();

	if (!localPlayer) {
		return;
	}

	static bool everyOther = false;

	if (everyOther) {

		everyOther = false;

		doAlert = false;
		TF2::Team myTeam = TF2::getTeam(localPlayer);

		for (int i = 1; i <= TF2::MAX_PLAYERS; i++) {

			unsigned long& entAdr = *((unsigned long*)(TF2::entityList + i * TF2::entityRefSize));
			if (entAdr == NULL || entAdr == localPlayer) {
				continue;
			}

			TF2::Team team = TF2::getTeam(entAdr);
			if ((team != TF2::Team::RED && team != TF2::Team::BLU) || team == myTeam) {
				continue;
			}

			if (TF2::getClass(entAdr) != TF2::Class::Spy) {
				continue;
			}

			int& observerMode = *((int*)(entAdr + TF2::m_iObserverMode));
			if (observerMode != 0) {
				/* Only living players */
				continue;
			}

			glm::vec3 spyPos = TF2::getPosition(entAdr);
			glm::vec3 myPos = TF2::getPosition(localPlayer);

			if (glm::distance(myPos, spyPos) <= ALERT_DIST) {

				doAlert = true;
				break;

			}

		}

	}
	else {
		everyOther = true;
	}

}

void TF2_SpyAlert::OnDraw() {

	if (!TF2::getLocalPlayer()) {
		doAlert = false;
		return;
	}

	if (doAlert) {

		
		DX::WriteText(fontPos, ALERT_TEXT, ALERT_COLOR, font);

	}

}