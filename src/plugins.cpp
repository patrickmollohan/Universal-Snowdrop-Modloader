#include "pch.hpp"
#include "plugins.hpp"

std::vector<HMODULE> loadedPlugins;

bool Plugins::LoadPlugins() {
    if (!Settings::EnablePlugins) return false;
    std::filesystem::path pluginsPath = g_ExeInfo.directory / L"plugins";
    std::filesystem::create_directory(pluginsPath);
    LoadPluginsFromDirectory(pluginsPath.wstring());
    return true;
}

void Plugins::LoadPluginsFromDirectory(const std::wstring& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            if (filePath.extension() == L".dll" || filePath.extension() == L".asi") {
                HMODULE hPlugin = LoadLibraryW(filePath.c_str());
                if (hPlugin != NULL) loadedPlugins.push_back(hPlugin);
            }
        }
    }
}

bool Plugins::UnloadPlugins() {
    if (!Settings::EnablePlugins) return false;
    for (HMODULE hPlugin : loadedPlugins) {
        if (hPlugin != NULL) FreeLibrary(hPlugin);
    }
    loadedPlugins.clear();
    return true;
}
