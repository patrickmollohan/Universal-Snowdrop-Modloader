#include "pch.hpp"
#include "utilities.hpp"

bool Utilities::Files::FileExists(LPCSTR filePath) {
    DWORD dwAttrib = GetFileAttributesA(filePath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

ModuleInfo Utilities::Module::GetModuleInfo(HMODULE hModule) {
    ModuleInfo info{};

    wchar_t pathBuffer[MAX_PATH]{};
    if (!GetModuleFileNameW(hModule, pathBuffer, MAX_PATH)) return info;

    std::filesystem::path fullPath(pathBuffer);
    info.fullPath = fullPath;
    info.directory = fullPath.parent_path();
    info.filename = fullPath.filename().string();
    info.stem = fullPath.stem().string();
    info.extension = fullPath.extension().string();

    auto* dos = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    auto* nt = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<std::byte*>(hModule) + dos->e_lfanew);

    size_t size = nt->OptionalHeader.SizeOfImage;
    info.image = std::span<const std::byte>(
        reinterpret_cast<const std::byte*>(hModule),
        size
    );

    return info;
}

std::vector<PatternByte> Utilities::PatternScanner::CompilePattern(const char* pattern) {
    std::vector<PatternByte> out;

    while (*pattern) {
        if (*pattern == ' ') { pattern++; continue; }

        if (pattern[0] == '?' && pattern[1] == '?') {
            out.push_back({ 0, true });
        } else {
            out.push_back({ (uint8_t)strtoul(pattern, nullptr, 16), false });
        }

        pattern += 2;
    }
    return out;
}

bool Utilities::PatternScanner::HasWildcards(const std::vector<PatternByte>& pattern) {
    for (auto& b : pattern)
        if (b.wildcard) return true;
    return false;
}

uintptr_t Utilities::PatternScanner::FindPattern(const char* pat) {
    auto compiled = CompilePattern(pat);

    return HasWildcards(compiled)
        ? FindPatternBMHWildcard(g_ExeInfo.image, compiled)
        : FindPatternBMH(g_ExeInfo.image, compiled);
}

uintptr_t Utilities::PatternScanner::FindPatternBMH(std::span<const std::byte> img, const std::vector<PatternByte>& pattern) {
    const size_t len = pattern.size();
    if (!len || img.size() < len) return 0;

    uint8_t skip[256];
    memset(skip, (int)len, 256);

    for (size_t i = 0; i < len - 1; i++)
        skip[pattern[i].value] = uint8_t(len - 1 - i);

    size_t i = 0;
    while (i <= img.size() - len) {
        auto last = std::to_integer<uint8_t>(img[i + len - 1]);

        if (last == pattern[len - 1].value) {
            if (memcmp(img.data() + i, &pattern[0].value, len - 1) == 0)
                return reinterpret_cast<uintptr_t>(img.data() + i);
        }
        i += skip[last];
    }
    return 0;
}

uintptr_t Utilities::PatternScanner::FindPatternBMHWildcard(std::span<const std::byte> img, const std::vector<PatternByte>& pattern) {
    const size_t len = pattern.size();
    if (len == 0 || img.size() < len) return 0;

    size_t anchor = len - 1;
    while (anchor > 0 && pattern[anchor].wildcard) anchor--;

    uint8_t skip[256];
    memset(skip, (int)len, 256);
    for (size_t i = 0; i < anchor; ++i) {
        if (!pattern[i].wildcard) skip[pattern[i].value] = static_cast<uint8_t>(anchor - i);
    }

    size_t i = 0;
    while (i <= img.size() - len) {
        uint8_t current = std::to_integer<uint8_t>(img[i + anchor]);

        if (!pattern[anchor].wildcard && current != pattern[anchor].value) {
            i += skip[current];
            continue;
        }

        bool found = true;
        for (size_t j = 0; j < len; ++j) {
            if (!pattern[j].wildcard &&
                std::to_integer<uint8_t>(img[i + j]) != pattern[j].value) {
                found = false;
                break;
            }
        }

        if (found) return reinterpret_cast<uintptr_t>(img.data() + i);
        i++;
    }

    return 0;
}

bool Utilities::SettingsParser::GetBoolean(const std::string& section, const std::string& key, bool defaultValue) {
    char result[256];
    GetPrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        defaultValue ? "true" : "false",
        result,
        sizeof(result),
        Settings::GetConfigPath()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);
    Utilities::String::ToLower(value);
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
        Settings::GetConfigPath()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);

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
        Settings::GetConfigPath()
    );

    std::string value(result);
    value = Utilities::SettingsParser::StripCommentsAndTrim(result);
    return value;
}

std::string Utilities::SettingsParser::StripCommentsAndTrim(const std::string& value) {
    auto result = value.substr(0, value.find_first_of(";#"));
    result.erase(0, result.find_first_not_of(" \t"));
    result.erase(result.find_last_not_of(" \t") + 1);
    return result;
}

bool Utilities::String::Contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

bool Utilities::String::ContainsIgnoreCase(const std::string& str, const std::string& substr) {
    std::string tempStr = str;
    std::string tempSubstr = substr;
    Utilities::String::ToLower(tempStr);
    Utilities::String::ToLower(tempSubstr);
    return tempStr.find(tempSubstr) != std::string::npos;
}

bool Utilities::String::Equals(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) return false;

    for (size_t i = 0; i < str1.size(); ++i) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

bool Utilities::String::EqualsIgnoreCase(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) return false;

    for (size_t i = 0; i < str1.size(); ++i) {
        if (std::tolower(str1[i]) != std::tolower(str2[i])) {
            return false;
        }
    }
    return true;
}

void Utilities::String::ToLower(std::string& str) {
    for (char& c : str) {
        c = std::tolower(c);
    }
}
