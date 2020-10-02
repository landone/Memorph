#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>

#include "MemProc.h"

class TF2 {
public:

	static void Initialize();

	enum class BoneVector {
		Type1 = 0,
		Type2,
		Type3,
		Position
	};

	enum class Class {
		None = 0,
		Scout,
		Sniper,
		Soldier,
		DemoMan,
		Medic,
		Heavy,
		Pyro,
		Spy,
		Engineer,
	};

	enum class Team {
		None = 0,
		Spectate,
		RED,
		BLU
	};

	enum class GameStatus {
		None = 0,
		Connecting,
		InGame
	};

	static std::string ClassToString(TF2::Class);
	static std::string TeamToString(TF2::Team);

	static glm::vec3 getBoneVector(unsigned long boneMat, int bone, TF2::BoneVector);
	static bool aimAtHead(unsigned long target);
	static bool aimAt(glm::vec3 pos);
	/* Returns address of bonematrix 4*4 */
	static unsigned long getBoneMatrix(unsigned long target);
	static TF2::Class getClass(unsigned long target);
	static TF2::Team getTeam(unsigned long target);
	static glm::vec3 getPosition(unsigned long target);
	static glm::vec3 getEyePosition();
	static glm::vec3 getVelocity(unsigned long target);
	static unsigned long getLocalPlayer();
	static void updateGlowEffect(unsigned long target, bool set);
	
	static const unsigned long handleSize = 0x4;
	static const unsigned long entityRefSize = 0x10;
	static const unsigned long MAX_PLAYERS = 64;
	static const float ROCKET_SPEED;
	static const std::string processName;
	static const std::string clientDllName;
	static const std::string engineDllName;

	static std::vector<MemProc::Signature> clientSigs;
	static std::vector<MemProc::Signature> engineSigs;

	static const int BoneOrder[10][17];
	static const int BoneOrderSize = 17;
	static const int Bone_ProfileEnd = 3;
	static const int Bone_LeftArmEnd = 6;
	static const int Bone_RightArmEnd = 9;
	static const int Bone_TorsoEnd = 12;
	static const int Bone_LeftLegEnd = 14;
	static const int Bone_RightLegEnd = 16;

	static unsigned long dwEntityList;
	static unsigned long dwLocalPlayer;
	static unsigned long dwButtonBase;
	static unsigned long dwAttack;
	static unsigned long dwIsInGame;
	static unsigned long dwViewAngles;
	static unsigned long dwGlowObjectManager;

	static unsigned long clientBase;
	static unsigned long clientSz;
	static unsigned long engineBase;
	static unsigned long engineSz;
	static unsigned long entityList;
	static unsigned long* localPlayerPtr;
	static float* viewMatrixPtr;

	static const unsigned long dwViewMatrix = 0x59EB80;
	static const unsigned long dwBoneMatrix = 0x5B0;
	static const unsigned long m_iHealth = 0xA8;
	static const unsigned long m_iMaxHealth = 0x17A0;
	static const unsigned long m_iClass = 0x154C;
	static const unsigned long m_iTeam = 0xB0;
	static const unsigned long m_vecOrigin = 0x364;
	static const unsigned long m_vecVelocity = 0x120;
	static const unsigned long m_vecViewOffset = 0xFC;
	static const unsigned long m_bDucked = 0xE60;
	static const unsigned long m_bJumping = 0x1A1C;
	static const unsigned long m_nDisguiseTeam = 0x18F0;
	static const unsigned long m_nDisguiseClass = 0x18F4;
	static const unsigned long m_hActiveWeapon = 0xDB8;
	static const unsigned long m_hMyWeapons = 0xCF8;
	static const unsigned long m_iClip1 = 0xAA8;
	static const unsigned long m_iClip2 = 0xAAC;
	static const unsigned long m_hOwnerEntity = 0x504;
	static const unsigned long m_flLastFireTime = 0xC10;
	static const unsigned long m_bReadyToBackstab = 0xC64;
	static const unsigned long m_hRagdoll = 0x14CC;
	static const unsigned long m_bGoldRagdoll = 0xCA0;
	static const unsigned long m_iNextMeleeCrit = 0x1B38;
	static const unsigned long m_angRotation = 0x370;
	static const unsigned long m_angEyeAngles = 0x1C70;
	static const unsigned long m_iObserverMode = 0x109C;
	static const unsigned long m_bGlowEnabled = 0xDBD;

};