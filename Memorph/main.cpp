#include <iostream>
#include <vector>
#include <Windows.h>

#include "MemProc.h"
#include "CSGO.h"
#include "TF2.h"

//https://github.com/frk1/hazedumper/blob/master/csgo.cs

int main() {

	unsigned long entList = NULL;
	unsigned long entSize = 0x10;
	const int MAX_PLAYERS = 64;
	std::string procName = "hl2.exe";
	MemProc proc;

	if (proc.attach(procName)) {

		std::string dllName = "client.dll";
		unsigned long mod = proc.getModule(dllName);
		entList = mod + TF2::dwEntityList;
		unsigned long localPlayer;
		proc.read(&localPlayer, sizeof(localPlayer), mod + TF2::dwLocalPlayer);

		int myTeam;
		proc.read(&myTeam, sizeof(myTeam), localPlayer + TF2::m_iTeam);

		if (entList == NULL) {
			std::cout << "No entity list" << std::endl;
		}
		else {

			int data[MAX_PLAYERS][6] = { 0 };
			for (int i = 0; i < MAX_PLAYERS; i++) {
				for (int j = 0; j < 6; j++) {
					data[i][j] = 0;
				}
			}

			while (true) {

				float myPos[3];
				proc.read(myPos, sizeof(float) * 3, localPlayer + TF2::m_vecOrigin);
				float closeDist = 0.0f;
				int closeIndex = -1;

				for (int i = 0; i < MAX_PLAYERS; i++) {

					unsigned long ent = NULL;
					proc.read(&ent, sizeof(ent), entList + i * entSize);
					if (ent != NULL) {
						proc.read(&data[i][0], sizeof(int), ent + TF2::m_iHealth);
						proc.read(&data[i][1], sizeof(int), ent + TF2::m_iMaxHealth);
						proc.read(&data[i][2], sizeof(int), ent + TF2::m_iClass);
						proc.read(&data[i][3], sizeof(int), ent + TF2::m_iTeam);
						proc.read(&data[i][4], sizeof(int), ent + TF2::m_nDisguiseTeam);
						proc.read(&data[i][5], sizeof(int), ent + TF2::m_nDisguiseClass);

						if (myTeam != data[i][3] && data[i][3] > 1 && data[i][2] == TF2::Class::Class_Spy) {
							float pos[3];
							proc.read(pos, sizeof(float) * 3, ent + TF2::m_vecOrigin);
							float dist = pow(myPos[0] - pos[0], 2) + pow(myPos[1] - pos[1], 2) + pow(myPos[2] - pos[2], 2);
							if (closeIndex == -1 || dist < closeDist) {
								closeDist = dist;
								closeIndex = i;
							}
						}

					}

				}

				system("cls");
				if (closeIndex != -1) {
					std::string name = "Spy";
					if (data[closeIndex][5] > 0) {
						name = "Disguised " + TF2::ClassToString(data[closeIndex][5]);
					}
					std::cout << name << " (" << closeDist << ")" << std::endl;
				}

				Sleep(150);

			}

		}
	}

	system("pause");

	return 0;

}