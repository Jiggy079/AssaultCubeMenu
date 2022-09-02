#pragma once
#include <Windows.h>

class Cheat {
private:
	HANDLE hProcess = NULL;
	bool attached = false;
	const int healthOffset = 0xF8;
	const uintptr_t playerBasePtr = 0x509B74;
	void* playerBase = nullptr;
	void godModeThread(bool* godMode) noexcept;

public:
	bool isAttached();
	bool attach(int pid);
	bool read(LPCVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = nullptr);
	bool write(LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten = nullptr);
	void enableGodMode(bool* godMode) noexcept;
	int getHealth() noexcept;
	void setHealth(int value) noexcept;
	void setArmour(int value) noexcept;
	uintptr_t getPlayerBase();
	};
	
