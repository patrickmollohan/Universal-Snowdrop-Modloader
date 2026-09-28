#pragma once

#include "utilities.hpp"

class Settings {
public:
    static void LoadSettings();
    static LPCSTR GetConfigPath();

    static bool IsPluginEnabled(const std::string& pluginFileName);
    static void SetPluginEnabled(const std::string& pluginFileName, bool enabled);

    static bool EnableMods;
    static bool EnablePlugins;
    static bool EnableGUI;

    static UINT MenuToggleKey;
    static std::string MenuToggleKeyName;

private:
    static UINT ParseVirtualKeyName(const std::string& name, UINT defaultValue);
};
