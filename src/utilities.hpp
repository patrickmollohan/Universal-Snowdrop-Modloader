#pragma once

#include "dll_main.hpp"
#include "settings.hpp"

#define PATTERN_WILDCARD 0x00
#define ThreadIoPriority (THREADINFOCLASS)21

typedef enum _IO_PRIORITY_HINT {
    IoPriorityVeryLow = 0,
    IoPriorityLow,
    IoPriorityNormal,
    IoPriorityHigh,
    IoPriorityCritical
} IO_PRIORITY_HINT;

typedef NTSTATUS(NTAPI* NtSetInformationThread_t)(
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength
);

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
        
        uintptr_t GetBaseAddress() const;
        uintptr_t GetCodeSize() const;
        size_t Begin() const;
        size_t End() const;
        std::string GetModulePath() const;
    };

    class Processes {
    public:
        static uintptr_t FindPatternAddress(const unsigned char* pattern, size_t patternLength);
        static uintptr_t FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask, size_t patternLength);
        static bool IsCompatibleExe();
        static bool IsGameAvatar();
        static bool IsGameOutlaws();
        static void SetExeName();
        static void SetGameID();
        static void SetPriorityLevels();
    };

    class SettingsParser {
    private:
        static std::string configFilePath;

    public:
        static bool GetBoolean(const std::string& section, const std::string& key, bool defaultValue);
        static int GetInt(const std::string& section, const std::string& key, int defaultValue);
        static std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue);
        static void SetConfigFilePath(HMODULE hModule);
        static std::string StripCommentsAndTrim(const std::string& value);
    };

    class String {
    public:
        static bool Contains(const std::string& str, const std::string& substr, bool ignoreCase);
        static bool EqualsIgnoreCase(const std::string& str1, const std::string& str2);
        static void ToLower(std::string& str);
    };
};