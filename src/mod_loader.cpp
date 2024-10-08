#include "mod_loader.hpp"

const unsigned char ModLoader::pattern[] = { 0x4C, 0x8B, 0xDC, 0x53, 0x57, 0x41, 0x54, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0xD8 };
const unsigned char ModLoader::mask[]    = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF };

ModLoader::open_file_stream_proc oldOpenFileStream = nullptr;

bool __fastcall ModLoader::HookOpenFileStream(uintptr_t stream, LPCSTR file_path, unsigned int flags) {
    if (Utilities::Files::FileExists(file_path)) {
        flags |= (1 << 0xA);
    }
    return oldOpenFileStream(stream, file_path, flags);
}

bool ModLoader::Enable() {
    uintptr_t openFileStreamAddress = Utilities::Processes::FindPatternAddressMask(pattern, mask, (sizeof(pattern) / sizeof(pattern[0])));
    if (!openFileStreamAddress) {
        MessageBoxA(NULL, "OpenFileStream pattern not found. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    oldOpenFileStream = reinterpret_cast<open_file_stream_proc>(openFileStreamAddress);
    if (MH_CreateHook(oldOpenFileStream, &HookOpenFileStream, reinterpret_cast<LPVOID*>(&oldOpenFileStream)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook for OpenFileStream. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }
    
    return true;
}

void ModLoader::Disable() {
    if (oldOpenFileStream) {
        MH_DisableHook(oldOpenFileStream);
    }
}
