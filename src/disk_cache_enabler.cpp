#include "disk_cache_enabler.hpp"
#include "dllmain.hpp"

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

void DiskCacheEnablerMain(HMODULE hModule, DWORD dwReason) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        // Initialize MinHook
        if (!minhookInitialised) {
            if (MH_Initialize() != MH_OK) {
                MessageBoxA(NULL, "Failed to initialise MinHook. Mod loading disabled.", "Error", MB_OK | MB_ICONERROR);
                return;
            }
            minhookInitialised = TRUE;
        }

        // Create hooks for CreateFileA and CreateFileW
        if (MH_CreateHook(reinterpret_cast<LPVOID>(CreateFileA), 
                          &CreateFileA_hook, 
                          reinterpret_cast<LPVOID*>(&CreateFileA_orig)) != MH_OK) {
            MessageBoxA(NULL, "Failed to create hook!", "Error", MB_OK);
        }

        if (MH_CreateHook(reinterpret_cast<LPVOID>(CreateFileW), 
                          &CreateFileW_hook, 
                          reinterpret_cast<LPVOID*>(&CreateFileW_orig)) != MH_OK) {
            MessageBoxA(NULL, "Failed to create hook for CreateFileW!", "Error", MB_OK);
        }

        // Enable the hooks
        if (MH_EnableHook(reinterpret_cast<LPVOID>(CreateFileA)) != MH_OK) {
            MessageBoxA(NULL, "Failed to enable hook!", "Error", MB_OK);
        }

        if (MH_EnableHook(reinterpret_cast<LPVOID>(CreateFileW)) != MH_OK) {
            MessageBoxA(NULL, "Failed to enable hook for CreateFileW!", "Error", MB_OK);
        }
    } else if (dwReason == DLL_PROCESS_DETACH) {
        // Disable the hooks
        MH_DisableHook(reinterpret_cast<LPVOID>(CreateFileA));
        MH_DisableHook(reinterpret_cast<LPVOID>(CreateFileW));

        // Uninitialize MinHook
        MH_Uninitialize();
        minhookInitialised = FALSE;
    }
}
