#include "CSGO.h"

const std::string CSGO::processName = "csgo.exe";
const std::string CSGO::clientDllName = "client_panorama.dll";
const std::string CSGO::engineDllName = "engine.dll";

const int CSGO::BoneOrder[] = {
	/*Profile*/
	8, 9, 10,
	/*Left Arm*/
	39,65,40,41,
	/*Right Arm*/
	11,37,12,13,
	/*Torso*/
	6,4,
	/*Left Leg*/
	66,71,68,
	/*Right Leg*/
	73,78,75
};