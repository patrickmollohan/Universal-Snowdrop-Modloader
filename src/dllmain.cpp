#include <shlobj.h>
#include "asi_loader.hpp"
#include "mod_loader.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*lpReserved*/) {
    ASILoaderMain(hModule, reason);
    ModLoaderMain(hModule, reason);
    return TRUE;
}
