#include "dll_main.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            MinHookHandler::Initialise();
            ASILoader::Enable(hModule);
            DiskCacheEnabler::Enable();
            ModLoader::Enable();
            MinHookHandler::EnableAllHooks();
            break;
        case DLL_PROCESS_DETACH:
            ASILoader::Disable();
            DiskCacheEnabler::Disable();
            ModLoader::Disable();
            MinHookHandler::Shutdown();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}
