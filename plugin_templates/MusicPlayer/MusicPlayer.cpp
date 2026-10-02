#include "plugin_api.hpp"
#include "RadioPlayer.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

extern "C" __declspec(dllexport) bool ModLoader_InitPlugin(ModLoaderGetProcFn getProc, ModLoaderPluginCtx* ctx);

namespace {
    ModLoaderHostAPI g_host{};
    ModLoaderPluginCtx* g_ctx = nullptr;
    RadioPlayer g_player;
    std::filesystem::path g_pluginRoot;

    std::filesystem::path GetPluginDirectory() {
        HMODULE module = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                reinterpret_cast<LPCWSTR>(&ModLoader_InitPlugin), &module)) return {};
        wchar_t path[MAX_PATH]{};
        const DWORD length = GetModuleFileNameW(module, path, MAX_PATH);
        if (length == 0 || length == MAX_PATH) return {};
        const std::filesystem::path dll(path);
        return dll.parent_path() / dll.stem();
    }

    const char* GetStationItem(void* userData, int index) {
        auto* names = static_cast<std::vector<std::string>*>(userData);
        return (index >= 0 && static_cast<size_t>(index) < names->size()) ? (*names)[index].c_str() : "";
    }

    void DrawMenu(ModLoaderPluginCtx* ctx) {
        const size_t stationCount = g_player.StationCount();
        if (stationCount == 0) {
            g_host.TextWrapped(ctx, "No radio stations were found. Create a %s/stations/ directory and add station folders.", g_pluginRoot.string().c_str());
            if (g_host.Button(ctx, "Refresh Stations")) g_player.RefreshStations();
            return;
        }

        g_host.Text(ctx, "Station: %s", g_player.StationName().c_str());
        const std::string title = g_player.TrackTitle();
        const std::string artist = g_player.TrackArtist();
        g_host.Text(ctx, "Track: %s", title.empty() ? "--" : title.c_str());
        if (!artist.empty()) g_host.Text(ctx, "Artist: %s", artist.c_str());
        const std::string status = g_player.Status();
        if (!status.empty()) g_host.TextDisabled(ctx, "%s", status.c_str());
        g_host.Separator(ctx);

        if (g_host.Button(ctx, "Previous")) g_player.Previous();
        g_host.SameLine(ctx);
        if (g_player.IsPlaying()) {
            if (g_host.Button(ctx, "Pause")) g_player.Pause();
        } else {
            if (g_host.Button(ctx, "Play")) g_player.Play();
        }
        g_host.SameLine(ctx);
        if (g_host.Button(ctx, "Stop")) g_player.Stop();
        g_host.SameLine(ctx);
        if (g_host.Button(ctx, "Next")) g_player.Next();

        int volume = g_player.Volume();
        if (g_host.SliderInt(ctx, "Volume", &volume, 0, 100)) g_player.SetVolume(volume);

        std::vector<std::string> names;
        names.reserve(stationCount);
        for (size_t i = 0; i < stationCount; ++i) {
            names.push_back(g_player.StationNameAt(i));
        }

        int selected = static_cast<int>(g_player.StationIndex());
        if (g_host.ListBox(ctx, "Stations", &selected, GetStationItem, &names, static_cast<int>(names.size()), 5)) {
            g_player.SetStation(static_cast<size_t>(selected));
        }

        if (g_host.Button(ctx, "Refresh Stations")) g_player.RefreshStations();

        g_host.TextWrapped(ctx, "Station playback settings are read from each station's station.ini. Track metadata uses [filename.ext] sections in that same file.");
    }

    void Command(ModLoaderPluginCtx*, const char* command) {
        if (!command) return;
        const std::string cmd(command);
        if (cmd == "play") g_player.Play();
        else if (cmd == "pause") g_player.Pause();
        else if (cmd == "stop") g_player.Stop();
        else if (cmd == "next") g_player.Next();
        else if (cmd == "previous") g_player.Previous();
        else if (cmd == "refresh") g_player.RefreshStations();
    }
}

extern "C" __declspec(dllexport) bool ModLoader_InitPlugin(ModLoaderGetProcFn getProc, ModLoaderPluginCtx* ctx) {
    if (ModLoader_BindAPI(getProc, &g_host) != 0) return false;
    if (!ModLoader_AllBound(g_host.SetPluginInfo, g_host.SetDrawMenuCallback, g_host.SetCommandCallback,
                            g_host.Text, g_host.TextWrapped, g_host.TextDisabled, g_host.Button,
                            g_host.Separator, g_host.SameLine, g_host.SliderInt, g_host.Checkbox,
                            g_host.ListBox, g_host.GetConfigBool, g_host.GetConfigInt,
                            g_host.SetConfigBool, g_host.Log)) return false;

    g_ctx = ctx;
    g_pluginRoot = GetPluginDirectory();
    if (g_pluginRoot.empty()) {
        g_host.Log(ctx, "MusicPlayer: could not determine plugin directory.");
        return false;
    }

    std::string error;
    if (!g_player.Initialize(g_pluginRoot, &error)) {
        g_host.Log(ctx, "MusicPlayer: %s", error.c_str());
        // Still initialize so the UI can explain the missing station directory.
    }

    g_host.SetPluginInfo(ctx, "Music Player", "1.0.0", "Universal Snowdrop Modloader");
    g_host.SetDrawMenuCallback(ctx, &DrawMenu);
    g_host.SetCommandCallback(ctx, &Command);
    g_host.Log(ctx, "MusicPlayer: content root is %s", g_pluginRoot.string().c_str());
    return true;
}
