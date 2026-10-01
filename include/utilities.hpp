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

    class Memory {
    public:
        static uintptr_t FindPattern(const char* pattern);
        static bool WriteBytes(uintptr_t address, const void* data, size_t size);
        static std::unique_ptr<MemoryPatch> CreatePatch(uintptr_t address, const uint8_t* bytes, size_t size);
        static std::unique_ptr<MemoryPatch> CreatePatch(const char* pattern, size_t offset, const uint8_t* bytes, size_t size);
        static bool SetPatchEnabled(MemoryPatch& patch, bool enabled);
    private:
        static std::vector<PatternByte> CompilePattern(const char* pattern);
        static uintptr_t FindPatternSIMD(std::span<const std::byte> image, const std::vector<PatternByte>& pattern);
    };

    class SettingsParser {
    public:
        static bool GetBoolean(const std::string& path, const std::string& section, const std::string& key, bool defaultValue, const char* comment = nullptr);
        static int GetInt(const std::string& path, const std::string& section, const std::string& key, int defaultValue, const char* comment = nullptr);
        static std::string GetString(const std::string& path, const std::string& section, const std::string& key, const std::string& defaultValue, const char* comment = nullptr);

        static void SetBoolean(const std::string& path, const std::string& section, const std::string& key, bool value, const char* comment = nullptr);
        static void SetInt(const std::string& path, const std::string& section, const std::string& key, int value, const char* comment = nullptr);
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
