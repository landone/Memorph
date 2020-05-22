#include <iostream>
#include <vector>
#include <Windows.h>

#include "MemProc.h"
#include "CSGO.h"

//https://github.com/frk1/hazedumper/blob/master/csgo.cs

int main() {

	unsigned long localPlayer = NULL;
	unsigned long playerRsc = NULL;
	std::string procName = "csgo.exe";

	MemProc proc;

	if (proc.attach(procName)) {

		std::string dllName = "client_panorama.dll";
		unsigned long mod = proc.getModule(dllName);
		proc.read(&localPlayer, sizeof(localPlayer), mod + CSGO::dwLocalPlayer);
		if (localPlayer == NULL) {
			std::cout << "Local player non-existant" << std::endl;
		}
		else {
			const int MAX_PLAYERS = 64;
			int hp[MAX_PLAYERS] = { 0 };
			std::string names[MAX_PLAYERS];
			bool refresh = true;
			int myTeam = 0;
			proc.read(&myTeam, sizeof(myTeam), localPlayer + CSGO::m_iTeamNum);
			while (true) {
				
				for (int i = 0; i < MAX_PLAYERS; i++) {

					unsigned long ent = NULL;
					proc.read(&ent, sizeof(ent), mod + CSGO::dwEntityList + i * 0x10);
					if (ent != NULL) {
						int team = 0;
						proc.read(&team, sizeof(team), ent + CSGO::m_iTeamNum);
						if (team > 0 && team != myTeam) {
							int newHp;
							proc.read(&newHp, sizeof(newHp), ent + CSGO::m_iHealth);
							if (newHp != hp[i]) {
								hp[i] = newHp;

								names[i].clear();
								for (int j = 0; j < MAX_PLAYERS; j++) {
									char c = '\0';
									proc.read(&c, sizeof(c), ent + CSGO::m_szCustomName + j);
									if (c == '\0') {
										break;
									}
									else {
										names[i].append("" + c);
									}
								}

								refresh = true;
							}
						}
					}

				}

				if (refresh) {
					
					system("cls");
					for (int i = 0; i < MAX_PLAYERS; i++) {
						if (hp[i] > 0) {
							std::cout << names[i] << " (" << i << "): " << hp[i] << "hp" << std::endl;
						}
					}
					refresh = false;

				}

				Sleep(20);

			}
		}
	}

	system("pause");

	return 0;

}