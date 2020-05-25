#pragma once

#include <string>

class TF2 {
public:

	enum Class {
		Class_None = 0,
		Class_Scout,
		Class_Sniper,
		Class_Soldier,
		Class_DemoMan,
		Class_Medic,
		Class_Heavy,
		Class_Pyro,
		Class_Spy,
		Class_Engineer,
	};

	enum Team {
		Team_None = 0,
		Team_Spectate,
		Team_RED,
		Team_BLU
	};

	static std::string ClassToString(int);
	static std::string TeamToString(int);

	static const unsigned long dwEntityList = 0xC480B4;
	static const unsigned long dwLocalPlayer = 0xC3A590;
	static const unsigned long m_iHealth = 0xA8;
	static const unsigned long m_iMaxHealth = 0x17A0;
	static const unsigned long m_iClass = 0x154C;
	static const unsigned long m_iTeam = 0xB0;
	static const unsigned long m_vecOrigin = 0x364;
	static const unsigned long m_bDucked = 0xE60;
	static const unsigned long m_bJumping = 0x1A1C;
	static const unsigned long m_nDisguiseTeam = 0x18F0;
	static const unsigned long m_nDisguiseClass = 0x18F4;

};