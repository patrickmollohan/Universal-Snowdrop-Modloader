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
    using open_file_stream_proc = bool(__fastcall*)(uintptr_t stream, LPCSTR file_path, unsigned int flags);

    static bool __fastcall HookOpenFileStream(uintptr_t stream, LPCSTR file_path, unsigned int flags);

    static open_file_stream_proc oldOpenFileStream;
    static int pattern_size;
    static unsigned char* pattern;
    static unsigned char* mask;
};