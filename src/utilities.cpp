#include "utilities.hpp"

const char* Utilities::Processes::allowedExes[] = {
    "Outlaws.exe",
    "Outlaws_Plus.exe",
    "AFOP.exe",
    "AFOP_Plus.exe"
};

bool Utilities::Files::LocalFileExists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

Utilities::MainModule::MainModule() 
    : BaseAddress(0), EndAddress(0) {

    // Load module information
    HMODULE Mod = GetModuleHandle(NULL);

    // Load PE information
    const IMAGE_DOS_HEADER* DOSHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(Mod);
    const IMAGE_NT_HEADERS* NTHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>((const uint8_t*)DOSHeader + DOSHeader->e_lfanew);

    // Calculate addresses
    BaseAddress = reinterpret_cast<uintptr_t>(Mod);
    EndAddress = BaseAddress + NTHeader->OptionalHeader.SizeOfCode;

    // Get module path
    char ModPath[MAX_PATH];
    if (GetModuleFileNameA(Mod, ModPath, sizeof(ModPath))) {
        ModulePath = ModPath;
    }
}

Utilities::MainModule::~MainModule() { }

uintptr_t Utilities::MainModule::GetBaseAddress() const {
    return BaseAddress;
}

uintptr_t Utilities::MainModule::GetCodeSize() const {
    return EndAddress - BaseAddress;
}

size_t Utilities::MainModule::Begin() const {
    return static_cast<size_t>(BaseAddress);
}

size_t Utilities::MainModule::End() const {
    return static_cast<size_t>(EndAddress);
}

std::string Utilities::MainModule::GetModulePath() const {
    return ModulePath;
}

uintptr_t Utilities::Processes::FindPatternAddress(const unsigned char* pattern, size_t patternLength) {
    Utilities::MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        if (memcmp(reinterpret_cast<void*>(baseAddress + i), pattern, patternLength) == 0) {
            return baseAddress + i;
        }
    }
    return 0;
}

uintptr_t Utilities::Processes::FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask, size_t patternLength) {
    Utilities::MainModule module;
    uintptr_t baseAddress = module.GetBaseAddress();
    size_t moduleSize = module.GetCodeSize();

    for (size_t i = 0; i <= moduleSize - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            if (mask[j] != PATTERN_WILDCARD && *(reinterpret_cast<unsigned char*>(baseAddress + i + j)) != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i;
        }
    }
    return 0;
}

std::string Utilities::Processes::GetExeName() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, sizeof(exePath));

    std::string exeName = std::string(exePath);
    size_t pos = exeName.find_last_of("\\/");
    if (pos != std::string::npos) {
        exeName = exeName.substr(pos + 1);
    }
    return exeName;
}

bool Utilities::Processes::IsCompatibleExe() {
    std::string exeName = Utilities::Processes::GetExeName();
    for (const char* allowedExe : allowedExes) {
        if (Utilities::String::EqualsIgnoreCase(exeName, allowedExe)) {
            return true;
        }
    }
    return false;
}

bool Utilities::Processes::IsGameAvatar() {
    std::string exeName = Utilities::Processes::GetExeName();
    return (Utilities::String::EqualsIgnoreCase(exeName, "AFOP.exe") ||
            Utilities::String::EqualsIgnoreCase(exeName, "AFOP_Plus.exe"));
}

bool Utilities::Processes::IsGameOutlaws() {
    std::string exeName = Utilities::Processes::GetExeName();
    return (Utilities::String::EqualsIgnoreCase(exeName, "Outlaws.exe") ||
            Utilities::String::EqualsIgnoreCase(exeName, "Outlaws_Plus.exe"));
}

