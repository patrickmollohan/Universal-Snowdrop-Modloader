#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shlobj.h>
#include <functional>
#include <set>
#include <intrin.h>

#ifdef _DEBUG
#pragma message ("You are compiling the code in Debug - be warned that wrappers for export functions may not have correct code generated")
#endif

void ASILoaderMain(HMODULE hModule, DWORD reason);
