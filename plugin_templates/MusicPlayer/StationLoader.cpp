#include "StationLoader.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {
    namespace fs = std::filesystem;

    struct IniDocument {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
    };

    std::string Trim(std::string value) {
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return {};
        const auto last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    std::string Lower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    bool ParseBool(const std::string& value, bool fallback) {
        const std::string v = Lower(Trim(value));
        if (v == "true" || v == "yes" || v == "1" || v == "on") return true;
        if (v == "false" || v == "no" || v == "0" || v == "off") return false;
        return fallback;
    }

    int ParseInt(const std::string& value, int fallback, int minValue, int maxValue) {
        try {
            const int result = std::stoi(Trim(value));
            return std::clamp(result, minValue, maxValue);
        } catch (...) {
            return fallback;
        }
    }

    IniDocument ReadIni(const fs::path& path) {
        IniDocument doc;
        std::ifstream input(path, std::ios::binary);
        if (!input) return doc;

        std::string line;
        std::string section;
        while (std::getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = Trim(line);
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;

            if (line.front() == '[' && line.back() == ']') {
                section = Trim(line.substr(1, line.size() - 2));
                doc.sections.try_emplace(section);
                continue;
            }

            const auto equals = line.find('=');
            if (equals == std::string::npos || section.empty()) continue;

            std::string key = Trim(line.substr(0, equals));
            std::string value = Trim(line.substr(equals + 1));
            doc.sections[section][key] = value;
        }
        return doc;
    }

    const std::unordered_map<std::string, std::string>* FindSection(const IniDocument& doc, const std::string& name) {
        auto exact = doc.sections.find(name);
        if (exact != doc.sections.end()) return &exact->second;
        for (const auto& [section, values] : doc.sections) {
            if (Lower(section) == Lower(name)) return &values;
        }
        return nullptr;
    }

    std::string Value(const IniDocument& doc, const std::string& section, const std::string& key, const std::string& fallback) {
        const auto* values = FindSection(doc, section);
        if (!values) return fallback;
        auto it = values->find(key);
        if (it != values->end()) return it->second;
        for (const auto& [storedKey, storedValue] : *values) {
            if (Lower(storedKey) == Lower(key)) return storedValue;
        }
        return fallback;
    }

    bool IsAudioFile(const fs::path& path) {
        if (!path.has_extension()) return false;
        const std::string ext = Lower(path.extension().string());
        return ext == ".mp3" || ext == ".wav" || ext == ".flac";
    }

    std::vector<fs::path> AudioFiles(const fs::path& directory) {
        std::vector<fs::path> result;
        std::error_code ec;
        if (!fs::is_directory(directory, ec)) return result;
        for (const auto& entry : fs::directory_iterator(directory, fs::directory_options::skip_permission_denied, ec)) {
            if (ec) break;
            if (entry.is_regular_file(ec) && !ec && IsAudioFile(entry.path())) result.push_back(entry.path());
        }
        std::sort(result.begin(), result.end(), [](const fs::path& a, const fs::path& b) {
            return Lower(a.filename().string()) < Lower(b.filename().string());
        });
        return result;
    }

    RadioTrack MakeTrack(const fs::path& path, RadioTrackType type, const IniDocument& ini) {
        RadioTrack track;
        track.path = path;
        track.fileName = path.filename().string();
        track.type = type;
        track.title = Value(ini, track.fileName, "Title", path.stem().string());
        track.artist = Value(ini, track.fileName, "Artist", "Unknown Artist");

        if (type == RadioTrackType::Sfx) {
            const std::string role = Lower(Value(ini, track.fileName, "Role", "OneShot"));
            if (role == "background" || role == "ambience" || role == "ambient") {
                track.sfxRole = RadioSfxRole::Background;
                track.loop = ParseBool(Value(ini, track.fileName, "Loop", "true"), true);
            } else if (role == "transition" || role == "static") {
                track.sfxRole = RadioSfxRole::Transition;
                track.loop = false;
            } else {
                track.sfxRole = RadioSfxRole::OneShot;
                track.loop = ParseBool(Value(ini, track.fileName, "Loop", "false"), false);
            }
            track.volumePercent = ParseInt(Value(ini, track.fileName, "Volume", "100"), 100, 0, 100);
        }
        return track;
    }
}

std::vector<RadioStation> StationLoader::LoadStations(const fs::path& root, std::string* error) {
    std::vector<RadioStation> stations;
    std::error_code ec;
    if (!fs::is_directory(root, ec)) {
        if (error) *error = "Station directory does not exist: " + root.string();
        return stations;
    }

    for (const auto& entry : fs::directory_iterator(root, fs::directory_options::skip_permission_denied, ec)) {
        if (ec) break;
        if (!entry.is_directory(ec) || ec) continue;
        try {
            RadioStation station = LoadStation(entry.path(), error);
            if (!station.music.empty() || !station.intro.empty()) stations.push_back(std::move(station));
        } catch (const std::exception& e) {
            if (error) *error = std::string("Failed to load station ") + entry.path().filename().string() + ": " + e.what();
        }
    }

    std::sort(stations.begin(), stations.end(), [](const RadioStation& a, const RadioStation& b) {
        return Lower(a.name) < Lower(b.name);
    });
    return stations;
}

RadioStation StationLoader::LoadStation(const std::filesystem::path& path, std::string* error) {
    RadioStation station;
    station.path = path;
    station.name = path.filename().string();

    const auto iniPath = path / "station.ini";
    const IniDocument ini = ReadIni(iniPath);

    station.name = Value(ini, "Station", "Name", station.name);
    station.description = Value(ini, "Station", "Description", "");
    station.shuffleMusic = ParseBool(Value(ini, "Playback", "ShuffleMusic", "true"), true);
    station.shuffleInterstitials = ParseBool(Value(ini, "Playback", "ShuffleInterstitials", "true"), true);
    station.minMusicBetweenInterstitials = ParseInt(Value(ini, "Playback", "MinMusicBetweenInterstitials", "3"), 3, 0, 100);
    station.maxMusicBetweenInterstitials = ParseInt(Value(ini, "Playback", "MaxMusicBetweenInterstitials", "5"), 5, station.minMusicBetweenInterstitials, 100);
    station.musicVolumePercent = ParseInt(Value(ini, "Audio", "MusicVolume", "100"), 100, 0, 100);
    station.interstitialVolumePercent = ParseInt(Value(ini, "Audio", "InterstitialVolume", "100"), 100, 0, 100);
    station.sfxVolumePercent = ParseInt(Value(ini, "Audio", "SfxVolume", "35"), 35, 0, 100);

    for (const auto& file : AudioFiles(path / "intro")) station.intro.push_back(MakeTrack(file, RadioTrackType::Intro, ini));
    for (const auto& file : AudioFiles(path / "music")) station.music.push_back(MakeTrack(file, RadioTrackType::Music, ini));
    for (const auto& file : AudioFiles(path / "interstitial")) station.interstitials.push_back(MakeTrack(file, RadioTrackType::Interstitial, ini));
    for (const auto& file : AudioFiles(path / "sfx")) station.sfx.push_back(MakeTrack(file, RadioTrackType::Sfx, ini));

    if (station.music.empty() && station.intro.empty() && error) {
        *error = "Station has no audio in intro/ or music/: " + path.string();
    }
    return station;
}
