#pragma once

#include <string>

/*
Utility for external program memory alteration.
*/
class MemProc {
public:

	MemProc();
	MemProc(std::string exeFile);

	bool attach(std::string exeFile);

	unsigned long getModule(std::string name);
	/* Get module address from current process */
	static unsigned long getCurrentModule(std::string name);
	bool write(const void* src, int len, unsigned long addr);
	bool read(const void* dest, int len, unsigned long addr);

	bool isAttached();
	void detach();

private:

	static unsigned long GetProcId(std::string procName);
	static unsigned long GetModuleBaseAddress(unsigned long procId, std::string modName);

	void* handle = nullptr;
	unsigned long pid = 0;

};