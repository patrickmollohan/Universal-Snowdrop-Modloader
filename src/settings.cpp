#include "pch.hpp"
#include "settings.hpp"

bool Settings::EnableMods;
bool Settings::EnablePlugins;

void Settings::LoadSettings() {
    using SettingsParser = Utilities::SettingsParser;

    EnableMods = SettingsParser::GetBoolean("Settings", "EnableMods", true);
    EnablePlugins = SettingsParser::GetBoolean("Settings", "EnablePlugins", true);
}

LPCSTR Settings::GetConfigPath() {
    thread_local static std::string path = (
        g_DllInfo.directory /
        (std::string{ g_DllInfo.stem } + ".ini")
        ).string();

    return path.c_str();
}

