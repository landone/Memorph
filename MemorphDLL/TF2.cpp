#include "TF2.h"

const std::string TF2::processName = "hl2.exe";
const std::string TF2::clientDllName = "client.dll";
const std::string TF2::engineDllName = "engine.dll";

unsigned long TF2::dwEntityList = NULL;
unsigned long TF2::dwLocalPlayer = NULL;
unsigned long TF2::dwAttack = NULL;
unsigned long TF2::dwIsInGame = NULL;
unsigned long TF2::dwViewAngles = NULL;

std::vector<MemProc::Signature> TF2::signatures = {
	MemProc::Signature {
		(unsigned char*)"\xA1\x00\x00\x00\x00\x33\xC9\x83\xC4\x04",
		(char*)"x????xxxxx",
		MemProc::ScanType::READ,
		1,
		(&TF2::dwLocalPlayer)
	}
};

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

void TF2::Initialize() {

	MemProc proc;

	unsigned long clientSz;
	unsigned long clientBase = proc.getModule(clientDllName, &clientSz);

	for (unsigned int i = 0; i < signatures.size(); i++) {

		MemProc::Signature& s = signatures[i];
		(*s.resultPtr) = proc.FindAddress(clientBase, clientSz, s.sig, s.mask, s.type, s.offset);

	}

	proc.detach();

}

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