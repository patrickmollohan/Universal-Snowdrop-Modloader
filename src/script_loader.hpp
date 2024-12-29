#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <string>
#include <windows.h>
#include "settings.hpp"

class ScriptLoader {
public:
    static void LoadScripts();

private:
    static void LoadScriptsFromDirectory(const std::wstring& directoryPath);
};
