#include "dll_main.hpp"

std::string DLLMain::exeName = "";
GameID DLLMain::gameID = GAME_UNSUPPORTED;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    VersionWrapper::Initialise();
    Utilities::Processes::SetExeName();
    Utilities::Processes::SetGameID();
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
