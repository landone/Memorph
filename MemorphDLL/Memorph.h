#pragma once

#include <vector>
#include <string>

#include "Hack.h"

class Memorph {
public:

	/* Start hacking process */
	static bool run();
	/* End hacking process */
	static bool stop();
	/* Put hack in running queue */
	static void addHack(Hack&);
	/* Add a line to the menu */
	static void addMenuText(std::string);
	/* Add button to listen for */
	static void addButton(unsigned char keyCode);
	/* Are hacks currently running */
	static bool isRunning() { return running; }

private:

	static bool running;
	static std::vector<Hack*> hacks;

	static std::vector<std::string> menuText;
	static bool showMenu;
	static unsigned int longestMenuText;

	static unsigned int thinkCounter;
	/* Amount of frames allowed before think frame */
	static unsigned int thinkFreq;

	struct Button {
		unsigned char keyCode;
		bool down;
	};
	static std::vector<Button> buttons;

	static void buttonEvent(unsigned char, bool);
	static void hookEndScene();

};