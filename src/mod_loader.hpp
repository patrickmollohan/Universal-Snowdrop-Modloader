#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "phook.h"
#include "detours\detours.h"

void ModLoaderMain(HMODULE hModule, DWORD reason);