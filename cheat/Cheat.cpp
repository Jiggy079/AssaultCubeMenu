#include "Cheat.h"
#include <thread>

bool Cheat::attach(int pid) {
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (this->hProcess == NULL) {
		return false;
	}
	else {
		Cheat::read((LPCVOID)playerBasePtr, &playerBase, 32);
		this->attached = true;
		return true;
	}

}
bool Cheat::read(LPCVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead) {
	if (!attached) {
		return false;
	}
	return ReadProcessMemory(this->hProcess, lpBaseAddress, (LPVOID)lpBuffer, nSize, nullptr);
}

bool Cheat::write(LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
	if (!attached) {
		return false;
	}
	return WriteProcessMemory(this->hProcess, lpBaseAddress, (LPVOID)lpBuffer, nSize, nullptr);
}

void Cheat::godModeThread(bool* godMode) {

}

void Cheat::enableGodMode(bool* godMode) noexcept {
	if (!attached) {
		return;
	}
	std::thread t1(godModeThread, godMode);
}

void Cheat::setHealth(int value) noexcept {
	if (!attached) {
		return;
	}
	Cheat::write((LPVOID)((uintptr_t)playerBase + healthOffset), &value, 4);
}

void Cheat::setArmour(int value) noexcept {
	if (!attached) {
		return;
	}
}