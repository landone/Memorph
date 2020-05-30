#pragma once

void Patch(BYTE* dest, BYTE* src, unsigned int size);

bool Hook(char* src, char* dest, int len);

char* TrampHook(char* src, char* dest, unsigned int len);