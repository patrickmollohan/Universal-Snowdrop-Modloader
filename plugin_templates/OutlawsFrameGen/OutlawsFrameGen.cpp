#include "plugin_api.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cwctype>
#include <string>
#include <vector>

namespace {
    struct Patch {
        const char* configKey;
        const char* label;
        const char* description;
        const char* configComment;
        const char* pattern;
        size_t offset;
        std::vector<uint8_t> bytes;
        bool defaultEnabled;

        ModLoaderPatch* handle = nullptr;
        bool enabled = false;
    };

    Patch g_Patches[] = {
        {
            "CutsceneFrameGen",
            "Enable frame generation in cutscenes",
            "Allows frame generation to stay active during cutscenes.",
            "Allows the use of frame generation in cutscenes (true/false, default: true)",
            "48 83 ?? ?? 00 74 ?? 48 8B ?? ?? ?? 01 E8 ?? ?? ?? ?? C5 ?? ?? ?? 76 ?? 48 8B ?? E8 ?? ?? ?? ??",
            12, { 0x00 }, true
        },
        {
            "FlightCutsceneFrameGen",
            "Enable frame generation in flight cutscenes",
            "Allows frame generation during flight take-off/landing cutscenes.",
            "Allows the use of frame generation in flight take-off/landing cutscenes (true/false, default: true)",
            "?? ?? 01 E8 ?? ?? ?? ?? C5 ?? ?? ?? 76 ?? 48 8B ?? E8 ?? ?? ?? ?? 48 8D ?? ?? ?? 48 8B ?? C5 ?? ?? ?? E8 ?? ?? ?? ??",
            2, { 0x00 }, true
        },
        {
            "AlwaysFrameGen",
            "Enable frame generation everywhere else",
            "Allows frame generation in all remaining places (lockpicking, sabacc, etc).",
            "Allows the use of frame generation everywhere else, i.e. lockpicking, sabacc etc (true/false, default: false)",
            "74 ?? 48 8B ?? E8 ?? ?? ?? ?? 84 ?? 75 ?? ?? 01 EB ?? 32 ?? 88 ?? ?? ?? ?? ?? 40 ?? ?? ?? ?? ?? ??",
            0, { 0x90, 0x90 }, false
        },
        {
            "CutsceneLetterboxing",
            "Disable cutscene letterboxing",
            "Removes the letterbox/pillarbox bars in cutscenes.",
            "Disables cutscene letterboxing/pillarboxing (true/false, default: true)",
            "80 ?? ?? 00 74 ?? 8B ?? ?? C5 ?? ?? ?? ?? 25 ?? ?? ?? ?? 83 ?? 01 74 ??",
            4, { 0xEB }, true
        },
    };

    bool IsOutlawsExecutable() {
        wchar_t path[MAX_PATH]{};
        if (!GetModuleFileNameW(nullptr, path, MAX_PATH)) return false;

        std::wstring name = path;
        for (auto& c : name) c = static_cast<wchar_t>(std::towlower(c));
        return name.find(L"outlaws") != std::wstring::npos;
    }
}

// ---------------------------------------------------------------------------
// Host API
// ---------------------------------------------------------------------------
static ModLoaderHostAPI g_hostApi;
static const ModLoaderHostAPI* const host = &g_hostApi;

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------
static void DrawMenu(ModLoaderPluginCtx* ctx) {
    host->TextWrapped(ctx, "Changes apply immediately, but the game may only re-check some of them when the next scene/cutscene loads.");
    host->Separator(ctx);

    for (auto& patch : g_Patches) {
        if (!patch.handle) {
            host->TextWrapped(ctx, "%s: pattern not found (unsupported game version?)", patch.label);
            continue;
        }

        bool enabled = patch.enabled;
        if (host->Checkbox(ctx, patch.label, &enabled)) {
            if (host->SetPatchEnabled(ctx, patch.handle, enabled)) {
                patch.enabled = enabled;
                host->SetConfigBool(ctx, patch.configKey, patch.configComment, patch.enabled);
            } else {
                host->Log(ctx, "Failed to %s patch %s", enabled ? "apply" : "revert", patch.configKey);
            }
        }
        host->TextWrapped(ctx, "%s", patch.description);
    }
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool ModLoader_InitPlugin(ModLoaderGetProcFn getProc, ModLoaderPluginCtx* ctx) {
    ModLoader_BindAPI(getProc, &g_hostApi);
    if (!ModLoader_AllBound(host->SetPluginInfo, host->SetDrawMenuCallback, host->TextWrapped, host->Separator,
                            host->Checkbox, host->GetConfigBool, host->SetConfigBool, host->CreatePatch,
                            host->SetPatchEnabled, host->GetPatchAddress, host->Log)) return false;
    if (!IsOutlawsExecutable()) return false;

    for (auto& patch : g_Patches) {
        patch.enabled = host->GetConfigBool(ctx, patch.configKey, patch.configComment, patch.defaultEnabled);

        patch.handle = host->CreatePatch(ctx, patch.pattern, patch.offset, patch.bytes.data(), patch.bytes.size());
        if (!patch.handle) {
            host->Log(ctx, "%s: pattern not found, skipping", patch.configKey);
            continue;
        }
        host->Log(ctx, "%s: found at %p", patch.configKey, reinterpret_cast<void*>(host->GetPatchAddress(ctx, patch.handle)));
    }

    for (auto& patch : g_Patches) {
        if (!patch.handle || !patch.enabled) continue;
        if (!host->SetPatchEnabled(ctx, patch.handle, true)) {
            host->Log(ctx, "%s: failed to write patch", patch.configKey);
            patch.enabled = false;
        }
    }

    host->SetPluginInfo(ctx, "Frame Generation and Letterboxing Fix", "1.0.0", nullptr);
    host->SetDrawMenuCallback(ctx, &DrawMenu);

    return true;
}
