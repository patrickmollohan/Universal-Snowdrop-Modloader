#include "dll_main.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    std::string exeName = Utilities::Processes::GetExeName();

    if (Utilities::String::EqualsIgnoreCase(exeName, "Outlaws.exe") || Utilities::String::EqualsIgnoreCase(exeName, "Outlaws_Plus.exe")) {
        Utilities::Processes::SetHighPriority();
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
    return FALSE;
}
