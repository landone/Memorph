#include "TF2.h"

const std::string TF2::processName = "hl2.exe";
const std::string TF2::clientDllName = "client.dll";
const std::string TF2::engineDllName = "engine.dll";

unsigned long TF2::dwEntityList = NULL;
unsigned long TF2::dwLocalPlayer = NULL;
unsigned long TF2::dwButtonBase = NULL;
unsigned long TF2::dwAttack = NULL;
unsigned long TF2::dwIsInGame = NULL;
unsigned long TF2::dwViewAngles = NULL;

std::vector<MemProc::Signature> TF2::clientSigs = {
	MemProc::Signature {
		(unsigned char*)"\xA1\x00\x00\x00\x00\x33\xC9\x83\xC4\x04",
		(char*)"x????xxxxx",
		MemProc::ScanType::READ,
		1,
		(&TF2::dwLocalPlayer)
	},
	MemProc::Signature {
		(unsigned char*)"\xA1\x00\x00\x00\x00\xA8\x01\x75\x51\x83\xC8\x01",
		(char*)"x????xxxxxxx",
		MemProc::ScanType::READ,
		1,
		(&TF2::dwEntityList)
	},
	MemProc::Signature {
		(unsigned char*)"\x68\x00\x00\x00\x00\x8B\x40\x28\xFF\xD0\xA1",
		(char*)"x????xxxxxx",
		MemProc::ScanType::READ,
		1,
		(&TF2::dwButtonBase)
	}
};
std::vector<MemProc::Signature> TF2::engineSigs = {
	MemProc::Signature {
		(unsigned char*)"\x83\x3D\x00\x00\x00\x00\x00\x0F\x9D\xC0\xC3",
		(char*)"xx?????xxxx",
		MemProc::ScanType::READ,
		2,
		(&TF2::dwIsInGame)
	},
	MemProc::Signature {
		(unsigned char*)"\xD9\x1D\x00\x00\x00\x00\xD9\x46\x04",
		(char*)"xx????xxx",
		MemProc::ScanType::READ,
		2,
		(&TF2::dwViewAngles)
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
	unsigned long engineSz;
	unsigned long engineBase = proc.getModule(engineDllName, &engineSz);

	for (unsigned int i = 0; i < clientSigs.size(); i++) {

		MemProc::Signature& s = clientSigs[i];
		(*s.resultPtr) = proc.FindAddress(clientBase, clientSz, s.sig, s.mask, s.type, s.offset);

	}
	for (unsigned int i = 0; i < engineSigs.size(); i++) {

		MemProc::Signature& s = engineSigs[i];
		(*s.resultPtr) = proc.FindAddress(engineBase, engineSz, s.sig, s.mask, s.type, s.offset);

	}

	TF2::dwEntityList += 0x18;
	TF2::dwAttack = TF2::dwButtonBase + 0x2C;

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