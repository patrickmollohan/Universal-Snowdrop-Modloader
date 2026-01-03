#include "pch.hpp"
#include "mods.hpp"

Mods::load_file_t Mods::origLoadFilePtr = nullptr;

bool Mods::LoadMods() {
    if (!Settings::EnableMods) return false;

    uintptr_t loadFilePtr;
    if (Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "afop") || Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "avatar")) {
        loadFilePtr = Utilities::PatternScanner::FindPattern("48 89 5C 24 ?? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ?? ?? ?? ?? 41 8B F8");
    } else if (Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "outlaws")) {
        loadFilePtr = Utilities::PatternScanner::FindPattern("4C 8B DC 53 57 41 54 48 81 EC ?? ?? ?? ?? 41 8B D8");
    } else {
        MessageBoxA(NULL, "Unsupported executable for mod loading. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    origLoadFilePtr = reinterpret_cast<load_file_t>(loadFilePtr);
    if (MH_CreateHook(origLoadFilePtr, &HookedLoadFile, reinterpret_cast<LPVOID*>(&origLoadFilePtr)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook for LoadFile. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

bool Mods::UnloadMods() {
    if (!Settings::EnableMods) return false;
    return true;
}

bool __fastcall Mods::HookedLoadFile(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags) {
    if (Utilities::Files::FileExists(filePath)) flags |= (1 << 0xA);
    return origLoadFilePtr(fileCtx, filePath, flags);
}
