#include "TF2.h"

const std::string TF2::processName = "hl2.exe";
const std::string TF2::clientDllName = "client.dll";

static std::string ClassNames[] = {
	"None",
	"Scout",
	"Sniper",
	"Soldier",
	"DemoMan",
	"Medic",
	"Heavy",
	"Pyro",
	"Spy",
	"Engineer"
};

static std::string TeamNames[] = {
	"None",
	"Spectate",
	"RED",
	"BLUE"
};

std::string TF2::ClassToString(int type) {

	if (type < 0 || type > Class_Engineer) {
		type = 0;
	}

	return ClassNames[type];

}

std::string TF2::TeamToString(int type) {

	if (type < 0 || type > Team_BLU) {
		type = 0;
	}

	return TeamNames[type];

}