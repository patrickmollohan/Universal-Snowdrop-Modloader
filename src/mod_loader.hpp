#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <MinHook.h>
#include "utilities.hpp"

class ModLoader {
public:
    static bool Enable();
    static void Disable();

    typedef bool(__fastcall* open_file_stream_proc)(__int64 stream, LPCSTR file_path, unsigned int flags);

private:
    static bool __fastcall HookOpenFileStream(uintptr_t stream, LPCSTR file_path, unsigned int flags);

    static const unsigned char pattern[];
    static const unsigned char mask[];
};