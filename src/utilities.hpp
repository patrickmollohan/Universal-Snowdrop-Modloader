#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <algorithm>
#include <string>

#define PATTERN_WILDCARD 0x00

class Utilities {
public:
    class Files {
    public:
        static bool LocalFileExists(LPCSTR file_path);
    };

    class MainModule {
    private:
        uintptr_t BaseAddress;
        uintptr_t EndAddress;
        std::string ModulePath;

    public:
        MainModule();
        ~MainModule();
        
        uintptr_t GetBaseAddress() const; // Gets the module base address
        uintptr_t GetCodeSize() const; // Gets the size of code reported by the PE header
        size_t Begin() const;
        size_t End() const;
        std::string GetModulePath() const;
    };

    class Processes {
    public:
        static uintptr_t FindPatternAddress(const unsigned char* pattern, size_t patternLength);
        static uintptr_t FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask, size_t patternLength);
        static std::string GetExeName();
        static void SetHighPriority();
    };

    class SettingsParser {
    private:
        static std::string configFilePath;

    public:
        static bool GetBoolean(const std::string& section, const std::string& key, bool defaultValue);
        static int GetInt(const std::string& section, const std::string& key, int defaultValue);
        static std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue);
        static void SetConfigFilePath(HMODULE hModule);
    };

    class String {
    public:
        static bool EqualsIgnoreCase(const std::string& str1, const std::string& str2);
    };
};