#pragma once
#include <Windows.h>

class Cheat {
private:
	HANDLE hProcess;
	bool attached = false;
	bool* godMode;
	const int healthOffset = 0xF8;
	const uintptr_t playerBasePtr = 0x509B74;
	void* playerBase;
	void godModeThread(bool* godMode);

public:
	bool attach(int pid);
	bool read(LPCVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = nullptr);
	bool write(LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten = nullptr);
	void enableGodMode(bool* godMode) noexcept;
	void setHealth(int value) noexcept;
	void setArmour(int value) noexcept;
	};
	
