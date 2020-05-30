#pragma once

/*
Utility for hooking function addresses
*/
class Hook {
public:

	static void Patch(unsigned char* dest, unsigned char* src, unsigned int size);

	static bool DefaultHook(char* src, char* dest, int len);

	static char* TrampHook(char* src, char* dest, unsigned int len);

};