#include "pch.hpp"
#include "mods.hpp"

Mods::load_file_t Mods::origLoadFilePtr = nullptr;
Mods::stream_t Mods::origStreamingPtr = nullptr;
volatile uint8_t* Mods::streamingFlag = nullptr;

volatile uint8_t* Mods::FindStreamingFlag() {
    uintptr_t anchor = Utilities::PatternScanner::FindPattern("C6 05 ?? ?? ?? ?? ?? 40 38 35");
    if (!anchor) return nullptr;

    int32_t ripOffset;
    memcpy(&ripOffset, reinterpret_cast<void*>(anchor + 2), sizeof(ripOffset));

    uintptr_t nextInstr = anchor + 7;
    return reinterpret_cast<volatile uint8_t*>(nextInstr + ripOffset);
}

bool Mods::LoadMods() {
    if (!Settings::EnableMods) return false;

    uintptr_t loadFilePtr;
    uintptr_t streamingPtr;

    if (Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "afop") || Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "avatar")) {
        loadFilePtr = Utilities::PatternScanner::FindPattern("48 89 5C 24 ?? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ?? ?? ?? ?? 41 8B F8");
        streamingPtr = Utilities::PatternScanner::FindPattern("44 88 44 24 18 48 89 54 24 10 48 89 4C 24 08 55 53 57");
    } else if (Utilities::String::ContainsIgnoreCase(g_ExeInfo.filename, "outlaws")) {
        loadFilePtr = Utilities::PatternScanner::FindPattern("4C 8B DC 53 57 41 54 48 81 EC ?? ?? ?? ?? 41 8B D8");
        streamingPtr = Utilities::PatternScanner::FindPattern("48 8B C4 44 88 40 18 48 89 50 10 55 53 57");
    } else {
        MessageBoxA(NULL, "Unsupported executable for mod loading. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    if (!loadFilePtr) {
        MessageBoxA(NULL, "Could not locate LoadFile. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    origLoadFilePtr = reinterpret_cast<load_file_t>(loadFilePtr);
    if (MH_CreateHook(reinterpret_cast<LPVOID>(origLoadFilePtr), reinterpret_cast<LPVOID>(&HookedLoadFile), reinterpret_cast<LPVOID*>(&origLoadFilePtr)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook for LoadFile. Mod loading disabled.", "Dank farrik!", MB_OK | MB_ICONERROR);
        return false;
    }

    streamingFlag = FindStreamingFlag();
    if (!streamingFlag) {
        MessageBoxA(NULL, "Could not locate the streaming flag. Streamed assets (meshes, etc.) will not be overridable; one-shot files are unaffected.", "Dank farrik!", MB_OK | MB_ICONWARNING);
    }

    if (!streamingPtr) {
        MessageBoxA(NULL, "Could not locate the streaming function. Streamed assets (meshes, etc.) will not be overridable; one-shot files are unaffected.", "Dank farrik!", MB_OK | MB_ICONWARNING);
    } else {
        origStreamingPtr = reinterpret_cast<stream_t>(streamingPtr);
        if (MH_CreateHook(reinterpret_cast<LPVOID>(origStreamingPtr), reinterpret_cast<LPVOID>(&HookedStream), reinterpret_cast<LPVOID*>(&origStreamingPtr)) != MH_OK) {
            MessageBoxA(NULL, "Failed to create hook for the streaming function. Streamed assets (meshes, etc.) will not be overridable; one-shot files are unaffected.", "Dank farrik!", MB_OK | MB_ICONWARNING);
            origStreamingPtr = nullptr;
        }
    }

    return true;
}

bool Mods::UnloadMods() {
    if (!Settings::EnableMods) return false;
    return true;
}

bool __fastcall Mods::HookedLoadFile(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags) {
    if (Utilities::Files::FileExists(filePath)) {
        flags = (flags & ~0x2u) | 0x400u;
    }
    return origLoadFilePtr(fileCtx, filePath, flags);
}

__int64 __fastcall Mods::HookedStream(uintptr_t a1, LPCSTR path, uint8_t flag) {
    if (streamingFlag) *streamingFlag = 1;
    __int64 result = origStreamingPtr(a1, path, flag);
    if (streamingFlag) *streamingFlag = 0;
    return result;
}
