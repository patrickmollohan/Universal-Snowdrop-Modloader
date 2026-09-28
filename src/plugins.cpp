#include "pch.hpp"
#include "plugins.hpp"
#include "gui.hpp"

std::vector<LoadedPlugin> loadedPlugins;

bool Plugins::LoadPlugins() {
    if (!Settings::EnablePlugins) return false;
    std::filesystem::path pluginsPath = g_ExeInfo.directory / L"plugins";
    std::filesystem::create_directory(pluginsPath);
    LoadPluginsFromDirectory(pluginsPath.wstring());
    return true;
}

void Plugins::LoadPluginsFromDirectory(const std::wstring& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;

        std::filesystem::path filePath = entry.path();
        if (filePath.extension() != L".dll" && filePath.extension() != L".asi") continue;

        HMODULE hPlugin = LoadLibraryW(filePath.c_str());
        if (!hPlugin) continue;

        LoadedPlugin plugin{};
        plugin.module = hPlugin;
        plugin.fileName = filePath.filename().string();
        plugin.ctx = std::make_unique<ModLoaderPluginCtx>();
        plugin.ctx->configSection = filePath.stem().string();

        TryInitPluginAPI(plugin);

        loadedPlugins.push_back(std::move(plugin));
    }
}

void Plugins::TryInitPluginAPI(LoadedPlugin& plugin) {
    auto initFn = reinterpret_cast<ModLoaderInitPluginFn>(GetProcAddress(plugin.module, "ModLoader_InitPlugin"));
    if (!initFn) return;

    ModLoaderPluginInfo info{};
    const ModLoaderHostAPI* host = GUI::GetPluginHostAPI();

    if (initFn(host, plugin.ctx.get(), &info) && info.name) {
        plugin.info = info;
        plugin.apiInitialised = true;
    }
}

bool Plugins::UnloadPlugins() {
    if (!Settings::EnablePlugins) return false;
    for (auto& plugin : loadedPlugins) {
        if (plugin.module) FreeLibrary(plugin.module);
    }
    loadedPlugins.clear();
    return true;
}

std::vector<LoadedPlugin>& Plugins::GetLoadedPlugins() {
    return loadedPlugins;
}

bool Plugins::SendCommand(const std::string& targetPlugin, const std::string& command) {
    for (auto& plugin : loadedPlugins) {
        if (plugin.ctx->configSection != targetPlugin) continue;

        if (plugin.apiInitialised && plugin.info.OnCommand) {
            plugin.info.OnCommand(GUI::GetPluginHostAPI(), plugin.ctx.get(), command.c_str());
            return true;
        }
        return false;
    }
    return false;
}
