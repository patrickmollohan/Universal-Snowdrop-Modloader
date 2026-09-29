#include "plugin_api.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#include <winternl.h>
#include <tlhelp32.h>
#include <avrt.h>
#include <process.h>

#include <MinHook.h>

#include <cstring>
#include <mutex>
#include <string>
#include <unordered_set>

#pragma comment(lib, "avrt.lib")

#define POWER_THROTTLING_EXECUTION_SPEED 0x1
#define PROCESS_PRIORITY_CLASS_IDLE         1
#define PROCESS_PRIORITY_CLASS_NORMAL       2
#define PROCESS_PRIORITY_CLASS_HIGH         3
#define PROCESS_PRIORITY_CLASS_REALTIME     4
#define PROCESS_PRIORITY_CLASS_BELOW_NORMAL 5
#define PROCESS_PRIORITY_CLASS_ABOVE_NORMAL 6

namespace NtDefs {
    constexpr PROCESS_INFORMATION_CLASS ProcessPagePriority =
        static_cast<PROCESS_INFORMATION_CLASS>(0x27);
    constexpr PROCESS_INFORMATION_CLASS ProcessPowerThrottling =
        static_cast<PROCESS_INFORMATION_CLASS>(0x4B);
    constexpr PROCESS_INFORMATION_CLASS ProcessPriorityClass =
        static_cast<PROCESS_INFORMATION_CLASS>(0x4D);
    constexpr PROCESS_INFORMATION_CLASS ProcessIoPriority =
        static_cast<PROCESS_INFORMATION_CLASS>(0x4E);
    constexpr PROCESS_INFORMATION_CLASS ProcessCpuPriority =
        static_cast<PROCESS_INFORMATION_CLASS>(0x4F);

    constexpr THREADINFOCLASS ThreadPriority = static_cast<THREADINFOCLASS>(0x15);
    constexpr THREADINFOCLASS ThreadIoPriority = static_cast<THREADINFOCLASS>(0x16);
}

enum IO_PRIORITY_HINT {
    IoPriorityVeryLow = 0,
    IoPriorityLow,
    IoPriorityNormal,
    IoPriorityHigh,
    IoPriorityCritical
};

typedef struct _PROCESS_PRIORITY_CLASS {
    BOOLEAN Foreground;
    UCHAR   PriorityClass;
} PROCESS_PRIORITY_CLASS, * PPROCESS_PRIORITY_CLASS;

typedef struct _PROCESS_PAGE_PRIORITY_INFORMATION {
    ULONG PagePriority;
} PROCESS_PAGE_PRIORITY_INFORMATION, * PPROCESS_PAGE_PRIORITY_INFORMATION;

using NtSetInformationThread_t = NTSTATUS(NTAPI*)(HANDLE, THREADINFOCLASS, PVOID, ULONG);
using NtSetInformationProcess_t = NTSTATUS(NTAPI*)(HANDLE, PROCESS_INFORMATION_CLASS, PVOID, ULONG);

static bool s_cacheCreateFileA = true;
static bool s_cacheCreateFileW = true;
static int  s_cpuPriorityLevel = 2;
static int  s_threadPriorityLevel = 2;
static bool s_highIoPriority = true;

static const char* const kPriorityLevelNames[3] = { "Normal", "Medium", "High" };

static const char* const kCommentCacheA = "Enables disk caching during calls to CreateFileA (true/false, default: true)";
static const char* const kCommentCacheW = "Enables disk caching during calls to CreateFileW (true/false, default: true)";
static const char* const kCommentCpuPriority = "Sets the priority level of the CPU (0=Normal, 1=Medium, 2=High, default: 2)";
static const char* const kCommentThreadPriority = "Sets the priority level of threads (0=Normal, 1=Medium, 2=High, default: 2)";
static const char* const kCommentHighIo = "Sets the I/O priority of threads (true=High, false=Normal, default: true)";

static int Clamp3(int value) {
    return value < 0 ? 0 : (value > 2 ? 2 : value);
}

static int ResolveProcessPriorityClass(int level) {
    switch (level) {
        case 2: return HIGH_PRIORITY_CLASS;
        case 1: return ABOVE_NORMAL_PRIORITY_CLASS;
        default: return NORMAL_PRIORITY_CLASS;
    }
}

static UCHAR ResolveNtProcessPriorityClass(int level) {
    switch (level) {
        case 2: return PROCESS_PRIORITY_CLASS_HIGH;
        case 1: return PROCESS_PRIORITY_CLASS_ABOVE_NORMAL;
        default: return PROCESS_PRIORITY_CLASS_NORMAL;
    }
}

