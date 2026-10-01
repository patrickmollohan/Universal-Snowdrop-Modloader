#pragma once

#include "globals.hpp"
#include "minhook.hpp"
#include "plugin_api.hpp"
#include "plugins.hpp"
#include "settings.hpp"
#include "utilities.hpp"

#include <cstdarg>
#include <cstdio>
#include <d3d12.h>
#include <dxgi1_4.h>

#include "../lib/ImGui/imgui.h"
#include "../lib/ImGui/backends/imgui_impl_win32.h"
#include "../lib/ImGui/backends/imgui_impl_dx12.h"

class GUI {
public:
    static bool LoadGUI();
    static void UnloadGUI();

    static bool Visible;

    static void* GetProc(const char* name);

private:
    static constexpr UINT BackBufferCount = DXGI_MAX_SWAP_CHAIN_BUFFERS;
    static constexpr int MaxInitFailures = 3;

    struct FrameContext {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    };

    struct SwapChainQueue {
        void* swapChain = nullptr;
        HWND window = nullptr;
        ID3D12CommandQueue* queue = nullptr;
    };
    static std::vector<SwapChainQueue> swapChainQueues;
    static std::mutex swapChainQueuesMutex;

    static std::atomic<bool> initialised;
    static bool disabled;
    static int initFailures;
    static void* unknownQueueSwapChain;

    static std::recursive_mutex stateMutex;
    static std::recursive_mutex imguiMutex;
    static ID3D12Device* device;
    static ID3D12CommandQueue* commandQueue;
    static ID3D12GraphicsCommandList* commandList;
    static ID3D12DescriptorHeap* rtvDescriptorHeap;
    static ID3D12DescriptorHeap* srvDescriptorHeap;
    static FrameContext frameContexts[BackBufferCount];
    static UINT frameCount;
    static UINT64 renderCounter;
    static DXGI_FORMAT backBufferFormat;

    static ID3D12Fence* fence;
    static HANDLE fenceEvent;
    static UINT64 fenceValue;
    static UINT64 frameFenceValues[BackBufferCount];

    static bool imguiContextCreated;
    static bool imguiWin32Ready;
    static bool imguiDx12Ready;

    static IDXGISwapChain3* activeSwapChain;
    static HWND gameWindow;
    static WNDPROC originalWndProc;

public:
    static void Host_SetPluginInfo(ModLoaderPluginCtx* ctx, const char* name, const char* version, const char* author);
    static void Host_SetDrawMenuCallback(ModLoaderPluginCtx* ctx, ModLoaderDrawMenuFn fn);
    static void Host_SetCommandCallback(ModLoaderPluginCtx* ctx, ModLoaderCommandFn fn);
    static void Host_Text(ModLoaderPluginCtx* ctx, const char* fmt, ...);
    static void Host_TextWrapped(ModLoaderPluginCtx* ctx, const char* fmt, ...);
    static bool Host_Checkbox(ModLoaderPluginCtx* ctx, const char* label, bool* value);
    static bool Host_SliderInt(ModLoaderPluginCtx* ctx, const char* label, int* value, int min, int max);
    static bool Host_SliderFloat(ModLoaderPluginCtx* ctx, const char* label, float* value, float min, float max);
    static bool Host_InputText(ModLoaderPluginCtx* ctx, const char* label, char* buf, size_t bufSize);
    static bool Host_Button(ModLoaderPluginCtx* ctx, const char* label);
    static void Host_Separator(ModLoaderPluginCtx* ctx);
    static bool Host_GetConfigBool(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool defaultValue);
    static int  Host_GetConfigInt(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int defaultValue);
    static void Host_SetConfigBool(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool value);
    static void Host_SetConfigInt(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int value);
    static bool Host_SendCommand(ModLoaderPluginCtx* ctx, const char* targetPlugin, const char* command);
    static void Host_Log(ModLoaderPluginCtx* ctx, const char* fmt, ...);
    static uintptr_t Host_FindPattern(ModLoaderPluginCtx* ctx, const char* pattern);
    static ModLoaderPatch* Host_CreatePatch(ModLoaderPluginCtx* ctx, const char* pattern, size_t offset, const uint8_t* bytes, size_t size);
    static ModLoaderPatch* Host_CreatePatchAt(ModLoaderPluginCtx* ctx, uintptr_t address, const uint8_t* bytes, size_t size);
    static bool Host_SetPatchEnabled(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch, bool enabled);
    static bool Host_IsPatchEnabled(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch);
    static uintptr_t Host_GetPatchAddress(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch);
    static void Host_DestroyPatch(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch);

private:
    static void InitialiseImGui(IDXGISwapChain3* swapChain);
    static void ShutdownGraphics();
    static void RenderFrame(IDXGISwapChain3* swapChain);
    static void DrawMenu();

    static void OnPresent(IDXGISwapChain3* swapChain, UINT flags);
    static bool GuardedOnPresent(IDXGISwapChain3* swapChain, UINT flags);
    static void PollToggleKey();
    static bool SwapChainStillMatches(IDXGISwapChain3* swapChain);

    static void RegisterSwapChain(IUnknown* queueOrDevice, IUnknown* swapChain, HWND window);
    static bool GuardedRegisterSwapChain(IUnknown* queueOrDevice, IUnknown* swapChain, HWND window);
    static ID3D12CommandQueue* FindQueueFor(IUnknown* swapChain);
    static void ReleaseRegisteredQueues();

    static void WaitForGpu();
    static bool WaitForFrame(UINT index);

    static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain3*, UINT, UINT);
    static Present_t originalPresent;
    static HRESULT STDMETHODCALLTYPE HookedPresent(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags);

    using Present1_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain3*, UINT, UINT, const DXGI_PRESENT_PARAMETERS*);
    static Present1_t originalPresent1;
    static HRESULT STDMETHODCALLTYPE HookedPresent1(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags, const DXGI_PRESENT_PARAMETERS* params);

    using CreateSwapChain_t = HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
    static CreateSwapChain_t originalCreateSwapChain;
    static HRESULT STDMETHODCALLTYPE HookedCreateSwapChain(IDXGIFactory* factory, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** swapChain);

    using CreateSwapChainForHwnd_t = HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory2*, IUnknown*, HWND, const DXGI_SWAP_CHAIN_DESC1*, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**);
    static CreateSwapChainForHwnd_t originalCreateSwapChainForHwnd;
    static HRESULT STDMETHODCALLTYPE HookedCreateSwapChainForHwnd(IDXGIFactory2* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreenDesc, IDXGIOutput* restrictToOutput, IDXGISwapChain1** swapChain);

    static bool FindTargetFunctions(void** presentFn, void** present1Fn, void** createSwapChainFn, void** createSwapChainForHwndFn);
};
