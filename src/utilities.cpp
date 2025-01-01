#include "utilities.hpp"

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

void Utilities::Processes::SetExeName() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, sizeof(exePath));

    std::string exeName = std::string(exePath);
    size_t pos = exeName.find_last_of("\\/");
    if (pos != std::string::npos) {
        exeName = exeName.substr(pos + 1);
    }

    DLLMain::exeName = exeName;
}

bool Utilities::Processes::IsCompatibleExe() {
    return (DLLMain::gameID != GAME_UNSUPPORTED);
}

bool Utilities::Processes::IsGameAvatar() {
    return (DLLMain::gameID == GAME_AVATAR);
}

bool Utilities::Processes::IsGameOutlaws() {
    return (DLLMain::gameID == GAME_OUTLAWS);
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

void Utilities::Processes::SetGameID() {
    if (Utilities::String::Contains(DLLMain::exeName, "Avatar", true)) {
        DLLMain::gameID = GAME_AVATAR;
    } else if (Utilities::String::Contains(DLLMain::exeName, "Outlaws", true)) {
        DLLMain::gameID = GAME_OUTLAWS;
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
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        defaultValue ? "true" : "false",
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);

    // Convert to lowercase for case-insensitive comparison
    Utilities::String::ToLower(value);

    // Interpret the result as a boolean
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

int Utilities::SettingsParser::GetInt(const std::string& section, const std::string& key, int defaultValue) {
    char result[256];
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        std::to_string(defaultValue).c_str(),
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);

    // Convert to integer
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

std::string Utilities::SettingsParser::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) {
    char result[256];
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        defaultValue.c_str(),
        result,
        sizeof(result),
        configFilePath.c_str()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);

    return value;
}

std::string Utilities::SettingsParser::StripCommentsAndTrim(const std::string& value) {
    auto result = value.substr(0, value.find_first_of(";#")); // Strip comments
    result.erase(0, result.find_first_not_of(" \t"));         // Left trim
    result.erase(result.find_last_not_of(" \t") + 1);         // Right trim
    return result;
}

bool Utilities::String::Contains(const std::string& str, const std::string& substr, bool ignoreCase) {
    std::string tempStr = str;
    std::string tempSubstr = substr;
    if (ignoreCase) {
        Utilities::String::ToLower(tempStr);
        Utilities::String::ToLower(tempSubstr);
    }
    return tempStr.find(tempSubstr) != std::string::npos;
}

bool Utilities::String::EqualsIgnoreCase(const std::string& str1, const std::string& str2) {
    return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

void Utilities::String::ToLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

