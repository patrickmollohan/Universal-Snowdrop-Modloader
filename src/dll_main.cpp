#include "dll_main.hpp"

std::string DLLMain::exeName = "";
GameID DLLMain::gameID = GAME_UNSUPPORTED;
bool DLLMain::initialised = false;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
        DisableThreadLibraryCalls(hModule);

        switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            VersionWrapper::Initialise();
            Utilities::Processes::SetExeName();
            Utilities::Processes::SetGameID();
            if (Utilities::Processes::IsCompatibleExe() && !DLLMain::initialised) {
                Utilities::SettingsParser::SetConfigFilePath(hModule);
                Settings::LoadSettings();
                Utilities::Processes::SetPriorityLevels();
                MinHookHandler::Initialise();
                DiskCacheEnabler::Enable();
                ModLoader::Enable();
                MinHookHandler::EnableAllHooks();
                ScriptLoader::LoadScripts();
                DLLMain::initialised = true;
                return true;
            }
            break;
        case DLL_PROCESS_DETACH:
            if (Utilities::Processes::IsCompatibleExe() && DLLMain::initialised) {
                DiskCacheEnabler::Disable();
                ModLoader::Disable();
                MinHookHandler::Shutdown();
                DLLMain::initialised = false;
            }
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        }
    return false;
}