static int ResolveThreadPriority(int level) {
    switch (level) {
        case 2: return THREAD_PRIORITY_HIGHEST;
        case 1: return THREAD_PRIORITY_ABOVE_NORMAL;
        default: return THREAD_PRIORITY_NORMAL;
    }
}

static decltype(&CreateFileA)  CreateFileA_Orig = CreateFileA;
static decltype(&CreateFileW)  CreateFileW_Orig = CreateFileW;
static decltype(&SetPriorityClass)  SetPriorityClass_Orig = SetPriorityClass;
static decltype(&SetThreadPriority) SetThreadPriority_Orig = SetThreadPriority;
static decltype(&CreateThread) CreateThread_Orig = CreateThread;
static decltype(&_beginthreadex) _beginthreadex_Orig = _beginthreadex;

static NtSetInformationProcess_t NtSetInformationProcess_Orig = nullptr;
static NtSetInformationThread_t  NtSetInformationThread_Orig = nullptr;

static std::mutex g_ProcessedThreadsMutex;
static std::unordered_set<DWORD> g_ProcessedThreads;

static bool IsCurrentProcess(HANDLE hProcess) {
    return hProcess == GetCurrentProcess() || GetProcessId(hProcess) == GetCurrentProcessId();
}

// ---------------------------------------------------------------------------
// Disk cache enabler
// ---------------------------------------------------------------------------
static HANDLE WINAPI CreateFileA_Hook(LPCSTR file, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disposition, DWORD flags, HANDLE tmpl) {
    if (s_cacheCreateFileA) flags &= ~FILE_FLAG_NO_BUFFERING;
    return CreateFileA_Orig(file, access, share, sa, disposition, flags, tmpl);
}

static HANDLE WINAPI CreateFileW_Hook(LPCWSTR file, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disposition, DWORD flags, HANDLE tmpl) {
    if (s_cacheCreateFileW) flags &= ~FILE_FLAG_NO_BUFFERING;
    return CreateFileW_Orig(file, access, share, sa, disposition, flags, tmpl);
}

// ---------------------------------------------------------------------------
// Priority enforcement
// ---------------------------------------------------------------------------
static BOOL WINAPI SetPriorityClass_Hook(HANDLE hProcess, DWORD priority) {
    if (!IsCurrentProcess(hProcess)) return SetPriorityClass_Orig(hProcess, priority);
    return SetPriorityClass_Orig(hProcess, ResolveProcessPriorityClass(s_cpuPriorityLevel));
}

static BOOL WINAPI SetThreadPriority_Hook(HANDLE hThread, int priority) {
    return SetThreadPriority_Orig(hThread, ResolveThreadPriority(s_threadPriorityLevel));
}

static NTSTATUS NTAPI NtSetInformationProcess_Hook(HANDLE hProcess, PROCESS_INFORMATION_CLASS infoClass, PVOID data, ULONG length) {
    if (IsCurrentProcess(hProcess)) {
        if (infoClass == NtDefs::ProcessPowerThrottling) {
            if (data && length >= sizeof(PROCESS_POWER_THROTTLING_STATE)) {
                auto* state = reinterpret_cast<PROCESS_POWER_THROTTLING_STATE*>(data);
                state->ControlMask = POWER_THROTTLING_EXECUTION_SPEED;
                state->StateMask = 0;
            }
            return NtSetInformationProcess_Orig(hProcess, infoClass, data, length);
        }

        if (infoClass == NtDefs::ProcessPriorityClass) {
            if (data && length >= sizeof(PROCESS_PRIORITY_CLASS)) {
                reinterpret_cast<PPROCESS_PRIORITY_CLASS>(data)->PriorityClass = ResolveNtProcessPriorityClass(s_cpuPriorityLevel);
            }
            return NtSetInformationProcess_Orig(hProcess, infoClass, data, length);
        }

        if (infoClass == NtDefs::ProcessCpuPriority || infoClass == NtDefs::ProcessIoPriority) {
            return STATUS_SUCCESS;
        }
    }

    return NtSetInformationProcess_Orig(hProcess, infoClass, data, length);
}

static void DisableThreadPowerThrottling(HANDLE hThread) {
    THREAD_POWER_THROTTLING_STATE state{};
    state.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = POWER_THROTTLING_EXECUTION_SPEED;
    state.StateMask = 0;
    SetThreadInformation(hThread, ThreadPowerThrottling, &state, sizeof(state));
}

static void DisableProcessPowerThrottling() {
    PROCESS_POWER_THROTTLING_STATE state{};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = POWER_THROTTLING_EXECUTION_SPEED;
    state.StateMask = 0;
    SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
}

