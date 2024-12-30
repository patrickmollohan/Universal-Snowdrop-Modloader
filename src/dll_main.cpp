#include "dll_main.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    VersionWrapper::Initialise();
    if (Utilities::Processes::IsCompatibleExe()) {
        Utilities::SettingsParser::SetConfigFilePath(hModule);
        Settings::LoadSettings();
        Utilities::Processes::SetPriorityLevels();

        switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            MinHookHandler::Initialise();
            DiskCacheEnabler::Enable();
            ModLoader::Enable();
            MinHookHandler::EnableAllHooks();
            ScriptLoader::LoadScripts();
            break;
        case DLL_PROCESS_DETACH:
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