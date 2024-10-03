#include "mod_loader.hpp"

open_file_stream_proc oldOpenFileStream = nullptr;

BOOL FileExists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL __fastcall HookOpenFileStream(uintptr_t stream, LPCSTR file_path, unsigned int flags) {
    if (FileExists(file_path)) {
        return oldOpenFileStream(stream, file_path, flags | (1 << 0xA));
    }
    return oldOpenFileStream(stream, file_path, flags);
}

uintptr_t FindOpenFileStreamAddress() {
    MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();
    size_t patternLength = sizeof(pattern);

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            // Check if the current byte is a wildcard or matches the pattern
            if (mask[j] != 0x00 && *(reinterpret_cast<unsigned char*>(baseAddress + i + j)) != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i; // Return the found address
        }
    }
    return 0; // Return 0 if not found
}

void ModLoaderMain(HMODULE hModule, DWORD reason) {
    if (reason == DLL_PROCESS_ATTACH) {
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            MessageBoxA(NULL, "Failed to initialise MinHook. Mod loading disabled.", "Error", MB_OK | MB_ICONERROR);
            return;
        }

        HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
        if (hKernel32) {
            // Hook the OpenFileStream function
            uintptr_t openFileStreamAddress = FindOpenFileStreamAddress();
            if (openFileStreamAddress) {
                oldOpenFileStream = reinterpret_cast<open_file_stream_proc>(openFileStreamAddress);
                if (MH_CreateHook(oldOpenFileStream, &HookOpenFileStream, reinterpret_cast<LPVOID*>(&oldOpenFileStream)) != MH_OK) {
                    MessageBoxA(NULL, "Failed to create hook for OpenFileStream. Mod loading disabled.", "Error", MB_OK | MB_ICONERROR);
                    return;
                }
            }

            // Enable the hooks
            MH_EnableHook(MH_ALL_HOOKS);
        }
    }
}
