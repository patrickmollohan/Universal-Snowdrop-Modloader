#include "pch.hpp"
#include "main.hpp"

static std::once_flag initFlag;
static std::once_flag cleanupFlag;

DWORD WINAPI Initialise(LPVOID) {
    std::call_once(initFlag, []() {
        Proxies::LoadProxy();
        Settings::LoadSettings();
        MinHook::LoadMinHook();
        Mods::LoadMods();
        GUI::Load();
        MinHook::EnableAllHooks();
        Plugins::LoadPlugins();
    });

    return 0;
}

void Cleanup() {
    std::call_once(cleanupFlag, []() {
        Mods::UnloadMods();
        Plugins::UnloadPlugins();
        GUI::Unload();
        MinHook::UnloadMinHook();
    });
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_ExeInfo = Utilities::Module::GetModuleInfo(GetModuleHandleW(nullptr));
        g_DllInfo = Utilities::Module::GetModuleInfo(hModule);
        CreateThread(nullptr, 0, Initialise, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        Cleanup();
        break;
    }
    return TRUE;
}
