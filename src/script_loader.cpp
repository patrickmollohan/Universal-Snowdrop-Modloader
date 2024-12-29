#include "script_loader.hpp"

void ScriptLoader::LoadScripts() {
    if (Settings::EnableScripts) {
        wchar_t gameRoot[MAX_PATH];
        GetModuleFileNameW(NULL, gameRoot, MAX_PATH);

        std::wstring gameRootPath = gameRoot;
        gameRootPath = gameRootPath.substr(0, gameRootPath.find_last_of(L"\\"));

        std::wstring scriptsPath = gameRootPath + L"\\scripts";

        LoadScriptsFromDirectory(scriptsPath);
    }
}

void ScriptLoader::LoadScriptsFromDirectory(const std::wstring& directoryPath) {
    // Append the wildcard to search for files
    std::wstring searchPath = directoryPath + L"\\*";

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        // If the directory is empty or not found, return
        return;
    }

    // Iterate over the files in the directory
    do {
        // Skip '.' and '..' entries
        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
            continue;
        }

        // Construct the full file path
        std::wstring filePath = directoryPath + L"\\" + findFileData.cFileName;

        // Check if the file has a .dll or .asi extension
        std::wstring fileExt = filePath.substr(filePath.find_last_of(L".") + 1);
        if (fileExt == L"dll" || fileExt == L"asi") {
            // Attempt to load the DLL file
            LoadLibraryW(filePath.c_str());
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    // Close the handle
    FindClose(hFind);
}
