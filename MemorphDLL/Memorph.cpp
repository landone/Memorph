#include "Memorph.h"
#include "DX.h"

bool Memorph::running = false;
std::vector<Hack*> Memorph::hacks;

std::vector<Memorph::MenuMessage> Memorph::menuText = {
	Memorph::MenuMessage{"MEMORPH", {255, 0, 0}},
	Memorph::MenuMessage{"=======", {255, 0, 0}},
	Memorph::MenuMessage{"HOME - Toggle Menu", {255, 255, 255}},
	Memorph::MenuMessage{"END - Exit Hack", {255, 255, 255}}
};
bool Memorph::showMenu = true;
unsigned int Memorph::longestMenuText = 0;

unsigned int Memorph::thinkCounter = 0;
unsigned int Memorph::thinkFreq = 5;

std::vector<Memorph::Button> Memorph::buttons = {
	Memorph::Button {
		VK_END, false
	},
	Memorph::Button {
		VK_HOME, false
	}
};

void Memorph::addMenuText(std::string line, glm::vec3 color) {

	MenuMessage msg = { line, {(int)color.r, (int)color.g, (int)color.b} };

	menuText.push_back(msg);
	if (longestMenuText < line.length()) {
		longestMenuText = line.length();
	}

}

void Memorph::addButton(unsigned char keyCode) {

	buttons.push_back( Memorph::Button{ keyCode, false } );

}

void Memorph::hookEndScene() {
	
	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnDraw();
	}

	thinkCounter++;
	if (thinkCounter == thinkFreq) {

		thinkCounter = 0;
		for (int i = 0; i < hacks.size(); i++) {
			hacks[i]->OnThink();
		}

	}

	if (showMenu) {
		DX::DrawFillRect(glm::vec2(0, 0), glm::vec2(longestMenuText * 7, menuText.size() * 16), glm::vec4(10, 10, 10, 255));
		for (unsigned int i = 0; i < menuText.size(); i++) {
			int* color = menuText[i].color;
			DX::WriteText(glm::vec2(0, i * 16), menuText[i].msg, glm::vec4(color[0], color[1], color[2], 255));
		}
	}

}

void Memorph::buttonEvent(unsigned char key, bool pressed) {

	if (pressed) {
		if (key == VK_END) {
			stop();
			return;
		}
		else if (key == VK_HOME) {
			showMenu = !showMenu;
			return;
		}
	}

	for (unsigned int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnKey(key, pressed);
	}

}

bool Memorph::run() {

	if (running) {
		return false;
	}

	running = true;

	for (unsigned int i = 0; i < menuText.size(); i++) {
		if (menuText[i].msg.length() > longestMenuText) {
			longestMenuText = menuText[i].msg.length();
		}
	}

	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnStart();
	}

	DX::HookEndScene(hookEndScene);

	while (true) {

		if (!running) {
			break;
		}

		for (unsigned int i = 0; i < buttons.size(); i++) {
			if (GetAsyncKeyState(buttons[i].keyCode) & 0x8000) {
				if (!buttons[i].down) {
					buttons[i].down = true;
					buttonEvent(buttons[i].keyCode, true);
				}
			}
			else {
				if (buttons[i].down) {
					buttons[i].down = false;
					buttonEvent(buttons[i].keyCode, false);
				}
			}
		}

		Sleep(12);

	}

	DX::UnhookEndScene();

	for (int i = 0; i < hacks.size(); i++) {
		hacks[i]->OnEnd();
	}

	return true;

}

bool Memorph::stop() {

	if (!running) {
		return false;
	}

	running = false;
	return true;

}

void Memorph::addHack(Hack& h) {

	hacks.push_back(&h);

}