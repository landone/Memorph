#pragma once

#include <string>

/*
Utility for external program memory alteration.
*/
class MemProc {
public:

	enum ScanType {
		NONE,
		READ,
		SUBTRACT
	};

	struct Signature {
		unsigned char* sig;
		char* mask;
		MemProc::ScanType type;
		unsigned long offset;
		/* Pointer to variable that shall contain the address */
		unsigned long* resultPtr;
	};

	MemProc();
	MemProc(std::string exeFile);

	bool attach(std::string exeFile);

	unsigned long getModule(std::string name);
	void printModules();

	/* Get module address from current process */
	static unsigned long getCurrentModule(std::string name, unsigned long* sizeBuf = nullptr);

	

	bool write(const void* src, int len, unsigned long addr);
	bool read(const void* dest, int len, unsigned long addr);

	bool isAttached();
	void detach();

	/* Find address in a module given a signature & mask */
	static unsigned long FindAddress(unsigned long mod, unsigned long modSize, const unsigned char* sig, const char* mask, ScanType def, unsigned long extra = 0);

private:

	static unsigned long GetProcId(std::string procName);
	static unsigned long GetModuleBaseAddress(unsigned long procId, std::string modName, unsigned long* sizeBuf = nullptr);

	/* Find signature in current process memory */
	static unsigned long FindSignature(unsigned long base, unsigned long size, const unsigned char* sign, const char* mask);
	static bool DataCompare(unsigned char* data, const unsigned char* sign, const char* mask);

	void* handle = nullptr;
	unsigned long pid = 0;

};