#include "utilities.hpp"

bool Utilities::FileExists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

uintptr_t Utilities::FindPatternAddress(const unsigned char* pattern) {
    MainModule module;
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

uintptr_t Utilities::FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask) {
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
            uintptr_t addr = baseAddress + i;
            //char message[256];
            //const char* foundAt = "Pattern found at: ";
            //const char* baseAddressString = "Base address: ";
            //snprintf(message, sizeof(message), "%s%p\n%s%p", foundAt, (void*)addr, baseAddressString, (void*)baseAddress);
            //MessageBoxA(NULL, message, "Information", MB_OK);
            return addr; // Return the found address
        }
    }
    return 0;
}