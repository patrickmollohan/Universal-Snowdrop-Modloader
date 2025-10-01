#include "pch.hpp"
#include "settings.hpp"

bool Settings::EnableMods = true;
bool Settings::EnableScripts = true;
bool Settings::CreateFileA = false;
bool Settings::CreateFileW = false;
std::string Settings::CPUPriority = "normal";
std::string Settings::IOPriority = "normal";
std::string Settings::ThreadPriority = "normal";

void Settings::LoadSettings() {
    using SettingsParser = Utilities::SettingsParser;

    EnableMods = SettingsParser::GetBoolean("Settings", "EnableMods", true);
    EnableScripts = SettingsParser::GetBoolean("Settings", "EnableScripts", true);
    CreateFileA = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileA", false);
    CreateFileW = SettingsParser::GetBoolean("DiskCacheEnabler", "CreateFileW", false);
    CPUPriority = SettingsParser::GetString("Priorities", "CPUPriority", "normal");
    IOPriority = SettingsParser::GetString("Priorities", "IOPriority", "normal");
    ThreadPriority = SettingsParser::GetString("Priorities", "ThreadPriority", "normal");
}
