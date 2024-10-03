#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

// Platform includes
#include <Windows.h>
#include <string>

// A class that provides information about the main module
class MainModule {
private:
	uintptr_t BaseAddress;
	uintptr_t EndAddress;
	std::string ModulePath;

public:
	MainModule() {
		this->BaseAddress = NULL;
		this->EndAddress = NULL;

		// Load module information
		auto Mod = (HMODULE)GetModuleHandle(NULL);

		// Load PE information
		auto DOSHeader = (const IMAGE_DOS_HEADER*)(Mod);
		auto NTHeader = (const IMAGE_NT_HEADERS*)((const uint8_t*)(DOSHeader)+DOSHeader->e_lfanew);

		// Calculate addresses
		this->BaseAddress = (uintptr_t)Mod;
		this->EndAddress = (uintptr_t)(this->BaseAddress + NTHeader->OptionalHeader.SizeOfCode);

		// Get module path
		char ModPath[2048] = { 0 };
		GetModuleFileNameA(NULL, ModPath, 2048);

		// Set it
		this->ModulePath = std::string(ModPath);
	}

	~MainModule() { }

	// Gets the module base address
	uintptr_t GetBaseAddress() {
		return this->BaseAddress;
	}

	// Gets the size of code reported by the PE header
	uintptr_t GetCodeSize() {
		return (this->EndAddress - this->BaseAddress);
	}

	size_t Begin() const {
		return (size_t)this->BaseAddress;
	}

	size_t End() const {
		return (size_t)this->EndAddress;
	}

	std::string GetModulePath() {
		return this->ModulePath;
	}
};