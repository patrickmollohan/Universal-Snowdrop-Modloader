#include "mod_loader.hpp"

ModLoader::open_file_stream_proc ModLoader::oldOpenFileStream = nullptr;

int ModLoader::pattern_size;
unsigned char* ModLoader::pattern;
unsigned char* ModLoader::mask;

bool __fastcall ModLoader::HookOpenFileStream(uintptr_t stream, LPCSTR file_path, unsigned int flags) {
    if (Utilities::Files::LocalFileExists(file_path)) {
        flags |= (1 << 0xA);
    }

    return oldOpenFileStream(stream, file_path, flags);
}

bool ModLoader::Enable() {
    if (!Settings::EnableMods) return false;

    if (Utilities::Processes::IsGameAvatar()) {
        pattern_size = 26;
        pattern = new unsigned char[pattern_size] { 0x48, 0x89, 0x5C, 0x24, 0x00, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0xF8 };
        mask    = new unsigned char[pattern_size] { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF };
    } else if (Utilities::Processes::IsGameOutlaws()) {
        pattern_size = 17;
        pattern = new unsigned char[pattern_size] { 0x4C, 0x8B, 0xDC, 0x53, 0x57, 0x41, 0x54, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0xD8 };
        mask    = new unsigned char[pattern_size] { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF };
    }

    uintptr_t openFileStreamAddress = Utilities::Processes::FindPatternAddressMask(pattern, mask, pattern_size);
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
