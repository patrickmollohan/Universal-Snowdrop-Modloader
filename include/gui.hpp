#pragma once

#include "globals.hpp"
#include "plugin_api.hpp"

#include <d3d12.h>
#include <dxgi1_4.h>

class GUI {
public:
    static bool Load();
    static void Unload();

    static bool Visible;

    static const ModLoaderHostAPI* GetPluginHostAPI();

private:
    static constexpr UINT BackBufferCount = 8;

    struct FrameContext {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        ID3D12Resource* backBuffer = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    };

    static bool initialised;

    static ID3D12Device* device;
    static ID3D12CommandQueue* commandQueue;
    static ID3D12GraphicsCommandList* commandList;
    static ID3D12DescriptorHeap* rtvDescriptorHeap;
    static ID3D12DescriptorHeap* srvDescriptorHeap;
    static FrameContext frameContexts[BackBufferCount];
    static UINT frameCount;

    static HWND gameWindow;
    static WNDPROC originalWndProc;

public:
    static void Host_Text(ModLoaderPluginCtx* ctx, const char* fmt, ...);
    static bool Host_Checkbox(ModLoaderPluginCtx* ctx, const char* label, bool* value);
    static bool Host_SliderInt(ModLoaderPluginCtx* ctx, const char* label, int* value, int min, int max);
    static bool Host_SliderFloat(ModLoaderPluginCtx* ctx, const char* label, float* value, float min, float max);
    static bool Host_InputText(ModLoaderPluginCtx* ctx, const char* label, char* buf, size_t bufSize);
    static bool Host_Button(ModLoaderPluginCtx* ctx, const char* label);
    static void Host_Separator(ModLoaderPluginCtx* ctx);
    static bool Host_GetConfigBool(ModLoaderPluginCtx* ctx, const char* key, bool defaultValue);
    static int  Host_GetConfigInt(ModLoaderPluginCtx* ctx, const char* key, int defaultValue);
    static void Host_SetConfigBool(ModLoaderPluginCtx* ctx, const char* key, bool value);
    static void Host_SetConfigInt(ModLoaderPluginCtx* ctx, const char* key, int value);
    static bool Host_SendCommand(ModLoaderPluginCtx* ctx, const char* targetPlugin, const char* command);
    static void Host_Log(ModLoaderPluginCtx* ctx, const char* fmt, ...);

private:
    static void InitialiseImGui(IDXGISwapChain3* swapChain);
    static void CreateRenderTargets(IDXGISwapChain3* swapChain);
    static void ReleaseRenderTargets();
    static void RenderFrame(IDXGISwapChain3* swapChain);
    static void DrawMenu();

    static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain3*, UINT, UINT);
    static Present_t originalPresent;
    static HRESULT STDMETHODCALLTYPE HookedPresent(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags);

    using ResizeBuffers_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    static ResizeBuffers_t originalResizeBuffers;
    static HRESULT STDMETHODCALLTYPE HookedResizeBuffers(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

    using ExecuteCommandLists_t = void(STDMETHODCALLTYPE*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
    static ExecuteCommandLists_t originalExecuteCommandLists;
    static void STDMETHODCALLTYPE HookedExecuteCommandLists(ID3D12CommandQueue* queue, UINT numCommandLists, ID3D12CommandList* const* commandLists);

    static bool FindTargetFunctions(void** presentFn, void** resizeBuffersFn, void** executeCommandListsFn);
};
