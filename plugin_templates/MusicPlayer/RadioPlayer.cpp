#include "RadioPlayer.hpp"
#include "StationLoader.hpp"

#include "lib/miniaudio/miniaudio.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <numeric>

namespace {
    constexpr auto kWorkerSleep = std::chrono::milliseconds(50);

    float PercentToVolume(int percent) {
        return static_cast<float>(std::clamp(percent, 0, 100)) / 100.0f;
    }

    void Shuffle(std::vector<size_t>& values, std::mt19937& rng) {
        std::shuffle(values.begin(), values.end(), rng);
    }
}

RadioPlayer::RadioPlayer()
    : m_rng(std::random_device{}()) {
}

RadioPlayer::~RadioPlayer() {
    Shutdown();
}

bool RadioPlayer::Initialize(const std::filesystem::path& pluginRoot, std::string* error) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pluginRoot = pluginRoot;
    m_volume = 100;
    m_stations = StationLoader::LoadStations(pluginRoot / "stations", error);

    if (!EnsureEngineLocked()) {
        if (error) *error = "Failed to initialize miniaudio.";
        return false;
    }

    m_stopRequested = true;
    m_workerRunning = true;
    m_worker = std::thread(&RadioPlayer::WorkerMain, this);
    m_status = m_stations.empty() ? "No stations found" : "Ready";
    return !m_stations.empty();
}

void RadioPlayer::Shutdown() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_workerRunning && !m_engine) return;
        m_stopRequested = true;
        m_wake.notify_all();
    }

    if (m_worker.joinable()) m_worker.join();

    std::lock_guard<std::mutex> lock(m_mutex);
    StopSoundLocked();
    StopBackgroundSfxLocked();
    if (m_engine) {
        ma_engine_uninit(m_engine.get());
        m_engine.reset();
    }
    m_workerRunning = false;
}

void RadioPlayer::RefreshStations() {
    std::string error;
    auto stations = StationLoader::LoadStations(m_pluginRoot / "stations", &error);
    std::lock_guard<std::mutex> lock(m_mutex);
    const std::string current = m_stations.empty() ? std::string{} : m_stations[m_stationIndex].name;
    m_stations = std::move(stations);
    if (m_stations.empty()) {
        m_stationIndex = 0;
        StopSoundLocked();
        StopBackgroundSfxLocked();
        m_status = error.empty() ? "No stations found" : error;
        return;
    }
    StopSoundLocked();
    StopBackgroundSfxLocked();
    m_startedStation = false;
    m_introPlaying = false;
    m_introPosition = 0;
    m_musicSinceInterstitial = 0;
    m_stationIndex = 0;
    for (size_t i = 0; i < m_stations.size(); ++i) {
        if (m_stations[i].name == current) {
            m_stationIndex = i;
            break;
        }
    }
    m_status = "Stations refreshed";
}

void RadioPlayer::Play() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stations.empty()) return;
    m_stopRequested = false;
    if (m_currentSound && m_paused) {
        ma_sound_start(m_currentSound.get());
        m_paused = false;
    } else {
        m_paused = false;
    }
    m_wake.notify_all();
}

void RadioPlayer::Pause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_currentSound) return;
    ma_sound_stop(m_currentSound.get());
    m_paused = true;
    m_status = "Paused";
}

void RadioPlayer::Stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stopRequested = true;
    m_paused = false;
    StopSoundLocked();
    StopBackgroundSfxLocked();
    m_status = "Stopped";
    m_currentTitle.clear();
    m_currentArtist.clear();
}

void RadioPlayer::Next() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stations.empty()) return;
    StopSoundLocked();
    m_introPlaying = false;
    m_startedStation = true;
    m_stopRequested = false;
    m_paused = false;
    m_wake.notify_all();
}

void RadioPlayer::Previous() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stations.empty()) return;
    if (m_history.size() >= 2) {
        m_history.pop_back();
        const size_t previous = m_history.back();
        if (previous < m_stations[m_stationIndex].music.size()) {
            m_musicOrder.clear();
            m_musicOrder.push_back(previous);
            m_musicPosition = 0;
        }
    } else if (!m_musicOrder.empty() && m_musicPosition > 0) {
        --m_musicPosition;
    }
    StopSoundLocked();
    m_introPlaying = false;
    m_startedStation = true;
    m_stopRequested = false;
    m_paused = false;
    m_wake.notify_all();
}

