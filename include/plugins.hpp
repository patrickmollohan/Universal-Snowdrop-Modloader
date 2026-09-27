#pragma once

#include "globals.hpp"
#include "settings.hpp"

class Plugins {
public:
    static bool LoadPlugins();
    static bool UnloadPlugins();
    static const std::vector<std::string>& GetLoadedPluginNames();

private:
    static void LoadPluginsFromDirectory(const std::wstring& directory);
};
