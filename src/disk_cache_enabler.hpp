#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <MinHook.h>
#include "settings.hpp"

class DiskCacheEnabler {
public:
    static bool Enable();
    static void Disable();

private:
    static HANDLE CreateFileA_hook(
        LPCSTR                lpFilename,
        DWORD                 dwDesiredAccess,
        DWORD                 dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD                 dwCreationDisposition,
        DWORD                 dwFlagsAndAttributes,
        HANDLE                hTemplateFile);

    static HANDLE CreateFileW_hook(
        LPCWSTR               lpFilename,
        DWORD                 dwDesiredAccess,
        DWORD                 dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD                 dwCreationDisposition,
        DWORD                 dwFlagsAndAttributes,
        HANDLE                hTemplateFile);
};