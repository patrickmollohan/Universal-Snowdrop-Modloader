#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

class Settings {
public:
    static void LoadSettings();

    static bool EnableMods;
    static bool EnableScripts;
    static bool CreateFileA;
    static bool CreateFileW;
};
