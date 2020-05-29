#pragma once

#include <string>

class MemProc {
public:

	MemProc();
	MemProc(std::string exeFile);

	bool attach(std::string exeFile);

	unsigned long getModule(std::string name);
	bool write(const void* src, int len, unsigned long addr);
	bool read(const void* dest, int len, unsigned long addr);

	bool isAttached();
	void detach();

private:

	void* handle = nullptr;
	unsigned long pid = 0;

};