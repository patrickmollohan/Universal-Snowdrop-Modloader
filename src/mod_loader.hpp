#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "phook.h"
#include "detours\detours.h"

const LPCSTR patterns[] = { "4C 8B DC 53 57 41 54 48 81 EC ? ? ? ? 41 8B D8" };

void ModLoaderMain(HMODULE hModule, DWORD reason);