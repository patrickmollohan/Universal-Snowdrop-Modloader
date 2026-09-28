#include "pch.hpp"
#include "proxies.hpp"

BOOL Proxies::LoadProxy() {
    char sysdir[MAX_PATH];
    GetSystemDirectoryA(sysdir, MAX_PATH);

    char path[MAX_PATH];
    snprintf(path, MAX_PATH, "%s\\%s", sysdir, g_DllInfo.filename);

    HMODULE dll = LoadLibraryA(path);
    if (!dll) return FALSE;

    for (int i = 0; i < g_DllExports.count; ++i) {
        g_DllExports.procs[i] = reinterpret_cast<UINT_PTR>(GetProcAddress(dll, g_DllExports.funcs[i]));
        if (!g_DllExports.procs[i]) return FALSE;
    }
    return TRUE;
}
