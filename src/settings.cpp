#include "settings.hpp"

bool Settings::EnableMods = false;
bool Settings::EnableScripts = false;
bool Settings::CreateFileA = false;
bool Settings::CreateFileW = false;
std::string Settings::CPUPriority = "normal";
std::string Settings::IOPriority = "normal";
std::string Settings::ThreadPriority = "normal";

void Settings::LoadSettings() {
    using SettingsParser = Utilities::SettingsParser;

    EnableMods = SettingsParser::GetBoolean("Settings", "EnableMods", false);
    EnableScripts = SettingsParser::GetBoolean("Settings", "EnableScripts", false);
    CreateFileA = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileA", false);
    CreateFileW = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileW", false);
    CPUPriority = SettingsParser::GetString("Priorities", "CPUPriority", "normal");
    IOPriority = SettingsParser::GetString("Priorities", "IOPriority", "normal");
    ThreadPriority = SettingsParser::GetString("Priorities", "ThreadPriority", "normal");
}
