#pragma once

#include "globals.hpp"

#include <d3d12.h>
#include <dxgi1_4.h>

class GUI {
public:
    static bool Load();
    static void Unload();

    static bool Visible;

private:
    static constexpr UINT ToggleKey = VK_INSERT;
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
