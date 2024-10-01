#pragma once
#include <windows.h>
#include <cctype>
#include <string>
#include <shlobj.h>
#include <stdio.h>
#include <filesystem>
#include <functional>
#include <set>
#include <intrin.h>

#ifdef _DEBUG
#pragma message ("You are compiling the code in Debug - be warned that wrappers for export functions may not have correct code generated")
#endif

void ASILoaderMain(HMODULE hModule, DWORD reason);
