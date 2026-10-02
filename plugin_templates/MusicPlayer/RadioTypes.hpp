#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

enum class RadioTrackType {
    Intro,
    Music,
    Interstitial,
    Sfx
};

enum class RadioSfxRole {
    Background,
    Transition,
    OneShot
};

struct RadioTrack {
    std::filesystem::path path;
    std::string fileName;
    std::string title;
    std::string artist;
    RadioTrackType type = RadioTrackType::Music;
    RadioSfxRole sfxRole = RadioSfxRole::OneShot;
    int volumePercent = 100;
    bool loop = false;
};

struct RadioStation {
    std::filesystem::path path;
    std::string name;
    std::string description;

    bool shuffleMusic = true;
    bool shuffleInterstitials = true;
    int minMusicBetweenInterstitials = 3;
    int maxMusicBetweenInterstitials = 5;

    int musicVolumePercent = 100;
    int interstitialVolumePercent = 100;
    int sfxVolumePercent = 35;

    std::vector<RadioTrack> intro;
    std::vector<RadioTrack> music;
    std::vector<RadioTrack> interstitials;
    std::vector<RadioTrack> sfx;
};
