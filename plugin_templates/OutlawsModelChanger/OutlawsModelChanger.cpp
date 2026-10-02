#include "plugin_api.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

namespace {
    // -----------------------------------------------------------------------
    // Signatures
    // -----------------------------------------------------------------------
    constexpr const char* kPatternPlayerVisualFn = "40 55 53 48 8D 6C 24 B1 48 ?? ?? ?? ?? 00 00 ?? ?? ?? ?? 00";
    constexpr const char* kPatternVisualListFn = "E8 ?? ?? ?? ?? 48 85 C0 74 66 45";

    constexpr const char* kPlayerVisualName = "kay.mgraphobject";

    // -----------------------------------------------------------------------
    // Game structure offsets
    // -----------------------------------------------------------------------

    // Player visual object
    constexpr uintptr_t kObjNameHolder     = 0x88;
    constexpr uintptr_t kNameHolderName    = 0x50;
    constexpr uintptr_t kObjLoader         = 0xA0;
    constexpr uintptr_t kObjReloadFlag     = 0x269;

    // Visual loader list container
    constexpr uintptr_t kListEntries       = 0x18;
    constexpr uintptr_t kListCount         = 0x20;
    constexpr uintptr_t kListEntryStride   = 0x18;
    constexpr uintptr_t kListEntryLoader   = 0x10;

    // Visual loader
    constexpr uintptr_t kLoaderId          = 0x08;
    constexpr uintptr_t kLoaderField10     = 0x10;
    constexpr uintptr_t kLoaderChildren    = 0x28;
    constexpr uintptr_t kLoaderChildCount  = 0x30;
    constexpr uintptr_t kLoaderNameRef     = 0x38;
    constexpr uintptr_t kChildStride       = 0x18;

    constexpr int kMaxLoaders  = 200000;
    constexpr int kMaxChildren = 4096;

    constexpr ULONGLONG kEmptyListRetryMs = 1000;
    constexpr size_t kNameCap = 256;
    constexpr size_t kMaxChildNamesShown = 12;

    // -----------------------------------------------------------------------
    // State
    // -----------------------------------------------------------------------
    struct LoaderRecord {
        uintptr_t ptr = 0;
        uint64_t id = 0;
        uint64_t field10 = 0;
        uintptr_t nameRef = 0;
        std::vector<uintptr_t> children;
    };

    struct UiEntry {
        uintptr_t ptr = 0;
        uint64_t id = 0;
        uint64_t field10 = 0;
        std::string hex;
        std::string name;
        std::string searchText;
        size_t childCount = 0;
        std::vector<std::string> childNames;
    };

    using PlayerVisualFn_t = uint64_t(__fastcall*)(void*);
    using VisualListFn_t   = uint64_t(__fastcall*)(void*, void*, void*, void*);

    ModLoaderHostAPI g_hostApi;
    const ModLoaderHostAPI* const host = &g_hostApi;
    ModLoaderPluginCtx* g_ctx = nullptr;

    uintptr_t g_playerVisualFnAddr = 0;
    uintptr_t g_visualListFnAddr = 0;
    ModLoaderHook* g_playerVisualHook = nullptr;
    ModLoaderHook* g_visualListHook = nullptr;
    PlayerVisualFn_t g_origPlayerVisual = nullptr;
    VisualListFn_t g_origVisualList = nullptr;

    std::atomic<uintptr_t> g_playerObject{0};
    std::atomic<uintptr_t> g_pendingLoader{0};
    std::atomic<bool> g_refreshRequested{false};
    std::atomic<uint64_t> g_listVersion{0};
    std::atomic<ULONGLONG> g_lastParseAttempt{0};

    std::mutex g_listMutex;
    std::vector<LoaderRecord> g_loaders;

    bool g_applyOnGameThread = false;

    constexpr const char* kCommentShowNames =
        "Shows each visual loader's name next to its id in the list, plus the string your search matched (true/false, default: true)";

    constexpr const char* kCommentApplyOnGameThread =
        "Applies the model change from inside the game's own update call instead of directly from the menu thread. "
        "Safer, but only takes effect once the game next updates the player's visual object (true/false, default: false)";

    // Menu (render thread only)
    struct UiState {
        char search[64] = {};
        std::string lastSearch;
        uint64_t seenVersion = ~0ull;
        bool showNames = true;
        bool seenShowNames = false;

        std::vector<UiEntry> all;
        std::vector<size_t> shown;
        std::vector<std::string> labels;

