#pragma once

#include "settings.hpp"

class ScriptLoader {
public:
    static void LoadScripts();

private:
    static void LoadScriptsFromDirectory(const std::wstring& directoryPath);
};
