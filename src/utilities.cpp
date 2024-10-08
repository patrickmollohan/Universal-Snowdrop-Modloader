#include "utilities.hpp"

bool Utilities::Files::FileExists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

Utilities::MainModule::MainModule() {
    BaseAddress = NULL;
    EndAddress = NULL;

    // Load module information
    auto Mod = (HMODULE)GetModuleHandle(NULL);

    // Load PE information
    auto DOSHeader = (const IMAGE_DOS_HEADER*)(Mod);
    auto NTHeader = (const IMAGE_NT_HEADERS*)((const uint8_t*)(DOSHeader) + DOSHeader->e_lfanew);

    // Calculate addresses
    BaseAddress = (uintptr_t)Mod;
    EndAddress = (uintptr_t)(this->BaseAddress + NTHeader->OptionalHeader.SizeOfCode);

    // Get module path
    char ModPath[2048] = { 0 };
    GetModuleFileNameA(NULL, ModPath, 2048);

    // Set it
    ModulePath = std::string(ModPath);
}

Utilities::MainModule::~MainModule() { }

uintptr_t Utilities::MainModule::GetBaseAddress() const {
    return BaseAddress;
}

uintptr_t Utilities::MainModule::GetCodeSize() const {
    return (EndAddress - BaseAddress);
}

size_t Utilities::MainModule::Begin() const {
    return (size_t)BaseAddress;
}

size_t Utilities::MainModule::End() const {
    return (size_t)EndAddress;
}

std::string Utilities::MainModule::GetModulePath() {
    return ModulePath;
}

uintptr_t Utilities::Processes::FindPatternAddress(const unsigned char* pattern) {
    Utilities::MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();
    size_t patternLength = sizeof(pattern);

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            // Check if the current byte matches the pattern
            if (*(reinterpret_cast<unsigned char*>(baseAddress + i + j)) != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i;
        }
    }
    return 0;
}

uintptr_t Utilities::Processes::FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask) {
    MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();
    size_t patternLength = sizeof(pattern);

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            // Check if the current byte is a wildcard (0x00) or matches the pattern (0xFF)
            if (mask[j] != 0x00 && *(reinterpret_cast<unsigned char*>(baseAddress + i + j)) != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i;
        }
    }
    return 0;
}