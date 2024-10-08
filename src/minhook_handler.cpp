#include "minhook_handler.hpp"

bool MinHookHandler::mhInitialised = false;

bool MinHookHandler::Initialise() {
    if (!mhInitialised) {
        if (MH_Initialize() == MH_OK) {
            mhInitialised = true;
        } else {
            int errorMessageResult = MessageBoxA(NULL, "Failed to initialise MinHook.\nSome functionality may be disabled.", "MinHook Error", MB_OKCANCEL | MB_ICONERROR);
            if (errorMessageResult == IDCANCEL) {
                ExitProcess(0);
            }
        }
    }
    return mhInitialised;
}

void MinHookHandler::EnableAllHooks() {
    MH_EnableHook(MH_ALL_HOOKS);
}

void MinHookHandler::Shutdown() {
    if (!mhInitialised) {
        return;
    }
    MH_Uninitialize();
    mhInitialised = false;
}