static void DisableMemoryPriority() {
    if (!NtSetInformationProcess_Orig) return;
    PROCESS_PAGE_PRIORITY_INFORMATION info{};
    info.PagePriority = 5;
    NtSetInformationProcess_Orig(GetCurrentProcess(), NtDefs::ProcessPagePriority, &info, sizeof(info));
}

static void ApplyIoPriority(HANDLE hThread) {
    if (!NtSetInformationThread_Orig) return;
    IO_PRIORITY_HINT io = s_highIoPriority ? IoPriorityHigh : IoPriorityNormal;
    NtSetInformationThread_Orig(hThread, NtDefs::ThreadIoPriority, &io, sizeof(io));
}

static void TrackThread(HANDLE hThread) {
    if (!hThread) return;
    DWORD tid = GetThreadId(hThread);

    {
        std::lock_guard<std::mutex> lock(g_ProcessedThreadsMutex);
        if (!g_ProcessedThreads.insert(tid).second) return;
    }

    DisableThreadPowerThrottling(hThread);
    ApplyIoPriority(hThread);
    SetThreadPriority_Orig(hThread, ResolveThreadPriority(s_threadPriorityLevel));
}

static void ApplyToExistingThreads() {
    const DWORD pid = GetCurrentProcessId();

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return;

    THREADENTRY32 te{};
    te.dwSize = sizeof(te);

    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid) continue;

            HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, te.th32ThreadID);
            if (!hThread) continue;

            TrackThread(hThread);
            CloseHandle(hThread);
        } while (Thread32Next(snap, &te));
    }

    CloseHandle(snap);
}

static void ApplyThreadPrioritiesNow() {
    std::unordered_set<DWORD> threadIds;
    {
        std::lock_guard<std::mutex> lock(g_ProcessedThreadsMutex);
        threadIds = g_ProcessedThreads;
    }

    for (DWORD tid : threadIds) {
        HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, tid);
        if (!hThread) continue;
        SetThreadPriority_Orig(hThread, ResolveThreadPriority(s_threadPriorityLevel));
        ApplyIoPriority(hThread);
        CloseHandle(hThread);
    }
}

static void ApplyCpuPriorityNow() {
    SetPriorityClass_Orig(GetCurrentProcess(), ResolveProcessPriorityClass(s_cpuPriorityLevel));
}

static HANDLE WINAPI CreateThread_Hook(LPSECURITY_ATTRIBUTES sa, SIZE_T stackSize, LPTHREAD_START_ROUTINE start, LPVOID param, DWORD flags, LPDWORD threadId) {
    HANDLE hThread = CreateThread_Orig(sa, stackSize, start, param, flags, threadId);
    TrackThread(hThread);
    return hThread;
}

