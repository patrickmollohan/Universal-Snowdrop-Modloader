#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "main_module.hpp"

class Utilities {
public:
    static bool FileExists(LPCSTR file_path);
    static uintptr_t FindPatternAddress(const unsigned char* pattern);
    static uintptr_t FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask);
};