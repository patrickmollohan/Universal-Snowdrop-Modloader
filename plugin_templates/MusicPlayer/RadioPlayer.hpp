#pragma once

#include "RadioTypes.hpp"

#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

struct ma_engine;
struct ma_sound;

class RadioPlayer {
public:
    RadioPlayer();
    ~RadioPlayer();

    bool Initialize(const std::filesystem::path& pluginRoot, std::string* error);
    void Shutdown();

    void RefreshStations();

    void Play();
    void Pause();
    void Stop();
    void Next();
    void Previous();
    void SetStation(size_t index);
    void SetVolume(int percent);

    bool IsPlaying() const;
    int Volume() const;
    size_t StationIndex() const;
    size_t StationCount() const;
    std::string StationName() const;
    std::string StationNameAt(size_t index) const;
    std::string TrackTitle() const;
    std::string TrackArtist() const;
    std::string Status() const;

private:
    void WorkerMain();
    void ProcessPlayback();
    bool StartCurrentTrackLocked();
    void StopSoundLocked();
    void ClearHistoryLocked();
    void BuildMusicOrderLocked();
    void BuildInterstitialOrderLocked();
    const RadioTrack* NextTrackLocked();
    const RadioTrack* PreviousTrackLocked();
    const RadioTrack* ChooseInterstitialLocked();
    void StartBackgroundSfxLocked();
    void StopBackgroundSfxLocked();
    void PlayTransitionSfxLocked();
    bool EnsureEngineLocked();

    std::filesystem::path m_pluginRoot;
    std::vector<RadioStation> m_stations;
    size_t m_stationIndex = 0;

    std::vector<size_t> m_musicOrder;
    std::vector<size_t> m_interstitialOrder;
    size_t m_musicPosition = 0;
    size_t m_introPosition = 0;
    size_t m_interstitialPosition = 0;
    std::vector<size_t> m_history;
    int m_musicSinceInterstitial = 0;
    bool m_introPlaying = false;
    bool m_startedStation = false;
    bool m_paused = false;
    bool m_stopRequested = true;
    int m_volume = 100;

    std::unique_ptr<ma_engine> m_engine;
    std::unique_ptr<ma_sound> m_currentSound;
    std::vector<std::unique_ptr<ma_sound>> m_backgroundSounds;

    mutable std::mutex m_mutex;
    std::condition_variable m_wake;
    std::thread m_worker;
    bool m_workerRunning = false;
    std::mt19937 m_rng;
    std::string m_status;
    std::string m_currentTitle;
    std::string m_currentArtist;
};
