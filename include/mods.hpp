#pragma once

#include "globals.hpp"
#include "minhook.hpp"
#include "settings.hpp"

class Mods {
public:
    static bool LoadMods();
    static bool UnloadMods();

private:
    using load_file_t = bool(__fastcall*)(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags);
    static load_file_t origLoadFilePtr;
    static bool __fastcall HookedLoadFile(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags);

    using stream_t = int64_t(__fastcall*)(uintptr_t a1, LPCSTR filePath, uint8_t flag);
    static stream_t origStreamingPtr;
    static int64_t __fastcall HookedStream(uintptr_t a1, LPCSTR filePath, uint8_t flag);

    static volatile uint8_t* noMeshStreamingFlag;
    static volatile uint8_t* FindNoMeshStreamingFlag();
};
