#include "dllmain.hpp"

BOOL minhookInitialised = FALSE;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*lpReserved*/) {
    ASILoaderMain(hModule, reason);
    DiskCacheEnablerMain(hModule, reason);
    ModLoaderMain(hModule, reason);
    return TRUE;
}
