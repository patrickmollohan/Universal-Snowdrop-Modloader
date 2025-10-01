#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <MinHook.h>
#include "settings.hpp"
#include "utilities.hpp"

class ModLoader {
public:
    static bool Enable();
    static void Disable();

private:
    using load_file_t = bool(__fastcall*)(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags);
    static load_file_t OrigLoadFile;
    static bool __fastcall HookedLoadFile(uintptr_t fileCtx, LPCSTR filePath, unsigned int flags);

    static int pattern_size;
    static unsigned char* pattern;
    static unsigned char* mask;
};