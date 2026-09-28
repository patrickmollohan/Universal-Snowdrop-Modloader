#pragma once

#include "globals.hpp"
#include "settings.hpp"

class Utilities {
public:
    class Files {
    public:
        static bool FileExists(LPCSTR filePath);
    };

    class Module {
    public:
        static ModuleInfo GetModuleInfo(HMODULE hModule);
    };

    class PatternScanner {
    public:
        static uintptr_t FindPattern(const char* pattern);
    private:
        static std::vector<PatternByte> CompilePattern(const char* pattern);
        static bool HasWildcards(const std::vector<PatternByte>& pattern);
        static uintptr_t FindPatternBMH(std::span<const std::byte> image, const std::vector<PatternByte>& pattern);
        static uintptr_t FindPatternBMHWildcard(std::span<const std::byte> image, const std::vector<PatternByte>& pattern);
    };

    class SettingsParser {
    public:
        static bool GetBoolean(const std::string& section, const std::string& key, bool defaultValue);
        static int GetInt(const std::string& section, const std::string& key, int defaultValue);
        static std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue);

        static bool GetBoolean(const std::string& path, const std::string& section, const std::string& key, bool defaultValue);
        static int GetInt(const std::string& path, const std::string& section, const std::string& key, int defaultValue);
        static std::string GetString(const std::string& path, const std::string& section, const std::string& key, const std::string& defaultValue);

        static void SetBoolean(const std::string& section, const std::string& key, bool value);
        static void SetBoolean(const std::string& path, const std::string& section, const std::string& key, bool value);
        static void SetInt(const std::string& path, const std::string& section, const std::string& key, int value);
        static std::string StripCommentsAndTrim(const std::string& value);
    };

    class String {
    public:
        static bool Contains(const std::string& str, const std::string& substr);
        static bool ContainsIgnoreCase(const std::string& str, const std::string& substr);
        static bool Equals(const std::string& str1, const std::string& str2);
        static bool EqualsIgnoreCase(const std::string& str1, const std::string& str2);
        static void ToLower(std::string& str);
    };
};
