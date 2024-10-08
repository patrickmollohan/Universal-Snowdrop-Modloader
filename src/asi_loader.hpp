#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shlobj.h>
#include <functional>
#include <set>
#include <intrin.h>

class ASILoader {
public:
    static bool Enable(HMODULE hModule);
    static void Disable();
};
