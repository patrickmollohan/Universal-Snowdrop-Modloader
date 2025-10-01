#include "pch.hpp"
#include "script_loader.hpp"

void ScriptLoader::LoadScripts() {
    if (Settings::EnableScripts) {
        wchar_t gameRoot[MAX_PATH];
        GetModuleFileNameW(NULL, gameRoot, MAX_PATH);
        std::filesystem::path gameRootPath(gameRoot);
        gameRootPath = gameRootPath.parent_path();
        std::filesystem::path scriptsPath = gameRootPath / L"scripts";

        // Create the scripts directory if it doesn't exist
        std::filesystem::create_directory(scriptsPath);

        // Load scripts from the scripts directory
        LoadScriptsFromDirectory(scriptsPath.wstring());
    }
}

void ScriptLoader::LoadScriptsFromDirectory(const std::wstring& directoryPath) {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            if (filePath.extension() == L".dll" || filePath.extension() == L".asi") {
                LoadLibraryW(filePath.c_str());
            }
        }
    }
}
