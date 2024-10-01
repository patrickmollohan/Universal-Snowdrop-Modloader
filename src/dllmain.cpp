#include <shlobj.h>
#include "asi_loader.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*lpReserved*/) {
    ASILoaderMain(hModule, reason);
    return TRUE;
}
