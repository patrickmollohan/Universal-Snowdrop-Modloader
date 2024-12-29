#include "settings.hpp"
#include "utilities.hpp"

bool Settings::EnableMods = false;
bool Settings::EnableScripts = false;
bool Settings::CreateFileA = false;
bool Settings::CreateFileW = false;

void Settings::LoadSettings() {
    using SettingsParser = Utilities::SettingsParser;

    EnableMods = SettingsParser::GetBoolean("Settings", "EnableMods", false);
    EnableScripts = SettingsParser::GetBoolean("Settings", "EnableScripts", false);
    CreateFileA = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileA", false);
    CreateFileW = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileW", false);
}
