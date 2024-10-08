#include "utilities.hpp"

bool Utilities::Files::FileExists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

Utilities::MainModule::MainModule() 
    : BaseAddress(0), EndAddress(0) {

    // Load module information
    HMODULE Mod = GetModuleHandle(NULL);

    // Load PE information
    const IMAGE_DOS_HEADER* DOSHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(Mod);
    const IMAGE_NT_HEADERS* NTHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>((const uint8_t*)DOSHeader + DOSHeader->e_lfanew);

    // Calculate addresses
    BaseAddress = reinterpret_cast<uintptr_t>(Mod);
    EndAddress = BaseAddress + NTHeader->OptionalHeader.SizeOfCode;

    // Get module path
    char ModPath[MAX_PATH];
    if (GetModuleFileNameA(Mod, ModPath, sizeof(ModPath))) {
        ModulePath = ModPath;
    }
}

Utilities::MainModule::~MainModule() { }

uintptr_t Utilities::MainModule::GetBaseAddress() const {
    return BaseAddress;
}

uintptr_t Utilities::MainModule::GetCodeSize() const {
    return EndAddress - BaseAddress;
}

size_t Utilities::MainModule::Begin() const {
    return static_cast<size_t>(BaseAddress);
}

size_t Utilities::MainModule::End() const {
    return static_cast<size_t>(EndAddress);
}

std::string Utilities::MainModule::GetModulePath() const {
    return ModulePath;
}

uintptr_t Utilities::Processes::FindPatternAddress(const unsigned char* pattern, size_t patternLength) {
    Utilities::MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        if (memcmp(reinterpret_cast<void*>(baseAddress + i), pattern, patternLength) == 0) {
            return baseAddress + i;
        }
    }
    return 0;
}

uintptr_t Utilities::Processes::FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask, size_t patternLength) {
    Utilities::MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            if (mask[j] != PATTERN_WILDCARD && *(reinterpret_cast<unsigned char*>(baseAddress + i + j)) != pattern[j]) {
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
