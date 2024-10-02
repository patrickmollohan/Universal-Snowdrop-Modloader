#include "mod_loader.hpp"

typedef bool(__fastcall* open_file_stream_proc)(__int64 stream, LPCSTR file_path, unsigned int flags);
open_file_stream_proc old_open_file_stream = nullptr;
typedef DWORD(WINAPI* GetFileAttributesWType)(LPCWSTR);
GetFileAttributesWType OriginalGetFileAttributesW = nullptr;
uintptr_t old_getsystemtimeasfiletime = 0;

const LPCSTR patterns[] = { "4C 8B DC 53 57 41 54 48 81 EC ? ? ? ? 41 8B D8" };

BOOL file_exists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL __fastcall hk_open_file_stream(uintptr_t stream, LPCSTR file_path, unsigned int flags) {
    if (file_exists(file_path)) {
        return old_open_file_stream(stream, file_path, flags | (1 << 0xA));
    }
    return old_open_file_stream(stream, file_path, flags);
}

DWORD WINAPI hk_getsystemtimeasfiletime(LPCWSTR lpSystemTimeAsFileTime) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)OriginalGetFileAttributesW, hk_getsystemtimeasfiletime);
    DetourTransactionCommit();
    
    MainModule Module;
    uintptr_t Result = (uintptr_t)-1;
    for (auto i = 0; i < ARRAYSIZE(patterns); i++) {
        Result = FindPattern(patterns[i], Module.GetBaseAddress(), Module.GetCodeSize());
        if (Result != (uintptr_t)-1) {
            break;
        }
    }

    if (Result == (uintptr_t)-1) {
        MessageBoxA(GetActiveWindow(), "Failed to find required modloader functions! Mod support will not be available.", "Ultimate Star Wars Outlaws ModLoader", MB_OK | MB_ICONERROR);
    } else {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        old_open_file_stream = (open_file_stream_proc)(Result + Module.GetBaseAddress());
        DetourAttach(&(PVOID&)old_open_file_stream, hk_open_file_stream);
        DetourTransactionCommit();
    }    

    return GetFileAttributesW(lpSystemTimeAsFileTime);
}

void ModLoaderMain(HMODULE hModule, DWORD reason) {
    if (reason == DLL_PROCESS_ATTACH) {
        HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
        if (hKernel32) {
            OriginalGetFileAttributesW = (GetFileAttributesWType)GetProcAddress(hKernel32, "GetFileAttributesW");
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)OriginalGetFileAttributesW, hk_getsystemtimeasfiletime);
            DetourTransactionCommit();
        }
    }
}