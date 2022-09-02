#include "Cheat.h"
#include <thread>

bool Cheat::isAttached() {
	return this->attached;
}

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
	if (!this->attached) {
		return false;
	}
	return ReadProcessMemory(this->hProcess, lpBaseAddress, (LPVOID)lpBuffer, nSize, nullptr);
}

bool Cheat::write(LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
	if (!this->attached) {
		return false;
	}
	return WriteProcessMemory(this->hProcess, lpBaseAddress, (LPVOID)lpBuffer, nSize, nullptr);
}

void Cheat::godModeThread(bool* godMode) noexcept {
	int startingHealth = Cheat::getHealth();
	int currentHealth;
	while (*godMode == true) {
		currentHealth = Cheat::getHealth();
		if (currentHealth != startingHealth) {
			Cheat::setHealth(startingHealth);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return;
}

void Cheat::enableGodMode(bool* godMode) noexcept {
	if (!this->attached) {
		return;
	}
	std::thread t1(&Cheat::godModeThread, this, godMode);
	t1.detach();
}

int Cheat::getHealth() noexcept {
	int health;
	if (!this->attached) {
		return NULL;
	}
	Cheat::read((LPCVOID)((uintptr_t)playerBase + healthOffset), &health, 4);
	return health;
}

void Cheat::setHealth(int value) noexcept {
	if (!this->attached) {
		return;
	}
	Cheat::write((LPVOID)((uintptr_t)playerBase + healthOffset), &value, 4);
}

void Cheat::setArmour(int value) noexcept {
	if (!this->attached) {
		return;
	}
}

uintptr_t Cheat::getPlayerBase() {
	return (uintptr_t)this->playerBase;
}