void RadioPlayer::SetStation(size_t index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index >= m_stations.size() || index == m_stationIndex) return;
    m_stationIndex = index;
    m_history.clear();
    m_musicSinceInterstitial = 0;
    m_musicPosition = 0;
    m_introPosition = 0;
    m_interstitialPosition = 0;
    m_introPlaying = false;
    m_startedStation = false;
    BuildMusicOrderLocked();
    BuildInterstitialOrderLocked();
    StopSoundLocked();
    StopBackgroundSfxLocked();
    PlayTransitionSfxLocked();
    m_stopRequested = false;
    m_paused = false;
    m_currentTitle.clear();
    m_currentArtist.clear();
    m_status = "Tuned to " + m_stations[m_stationIndex].name;
    m_wake.notify_all();
}

void RadioPlayer::SetVolume(int percent) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_volume = std::clamp(percent, 0, 100);
    if (m_currentSound) ma_sound_set_volume(m_currentSound.get(), PercentToVolume(m_volume));
    for (auto& sound : m_backgroundSounds) ma_sound_set_volume(sound.get(), PercentToVolume(m_volume));
}

bool RadioPlayer::IsPlaying() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSound && !m_paused && !m_stopRequested && ma_sound_is_playing(m_currentSound.get());
}

int RadioPlayer::Volume() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_volume;
}

size_t RadioPlayer::StationIndex() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stationIndex;
}

size_t RadioPlayer::StationCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stations.size();
}

std::string RadioPlayer::StationName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stations.empty() ? std::string{} : m_stations[m_stationIndex].name;
}

std::string RadioPlayer::TrackTitle() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentTitle;
}

std::string RadioPlayer::TrackArtist() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentArtist;
}

std::string RadioPlayer::StationNameAt(size_t index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return index < m_stations.size() ? m_stations[index].name : std::string{};
}

std::string RadioPlayer::Status() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_status;
}

bool RadioPlayer::EnsureEngineLocked() {
    if (m_engine) return true;
    m_engine = std::make_unique<ma_engine>();
    ma_engine_config config = ma_engine_config_init();
    if (ma_engine_init(&config, m_engine.get()) != MA_SUCCESS) {
        m_engine.reset();
        return false;
    }
    return true;
}

void RadioPlayer::WorkerMain() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_stopRequested || !m_paused) {
        m_wake.wait_for(lock, kWorkerSleep);
        if (!m_workerRunning) break;
        ProcessPlayback();
        if (m_stopRequested) {
            if (m_currentSound) StopSoundLocked();
            continue;
        }
        if (m_paused) continue;
        if (!m_currentSound || ma_sound_at_end(m_currentSound.get())) {
            if (m_currentSound) StopSoundLocked();
            StartCurrentTrackLocked();
        }
    }
}

void RadioPlayer::ProcessPlayback() {
    if (m_stations.empty() || m_stopRequested || m_paused) return;
    if (!m_currentSound) StartCurrentTrackLocked();
}

void RadioPlayer::StopSoundLocked() {
    if (!m_currentSound) return;
    ma_sound_stop(m_currentSound.get());
    ma_sound_uninit(m_currentSound.get());
    m_currentSound.reset();
}

void RadioPlayer::ClearHistoryLocked() {
    m_history.clear();
}

void RadioPlayer::BuildMusicOrderLocked() {
    m_musicOrder.resize(m_stations[m_stationIndex].music.size());
    std::iota(m_musicOrder.begin(), m_musicOrder.end(), 0);
    if (m_stations[m_stationIndex].shuffleMusic) Shuffle(m_musicOrder, m_rng);
    m_musicPosition = 0;
}

void RadioPlayer::BuildInterstitialOrderLocked() {
    m_interstitialOrder.resize(m_stations[m_stationIndex].interstitials.size());
    std::iota(m_interstitialOrder.begin(), m_interstitialOrder.end(), 0);
    if (m_stations[m_stationIndex].shuffleInterstitials) Shuffle(m_interstitialOrder, m_rng);
    m_interstitialPosition = 0;
}

const RadioTrack* RadioPlayer::ChooseInterstitialLocked() {
    auto& station = m_stations[m_stationIndex];
    if (station.interstitials.empty()) return nullptr;
    if (m_interstitialOrder.empty() || m_interstitialPosition >= m_interstitialOrder.size()) BuildInterstitialOrderLocked();
    return &station.interstitials[m_interstitialOrder[m_interstitialPosition++]];
}

const RadioTrack* RadioPlayer::NextTrackLocked() {
    auto& station = m_stations[m_stationIndex];
    if (station.music.empty()) return nullptr;
    if (m_musicOrder.empty() || m_musicPosition >= m_musicOrder.size()) BuildMusicOrderLocked();
    const size_t index = m_musicOrder[m_musicPosition++];
    m_history.push_back(index);
    if (m_history.size() > 32) m_history.erase(m_history.begin());
    ++m_musicSinceInterstitial;
    return &station.music[index];
}