void Utilities::Processes::SetPriorityLevels() {
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    // Set CPU Priority
    SetProcessPriorityBoost(hProcess, FALSE);
    if (Settings::CPUPriority == "normal") {
        SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
    } else if (Settings::CPUPriority == "medium") {
        SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
    } else if (Settings::CPUPriority == "high") {
        SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
    }

    // Set Thread Priority
    if (Settings::ThreadPriority == "normal") {
        SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
    } else if (Settings::ThreadPriority == "medium") {
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    } else if (Settings::ThreadPriority == "high") {
        SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
    }

    // Set IO Priority
    HMODULE hNtDll = LoadLibrary(L"ntdll.dll");
    if (hNtDll) {
        auto NtSetInformationThread = (NtSetInformationThread_t)GetProcAddress(hNtDll, "NtSetInformationThread");
        if (NtSetInformationThread) {
            IO_PRIORITY_HINT ioPriorityHint = IoPriorityNormal;

            if (Settings::IOPriority == "high") {
                ioPriorityHint = IoPriorityHigh;
            }

            NTSTATUS status = NtSetInformationThread(
                hThread,
                ThreadIoPriority,
                &ioPriorityHint,
                sizeof(IO_PRIORITY_HINT)
            );
        }
        FreeLibrary(hNtDll);
    }
}

std::string Utilities::SettingsParser::configFilePath = "";

void Utilities::SettingsParser::SetConfigFilePath(HMODULE hModule) {
    char dllPath[MAX_PATH] = { 0 };

    if (GetModuleFileNameA(hModule, dllPath, MAX_PATH) == 0) {
        return;
    }

    std::string path(dllPath);

    size_t lastSlashPos = path.find_last_of("\\/");
    if (lastSlashPos != std::string::npos) {
        path = path.substr(0, lastSlashPos + 1);
    }

    path += "version.ini";
    configFilePath = path;
}

bool Utilities::SettingsParser::GetBoolean(const std::string& section, const std::string& key, bool defaultValue) {
    char result[256];
    DWORD length = GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        defaultValue ? "true" : "false",
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);

    // Strip inline comments (anything after ';' or '#')
    size_t commentPos = value.find_first_of(";#");
    if (commentPos != std::string::npos) {
        value = value.substr(0, commentPos);
    }

    // Trim whitespace from the value
    value.erase(0, value.find_first_not_of(" \t")); // Left trim
    value.erase(value.find_last_not_of(" \t") + 1); // Right trim

    // Convert to lowercase for case-insensitive comparison
    Utilities::String::ToLower(value);

    // Interpret the result as a boolean
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

int Utilities::SettingsParser::GetInt(const std::string& section, const std::string& key, int defaultValue) {
    char result[256];
    DWORD length = GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        std::to_string(defaultValue).c_str(), // Default value as string
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);

    // Strip inline comments (anything after ';' or '#')
    size_t commentPos = value.find_first_of(";#");
    if (commentPos != std::string::npos) {
        value = value.substr(0, commentPos);
    }

    // Trim whitespace from the value
    value.erase(0, value.find_first_not_of(" \t")); // Left trim
    value.erase(value.find_last_not_of(" \t") + 1); // Right trim

    // Convert to integer
    try {
        return std::stoi(value);
    }
    catch (...) {
        return defaultValue; // Return default value if conversion fails
    }
}

std::string Utilities::SettingsParser::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) {
    char result[256];
    DWORD length = GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        defaultValue.c_str(),
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);

    // Strip inline comments (anything after ';' or '#')
    size_t commentPos = value.find_first_of(";#");
    if (commentPos != std::string::npos) {
        value = value.substr(0, commentPos);
    }

    // Trim whitespace from the value
    value.erase(0, value.find_first_not_of(" \t")); // Left trim
    value.erase(value.find_last_not_of(" \t") + 1); // Right trim

    return value;
}

bool Utilities::String::EqualsIgnoreCase(const std::string& str1, const std::string& str2) {
    return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

void Utilities::String::ToLower(std::string& str) {
    for (size_t i = 0; i < str.size(); ++i) {
        str[i] = std::tolower(static_cast<unsigned char>(str[i]));
    }
}