#pragma once

#include <string>

class Ini {
public:
    static std::string ReadOrCreate(const std::string& path, const std::string& section, const std::string& key, const std::string& defaultValue, const char* comment);

    static bool Write(const std::string& path, const std::string& section, const std::string& key, const std::string& value, const char* comment);
};
