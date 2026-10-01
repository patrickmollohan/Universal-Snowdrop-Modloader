#include "pch.hpp"
#include "plugins.hpp"
#include "gui.hpp"
#include "utilities.hpp"

#include <algorithm>
#include <cwctype>
#include <map>

std::vector<LoadedPlugin> loadedPlugins;

bool Plugins::LoadPlugins() {
    if (!Settings::EnablePlugins) return false;

    for (const auto& dirName : {L"plugins", L"scripts"}) {
        std::filesystem::path dir = g_ExeInfo.directory / dirName;
        LoadPluginsFromDirectory(dir.wstring());
    }
    return true;
}

namespace {
    namespace fs = std::filesystem;

    std::wstring ToLowerW(std::wstring s) {
        std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
        return s;
    }

    fs::path SharedIniPath(const fs::path& plugin) {
        return plugin.parent_path() / (plugin.stem().wstring() + L".ini");
    }

    fs::path SpecificIniPath(const fs::path& plugin) {
        return plugin.parent_path() / (plugin.filename().wstring() + L".ini");
    }

    ULONGLONG GetCreationTime(const fs::path& file) {
        WIN32_FILE_ATTRIBUTE_DATA data{};
        if (!GetFileAttributesExW(file.c_str(), GetFileExInfoStandard, &data)) return ~0ULL;
        return (static_cast<ULONGLONG>(data.ftCreationTime.dwHighDateTime) << 32) | data.ftCreationTime.dwLowDateTime;
    }

    void MigrateSharedConfig(const std::vector<fs::path>& group) {
        const fs::path shared = SharedIniPath(group.front());
        std::error_code ec;
        if (!fs::exists(shared, ec)) return;

        const fs::path* owner = &group.front();
        auto ownerKey = std::make_pair(GetCreationTime(*owner), ToLowerW(owner->filename().wstring()));
        for (const auto& candidate : group) {
            auto key = std::make_pair(GetCreationTime(candidate), ToLowerW(candidate.filename().wstring()));
            if (key < ownerKey) {
                owner = &candidate;
                ownerKey = key;
            }
        }

        const fs::path target = SpecificIniPath(*owner);
        if (fs::exists(target, ec)) return;
        fs::rename(shared, target, ec);
    }

    fs::path ResolveConfigPath(const fs::path& plugin, bool sharesStemWithOtherPlugin) {
        if (sharesStemWithOtherPlugin) return SpecificIniPath(plugin);

        std::error_code ec;
        const fs::path shared = SharedIniPath(plugin);
        const fs::path specific = SpecificIniPath(plugin);
        if (!fs::exists(shared, ec) && fs::exists(specific, ec)) return specific;

        return shared;
    }
}

void Plugins::LoadPluginsFromDirectory(const std::wstring& directory) {
    std::error_code ec;

    if (!fs::is_directory(directory, ec)) return;

    std::vector<fs::path> pluginFiles;

    fs::directory_iterator it(directory, fs::directory_options::skip_permission_denied, ec);
    if (ec) return;

    for (const fs::directory_iterator end; it != end; it.increment(ec)) {
        if (ec) break;

        std::error_code entryEc;
        if (!it->is_regular_file(entryEc) || entryEc) continue;

        fs::path filePath = it->path();
        if (filePath.extension() != L".dll" && filePath.extension() != L".asi") continue;

        pluginFiles.push_back(std::move(filePath));
    }

    std::map<std::wstring, std::vector<fs::path>> stemGroups;
    for (const auto& filePath : pluginFiles) {
        stemGroups[ToLowerW(filePath.stem().wstring())].push_back(filePath);
    }
    for (const auto& [stem, group] : stemGroups) {
        if (group.size() > 1) MigrateSharedConfig(group);
    }

    for (const auto& filePath : pluginFiles) {
        const bool sharesStem = stemGroups[ToLowerW(filePath.stem().wstring())].size() > 1;

        LoadedPlugin plugin{};
        plugin.fileName = filePath.filename().string();
        plugin.enabled = Settings::IsPluginEnabled(plugin.fileName);
        plugin.ctx = std::make_unique<ModLoaderPluginCtx>();
        plugin.ctx->pluginId = filePath.stem().string();
        plugin.ctx->configPath = ResolveConfigPath(filePath, sharesStem).string();

        if (!plugin.enabled) {
            loadedPlugins.push_back(std::move(plugin));
            continue;
        }

        HMODULE hPlugin = LoadLibraryW(filePath.c_str());
        if (!hPlugin) continue;

        plugin.module = hPlugin;
        TryInitPluginAPI(plugin);

        loadedPlugins.push_back(std::move(plugin));
    }
}

void Plugins::TryInitPluginAPI(LoadedPlugin& plugin) {
    auto initFn = reinterpret_cast<ModLoaderInitPluginFn>(GetProcAddress(plugin.module, "ModLoader_InitPlugin"));
    if (!initFn) return;

    if (initFn(&GUI::GetProc, plugin.ctx.get()) && !plugin.ctx->name.empty()) {
        plugin.apiInitialised = true;
    }
}

bool Plugins::UnloadPlugins() {
    if (!Settings::EnablePlugins) return false;
    for (auto& plugin : loadedPlugins) {
        if (plugin.ctx) {
            for (auto& patch : plugin.ctx->patches) Utilities::Memory::SetPatchEnabled(*patch, false);
            plugin.ctx->patches.clear();
        }
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
        if (!plugin.module || plugin.ctx->pluginId != targetPlugin) continue;

        if (plugin.apiInitialised && plugin.ctx->onCommand) {
            plugin.ctx->onCommand(plugin.ctx.get(), command.c_str());
            return true;
        }
        return false;
    }
    return false;
}
