#include "TF2.h"

const std::string TF2::processName = "hl2.exe";
const std::string TF2::clientDllName = "client.dll";
const std::string TF2::engineDllName = "engine.dll";
const int TF2::BoneOrder[10][17] = {
	{
		0,0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,
		0,0
	},
	{
		6,5,4,8,
		10,12,20,
		9,11,19,
		3,2,0,
		14,16,
		13,15
	},
	{
		23,6,5,8,
		10,12,20,
		9,11,19,
		3,2,0,
		16,18,
		15,17
	},
	{
		32,6,5,4,
		10,12,20,
		9,11,19,
		3,2,0,
		16,18,
		15,17
	},
	{
		16,35,15,4,
		8,14,20,
		6,19,17,
		3,2,0,
		12,26,
		10,25
	},
	{
		33,6,5,8,
		10,12,60,
		9,11,59,
		3,2,0,
		16,35,
		15,34
	},
	{
		6,5,8,4,
		10,12,18,
		9,11,17,
		3,2,0,
		16,30,
		15,29
	},
	{
		6,5,4,7,
		12,13,14,
		8,9,10,
		3,2,0,
		20,21,
		16,17
	},
	{
		7,6,5,8,
		38,13,34,
		37,12,35,
		3,2,0,
		19,21,
		18,20
	},
	{
		61,8,7,6,
		15,16,19,
		12,13,20,
		4,3,0,
		2,18,
		16,17
	}
};

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