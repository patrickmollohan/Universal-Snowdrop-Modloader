#include "disk_cache_enabler.hpp"

// Original function pointers
static HANDLE(WINAPI *CreateFileA_orig)(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile) = CreateFileA;

static HANDLE(WINAPI *CreateFileW_orig)(
    LPCWSTR               lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile) = CreateFileW;

HANDLE CreateFileA_hook(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile) {

    dwFlagsAndAttributes &= ~(FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN);
    dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;

    return CreateFileA_orig(
        lpFilename,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

HANDLE CreateFileW_hook(
    LPCWSTR               lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile) {

    dwFlagsAndAttributes &= ~(FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN);
    dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;

    return CreateFileW_orig(
        lpFilename,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
}

bool DiskCacheEnabler::Enable() {
    if (MH_CreateHook(reinterpret_cast<LPVOID>(CreateFileA),
        &CreateFileA_hook,
        reinterpret_cast<LPVOID*>(&CreateFileA_orig)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook for CreateFileA!", "Error", MB_OK);
        return false;
    }

    if (MH_CreateHook(reinterpret_cast<LPVOID>(CreateFileW),
        &CreateFileW_hook,
        reinterpret_cast<LPVOID*>(&CreateFileW_orig)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook for CreateFileW!", "Error", MB_OK);
        return false;
    }

    // Enable the hooks
    if (MH_EnableHook(reinterpret_cast<LPVOID>(CreateFileA)) != MH_OK) {
        MessageBoxA(NULL, "Failed to enable hook for CreateFileA!", "Error", MB_OK);
        return false;
    }

    if (MH_EnableHook(reinterpret_cast<LPVOID>(CreateFileW)) != MH_OK) {
        MessageBoxA(NULL, "Failed to enable hook for CreateFileW!", "Error", MB_OK);
        return false;
    }
    return true;
}

void DiskCacheEnabler::Disable() {
    MH_DisableHook(reinterpret_cast<LPVOID>(CreateFileA));
    MH_DisableHook(reinterpret_cast<LPVOID>(CreateFileW));
}
