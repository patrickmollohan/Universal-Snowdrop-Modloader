#pragma once

#include "RadioTypes.hpp"

#include <filesystem>
#include <string>
#include <vector>

class StationLoader {
public:
    static std::vector<RadioStation> LoadStations(const std::filesystem::path& root, std::string* error);

private:
    static RadioStation LoadStation(const std::filesystem::path& path, std::string* error);
};
