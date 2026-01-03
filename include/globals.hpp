#pragma once

struct PatternByte {
    uint8_t value;
    bool wildcard;
};

struct ModuleInfo {
    std::filesystem::path fullPath;
    std::filesystem::path directory;
    std::string filename;
    std::string stem;
    std::string extension;
    std::span<const std::byte> image;
};

struct DllExports {
    const char** funcs;
    UINT_PTR* procs;
    int count;
};

extern ModuleInfo g_DllInfo;
extern ModuleInfo g_ExeInfo;
extern DllExports g_DllExports;