        int selected = -1;
        uintptr_t selectedPtr = 0;

        std::string status;
    } g_ui;

    // -----------------------------------------------------------------------
    // Safe memory access
    // -----------------------------------------------------------------------
    bool IsPlayerVisualObject(uintptr_t object) {
        __try {
            const uintptr_t holder = *reinterpret_cast<const uintptr_t*>(object + kObjNameHolder);
            if (!holder) return false;

            const char* name = *reinterpret_cast<const char* const*>(holder + kNameHolderName);
            if (!name) return false;

            return std::strstr(name, kPlayerVisualName) != nullptr;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    bool WriteLoaderToObject(uintptr_t object, uintptr_t loader) {
        __try {
            *reinterpret_cast<uintptr_t*>(object + kObjLoader) = loader;
            *reinterpret_cast<uint8_t*>(object + kObjReloadFlag) = 1;
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    bool SafeReadName(uintptr_t holder, char* buf, size_t cap) {
        __try {
            if (!holder) return false;

            const char* text = *reinterpret_cast<const char* const*>(holder);
            if (!text) return false;

            size_t i = 0;
            for (; i + 1 < cap && text[i]; ++i) buf[i] = text[i];
            buf[i] = '\0';
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    template <typename T>
    T Read(uintptr_t address) {
        return *reinterpret_cast<const T*>(address);
    }

    void ParseContainer(uintptr_t container, std::vector<LoaderRecord>& out) {
        const uintptr_t entries = Read<uintptr_t>(container + kListEntries);
        const int count = Read<int32_t>(container + kListCount);
        if (!entries || count <= 0 || count > kMaxLoaders) return;

        out.reserve(static_cast<size_t>(count));
        for (int i = 0; i < count; ++i) {
            const uintptr_t loader = Read<uintptr_t>(entries + kListEntryLoader + static_cast<uintptr_t>(i) * kListEntryStride);
            if (!loader) continue;

            LoaderRecord record;
            record.ptr = loader;
            record.id = Read<uint64_t>(loader + kLoaderId);
            record.field10 = Read<uint64_t>(loader + kLoaderField10);
            record.nameRef = Read<uintptr_t>(loader + kLoaderNameRef);

            const uintptr_t children = Read<uintptr_t>(loader + kLoaderChildren);
            const int childCount = Read<int32_t>(loader + kLoaderChildCount);
            if (children && childCount > 0 && childCount <= kMaxChildren) {
                for (int j = 0; j < childCount; ++j) {
                    const uintptr_t child = Read<uintptr_t>(children + static_cast<uintptr_t>(j) * kChildStride);
                    if (child) record.children.push_back(child);
                }
            }

            out.push_back(std::move(record));
        }
    }

    bool SafeParseContainer(uintptr_t container, std::vector<LoaderRecord>* out) {
        __try {
            ParseContainer(container, *out);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    // -----------------------------------------------------------------------
    // Detours
    // -----------------------------------------------------------------------
    void MaybeParseList(uintptr_t container) {
        if (!container) return;

        const bool refresh = g_refreshRequested.exchange(false);
        if (!refresh) {
            bool empty;
            {
                std::lock_guard<std::mutex> lock(g_listMutex);
                empty = g_loaders.empty();
            }
            if (!empty) return;

            const ULONGLONG now = GetTickCount64();
            if (now - g_lastParseAttempt.load() < kEmptyListRetryMs) return;
        }
        g_lastParseAttempt.store(GetTickCount64());

        std::vector<LoaderRecord> parsed;
        if (!SafeParseContainer(container, &parsed)) {
            host->Log(g_ctx, "Visual loader list: container could not be read, ignoring");
            return;
        }
        if (parsed.empty()) return;

        const size_t count = parsed.size();
        {
            std::lock_guard<std::mutex> lock(g_listMutex);
            g_loaders = std::move(parsed);
        }
        g_listVersion.fetch_add(1);
        host->Log(g_ctx, "ParseVisualLoaderList: Parsed %zu Visual Loaders!", count);
    }

    uint64_t __fastcall Hook_PlayerVisual(void* self) {
        const uintptr_t object = reinterpret_cast<uintptr_t>(self);
        if (object && IsPlayerVisualObject(object)) {
            g_playerObject.store(object);

            const uintptr_t pending = g_pendingLoader.exchange(0);
            if (pending) WriteLoaderToObject(object, pending);
        }
        return g_origPlayerVisual(self);
    }

    uint64_t __fastcall Hook_VisualList(void* a1, void* a2, void* a3, void* a4) {
        MaybeParseList(reinterpret_cast<uintptr_t>(a1));
        return g_origVisualList(a1, a2, a3, a4);
    }

    // -----------------------------------------------------------------------
    // Menu helpers
    // -----------------------------------------------------------------------
    std::string ToLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    std::string Trim(const std::string& s) {
        size_t begin = 0, end = s.size();
        while (begin < end && std::isspace(static_cast<unsigned char>(s[begin]))) ++begin;
        while (end > begin && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return s.substr(begin, end - begin);
    }

    std::string ReadName(uintptr_t holder) {
        char buf[kNameCap];
        return SafeReadName(holder, buf, sizeof(buf)) ? std::string(buf) : std::string();
    }

    const UiEntry* SelectedEntry() {
        if (g_ui.selected < 0 || static_cast<size_t>(g_ui.selected) >= g_ui.shown.size()) return nullptr;
        return &g_ui.all[g_ui.shown[static_cast<size_t>(g_ui.selected)]];
    }

    std::string BuildLabel(const UiEntry& entry, const std::string& needle) {
        if (!g_ui.showNames) return entry.hex;

        std::string label = entry.hex;
        if (!entry.name.empty()) label += "  " + entry.name;

        const bool nameMatches = !needle.empty() && ToLower(entry.name).find(needle) != std::string::npos;
        const std::string* shownChild = nullptr;

        if (!needle.empty() && !nameMatches) {
            for (const auto& childName : entry.childNames) {
                if (ToLower(childName).find(needle) != std::string::npos) {
                    shownChild = &childName;
                    break;
                }
            }
        }
        if (!shownChild && entry.name.empty() && !entry.childNames.empty()) shownChild = &entry.childNames.front();

        if (shownChild) label += "  [" + *shownChild + "]";
        return label;
    }

    void RebuildUi() {
        const uint64_t version = g_listVersion.load();
        const std::string search = g_ui.search;
        if (version == g_ui.seenVersion && search == g_ui.lastSearch && g_ui.showNames == g_ui.seenShowNames) return;

        if (version != g_ui.seenVersion) {
            std::vector<LoaderRecord> records;
            {
                std::lock_guard<std::mutex> lock(g_listMutex);
                records = g_loaders;
            }

            std::vector<UiEntry> snapshot;
            snapshot.reserve(records.size());
            for (const auto& record : records) {
                char hex[32];
                std::snprintf(hex, sizeof(hex), "0x%llX", static_cast<unsigned long long>(record.id));

                UiEntry entry;
                entry.ptr = record.ptr;
                entry.id = record.id;
                entry.field10 = record.field10;
                entry.hex = hex;
                entry.name = ReadName(record.nameRef);
                entry.childCount = record.children.size();

                entry.searchText = ToLower(entry.hex);
                if (!entry.name.empty()) {
                    entry.searchText += '\n';
                    entry.searchText += ToLower(entry.name);
                }
                for (const uintptr_t child : record.children) {
                    std::string childName = ReadName(child);
                    if (childName.empty()) continue;
                    entry.searchText += '\n';
                    entry.searchText += ToLower(childName);
                    entry.childNames.push_back(std::move(childName));
                }

                snapshot.push_back(std::move(entry));
            }
            g_ui.all = std::move(snapshot);
            g_ui.seenVersion = version;
        }

        const std::string needle = ToLower(Trim(search));
        g_ui.shown.clear();
        g_ui.labels.clear();
        for (size_t i = 0; i < g_ui.all.size(); ++i) {
            const UiEntry& entry = g_ui.all[i];
            if (!needle.empty() && entry.searchText.find(needle) == std::string::npos) continue;

            g_ui.shown.push_back(i);
            g_ui.labels.push_back(BuildLabel(entry, needle));
        }
        g_ui.lastSearch = search;
        g_ui.seenShowNames = g_ui.showNames;

        g_ui.selected = -1;
        for (size_t i = 0; i < g_ui.shown.size(); ++i) {
            if (g_ui.all[g_ui.shown[i]].ptr == g_ui.selectedPtr) {
                g_ui.selected = static_cast<int>(i);
                break;
            }
        }
    }

    const char* GetListItem(void*, int index) {
        if (index < 0 || static_cast<size_t>(index) >= g_ui.labels.size()) return "";
        return g_ui.labels[static_cast<size_t>(index)].c_str();
    }

    void ApplySelected() {
        const UiEntry* entry = SelectedEntry();
        if (!entry) {
            g_ui.status = "Select a visual loader first.";
            return;
        }

        const uintptr_t object = g_playerObject.load();
        if (!object) {
            g_ui.status = "The player's visual object hasn't been captured yet. Load into the game world first.";
            return;
        }

        char text[96];
        std::snprintf(text, sizeof(text), "Applying visual loader 0x%llX", static_cast<unsigned long long>(entry->id));
        host->Log(g_ctx, "%s%s%s", text, entry->name.empty() ? "" : " - ", entry->name.c_str());

        if (g_applyOnGameThread) {
            g_pendingLoader.store(entry->ptr);
            g_ui.status = std::string(text) + " (queued for the next game update)";
        } else if (WriteLoaderToObject(object, entry->ptr)) {
            g_ui.status = text;
        } else {
            g_ui.status = "Failed to write to the player's visual object (pointer is no longer valid?).";
            host->Log(g_ctx, "Failed to write loader %p to object %p", reinterpret_cast<void*>(entry->ptr), reinterpret_cast<void*>(object));
        }
    }

    // -----------------------------------------------------------------------
    // Menu
    // -----------------------------------------------------------------------
    void DrawMenu(ModLoaderPluginCtx* ctx) {
        host->Text(ctx, "MODEL CHANGER");
        host->Separator(ctx);

        if (!g_playerVisualHook) {
            host->TextWrapped(ctx, "Player visual hook: pattern not found (unsupported game version?)");
        }
        if (!g_visualListHook) {
            host->TextWrapped(ctx, "Visual loader list hook: pattern not found (unsupported game version?)");
        }

        RebuildUi();

        if (g_ui.all.empty()) {
            host->TextWrapped(ctx, "No visual loaders found yet! You can try opening the Inventory Menu first.");
        } else {
            host->InputText(ctx, "Search:", g_ui.search, sizeof(g_ui.search));
            host->TextDisabled(ctx, "Matches the loader id, its name, and its children's names (e.g. Vader, Boba). Showing %zu of %zu.", g_ui.shown.size(), g_ui.all.size());

            if (host->ListBox(ctx, "Visual Loaders", &g_ui.selected, &GetListItem, nullptr, static_cast<int>(g_ui.shown.size()), 12)) {
                if (const UiEntry* picked = SelectedEntry()) g_ui.selectedPtr = picked->ptr;
            }

            if (host->Button(ctx, "Apply Selected")) ApplySelected();
            host->SameLine(ctx);
            if (host->Button(ctx, "Refresh List")) {
                g_refreshRequested.store(true);
                g_ui.status = "List will refresh the next time the game calls into it (open the Inventory Menu).";
            }
        }

        if (!g_ui.status.empty()) {
            host->Separator(ctx);
            host->TextWrapped(ctx, "%s", g_ui.status.c_str());
        }

        host->Separator(ctx);
        host->Text(ctx, "===MODEL CHANGER INFO===");
        host->Text(ctx, "Base: %p", reinterpret_cast<void*>(GetModuleHandleW(nullptr)));

        const uintptr_t object = g_playerObject.load();
        if (object) {
            host->Text(ctx, "Player visual object: %p", reinterpret_cast<void*>(object));
        } else {
            host->Text(ctx, "Player visual object: not captured yet");
        }

        if (const UiEntry* entry = SelectedEntry()) {
            host->Text(ctx, "Selected loader: %p", reinterpret_cast<void*>(entry->ptr));
            host->Text(ctx, "  Id: %s", entry->hex.c_str());
            host->Text(ctx, "  Name: %s", entry->name.empty() ? "(none)" : entry->name.c_str());
            host->Text(ctx, "  Children: %zu", entry->childCount);

            const size_t toShow = std::min(entry->childNames.size(), kMaxChildNamesShown);
            for (size_t i = 0; i < toShow; ++i) host->Text(ctx, "    %s", entry->childNames[i].c_str());
            if (entry->childNames.size() > toShow) host->TextDisabled(ctx, "    ... and %zu more", entry->childNames.size() - toShow);
        }

        host->Separator(ctx);
        if (host->Checkbox(ctx, "Show names in list", &g_ui.showNames)) {
            host->SetConfigBool(ctx, "ShowNames", kCommentShowNames, g_ui.showNames);
        }
        if (host->Checkbox(ctx, "Apply on game thread", &g_applyOnGameThread)) {
            host->SetConfigBool(ctx, "ApplyOnGameThread", kCommentApplyOnGameThread, g_applyOnGameThread);
            if (!g_applyOnGameThread) g_pendingLoader.store(0);
        }
        host->TextDisabled(ctx, "When on, the change is queued and applied from the game's own update call. When off, it is written immediately.");

        if (g_playerVisualFnAddr) host->TextDisabled(ctx, "Player visual fn: %p", reinterpret_cast<void*>(g_playerVisualFnAddr));
        if (g_visualListFnAddr) host->TextDisabled(ctx, "Visual list fn: %p", reinterpret_cast<void*>(g_visualListFnAddr));
    }

    // -----------------------------------------------------------------------
    // Setup
    // -----------------------------------------------------------------------
    bool IsOutlawsExecutable() {
        wchar_t path[MAX_PATH]{};
        if (!GetModuleFileNameW(nullptr, path, MAX_PATH)) return false;

        std::wstring name = path;
        for (auto& c : name) c = static_cast<wchar_t>(std::towlower(c));
        return name.find(L"outlaws") != std::wstring::npos;
    }

    ModLoaderHook* InstallHook(ModLoaderPluginCtx* ctx, const char* name, uintptr_t target, void* detour, void** original) {
        ModLoaderHook* hook = host->CreateHook(ctx, target, detour, original);
        if (!hook) {
            host->Log(ctx, "%s: failed to create hook at %p", name, reinterpret_cast<void*>(target));
            return nullptr;
        }
        if (!host->SetHookEnabled(ctx, hook, true)) {
            host->Log(ctx, "%s: failed to enable hook at %p", name, reinterpret_cast<void*>(target));
            host->DestroyHook(ctx, hook);
            return nullptr;
        }
        host->Log(ctx, "%s: hooked at %p", name, reinterpret_cast<void*>(target));
        return hook;
    }
}

extern "C" __declspec(dllexport) bool ModLoader_InitPlugin(ModLoaderGetProcFn getProc, ModLoaderPluginCtx* ctx) {
    ModLoader_BindAPI(getProc, &g_hostApi);
    if (!ModLoader_AllBound(host->SetPluginInfo, host->SetDrawMenuCallback, host->Text, host->TextWrapped,
                            host->TextDisabled, host->Checkbox, host->InputText, host->Button, host->SameLine,
                            host->ListBox, host->Separator, host->GetConfigBool, host->SetConfigBool, host->Log,
                            host->FindPattern, host->ResolveRelative, host->CreateHook, host->SetHookEnabled,
                            host->DestroyHook)) return false;
    if (!IsOutlawsExecutable()) return false;

    g_ctx = ctx;
    g_applyOnGameThread = host->GetConfigBool(ctx, "ApplyOnGameThread", kCommentApplyOnGameThread, false);
    g_ui.showNames = host->GetConfigBool(ctx, "ShowNames", kCommentShowNames, true);

    g_playerVisualFnAddr = host->FindPattern(ctx, kPatternPlayerVisualFn);
    if (g_playerVisualFnAddr) {
        g_playerVisualHook = InstallHook(ctx, "PlayerVisual", g_playerVisualFnAddr,
                                         reinterpret_cast<void*>(&Hook_PlayerVisual),
                                         reinterpret_cast<void**>(&g_origPlayerVisual));
    } else {
        host->Log(ctx, "PlayerVisual: pattern not found, skipping");
    }

    const uintptr_t callSite = host->FindPattern(ctx, kPatternVisualListFn);
    if (callSite) {
        g_visualListFnAddr = host->ResolveRelative(ctx, callSite, 1, 5);
        host->Log(ctx, "VisualList: call site at %p, target %p", reinterpret_cast<void*>(callSite), reinterpret_cast<void*>(g_visualListFnAddr));
    }
    if (g_visualListFnAddr) {
        g_visualListHook = InstallHook(ctx, "VisualList", g_visualListFnAddr,
                                       reinterpret_cast<void*>(&Hook_VisualList),
                                       reinterpret_cast<void**>(&g_origVisualList));
    } else {
        host->Log(ctx, "VisualList: pattern not found, skipping");
    }

    host->SetPluginInfo(ctx, "Outlaws Model Changer", "1.0.0", nullptr);
    host->SetDrawMenuCallback(ctx, &DrawMenu);
    return true;
}
