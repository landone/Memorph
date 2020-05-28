#include <iostream>
#include <Windows.h>

#include "MemProc.h"
#include "CSGO.h"
#include "TF2.h"

void AutoBackstab();
void SpyDetector();
void click(bool left = true);

int main() {

	AutoBackstab();

	return 0;

}

void AutoBackstab() {

	unsigned long entList = NULL;
	MemProc proc;
	int backstabs = 0;
	std::cout << "Backstabs: 0" << std::endl;

	if (proc.attach(TF2::processName)) {

		unsigned long mod = proc.getModule(TF2::clientDllName);
		entList = mod + TF2::dwEntityList;
		unsigned long localPlayer;
		proc.read(&localPlayer, sizeof(localPlayer), mod + TF2::dwLocalPlayer);

		if (entList == NULL) {
			std::cout << "No entity list" << std::endl;
		}
		else {

			while (true) {

				int myTeam;
				proc.read(&myTeam, sizeof(myTeam), localPlayer + TF2::m_iTeam);
				int myClass;
				proc.read(&myClass, sizeof(myClass), localPlayer + TF2::m_iClass);

				if (myClass == TF2::Class_Spy) {

					unsigned long wep[2];
					proc.read(&wep[0], sizeof(long), localPlayer + TF2::m_hActiveWeapon);
					proc.read(&wep[1], sizeof(long), localPlayer + TF2::m_hMyWeapons + TF2::handleSize);
					if (wep[0] == wep[1]) {
						unsigned long wepBase = 0;
						proc.read(&wepBase, sizeof(wepBase), entList + (wep[0] & 0xFFF) * TF2::entityRefSize);
						byte backstabReady = 0;
						proc.read(&backstabReady, sizeof(backstabReady), wepBase + TF2::m_bReadyToBackstab);
						if (backstabReady > 0) {
							click();
							backstabs++;
							system("cls");
							std::cout << "Backstabs: " << backstabs << std::endl;
						}
					}

				}

				Sleep(20);

			}

		}
	}

	system("pause");

}

void SpyDetector() {

	unsigned long entList = NULL;
	MemProc proc;

	if (proc.attach(TF2::processName)) {

		unsigned long mod = proc.getModule(TF2::clientDllName);
		entList = mod + TF2::dwEntityList;
		unsigned long localPlayer;
		proc.read(&localPlayer, sizeof(localPlayer), mod + TF2::dwLocalPlayer);

		if (entList == NULL) {
			std::cout << "No entity list" << std::endl;
		}
		else {

			int data[TF2::MAX_PLAYERS][6] = { 0 };
			for (int i = 0; i < TF2::MAX_PLAYERS; i++) {
				for (int j = 0; j < 6; j++) {
					data[i][j] = 0;
				}
			}

			while (true) {

				int myTeam;
				proc.read(&myTeam, sizeof(myTeam), localPlayer + TF2::m_iTeam);

				float myPos[3];
				proc.read(myPos, sizeof(float) * 3, localPlayer + TF2::m_vecOrigin);
				float closeDist = 0.0f;
				int closeIndex = -1;

				for (int i = 0; i < TF2::MAX_PLAYERS; i++) {

					unsigned long ent = NULL;
					proc.read(&ent, sizeof(ent), entList + i * TF2::entityRefSize);
					if (ent != NULL) {
						proc.read(&data[i][2], sizeof(int), ent + TF2::m_iClass);
						proc.read(&data[i][3], sizeof(int), ent + TF2::m_iTeam);
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

				Sleep(200);

			}

		}
	}

	system("pause");

}

void click(bool left) {

	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | (left ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN);
	POINT mousePos = { 0 };
	GetCursorPos(&mousePos);
	input.mi.dx = mousePos.x;
	input.mi.dy = mousePos.y;
	SendInput(1, &input, sizeof(INPUT));
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | (left ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP);
	SendInput(1, &input, sizeof(INPUT));

}