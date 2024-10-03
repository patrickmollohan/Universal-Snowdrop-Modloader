#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <MinHook.h>

void DiskCacheEnablerMain(HMODULE hModule, DWORD dwReason);