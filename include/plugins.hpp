#pragma once

#include "globals.hpp"
#include "plugin_api.hpp"
#include "settings.hpp"

struct ModLoaderPluginCtx {
    std::string configSection;
};

struct LoadedPlugin {
    HMODULE module = nullptr;
    std::string fileName;
    ModLoaderPluginInfo info{};
    std::unique_ptr<ModLoaderPluginCtx> ctx;
    bool apiInitialised = false;
};

class Plugins {
public:
    static bool LoadPlugins();
    static bool UnloadPlugins();
    static std::vector<LoadedPlugin>& GetLoadedPlugins();

    static bool SendCommand(const std::string& targetPlugin, const std::string& command);

private:
    static void LoadPluginsFromDirectory(const std::wstring& directory);
    static void TryInitPluginAPI(LoadedPlugin& plugin);
};