const RadioTrack* RadioPlayer::PreviousTrackLocked() {
    auto& station = m_stations[m_stationIndex];
    if (m_history.size() < 2) return nullptr;
    m_history.pop_back();
    const size_t index = m_history.back();
    for (size_t i = 0; i < m_musicOrder.size(); ++i) {
        if (m_musicOrder[i] == index) {
            m_musicPosition = i + 1;
            break;
        }
    }
    return &station.music[index];
}

bool RadioPlayer::StartCurrentTrackLocked() {
    if (!m_engine || m_stations.empty()) return false;
    auto& station = m_stations[m_stationIndex];

    if (m_musicOrder.empty() && !station.music.empty()) BuildMusicOrderLocked();
    if (m_interstitialOrder.empty() && !station.interstitials.empty()) BuildInterstitialOrderLocked();
    if (!m_startedStation) {
        StartBackgroundSfxLocked();
        m_startedStation = true;
        m_introPosition = 0;
        m_introPlaying = !station.intro.empty();
    }

    if (m_introPlaying && m_introPosition < station.intro.size()) {
        const RadioTrack& track = station.intro[m_introPosition++];
        m_currentSound = std::make_unique<ma_sound>();
        if (ma_sound_init_from_file(m_engine.get(), track.path.string().c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM, nullptr, nullptr, m_currentSound.get()) != MA_SUCCESS) {
            m_currentSound.reset();
            m_introPlaying = false;
        } else {
            ma_sound_set_volume(m_currentSound.get(), PercentToVolume(m_volume) * PercentToVolume(station.musicVolumePercent));
            ma_sound_start(m_currentSound.get());
            m_currentTitle = track.title;
            m_currentArtist = track.artist;
            m_status = track.title;
            return true;
        }
    }

    if (m_introPlaying && m_introPosition >= station.intro.size()) m_introPlaying = false;

    const bool interstitialDue = !station.interstitials.empty() &&
        m_musicSinceInterstitial >= station.minMusicBetweenInterstitials &&
        (m_musicSinceInterstitial >= station.maxMusicBetweenInterstitials ||
         std::uniform_int_distribution<int>(station.minMusicBetweenInterstitials, station.maxMusicBetweenInterstitials)(m_rng) <= m_musicSinceInterstitial);

    const RadioTrack* track = interstitialDue ? ChooseInterstitialLocked() : NextTrackLocked();
    if (!track) return false;
    if (interstitialDue) m_musicSinceInterstitial = 0;

    m_currentSound = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(m_engine.get(), track->path.string().c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM, nullptr, nullptr, m_currentSound.get()) != MA_SUCCESS) {
        m_currentSound.reset();
        m_status = "Failed to load " + track->fileName;
        return false;
    }

    const int categoryVolume = track->type == RadioTrackType::Interstitial ? station.interstitialVolumePercent : station.musicVolumePercent;
    ma_sound_set_volume(m_currentSound.get(), PercentToVolume(m_volume) * PercentToVolume(categoryVolume));
    ma_sound_start(m_currentSound.get());
    m_currentTitle = track->title;
    m_currentArtist = track->artist;
    m_status = track->title;
    return true;
}

void RadioPlayer::StartBackgroundSfxLocked() {
    StopBackgroundSfxLocked();
    if (m_stations.empty()) return;
    auto& station = m_stations[m_stationIndex];
    for (const auto& track : station.sfx) {
        if (track.sfxRole != RadioSfxRole::Background) continue;
        auto sound = std::make_unique<ma_sound>();
        if (ma_sound_init_from_file(m_engine.get(), track.path.string().c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound.get()) != MA_SUCCESS) continue;
        ma_sound_set_looping(sound.get(), track.loop);
        ma_sound_set_volume(sound.get(), PercentToVolume(m_volume) * PercentToVolume(station.sfxVolumePercent) * PercentToVolume(track.volumePercent));
        ma_sound_start(sound.get());
        m_backgroundSounds.push_back(std::move(sound));
    }
}

void RadioPlayer::StopBackgroundSfxLocked() {
    for (auto& sound : m_backgroundSounds) {
        ma_sound_stop(sound.get());
        ma_sound_uninit(sound.get());
    }
    m_backgroundSounds.clear();
}

void RadioPlayer::PlayTransitionSfxLocked() {
    if (!m_engine || m_stations.empty()) return;
    auto& station = m_stations[m_stationIndex];
    for (const auto& track : station.sfx) {
        if (track.sfxRole != RadioSfxRole::Transition) continue;
        ma_engine_play_sound(m_engine.get(), track.path.string().c_str(), nullptr);
        break;
    }
}
