#pragma once

#include "utilities.hpp"

class Settings {
public:
    static void LoadSettings();
    static LPCSTR GetConfigPath();

    static bool EnableMods;
    static bool EnablePlugins;
    static bool EnableGUI;
};
