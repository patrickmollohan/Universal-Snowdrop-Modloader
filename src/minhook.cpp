#include "pch.hpp"
#include "minhook.hpp"

void MinHook::EnableAllHooks() {
    MH_EnableHook(MH_ALL_HOOKS);
}

bool MinHook::LoadMinHook() {
    if (MH_Initialize() == MH_OK) return true;
    int errorMessageResult = MessageBoxA(NULL, "Failed to initialise MinHook.\nSome functionality may be disabled.", "Dank farrik!", MB_OKCANCEL | MB_ICONERROR);
    if (errorMessageResult == IDCANCEL) ExitProcess(0);
    return false;
}

void MinHook::UnloadMinHook() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
