#include "dllmain.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        MinHookHandler::Initialise();
        ASILoader::Enable(hModule);
        DiskCacheEnabler::Enable();
        ModLoader::Enable();
        MinHookHandler::EnableAllHooks();
    } else if (dwReason == DLL_PROCESS_DETACH) {
        ASILoader::Disable();
        DiskCacheEnabler::Disable();
        ModLoader::Disable();
        MinHookHandler::Shutdown();
    } else {
        return FALSE;
    }
    return TRUE;
}
