#include "dll_main.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    VersionWrapper::Initialise();
    Utilities::SettingsParser::SetConfigFilePath(hModule);
    Settings::LoadSettings();
    Utilities::Processes::SetHighPriority();

    std::string exeName = Utilities::Processes::GetExeName();
    if (Utilities::String::EqualsIgnoreCase(exeName, "Outlaws.exe") || Utilities::String::EqualsIgnoreCase(exeName, "Outlaws_Plus.exe")) {
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