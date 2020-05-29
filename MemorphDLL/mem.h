#pragma once

#include <vector>
#include "stdafx.h"

namespace mem
{
	void PatchEx(void* dest, void* src, unsigned int size, HANDLE hProcess);
	void NopEx(void* dest, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	void Patch(void* dest, void* src, unsigned int size);
	void Nop(void* dest, unsigned int size);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}