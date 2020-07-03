#pragma once

#include <string>

/*
Utility for external program memory alteration.
*/
class MemProc {
public:

	enum class ScanType {
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
	bool attachSelf();

	unsigned long getModule(std::string name, unsigned long* sizeBuf = nullptr);
	void printModules();

	bool write(const void* src, int len, unsigned long addr);
	bool read(const void* dest, int len, unsigned long addr);

	bool isAttached();
	void detach();

	/* Find address in a module given a signature & mask */
	unsigned long FindAddress(unsigned long mod, unsigned long modSize, const unsigned char* sig, const char* mask, ScanType def, unsigned long extra = 0);

private:

	/* Find signature in current process memory */
	unsigned long FindSignature(unsigned long base, unsigned long size, const unsigned char* sign, const char* mask);
	/* Check if signature matches data chunk */
	static bool DataCompare(unsigned char* data, const unsigned char* sign, const char* mask);

	bool openProc(unsigned long pid);

	void* hProc = nullptr;
	unsigned long pid = 0;

};