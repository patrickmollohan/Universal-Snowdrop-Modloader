#pragma once

#include "globals.hpp"
#include "plugin_api.hpp"
#include "settings.hpp"

inline constexpr const char* kPluginConfigSection = "Settings";

struct ModLoaderHook {
    LPVOID target = nullptr;
    bool enabled = false;
};

struct ModLoaderPluginCtx {
    std::string pluginId;
    std::string configPath;
    std::vector<std::unique_ptr<MemoryPatch>> patches;
    std::vector<std::unique_ptr<ModLoaderHook>> hooks;

    std::string name;
    std::string version;
    std::string author;
    ModLoaderDrawMenuFn drawMenu = nullptr;
    ModLoaderCommandFn onCommand = nullptr;
};

struct LoadedPlugin {
    HMODULE module = nullptr;
    bool enabled = true;
    std::string fileName;
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
