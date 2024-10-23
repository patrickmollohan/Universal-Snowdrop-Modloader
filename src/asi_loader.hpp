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
private:
    static void _GetFileVersionInfoA();
    static void _GetFileVersionInfoByHandle();
    static void _GetFileVersionInfoExA();
    static void _GetFileVersionInfoExW();
    static void _GetFileVersionInfoSizeA();
    static void _GetFileVersionInfoSizeExA();
    static void _GetFileVersionInfoSizeExW();
    static void _GetFileVersionInfoSizeW();
    static void _GetFileVersionInfoW();
    static void _VerFindFileA();
    static void _VerFindFileW();
    static void _VerInstallFileA();
    static void _VerInstallFileW();
    static void _VerLanguageNameA();
    static void _VerLanguageNameW();
    static void _VerQueryValueA();
    static void _VerQueryValueW();

    static void _DllRegisterServer();
    static void _DllUnregisterServer();
    static void _DllCanUnloadNow();
    static void _DllGetClassObject();

    static bool WINAPI IsUltimateASILoader();

    static void* WINAPI GetMemoryModule();
};
