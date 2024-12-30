#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "utilities.hpp"

class Settings {
public:
    static void LoadSettings();

    static bool EnableMods;
    static bool EnableScripts;
    static bool CreateFileA;
    static bool CreateFileW;
    static std::string CPUPriority;
    static std::string IOPriority;
    static std::string ThreadPriority;
};
