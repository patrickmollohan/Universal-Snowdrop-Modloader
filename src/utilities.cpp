#include "pch.hpp"
#include "utilities.hpp"
#include "ini.hpp"

#include <bit>
#include <emmintrin.h>

bool Utilities::Files::FileExists(LPCSTR filePath) {
    DWORD dwAttrib = GetFileAttributesA(filePath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

ModuleInfo Utilities::Module::GetModuleInfo(HMODULE hModule) {
    ModuleInfo info{};

    wchar_t pathBuffer[MAX_PATH]{};
    if (!GetModuleFileNameW(hModule, pathBuffer, MAX_PATH)) return info;

    std::filesystem::path fullPath(pathBuffer);

    WIN32_FIND_DATAW findData{};
    if (HANDLE hFind = FindFirstFileW(fullPath.c_str(), &findData); hFind != INVALID_HANDLE_VALUE) {
        fullPath = fullPath.parent_path() / findData.cFileName;
        FindClose(hFind);
    }

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

uintptr_t Utilities::PatternScanner::FindPattern(const char* pat) {
    auto compiled = CompilePattern(pat);

    return FindPatternSIMD(g_ExeInfo.image, compiled);
}

uintptr_t Utilities::PatternScanner::FindPatternSIMD(std::span<const std::byte> img, const std::vector<PatternByte>& pattern) {
    const size_t len = pattern.size();
    if (len == 0 || img.size() < len) return 0;

    size_t first = 0;
    while (first < len && pattern[first].wildcard) ++first;
    if (first == len) return reinterpret_cast<uintptr_t>(img.data());

    size_t last = len - 1;
    while (pattern[last].wildcard) --last;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(img.data());
    const size_t lastStart = img.size() - len;

    const __m128i firstByte = _mm_set1_epi8(static_cast<char>(pattern[first].value));
    const __m128i lastByte = _mm_set1_epi8(static_cast<char>(pattern[last].value));

    auto matchesAt = [&](size_t i) {
        for (size_t j = 0; j < len; ++j) {
            if (!pattern[j].wildcard && data[i + j] != pattern[j].value) return false;
        }
        return true;
    };

    size_t i = 0;
    for (; i + 16 <= lastStart + 1; i += 16) {
        const __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + first));
        const __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + last));
        auto mask = static_cast<unsigned>(_mm_movemask_epi8(_mm_and_si128(_mm_cmpeq_epi8(a, firstByte), _mm_cmpeq_epi8(b, lastByte))));

        while (mask) {
            const size_t candidate = i + std::countr_zero(mask);
            if (matchesAt(candidate)) return reinterpret_cast<uintptr_t>(data + candidate);
            mask &= mask - 1;
        }
    }

    for (; i <= lastStart; ++i) {
        if (matchesAt(i)) return reinterpret_cast<uintptr_t>(data + i);
    }

    return 0;
}

bool Utilities::SettingsParser::GetBoolean(const std::string& path, const std::string& section, const std::string& key, bool defaultValue, const char* comment) {
    std::string value = GetString(path, section, key, defaultValue ? "true" : "false", comment);
    Utilities::String::ToLower(value);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

int Utilities::SettingsParser::GetInt(const std::string& path, const std::string& section, const std::string& key, int defaultValue, const char* comment) {
    std::string value = GetString(path, section, key, std::to_string(defaultValue), comment);

    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

std::string Utilities::SettingsParser::GetString(const std::string& path, const std::string& section, const std::string& key, const std::string& defaultValue, const char* comment) {
    return Ini::ReadOrCreate(path, section, key, defaultValue, comment);
}

void Utilities::SettingsParser::SetBoolean(const std::string& path, const std::string& section, const std::string& key, bool value, const char* comment) {
    Ini::Write(path, section, key, value ? "true" : "false", comment);
}

void Utilities::SettingsParser::SetInt(const std::string& path, const std::string& section, const std::string& key, int value, const char* comment) {
    Ini::Write(path, section, key, std::to_string(value), comment);
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
