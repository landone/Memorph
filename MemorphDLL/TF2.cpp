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
unsigned long TF2::dwGlowObjectManager = NULL;

unsigned long TF2::clientBase = NULL;
unsigned long TF2::clientSz = NULL;
unsigned long TF2::engineBase = NULL;
unsigned long TF2::engineSz = NULL;
unsigned long TF2::entityList = NULL;
unsigned long* TF2::localPlayerPtr = nullptr;
float* TF2::viewMatrixPtr = nullptr;

const float TF2::ROCKET_SPEED = 1100.0f;

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
	},
	MemProc::Signature {
		(unsigned char*)"\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xB0\x01\x5D",
		(char*)"x????x????xxx",
		MemProc::ScanType::READ,
		1,
		(&TF2::dwGlowObjectManager)
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

	static bool initialized = false;
	if (initialized) {
		return;
	}
	else {
		initialized = true;
	}

	MemProc proc;

	TF2::clientBase = proc.getModule(clientDllName, &clientSz);
	TF2::engineBase = proc.getModule(engineDllName, &engineSz);

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

	TF2::entityList = clientBase + TF2::dwEntityList;
	TF2::localPlayerPtr = (unsigned long*)(clientBase + TF2::dwLocalPlayer);
	TF2::viewMatrixPtr = (float*)(engineBase + TF2::dwViewMatrix);

	proc.detach();

}

glm::vec3 TF2::getBoneVector(unsigned long boneMat, int bone, TF2::BoneVector type) {

	/* Bone data is stored in a 4x4 float matrix where each column represents a vector */

	static int rowOffset = sizeof(float) * 4;
	static int boneSize = sizeof(float) * 12;

	if (boneMat == NULL) {
		return glm::vec3(0,0,0);
	}

	int columnOffset = (int)type * sizeof(float);
	glm::vec3 output;
	output.x = *(float*)(boneMat + boneSize * bone + columnOffset);
	output.y = *(float*)(boneMat + boneSize * bone + columnOffset + rowOffset);
	output.z = *(float*)(boneMat + boneSize * bone + columnOffset + 2 * rowOffset);
	return output;

}

unsigned long TF2::getLocalPlayer() {

	return (*localPlayerPtr);

}

void TF2::updateGlowEffect(unsigned long target, bool set) {

	*(bool*)(target + TF2::m_bGlowEnabled) = set;
	((void(*)(unsigned long))(*(unsigned long**)(target))[0x35C / 4])(target);

}

bool TF2::aimAtHead(unsigned long target) {

	if ((*localPlayerPtr) == NULL) {
		return false;
	}

	unsigned long boneMat = TF2::getBoneMatrix(target);
	if (boneMat == NULL) {
		return false;
	}

	TF2::Class targClass = TF2::getClass(target);
	if (targClass == TF2::Class::Spy) {
		/* Aim at disguise head */
		TF2::Class disguise = *((TF2::Class*)(target + TF2::m_nDisguiseClass));
		if (disguise != TF2::Class::None) {
			targClass = disguise;
		}
	}

	glm::vec3 headPos = getBoneVector(boneMat, TF2::BoneOrder[(int)targClass][1], TF2::BoneVector::Position);

	return aimAt(headPos);

}

bool TF2::aimAt(glm::vec3 targPos) {

	if ((*localPlayerPtr) == NULL) {
		return false;
	}

	glm::vec3 myHead = TF2::getEyePosition();
	float* viewAng = (float*)(engineBase + TF2::dwViewAngles);

	float flatDist = sqrtf(powf(targPos[0] - myHead[0], 2) + powf(targPos[1] - myHead[1], 2));
	static float RAD_TO_DEG = 180.0f / 3.14159f;
	viewAng[0] = -atan2f(targPos[2] - myHead[2], flatDist) * RAD_TO_DEG;
	viewAng[1] = atan2f(targPos[1] - myHead[1], targPos[0] - myHead[0]) * RAD_TO_DEG;

	return true;

}

TF2::Class TF2::getClass(unsigned long target) {

	if (target == NULL) {
		return Class::None;
	}

	return (TF2::Class)(*((int*)(target + TF2::m_iClass)));

}

TF2::Team TF2::getTeam(unsigned long target) {

	if (target == NULL) {
		return Team::None;
	}

	return (TF2::Team)(*((TF2::Team*)(target + TF2::m_iTeam)));

}

unsigned long TF2::getBoneMatrix(unsigned long target) {

	if (target == NULL) {
		return NULL;
	}

	return *((unsigned long*)(target + TF2::dwBoneMatrix));

}

glm::vec3 TF2::getPosition(unsigned long target) {

	if (target == NULL) {
		return glm::vec3(0, 0, 0);
	}

	float* origin = (float*)(target + TF2::m_vecOrigin);
	return glm::vec3(origin[0], origin[1], origin[2]);

}

glm::vec3 TF2::getEyePosition() {

	if ((*localPlayerPtr) == NULL) {
		return glm::vec3();
	}

	float* myViewOffs = (float*)((*localPlayerPtr) + TF2::m_vecViewOffset);
	return getPosition(*localPlayerPtr) + glm::vec3(myViewOffs[0], myViewOffs[1], myViewOffs[2]);

}

glm::vec3 TF2::getVelocity(unsigned long target) {

	if (target == NULL) {
		return glm::vec3(0, 0, 0);
	}

	float* vel = (float*)(target + TF2::m_vecVelocity);
	return glm::vec3(vel[0], vel[1], vel[2]);

}

std::string TF2::ClassToString(TF2::Class mClass) {

	int type = (int)mClass;

	if (type < 0 || type > (int)TF2::Class::Engineer) {
		type = 0;
	}

	return ClassNames[type];

}

std::string TF2::TeamToString(TF2::Team team) {

	int type = (int)team;

	if (type < 0 || type > (int)TF2::Team::BLU) {
		type = 0;
	}

	return TeamNames[type];

}