static uintptr_t __stdcall _beginthreadex_Hook(void* security, unsigned stackSize, unsigned(__stdcall* startAddr)(void*), void* arglist, unsigned initFlag, unsigned* threadAddr) {
    uintptr_t hThread = _beginthreadex_Orig(security, stackSize, startAddr, arglist, initFlag, threadAddr);
    TrackThread(reinterpret_cast<HANDLE>(hThread));
    return hThread;
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------
static void DrawMenu(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx) {
    host->Text(ctx, "Disk cache");
    if (host->Checkbox(ctx, "Enable for CreateFileA", &s_cacheCreateFileA)) {
        host->SetConfigBool(ctx, "CacheCreateFileA", kCommentCacheA, s_cacheCreateFileA);
    }
    if (host->Checkbox(ctx, "Enable for CreateFileW", &s_cacheCreateFileW)) {
        host->SetConfigBool(ctx, "CacheCreateFileW", kCommentCacheW, s_cacheCreateFileW);
    }

    host->Separator(ctx);
    host->Text(ctx, "Priorities");

    int cpuLevel = s_cpuPriorityLevel;
    if (host->SliderInt(ctx, "CPU priority (0=Normal, 1=Medium, 2=High)", &cpuLevel, 0, 2)) {
        s_cpuPriorityLevel = Clamp3(cpuLevel);
        host->SetConfigInt(ctx, "CPUPriority", kCommentCpuPriority, s_cpuPriorityLevel);
        ApplyCpuPriorityNow();
    }
    host->Text(ctx, "Current: %s", kPriorityLevelNames[s_cpuPriorityLevel]);

    int threadLevel = s_threadPriorityLevel;
    if (host->SliderInt(ctx, "Thread priority (0=Normal, 1=Medium, 2=High)", &threadLevel, 0, 2)) {
        s_threadPriorityLevel = Clamp3(threadLevel);
        host->SetConfigInt(ctx, "ThreadPriority", kCommentThreadPriority, s_threadPriorityLevel);
        ApplyThreadPrioritiesNow();
    }
    host->Text(ctx, "Current: %s", kPriorityLevelNames[s_threadPriorityLevel]);

    if (host->Checkbox(ctx, "High I/O priority", &s_highIoPriority)) {
        host->SetConfigBool(ctx, "HighIOPriority", kCommentHighIo, s_highIoPriority);
        ApplyThreadPrioritiesNow();
    }
}

static void OnCommand(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx, const char* command) {
    if (!command) return;

    if (strcmp(command, "reapply") == 0) {
        ApplyCpuPriorityNow();
        ApplyThreadPrioritiesNow();
        host->Log(ctx, "Re-applied CPU/thread/I-O priority settings");
    }
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
static bool CreateAndEnableHook(void* target, void* detour, void** original) {
    if (!target) return false;
    if (MH_CreateHook(target, detour, original) != MH_OK) return false;
    return MH_EnableHook(target) == MH_OK;
}

static bool InstallHooks() {
    if (MH_Initialize() != MH_OK) return false;

    if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
        NtSetInformationProcess_Orig = reinterpret_cast<NtSetInformationProcess_t>(GetProcAddress(ntdll, "NtSetInformationProcess"));
        NtSetInformationThread_Orig = reinterpret_cast<NtSetInformationThread_t>(GetProcAddress(ntdll, "NtSetInformationThread"));
    }

    CreateAndEnableHook(reinterpret_cast<void*>(CreateFileA), reinterpret_cast<void*>(&CreateFileA_Hook), reinterpret_cast<void**>(&CreateFileA_Orig));
    CreateAndEnableHook(reinterpret_cast<void*>(CreateFileW), reinterpret_cast<void*>(&CreateFileW_Hook), reinterpret_cast<void**>(&CreateFileW_Orig));
    CreateAndEnableHook(reinterpret_cast<void*>(SetPriorityClass), reinterpret_cast<void*>(&SetPriorityClass_Hook), reinterpret_cast<void**>(&SetPriorityClass_Orig));
    CreateAndEnableHook(reinterpret_cast<void*>(SetThreadPriority), reinterpret_cast<void*>(&SetThreadPriority_Hook), reinterpret_cast<void**>(&SetThreadPriority_Orig));
    CreateAndEnableHook(reinterpret_cast<void*>(CreateThread), reinterpret_cast<void*>(&CreateThread_Hook), reinterpret_cast<void**>(&CreateThread_Orig));
    CreateAndEnableHook(reinterpret_cast<void*>(_beginthreadex), reinterpret_cast<void*>(&_beginthreadex_Hook), reinterpret_cast<void**>(&_beginthreadex_Orig));

    if (NtSetInformationProcess_Orig) {
        CreateAndEnableHook(reinterpret_cast<void*>(NtSetInformationProcess_Orig), reinterpret_cast<void*>(&NtSetInformationProcess_Hook), reinterpret_cast<void**>(&NtSetInformationProcess_Orig));
    }

    return true;
}

extern "C" __declspec(dllexport) bool ModLoader_InitPlugin(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx, ModLoaderPluginInfo* outInfo) {
    if (host->apiVersion != MODLOADER_PLUGIN_API_VERSION) return false;

    s_cacheCreateFileA = host->GetConfigBool(ctx, "CacheCreateFileA", kCommentCacheA, true);
    s_cacheCreateFileW = host->GetConfigBool(ctx, "CacheCreateFileW", kCommentCacheW, true);
    s_cpuPriorityLevel = Clamp3(host->GetConfigInt(ctx, "CPUPriority", kCommentCpuPriority, 2));
    s_threadPriorityLevel = Clamp3(host->GetConfigInt(ctx, "ThreadPriority", kCommentThreadPriority, 2));
    s_highIoPriority = host->GetConfigBool(ctx, "HighIOPriority", kCommentHighIo, true);

    if (!InstallHooks()) {
        host->Log(ctx, "Failed to install performance hooks (MinHook init or a hook creation failed)");
        return false;
    }

    DisableProcessPowerThrottling();
    DisableMemoryPriority();
    ApplyCpuPriorityNow();
    ApplyToExistingThreads();

    outInfo->name = "Performance Tweaks";
    outInfo->version = "2.0.0";
    outInfo->author = nullptr;
    outInfo->DrawMenu = &DrawMenu;
    outInfo->OnCommand = &OnCommand;

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
