#include "pch.hpp"
#include "gui.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <intrin.h>
#include <string>
#include <type_traits>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool GUI::Visible = false;
std::atomic<bool> GUI::initialised{ false };
bool GUI::disabled = false;
int GUI::initFailures = 0;
void* GUI::unknownQueueSwapChain = nullptr;

std::vector<GUI::SwapChainQueue> GUI::swapChainQueues;
std::mutex GUI::swapChainQueuesMutex;
std::recursive_mutex GUI::stateMutex;
std::recursive_mutex GUI::imguiMutex;

ID3D12Device* GUI::device = nullptr;
ID3D12CommandQueue* GUI::commandQueue = nullptr;
ID3D12GraphicsCommandList* GUI::commandList = nullptr;
ID3D12DescriptorHeap* GUI::rtvDescriptorHeap = nullptr;
ID3D12DescriptorHeap* GUI::srvDescriptorHeap = nullptr;
GUI::FrameContext GUI::frameContexts[GUI::BackBufferCount]{};
UINT GUI::frameCount = 0;
UINT64 GUI::renderCounter = 0;
DXGI_FORMAT GUI::backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

ID3D12Fence* GUI::fence = nullptr;
HANDLE GUI::fenceEvent = nullptr;
UINT64 GUI::fenceValue = 0;
UINT64 GUI::frameFenceValues[GUI::BackBufferCount]{};

bool GUI::imguiContextCreated = false;
bool GUI::imguiWin32Ready = false;
bool GUI::imguiDx12Ready = false;

IDXGISwapChain3* GUI::activeSwapChain = nullptr;
HWND GUI::gameWindow = nullptr;
WNDPROC GUI::originalWndProc = nullptr;

std::atomic<GUI::CursorMode> GUI::cursorMode{ GUI::CursorMode::Undecided };

GUI::SetCursorPosFn GUI::originalSetCursorPos = nullptr;
GUI::GetCursorPosFn GUI::originalGetCursorPos = nullptr;
GUI::ClipCursorFn GUI::originalClipCursor = nullptr;
GUI::GetRawInputDataFn GUI::originalGetRawInputData = nullptr;
GUI::ShowCursorFn GUI::originalShowCursor = nullptr;
GUI::SetCursorFn GUI::originalSetCursor = nullptr;
bool GUI::cursorHooksInstalled = false;
std::atomic<bool> GUI::cursorCaptured{ false };
std::mutex GUI::cursorMutex;
POINT GUI::gameCursorPos{};
bool GUI::hasGameCursorPos = false;
bool GUI::gameMovedCursorWhileCaptured = false;
std::atomic<ULONGLONG> GUI::lastGameSetCursorPosTick{ 0 };
POINT GUI::savedMenuClientPos{};
bool GUI::hasSavedMenuPos = false;
RECT GUI::gameClipRect{};
bool GUI::hasGameClipRect = false;
HCURSOR GUI::gameCursor = nullptr;
bool GUI::hasGameCursor = false;
int GUI::osCursorAdded = 0;
int GUI::showCountAtCapture = 0;
int GUI::gameShowCount = 0;
RECT GUI::confineRect{};
bool GUI::hasConfineRect = false;

bool GUI::cursorOverrideActive = false;
bool GUI::savedCursorVisible = false;
HCURSOR GUI::savedCursorHandle = nullptr;
bool GUI::savedClipValid = false;
bool GUI::savedClipWasActive = false;
RECT GUI::savedClipRect{};
HWND GUI::savedCaptureWindow = nullptr;

GUI::Present_t GUI::originalPresent = nullptr;
GUI::Present1_t GUI::originalPresent1 = nullptr;
GUI::CreateSwapChain_t GUI::originalCreateSwapChain = nullptr;
GUI::CreateSwapChainForHwnd_t GUI::originalCreateSwapChainForHwnd = nullptr;

namespace {
    constexpr DWORD kGpuWaitTimeoutMs = 2000;

    thread_local bool t_inPresentHook = false;

    struct PresentScope {
        bool owner;
        PresentScope() : owner(!t_inPresentHook) { if (owner) t_inPresentHook = true; }
        ~PresentScope() { if (owner) t_inPresentHook = false; }
    };

    DXGI_FORMAT ResolveRtvFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        default:                                return format;
        }
    }

    bool ProcessHasForeground() {
        DWORD pid = 0;
        GetWindowThreadProcessId(GetForegroundWindow(), &pid);
        return pid == GetCurrentProcessId();
    }
}

bool GUI::FindTargetFunctions(void** presentFn, void** present1Fn, void** createSwapChainFn, void** createSwapChainForHwndFn) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"ModloaderDummyWndClass";
    RegisterClassExW(&wc);

    HWND dummyWindow = CreateWindowExW(0, wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    if (!dummyWindow) {
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return false;
    }

    bool success = false;
    ID3D12Device* dummyDevice = nullptr;
    ID3D12CommandQueue* dummyQueue = nullptr;
    IDXGIFactory4* dummyFactory = nullptr;
    IDXGISwapChain1* dummySwapChain = nullptr;

    do {
        if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dummyDevice)))) break;

        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        if (FAILED(dummyDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&dummyQueue)))) break;

        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dummyFactory)))) break;

        DXGI_SWAP_CHAIN_DESC1 swapDesc{};
        swapDesc.BufferCount = 2;
        swapDesc.Width = 100;
        swapDesc.Height = 100;
        swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapDesc.SampleDesc.Count = 1;

        if (FAILED(dummyFactory->CreateSwapChainForHwnd(dummyQueue, dummyWindow, &swapDesc, nullptr, nullptr, &dummySwapChain))) break;

        void** swapChainVTable = *reinterpret_cast<void***>(dummySwapChain);
        void** factoryVTable = *reinterpret_cast<void***>(dummyFactory);

        *presentFn = swapChainVTable[8];
        *present1Fn = swapChainVTable[22];
        *createSwapChainFn = factoryVTable[10];
        *createSwapChainForHwndFn = factoryVTable[15];

        success = true;
    } while (false);

    if (dummySwapChain) dummySwapChain->Release();
    if (dummyFactory) dummyFactory->Release();
    if (dummyQueue) dummyQueue->Release();
    if (dummyDevice) dummyDevice->Release();

    DestroyWindow(dummyWindow);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return success;
}

bool GUI::LoadGUI() {
    if (!Settings::EnableGUI) return false;

    void* presentFn = nullptr;
    void* present1Fn = nullptr;
    void* createSwapChainFn = nullptr;
    void* createSwapChainForHwndFn = nullptr;

    if (!FindTargetFunctions(&presentFn, &present1Fn, &createSwapChainFn, &createSwapChainForHwndFn)) {
        MessageBoxA(NULL, "Failed to locate DirectX 12 functions. The overlay menu will be disabled.", "Dank farrik!", MB_OK | MB_ICONWARNING);
        return false;
    }

    if (MH_CreateHook(presentFn, reinterpret_cast<LPVOID>(&HookedPresent), reinterpret_cast<LPVOID*>(&originalPresent)) != MH_OK ||
        MH_CreateHook(createSwapChainFn, reinterpret_cast<LPVOID>(&HookedCreateSwapChain), reinterpret_cast<LPVOID*>(&originalCreateSwapChain)) != MH_OK ||
        MH_CreateHook(createSwapChainForHwndFn, reinterpret_cast<LPVOID>(&HookedCreateSwapChainForHwnd), reinterpret_cast<LPVOID*>(&originalCreateSwapChainForHwnd)) != MH_OK) {
        MessageBoxA(NULL, "Failed to hook DirectX 12 functions. The overlay menu will be disabled.", "Dank farrik!", MB_OK | MB_ICONWARNING);
        return false;
    }

    if (MH_CreateHook(present1Fn, reinterpret_cast<LPVOID>(&HookedPresent1), reinterpret_cast<LPVOID*>(&originalPresent1)) != MH_OK) {
        originalPresent1 = nullptr;
    }

    MH_EnableHook(createSwapChainFn);
    MH_EnableHook(createSwapChainForHwndFn);

    return true;
}

void GUI::UnloadGUI() {
    UpdateCursor(false);
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex);
        ShutdownGraphics();
    }
    ReleaseRegisteredQueues();
}

void GUI::RegisterSwapChain(IUnknown* queueOrDevice, IUnknown* swapChain, HWND window) {
    if (!queueOrDevice || !swapChain) return;

    ID3D12CommandQueue* queue = nullptr;
    if (FAILED(queueOrDevice->QueryInterface(IID_PPV_ARGS(&queue))) || !queue) return;

    void* identity = swapChain;
    IUnknown* canonical = nullptr;
    if (SUCCEEDED(swapChain->QueryInterface(IID_PPV_ARGS(&canonical))) && canonical) {
        identity = canonical;
        canonical->Release();
    }

    const D3D12_COMMAND_QUEUE_DESC desc = queue->GetDesc();

    std::vector<ID3D12CommandQueue*> toRelease;
    {
        std::lock_guard<std::mutex> lock(swapChainQueuesMutex);
        for (auto it = swapChainQueues.begin(); it != swapChainQueues.end();) {
            if (it->swapChain == identity || (window && it->window == window)) {
                toRelease.push_back(it->queue);
                it = swapChainQueues.erase(it);
            } else {
                ++it;
            }
        }

        constexpr size_t kMaxEntries = 8;
        while (swapChainQueues.size() >= kMaxEntries) {
            toRelease.push_back(swapChainQueues.front().queue);
            swapChainQueues.erase(swapChainQueues.begin());
        }

        SwapChainQueue entry;
        entry.swapChain = identity;
        entry.window = window;
        entry.queue = queue;
        swapChainQueues.push_back(entry);
    }

    for (ID3D12CommandQueue* stale : toRelease) stale->Release();
}

bool GUI::GuardedRegisterSwapChain(IUnknown* queueOrDevice, IUnknown* swapChain, HWND window) {
    RegisterSwapChain(queueOrDevice, swapChain, window);
    return true;
}

ID3D12CommandQueue* GUI::FindQueueFor(IUnknown* swapChain) {
    void* identity = swapChain;
    IUnknown* canonical = nullptr;
    if (SUCCEEDED(swapChain->QueryInterface(IID_PPV_ARGS(&canonical))) && canonical) {
        identity = canonical;
        canonical->Release();
    }

    std::lock_guard<std::mutex> lock(swapChainQueuesMutex);
    for (const auto& entry : swapChainQueues) {
        if (entry.swapChain == identity) {
            entry.queue->AddRef();
            return entry.queue;
        }
    }
    return nullptr;
}

void GUI::ReleaseRegisteredQueues() {
    std::vector<SwapChainQueue> entries;
    {
        std::lock_guard<std::mutex> lock(swapChainQueuesMutex);
        entries.swap(swapChainQueues);
    }
    for (auto& entry : entries) entry.queue->Release();
}

void GUI::WaitForGpu() {
    if (!commandQueue || !fence || !fenceEvent) return;

    const UINT64 value = ++fenceValue;
    if (FAILED(commandQueue->Signal(fence, value))) return;

    if (fence->GetCompletedValue() < value && SUCCEEDED(fence->SetEventOnCompletion(value, fenceEvent))) {
        WaitForSingleObject(fenceEvent, kGpuWaitTimeoutMs);
    }
}

bool GUI::WaitForFrame(UINT index) {
    const UINT64 target = frameFenceValues[index];
    if (target == 0 || fence->GetCompletedValue() >= target) return true;

    if (FAILED(fence->SetEventOnCompletion(target, fenceEvent))) return false;
    return WaitForSingleObject(fenceEvent, kGpuWaitTimeoutMs) == WAIT_OBJECT_0;
}

void GUI::ShutdownGraphics() {
    UpdateCursor(false);
    initialised = false;
    activeSwapChain = nullptr;

    WaitForGpu();

    if (gameWindow && originalWndProc && IsWindow(gameWindow)) {
        if (GetWindowLongPtrW(gameWindow, GWLP_WNDPROC) == reinterpret_cast<LONG_PTR>(&WndProcHook)) {
            SetWindowLongPtrW(gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
            originalWndProc = nullptr;
        }
    }

    {
        std::lock_guard<std::recursive_mutex> imguiLock(imguiMutex);
        if (imguiDx12Ready) { ImGui_ImplDX12_Shutdown(); imguiDx12Ready = false; }
        if (imguiWin32Ready) { ImGui_ImplWin32_Shutdown(); imguiWin32Ready = false; }
        if (imguiContextCreated) { ImGui::DestroyContext(); imguiContextCreated = false; }
    }

    for (UINT i = 0; i < BackBufferCount; ++i) {
        if (frameContexts[i].commandAllocator) {
            frameContexts[i].commandAllocator->Release();
        }
        frameContexts[i] = {};
        frameFenceValues[i] = 0;
    }

    if (commandList) { commandList->Release(); commandList = nullptr; }
    if (rtvDescriptorHeap) { rtvDescriptorHeap->Release(); rtvDescriptorHeap = nullptr; }
    if (srvDescriptorHeap) { srvDescriptorHeap->Release(); srvDescriptorHeap = nullptr; }
    if (fence) { fence->Release(); fence = nullptr; }
    if (fenceEvent) { CloseHandle(fenceEvent); fenceEvent = nullptr; }
    if (commandQueue) { commandQueue->Release(); commandQueue = nullptr; }
    if (device) { device->Release(); device = nullptr; }

    fenceValue = 0;
    frameCount = 0;
    renderCounter = 0;
}

void GUI::InitialiseImGui(IDXGISwapChain3* swapChain) {
    auto fail = [](const char* reason) {
        ShutdownGraphics();
        Visible = false;
        if (++initFailures >= MaxInitFailures) {
            disabled = true;
        }
    };

    ID3D12CommandQueue* queue = FindQueueFor(swapChain);
    if (!queue) {
        if (unknownQueueSwapChain != swapChain) {
            unknownQueueSwapChain = swapChain;
        }
        Visible = false;
        return;
    }
    commandQueue = queue;

    ID3D12Device* queueDevice = nullptr;
    if (FAILED(queue->GetDevice(IID_PPV_ARGS(&queueDevice))) || !queueDevice) { fail("could not get the queue's device"); return; }
    device = queueDevice;

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    if (FAILED(swapChain->GetDesc(&swapDesc))) { fail("GetDesc failed"); return; }
    if (!swapDesc.OutputWindow) { fail("swap chain has no HWND"); return; }
    if (swapDesc.BufferCount == 0 || swapDesc.BufferCount > BackBufferCount) { fail("unsupported back buffer count"); return; }

    gameWindow = swapDesc.OutputWindow;
    frameCount = swapDesc.BufferCount;
    backBufferFormat = ResolveRtvFormat(swapDesc.BufferDesc.Format);

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = frameCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap)))) { fail("RTV heap"); return; }

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap)))) { fail("SRV heap"); return; }

    const UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < frameCount; ++i) {
        frameContexts[i].rtvHandle = rtvHandle;
        rtvHandle.ptr += rtvDescriptorSize;

        if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameContexts[i].commandAllocator)))) { fail("command allocator"); return; }
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameContexts[0].commandAllocator, nullptr, IID_PPV_ARGS(&commandList)))) { fail("command list"); return; }
    commandList->Close();

    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) { fail("fence"); return; }
    fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) { fail("fence event"); return; }
    fenceValue = 0;
    renderCounter = 0;

    {
        std::lock_guard<std::recursive_mutex> imguiLock(imguiMutex);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imguiContextCreated = true;

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        ImGui::StyleColorsDark();

        if (!ImGui_ImplWin32_Init(gameWindow)) { fail("ImGui Win32 backend"); return; }
        imguiWin32Ready = true;

        if (!ImGui_ImplDX12_Init(device, static_cast<int>(frameCount), backBufferFormat, srvDescriptorHeap,
            srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart())) { fail("ImGui DX12 backend"); return; }
        imguiDx12Ready = true;
    }

    InstallWindowHook(gameWindow);

    activeSwapChain = swapChain;
    initialised = true;
    initFailures = 0;
}

bool GUI::SwapChainStillMatches(IDXGISwapChain3* swapChain) {
    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(&desc))) return false;

    return desc.BufferCount == frameCount &&
        desc.OutputWindow == gameWindow &&
        ResolveRtvFormat(desc.BufferDesc.Format) == backBufferFormat;
}

void GUI::RenderFrame(IDXGISwapChain3* swapChain) {
    if (FAILED(device->GetDeviceRemovedReason())) {
        disabled = true;
        ShutdownGraphics();
        return;
    }

    const UINT ring = static_cast<UINT>(renderCounter % frameCount);
    FrameContext& frame = frameContexts[ring];
    if (!WaitForFrame(ring)) return;

    const UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    ID3D12Resource* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(backBufferIndex, IID_PPV_ARGS(&backBuffer))) || !backBuffer) return;

    struct BackBufferRelease {
        ID3D12Resource*& resource;
        ~BackBufferRelease() { if (resource) { resource->Release(); resource = nullptr; } }
    } backBufferRelease{ backBuffer };

    {
        std::lock_guard<std::recursive_mutex> imguiLock(imguiMutex);

        PollMouseButtons();

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        DrawMenu();

        ImGui::Render();
    }

    if (FAILED(frame.commandAllocator->Reset())) return;
    if (FAILED(commandList->Reset(frame.commandAllocator, nullptr))) return;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = backBufferFormat;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    device->CreateRenderTargetView(backBuffer, &rtvDesc, frame.rtvHandle);

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = backBuffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandList->ResourceBarrier(1, &barrier);

    commandList->OMSetRenderTargets(1, &frame.rtvHandle, FALSE, nullptr);
    commandList->SetDescriptorHeaps(1, &srvDescriptorHeap);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    ++renderCounter;

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &barrier);

    if (FAILED(commandList->Close())) return;

    ID3D12CommandList* lists[] = { commandList };
    commandQueue->ExecuteCommandLists(1, lists);

    const UINT64 value = ++fenceValue;
    if (SUCCEEDED(commandQueue->Signal(fence, value))) {
        frameFenceValues[ring] = value;
    }
}

void GUI::Host_SetPluginInfo(ModLoaderPluginCtx* ctx, const char* name, const char* version, const char* author) {
    if (!ctx) return;
    ctx->name = name ? name : "";
    ctx->version = version ? version : "";
    ctx->author = author ? author : "";
}

void GUI::Host_SetDrawMenuCallback(ModLoaderPluginCtx* ctx, ModLoaderDrawMenuFn fn) {
    if (ctx) ctx->drawMenu = fn;
}

void GUI::Host_SetCommandCallback(ModLoaderPluginCtx* ctx, ModLoaderCommandFn fn) {
    if (ctx) ctx->onCommand = fn;
}

void GUI::Host_Text(ModLoaderPluginCtx*, const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    ImGui::TextWrapped("%s", buffer);
}

void GUI::Host_TextWrapped(ModLoaderPluginCtx*, const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    ImGui::TextWrapped("%s", buffer);
}

bool GUI::Host_Checkbox(ModLoaderPluginCtx*, const char* label, bool* value) {
    return ImGui::Checkbox(label, value);
}

static void DrawWrappedLabel(const char* label) {
    const char* end = strstr(label, "##");
    if (!end) end = label + strlen(label);
    if (end == label) return;

    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextUnformatted(label, end);
    ImGui::PopTextWrapPos();
}

bool GUI::Host_SliderInt(ModLoaderPluginCtx*, const char* label, int* value, int min, int max) {
    DrawWrappedLabel(label);
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::SliderInt((std::string("##") + label).c_str(), value, min, max);
}

bool GUI::Host_SliderFloat(ModLoaderPluginCtx*, const char* label, float* value, float min, float max) {
    DrawWrappedLabel(label);
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::SliderFloat((std::string("##") + label).c_str(), value, min, max);
}

bool GUI::Host_InputText(ModLoaderPluginCtx*, const char* label, char* buf, size_t bufSize) {
    DrawWrappedLabel(label);
    ImGui::SetNextItemWidth(-FLT_MIN);
    return ImGui::InputText((std::string("##") + label).c_str(), buf, bufSize);
}

bool GUI::Host_Button(ModLoaderPluginCtx*, const char* label) {
    return ImGui::Button(label);
}

void GUI::Host_Separator(ModLoaderPluginCtx*) {
    ImGui::Separator();
}

bool GUI::Host_GetConfigBool(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool defaultValue) {
    if (!ctx || !key) return defaultValue;
    return Utilities::SettingsParser::GetBoolean(ctx->configPath, kPluginConfigSection, key, defaultValue, comment);
}

int GUI::Host_GetConfigInt(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int defaultValue) {
    if (!ctx || !key) return defaultValue;
    return Utilities::SettingsParser::GetInt(ctx->configPath, kPluginConfigSection, key, defaultValue, comment);
}

void GUI::Host_SetConfigBool(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool value) {
    if (!ctx || !key) return;
    Utilities::SettingsParser::SetBoolean(ctx->configPath, kPluginConfigSection, key, value, comment);
}

void GUI::Host_SetConfigInt(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int value) {
    if (!ctx || !key) return;
    Utilities::SettingsParser::SetInt(ctx->configPath, kPluginConfigSection, key, value, comment);
}

bool GUI::Host_SendCommand(ModLoaderPluginCtx*, const char* targetPlugin, const char* command) {
    if (!targetPlugin || !command) return false;
    return Plugins::SendCommand(targetPlugin, command);
}

void GUI::Host_Log(ModLoaderPluginCtx* ctx, const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::string line = "[" + (ctx ? ctx->pluginId : std::string("Plugin")) + "] " + buffer;
    OutputDebugStringA((line + "\n").c_str());

    std::string logPath = (g_DllInfo.directory / "plugins.log").string();
    FILE* f = nullptr;
    if (fopen_s(&f, logPath.c_str(), "a") == 0 && f) {
        fprintf(f, "%s\n", line.c_str());
        fclose(f);
    }
}

uintptr_t GUI::Host_FindPattern(ModLoaderPluginCtx*, const char* pattern) {
    if (!pattern) return 0;
    return Utilities::Memory::FindPattern(pattern);
}

static MemoryPatch* FindOwnedPatch(ModLoaderPluginCtx* ctx, ModLoaderPatch* handle) {
    if (!ctx || !handle) return nullptr;
    for (auto& patch : ctx->patches) {
        if (patch.get() == reinterpret_cast<MemoryPatch*>(handle)) return patch.get();
    }
    return nullptr;
}

static ModLoaderPatch* RegisterPatch(ModLoaderPluginCtx* ctx, std::unique_ptr<MemoryPatch> patch) {
    if (!ctx || !patch) return nullptr;
    auto* handle = reinterpret_cast<ModLoaderPatch*>(patch.get());
    ctx->patches.push_back(std::move(patch));
    return handle;
}

ModLoaderPatch* GUI::Host_CreatePatch(ModLoaderPluginCtx* ctx, const char* pattern, size_t offset, const uint8_t* bytes, size_t size) {
    return RegisterPatch(ctx, Utilities::Memory::CreatePatch(pattern, offset, bytes, size));
}

ModLoaderPatch* GUI::Host_CreatePatchAt(ModLoaderPluginCtx* ctx, uintptr_t address, const uint8_t* bytes, size_t size) {
    return RegisterPatch(ctx, Utilities::Memory::CreatePatch(address, bytes, size));
}

bool GUI::Host_SetPatchEnabled(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch, bool enabled) {
    MemoryPatch* p = FindOwnedPatch(ctx, patch);
    return p && Utilities::Memory::SetPatchEnabled(*p, enabled);
}

bool GUI::Host_IsPatchEnabled(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch) {
    const MemoryPatch* p = FindOwnedPatch(ctx, patch);
    return p && p->enabled;
}

uintptr_t GUI::Host_GetPatchAddress(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch) {
    const MemoryPatch* p = FindOwnedPatch(ctx, patch);
    return p ? p->address : 0;
}

void GUI::Host_DestroyPatch(ModLoaderPluginCtx* ctx, ModLoaderPatch* patch) {
    MemoryPatch* p = FindOwnedPatch(ctx, patch);
    if (!p) return;

    Utilities::Memory::SetPatchEnabled(*p, false);
    std::erase_if(ctx->patches, [p](const auto& owned) { return owned.get() == p; });
}

void GUI::Host_TextDisabled(ModLoaderPluginCtx*, const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextDisabled("%s", buffer);
    ImGui::PopTextWrapPos();
}

void GUI::Host_SameLine(ModLoaderPluginCtx*) {
    ImGui::SameLine();
}

bool GUI::Host_ListBox(ModLoaderPluginCtx*, const char* label, int* currentItem, ModLoaderListItemFn getItem, void* userData, int itemCount, int heightInItems) {
    if (!label || !currentItem || !getItem) return false;

    DrawWrappedLabel(label);

    if (heightInItems < 0) heightInItems = itemCount < 7 ? itemCount : 7;
    const ImGuiStyle& style = ImGui::GetStyle();
    const float height = ImGui::GetTextLineHeightWithSpacing() * (static_cast<float>(heightInItems) + 0.25f) + style.FramePadding.y * 2.0f;

    bool changed = false;
    if (ImGui::BeginListBox((std::string("##") + label).c_str(), ImVec2(-FLT_MIN, height))) {
        ImGuiListClipper clipper;
        clipper.Begin(itemCount);
        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                const char* text = getItem(userData, i);
                ImGui::PushID(i);
                if (ImGui::Selectable(text ? text : "", *currentItem == i)) {
                    *currentItem = i;
                    changed = true;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndListBox();
    }
    return changed;
}

static bool SafeReadInt32(uintptr_t address, int32_t* out) {
    __try {
        *out = *reinterpret_cast<const volatile int32_t*>(address);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

uintptr_t GUI::Host_ResolveRelative(ModLoaderPluginCtx*, uintptr_t instructionAddress, size_t opcodeLength, size_t instructionLength) {
    if (!instructionAddress) return 0;

    int32_t rel = 0;
    if (!SafeReadInt32(instructionAddress + opcodeLength, &rel)) return 0;
    return instructionAddress + instructionLength + static_cast<intptr_t>(rel);
}

static ModLoaderHook* FindOwnedHook(ModLoaderPluginCtx* ctx, ModLoaderHook* handle) {
    if (!ctx || !handle) return nullptr;
    for (auto& hook : ctx->hooks) {
        if (hook.get() == handle) return hook.get();
    }
    return nullptr;
}

ModLoaderHook* GUI::Host_CreateHook(ModLoaderPluginCtx* ctx, uintptr_t target, void* detour, void** original) {
    if (!ctx || !target || !detour) return nullptr;

    for (const auto& hook : ctx->hooks) {
        if (hook->target == reinterpret_cast<LPVOID>(target)) return nullptr;
    }

    if (MH_CreateHook(reinterpret_cast<LPVOID>(target), detour, original) != MH_OK) return nullptr;

    auto hook = std::make_unique<ModLoaderHook>();
    hook->target = reinterpret_cast<LPVOID>(target);
    ModLoaderHook* handle = hook.get();
    ctx->hooks.push_back(std::move(hook));
    return handle;
}

bool GUI::Host_SetHookEnabled(ModLoaderPluginCtx* ctx, ModLoaderHook* hook, bool enabled) {
    ModLoaderHook* h = FindOwnedHook(ctx, hook);
    if (!h) return false;
    if (h->enabled == enabled) return true;

    const MH_STATUS status = enabled ? MH_EnableHook(h->target) : MH_DisableHook(h->target);
    if (status != MH_OK && status != MH_ERROR_ENABLED && status != MH_ERROR_DISABLED) return false;

    h->enabled = enabled;
    return true;
}

bool GUI::Host_IsHookEnabled(ModLoaderPluginCtx* ctx, ModLoaderHook* hook) {
    const ModLoaderHook* h = FindOwnedHook(ctx, hook);
    return h && h->enabled;
}

void GUI::Host_DestroyHook(ModLoaderPluginCtx* ctx, ModLoaderHook* hook) {
    ModLoaderHook* h = FindOwnedHook(ctx, hook);
    if (!h) return;

    MH_DisableHook(h->target);
    MH_RemoveHook(h->target);
    std::erase_if(ctx->hooks, [h](const auto& owned) { return owned.get() == h; });
}

#define MODLOADER_X_CHECK(ret, name, params) \
    static_assert(std::is_same_v<decltype(&GUI::Host_##name), ret (*) params>, \
                  "GUI::Host_" #name " does not match its declaration in MODLOADER_API");
MODLOADER_API(MODLOADER_X_CHECK)
#undef MODLOADER_X_CHECK

void* GUI::GetProc(const char* name) {
    if (!name) return nullptr;

#define MODLOADER_X_LOOKUP(ret, fname, params) \
    if (strcmp(name, #fname) == 0) return reinterpret_cast<void*>(&GUI::Host_##fname);
    MODLOADER_API(MODLOADER_X_LOOKUP)
#undef MODLOADER_X_LOOKUP

    return nullptr;
}

void GUI::DrawMenu() {
    ImGui::SetNextWindowSize(ImVec2(420, 340), ImGuiCond_FirstUseEver);
    ImGui::Begin("Universal Snowdrop Modloader", &Visible);

    ImGui::TextUnformatted(g_ExeInfo.filename.c_str());
    ImGui::TextDisabled("%s to toggle this menu", Settings::MenuToggleKeyName.c_str());
    if (cursorMode.load() == CursorMode::External) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        ImGui::TextWrapped("Another overlay (e.g. OptiScaler) controls the mouse cursor. If the mouse does nothing in-game, open that overlay's menu too.");
        ImGui::PopStyleColor();
    }
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const std::string configFileName = std::filesystem::path(Settings::GetConfigPath()).filename().string();
        ImGui::TextWrapped("Toggling these updates %s; mod/plugin loading itself only re-runs on the next launch.", configFileName.c_str());

        bool enableMods = Settings::EnableMods;
        if (ImGui::Checkbox("Enable mods", &enableMods)) {
            Settings::EnableMods = enableMods;
            Utilities::SettingsParser::SetBoolean(Settings::GetConfigPath(), "Settings", "EnableMods", enableMods);
        }

        bool enablePlugins = Settings::EnablePlugins;
        if (ImGui::Checkbox("Enable plugins", &enablePlugins)) {
            Settings::EnablePlugins = enablePlugins;
            Utilities::SettingsParser::SetBoolean(Settings::GetConfigPath(), "Settings", "EnablePlugins", enablePlugins);
        }

        bool enableGUI = Settings::EnableGUI;
        if (ImGui::Checkbox("Enable this menu", &enableGUI)) {
            Settings::EnableGUI = enableGUI;
            Utilities::SettingsParser::SetBoolean(Settings::GetConfigPath(), "Settings", "EnableGUI", enableGUI);
        }
    }

    if (ImGui::CollapsingHeader("Plugins", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& plugins = Plugins::GetLoadedPlugins();

        if (!Settings::EnablePlugins) {
            ImGui::TextDisabled("Plugin loading is disabled.");
        } else if (plugins.empty()) {
            ImGui::TextDisabled("No plugins found.");
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::TextWrapped("Enabling/disabling a plugin takes effect on the next launch.");
            ImGui::PopStyleColor();

            const float labelIndent = ImGui::GetTreeNodeToLabelSpacing();
            const float checkboxSize = ImGui::GetFrameHeight();

            for (int i = 0; i < static_cast<int>(plugins.size()); ++i) {
                LoadedPlugin& plugin = plugins[i];
                const bool isLoaded = plugin.module != nullptr;
                const bool hasMenu = isLoaded && plugin.apiInitialised && plugin.ctx->drawMenu != nullptr;

                std::string label = (plugin.apiInitialised && !plugin.ctx->name.empty()) ? plugin.ctx->name : plugin.fileName;
                if (plugin.enabled != isLoaded) {
                    label += plugin.enabled ? " (enabled on restart)" : " (disabled on restart)";
                }

                ImGui::PushID(i);

                const float rowStartX = ImGui::GetCursorPosX();
                const float checkboxX = rowStartX + ImGui::GetContentRegionAvail().x - checkboxSize;

                bool open = false;
                if (hasMenu) {
                    const std::string nodeLabel = label + "###plugin";
                    open = ImGui::TreeNodeEx(nodeLabel.c_str(), ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding);
                } else {
                    ImGui::AlignTextToFramePadding();
                    ImGui::Indent(labelIndent);
                    ImGui::TextUnformatted(label.c_str());
                    ImGui::Unindent(labelIndent);
                }

                ImGui::SameLine(checkboxX);
                bool enabled = plugin.enabled;
                if (ImGui::Checkbox("##enabled", &enabled)) {
                    plugin.enabled = enabled;
                    Settings::SetPluginEnabled(plugin.fileName, enabled);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s (applies on next launch)", plugin.enabled ? "Enabled" : "Disabled");
                }

                if (hasMenu && open) {
                    plugin.ctx->drawMenu(plugin.ctx.get());
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }
    }

    ImGui::End();
}

LRESULT CALLBACK GUI::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ServiceOsCursor();

    if (initialised.load() && Visible) {
        std::lock_guard<std::recursive_mutex> lock(imguiMutex);

        if (initialised.load() && imguiContextCreated) {
            const LRESULT handled = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

            ImGuiIO& io = ImGui::GetIO();

            const bool isKeyboardMsg = (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR ||
                msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP);
            const bool isMouseMsg = (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST);

            if (isKeyboardMsg && io.WantCaptureKeyboard) {
                return TRUE;
            }

            const CursorMode mode = cursorMode.load();

            if (mode == CursorMode::External) {
                if (msg == WM_SETCURSOR || isMouseMsg || msg == WM_INPUT) {
                    if (IsExternalWndProc(originalWndProc)) {
                        const LRESULT externalResult = CallWindowProcW(originalWndProc, hWnd, msg, wParam, lParam);
                        if (externalResult != 0) return externalResult;
                    }
                    return TRUE;
                }
            } else if (mode == CursorMode::Own) {
                if (msg == WM_SETCURSOR) {
                    if (handled) return TRUE;
                } else if (isMouseMsg) {
                    return TRUE;
                } else if (msg == WM_INPUT) {
                    RAWINPUTHEADER header{};
                    UINT size = sizeof(header);
                    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_HEADER, &header, &size, sizeof(RAWINPUTHEADER)) != static_cast<UINT>(-1)) {
                        const bool rawMouse = header.dwType == RIM_TYPEMOUSE;
                        const bool rawKeyboard = header.dwType == RIM_TYPEKEYBOARD;
                        if (rawMouse || (rawKeyboard && io.WantCaptureKeyboard)) {
                            return DefWindowProcW(hWnd, msg, wParam, lParam);
                        }
                    }
                }
            }
        }
    }

    return originalWndProc ? CallWindowProcW(originalWndProc, hWnd, msg, wParam, lParam) : DefWindowProcW(hWnd, msg, wParam, lParam);
}

void GUI::PollToggleKey() {
    static bool wasDown = false;

    const bool down = (GetAsyncKeyState(static_cast<int>(Settings::MenuToggleKey)) & 0x8000) != 0;
    if (down && !wasDown && ProcessHasForeground()) {
        Visible = !Visible;
    }
    wasDown = down;
}

// ---------------------------------------------------------------------------
// Cursor handling
// ---------------------------------------------------------------------------
namespace {
    bool SafeRead(const void* source, void* destination, size_t size) {
        __try {
            memcpy(destination, source, size);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    HMODULE ModuleFromAddress(const void* address) {
        HMODULE module = nullptr;
        if (!address || !GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(address), &module)) {
            return nullptr;
        }
        return module;
    }

    bool IsSystemModule(HMODULE module) {
        static const wchar_t* const names[] = { L"user32.dll", L"win32u.dll", L"ntdll.dll", L"kernelbase.dll", L"kernel32.dll" };
        for (const wchar_t* name : names) {
            if (module == GetModuleHandleW(name)) return true;
        }
        return false;
    }

    const uint8_t* ResolveLeadingJump(const uint8_t* function) {
        uint8_t b[16]{};
        if (!SafeRead(function, b, sizeof(b))) return nullptr;

        int32_t rel = 0;
        const uint8_t* target = nullptr;

        if (b[0] == 0xE9) {
            memcpy(&rel, b + 1, sizeof(rel));
            return function + 5 + rel;
        }
        if (b[0] == 0xEB) {
            return function + 2 + static_cast<int8_t>(b[1]);
        }
        if (b[0] == 0xFF && b[1] == 0x25) {
            memcpy(&rel, b + 2, sizeof(rel));
            return SafeRead(function + 6 + rel, &target, sizeof(target)) ? target : nullptr;
        }
        if (b[0] == 0x48 && b[1] == 0xFF && b[2] == 0x25) {
            memcpy(&rel, b + 3, sizeof(rel));
            return SafeRead(function + 7 + rel, &target, sizeof(target)) ? target : nullptr;
        }
        if (b[0] == 0x48 && b[1] == 0xB8 && b[10] == 0xFF && b[11] == 0xE0) {
            memcpy(&target, b + 2, sizeof(target));
            return target;
        }
        if (b[0] == 0x49 && b[1] == 0xBA && b[10] == 0x41 && b[11] == 0xFF && b[12] == 0xE2) {
            memcpy(&target, b + 2, sizeof(target));
            return target;
        }
        return nullptr;
    }

    struct ExportHook {
        bool hooked = false;
        HMODULE owner = nullptr;
    };

    ExportHook InspectExport(HMODULE user32, const char* name) {
        ExportHook result;

        const auto* code = reinterpret_cast<const uint8_t*>(GetProcAddress(user32, name));
        if (!code) return result;

        for (int hop = 0; hop < 6; ++hop) {
            const uint8_t* target = ResolveLeadingJump(code);
            if (!target) {
                result.hooked = hop > 0;
                return result;
            }

            if (HMODULE module = ModuleFromAddress(target)) {
                result.hooked = !IsSystemModule(module);
                result.owner = result.hooked ? module : nullptr;
                return result;
            }
            code = target;
        }

        result.hooked = true;
        return result;
    }

    bool IsIgnorableHooker(const std::wstring& moduleFileName) {
        static const wchar_t* const ignored[] = {
            L"gameoverlayrenderer64.dll", L"gameoverlayrenderer.dll",
            L"discordhook64.dll",
            L"rtsshooks64.dll", L"rtsshooks.dll",
        };

        std::wstring lower = moduleFileName;
        for (wchar_t& c : lower) c = static_cast<wchar_t>(towlower(c));
        for (const wchar_t* name : ignored) {
            if (lower == name) return true;
        }
        return false;
    }

    bool FindForeignCursorHook(std::string* hookerName) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32) return false;

        const HMODULE self = ModuleFromAddress(reinterpret_cast<const void*>(&InspectExport));

        for (const char* name : { "SetCursorPos", "ClipCursor", "GetCursorPos" }) {
            const ExportHook hook = InspectExport(user32, name);
            if (!hook.hooked || (hook.owner && hook.owner == self)) continue;

            std::string owner = "an unknown module";
            if (hook.owner) {
                wchar_t path[MAX_PATH]{};
                if (GetModuleFileNameW(hook.owner, path, MAX_PATH)) {
                    const wchar_t* file = wcsrchr(path, L'\\');
                    file = file ? file + 1 : path;
                    if (IsIgnorableHooker(file)) continue;

                    char narrow[MAX_PATH]{};
                    WideCharToMultiByte(CP_UTF8, 0, file, -1, narrow, sizeof(narrow), nullptr, nullptr);
                    owner = narrow;
                }
            }

            if (hookerName) *hookerName = std::string(name) + " is hooked by " + owner;
            return true;
        }
        return false;
    }
}

void GUI::DecideCursorMode() {
    std::string hooker;
    CursorMode mode;

    if (FindForeignCursorHook(&hooker)) {
        mode = CursorMode::External;
        OutputDebugStringA(("[USM] Cursor: " + hooker + "; leaving cursor control to it.\n").c_str());
    } else if (InstallCursorHooks()) {
        mode = CursorMode::Own;
        OutputDebugStringA("[USM] Cursor: no other cursor hooks found; using our own.\n");
    } else {
        mode = CursorMode::External;
        OutputDebugStringA("[USM] Cursor: could not install cursor hooks; unblocking input only.\n");
    }

    cursorMode.store(mode);
}

void GUI::UpdateCursor(bool menuOpen) {
    if (menuOpen && cursorMode.load() == CursorMode::Undecided) DecideCursorMode();

    switch (cursorMode.load()) {
    case CursorMode::Own:
        SetCursorCaptured(menuOpen);
        UpdateCursorConfinement();
        break;
    case CursorMode::External:
        if (menuOpen) ReleaseCursorForMenu();
        else RestoreCursorToGame();
        break;
    default:
        break;
    }
}

void GUI::InstallWindowHook(HWND window) {
    if (!window || originalWndProc || !IsWindow(window)) return;
    if (GetWindowLongPtrW(window, GWLP_WNDPROC) == reinterpret_cast<LONG_PTR>(&WndProcHook)) return;

    originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProcHook)));
}

// ---------------------------------------------------------------------------
// Own cursor mode
// ---------------------------------------------------------------------------
bool GUI::IsGameCaller(const void* returnAddress) {
    if (g_ExeInfo.image.empty()) return true;

    const auto begin = reinterpret_cast<uintptr_t>(g_ExeInfo.image.data());
    const auto end = begin + g_ExeInfo.image.size();
    const auto address = reinterpret_cast<uintptr_t>(returnAddress);
    return address >= begin && address < end;
}

BOOL WINAPI GUI::HookedSetCursorPos(int x, int y) {
    if (IsGameCaller(_ReturnAddress())) {
        lastGameSetCursorPosTick.store(GetTickCount64(), std::memory_order_relaxed);
        bool captured;
        {
            std::lock_guard<std::mutex> lock(cursorMutex);
            captured = cursorCaptured.load();
            gameCursorPos = { x, y };
            hasGameCursorPos = true;
            if (captured) gameMovedCursorWhileCaptured = true;
        }
        if (captured) return TRUE;
    }
    return originalSetCursorPos(x, y);
}

BOOL WINAPI GUI::HookedGetCursorPos(LPPOINT point) {
    if (point && cursorCaptured.load() && IsGameCaller(_ReturnAddress())) {
        std::lock_guard<std::mutex> lock(cursorMutex);
        if (hasGameCursorPos) {
            *point = gameCursorPos;
            return TRUE;
        }
    }
    return originalGetCursorPos(point);
}

BOOL WINAPI GUI::HookedClipCursor(const RECT* rect) {
    if (IsGameCaller(_ReturnAddress())) {
        {
            std::lock_guard<std::mutex> lock(cursorMutex);
            if (rect) {
                gameClipRect = *rect;
                hasGameClipRect = true;
            } else {
                hasGameClipRect = false;
            }
        }
        if (cursorCaptured.load()) return TRUE;
    }
    return originalClipCursor(rect);
}

UINT WINAPI GUI::HookedGetRawInputData(HRAWINPUT rawInput, UINT command, LPVOID data, PUINT size, UINT headerSize) {
    const UINT result = originalGetRawInputData(rawInput, command, data, size, headerSize);

    if (result != static_cast<UINT>(-1) && data && command == RID_INPUT &&
        cursorCaptured.load() && IsGameCaller(_ReturnAddress())) {
        auto* raw = static_cast<RAWINPUT*>(data);
        if (raw->header.dwType == RIM_TYPEMOUSE && result >= offsetof(RAWINPUT, data.mouse) + sizeof(RAWMOUSE)) {
            raw->data.mouse.lLastX = 0;
            raw->data.mouse.lLastY = 0;
            raw->data.mouse.usButtonFlags = 0;
            raw->data.mouse.usButtonData = 0;
        }
    }
    return result;
}

int WINAPI GUI::HookedShowCursor(BOOL show) {
    if (cursorCaptured.load() && IsGameCaller(_ReturnAddress())) {
        ServiceOsCursor();

        std::lock_guard<std::mutex> lock(cursorMutex);
        gameShowCount += show ? 1 : -1;
        return gameShowCount;
    }
    return originalShowCursor(show);
}

HCURSOR WINAPI GUI::HookedSetCursor(HCURSOR cursor) {
    if (IsGameCaller(_ReturnAddress())) {
        {
            std::lock_guard<std::mutex> lock(cursorMutex);
            gameCursor = cursor;
            hasGameCursor = true;
        }
        if (cursorCaptured.load()) return GetCursor();
    }
    return originalSetCursor(cursor);
}

bool GUI::InstallCursorHooks() {
    if (cursorHooksInstalled) return true;

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return false;

    auto hookExport = [user32](const char* name, LPVOID detour, LPVOID* original) {
        FARPROC target = GetProcAddress(user32, name);
        if (!target) return false;
        if (MH_CreateHook(reinterpret_cast<LPVOID>(target), detour, original) != MH_OK) return false;
        return MH_EnableHook(reinterpret_cast<LPVOID>(target)) == MH_OK;
    };

    if (!hookExport("SetCursorPos", reinterpret_cast<LPVOID>(&HookedSetCursorPos), reinterpret_cast<LPVOID*>(&originalSetCursorPos))) return false;
    if (!hookExport("ClipCursor", reinterpret_cast<LPVOID>(&HookedClipCursor), reinterpret_cast<LPVOID*>(&originalClipCursor))) return false;

    if (!hookExport("GetCursorPos", reinterpret_cast<LPVOID>(&HookedGetCursorPos), reinterpret_cast<LPVOID*>(&originalGetCursorPos))) originalGetCursorPos = nullptr;
    if (!hookExport("GetRawInputData", reinterpret_cast<LPVOID>(&HookedGetRawInputData), reinterpret_cast<LPVOID*>(&originalGetRawInputData))) originalGetRawInputData = nullptr;
    if (!hookExport("ShowCursor", reinterpret_cast<LPVOID>(&HookedShowCursor), reinterpret_cast<LPVOID*>(&originalShowCursor))) originalShowCursor = nullptr;
    if (!hookExport("SetCursor", reinterpret_cast<LPVOID>(&HookedSetCursor), reinterpret_cast<LPVOID*>(&originalSetCursor))) originalSetCursor = nullptr;

    cursorHooksInstalled = true;
    return true;
}

void GUI::PollMouseButtons() {
    if (cursorMode.load() != CursorMode::Own || !cursorCaptured.load() || !ProcessHasForeground()) return;

    ImGuiIO& io = ImGui::GetIO();
    const bool swapped = GetSystemMetrics(SM_SWAPBUTTON) != 0;
    io.AddMouseButtonEvent(0, (GetAsyncKeyState(swapped ? VK_RBUTTON : VK_LBUTTON) & 0x8000) != 0);
    io.AddMouseButtonEvent(1, (GetAsyncKeyState(swapped ? VK_LBUTTON : VK_RBUTTON) & 0x8000) != 0);
    io.AddMouseButtonEvent(2, (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
}

void GUI::ServiceOsCursor() {
    if (cursorMode.load() != CursorMode::Own) return;

    const bool captured = cursorCaptured.load();
    if (captured == (osCursorAdded > 0)) return;

    std::lock_guard<std::mutex> lock(cursorMutex);

    auto showCursor = [](BOOL show) { return originalShowCursor ? originalShowCursor(show) : ShowCursor(show); };
    auto setCursor = [](HCURSOR cursor) { return originalSetCursor ? originalSetCursor(cursor) : SetCursor(cursor); };

    if (captured) {
        int count = showCursor(TRUE);
        osCursorAdded = 1;
        showCountAtCapture = count - 1;
        gameShowCount = showCountAtCapture;

        while (count < 0 && osCursorAdded < 32) {
            count = showCursor(TRUE);
            ++osCursorAdded;
        }

        if (!GetCursor()) setCursor(LoadCursorW(nullptr, IDC_ARROW));
        return;
    }

    while (osCursorAdded > 0) {
        showCursor(FALSE);
        --osCursorAdded;
    }

    int drift = gameShowCount - showCountAtCapture;
    while (drift > 0) { showCursor(TRUE); --drift; }
    while (drift < 0) { showCursor(FALSE); ++drift; }

    if (hasGameCursor) setCursor(gameCursor);
}

void GUI::UpdateCursorConfinement() {
    if (!cursorCaptured.load()) return;

    RECT rect{};
    bool confine = false;
    if (gameWindow && IsWindow(gameWindow) && GetClientRect(gameWindow, &rect)) {
        POINT topLeft{ rect.left, rect.top };
        POINT bottomRight{ rect.right, rect.bottom };
        if (ClientToScreen(gameWindow, &topLeft) && ClientToScreen(gameWindow, &bottomRight)) {
            rect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
            confine = rect.right > rect.left && rect.bottom > rect.top;
        }
    }

    if (!confine) {
        if (hasConfineRect) {
            hasConfineRect = false;
            originalClipCursor(nullptr);
        }
        return;
    }

    if (!hasConfineRect || !EqualRect(&rect, &confineRect)) {
        confineRect = rect;
        hasConfineRect = true;
        originalClipCursor(&confineRect);
    }
}

void GUI::SetCursorCaptured(bool captured) {
    if (!cursorHooksInstalled) return;
    if (cursorCaptured.load() == captured) return;

    if (captured) {
        {
            std::lock_guard<std::mutex> lock(cursorMutex);
            POINT current{};
            if (originalGetCursorPos && originalGetCursorPos(&current)) {
                gameCursorPos = current;
                hasGameCursorPos = true;
            }
            gameMovedCursorWhileCaptured = false;
            cursorCaptured.store(true);
        }
        hasConfineRect = false;
        originalClipCursor(nullptr);

        if (hasSavedMenuPos && gameWindow && IsWindow(gameWindow)) {
            RECT client{};
            POINT current{};
            if (GetClientRect(gameWindow, &client) && client.right > client.left && client.bottom > client.top &&
                originalGetCursorPos && originalGetCursorPos(&current)) {
                POINT local = current;
                ScreenToClient(gameWindow, &local);

                const LONG centreX = (client.left + client.right) / 2;
                const LONG centreY = (client.top + client.bottom) / 2;
                const LONG tolerance = 8;
                const bool atCentre = std::abs(local.x - centreX) <= tolerance && std::abs(local.y - centreY) <= tolerance;

                bool gameHoldingCursor = atCentre;
                {
                    std::lock_guard<std::mutex> lock(cursorMutex);
                    const ULONGLONG lastCall = lastGameSetCursorPosTick.load(std::memory_order_relaxed);
                    if (lastCall != 0 && GetTickCount64() - lastCall <= 250) gameHoldingCursor = true;
                    if (hasGameClipRect) {
                        const LONG clipW = gameClipRect.right - gameClipRect.left;
                        const LONG clipH = gameClipRect.bottom - gameClipRect.top;
                        if (clipW < (client.right - client.left) / 2 && clipH < (client.bottom - client.top) / 2) gameHoldingCursor = true;
                    }
                }

                if (gameHoldingCursor) {
                    POINT target = savedMenuClientPos;
                    target.x = (std::min)((std::max)(target.x, client.left), client.right - 1);
                    target.y = (std::min)((std::max)(target.y, client.top), client.bottom - 1);
                    if (ClientToScreen(gameWindow, &target)) originalSetCursorPos(target.x, target.y);
                }
            }
        }
    } else {
        RECT clip{};
        POINT pos{};
        bool hasClip = false, hasPos = false;

        POINT menuPos{};
        const bool haveMenuPos = originalGetCursorPos && originalGetCursorPos(&menuPos);
        {
            std::lock_guard<std::mutex> lock(cursorMutex);
            cursorCaptured.store(false);
            clip = gameClipRect;
            hasClip = hasGameClipRect;
            pos = gameCursorPos;
            hasPos = hasGameCursorPos && gameMovedCursorWhileCaptured;
            if (haveMenuPos && gameWindow && IsWindow(gameWindow) && ScreenToClient(gameWindow, &menuPos)) {
                savedMenuClientPos = menuPos;
                hasSavedMenuPos = true;
            }
            gameMovedCursorWhileCaptured = false;
        }

        if (hasPos) originalSetCursorPos(pos.x, pos.y);
        originalClipCursor(hasClip ? &clip : nullptr);
        hasConfineRect = false;
    }

    if (gameWindow && IsWindow(gameWindow)) PostMessageW(gameWindow, WM_NULL, 0, 0);
}

// ---------------------------------------------------------------------------
// External cursor mode
// ---------------------------------------------------------------------------
bool GUI::IsExternalWndProc(WNDPROC proc) {
    if (!proc) return false;

    const HMODULE module = ModuleFromAddress(reinterpret_cast<const void*>(proc));
    if (!module) return false;

    const HMODULE selfModule = ModuleFromAddress(reinterpret_cast<const void*>(&GUI::WndProcHook));

    return module != GetModuleHandleW(nullptr) && module != selfModule;
}

bool GUI::IsVirtualScreenRect(const RECT& rect) {
    RECT virtualScreen{};
    virtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    virtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    virtualScreen.right = virtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    virtualScreen.bottom = virtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    return rect.left == virtualScreen.left &&
        rect.top == virtualScreen.top &&
        rect.right == virtualScreen.right &&
        rect.bottom == virtualScreen.bottom;
}

void GUI::SetCursorVisible(bool visible) {
    CURSORINFO cursorInfo{};
    cursorInfo.cbSize = sizeof(cursorInfo);
    if (!GetCursorInfo(&cursorInfo)) return;

    const bool currentlyVisible = (cursorInfo.flags & CURSOR_SHOWING) != 0;
    if (currentlyVisible == visible) return;

    for (int i = 0; i < 32; ++i) {
        ShowCursor(visible ? TRUE : FALSE);

        CURSORINFO updated{};
        updated.cbSize = sizeof(updated);
        if (!GetCursorInfo(&updated)) break;

        const bool nowVisible = (updated.flags & CURSOR_SHOWING) != 0;
        if (nowVisible == visible) break;
    }
}

void GUI::ForceCursorVisible() {
    SetCursorVisible(true);
    SetCursor(LoadCursorW(nullptr, IDC_ARROW));
}

void GUI::ReleaseCursorForMenu() {
    if (!gameWindow || !IsWindow(gameWindow)) return;

    if (!cursorOverrideActive) {
        savedCursorVisible = false;
        CURSORINFO cursorInfo{};
        cursorInfo.cbSize = sizeof(cursorInfo);
        if (GetCursorInfo(&cursorInfo)) {
            savedCursorVisible = (cursorInfo.flags & CURSOR_SHOWING) != 0;
            savedCursorHandle = cursorInfo.hCursor;
        } else {
            savedCursorHandle = nullptr;
        }

        savedClipValid = GetClipCursor(&savedClipRect) != FALSE;
        savedClipWasActive = savedClipValid && !IsVirtualScreenRect(savedClipRect);
        savedCaptureWindow = GetCapture();
        cursorOverrideActive = true;
    }

    ClipCursor(nullptr);
    if (GetCapture()) ReleaseCapture();
    ForceCursorVisible();
}

void GUI::RestoreCursorToGame() {
    if (!cursorOverrideActive) return;

    if (savedClipValid && savedClipWasActive) {
        ClipCursor(&savedClipRect);
    } else {
        ClipCursor(nullptr);
    }

    if (savedCaptureWindow && IsWindow(savedCaptureWindow) && GetForegroundWindow() == savedCaptureWindow) {
        SetCapture(savedCaptureWindow);
    }

    SetCursorVisible(savedCursorVisible);
    SetCursor(savedCursorHandle);

    savedCursorVisible = false;
    savedCursorHandle = nullptr;
    savedClipValid = false;
    savedClipWasActive = false;
    savedClipRect = {};
    savedCaptureWindow = nullptr;
    cursorOverrideActive = false;
}

void GUI::OnPresent(IDXGISwapChain3* swapChain, UINT flags) {
    if (!Settings::EnableGUI || disabled || !swapChain) {
        UpdateCursor(false);
        return;
    }

    if (flags & DXGI_PRESENT_TEST) return;

    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    PollToggleKey();
    if (!Visible) {
        UpdateCursor(false);
        return;
    }

    if (initialised && swapChain != activeSwapChain) {
        DXGI_SWAP_CHAIN_DESC desc{};
        if (FAILED(swapChain->GetDesc(&desc)) || desc.OutputWindow != gameWindow) return;
        ShutdownGraphics();
    }

    if (initialised && !SwapChainStillMatches(swapChain)) {
        ShutdownGraphics();
    }

    if (!initialised) {
        InitialiseImGui(swapChain);
        if (!initialised) {
            UpdateCursor(false);
            return;
        }
    }

    UpdateCursor(true);
    RenderFrame(swapChain);

    if (!Visible) UpdateCursor(false);
}

bool GUI::GuardedOnPresent(IDXGISwapChain3* swapChain, UINT flags) {
    OnPresent(swapChain, flags);
    return true;
}

HRESULT STDMETHODCALLTYPE GUI::HookedPresent(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags) {
    PresentScope scope;
    if (scope.owner && !GuardedOnPresent(swapChain, flags)) {
        disabled = true;
        initialised = false;
        UpdateCursor(false);
    }

    return originalPresent(swapChain, syncInterval, flags);
}

HRESULT STDMETHODCALLTYPE GUI::HookedPresent1(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags, const DXGI_PRESENT_PARAMETERS* params) {
    PresentScope scope;
    if (scope.owner && !GuardedOnPresent(swapChain, flags)) {
        disabled = true;
        initialised = false;
        UpdateCursor(false);
    }

    return originalPresent1(swapChain, syncInterval, flags, params);
}

HRESULT STDMETHODCALLTYPE GUI::HookedCreateSwapChain(IDXGIFactory* factory, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** swapChain) {
    const HRESULT result = originalCreateSwapChain(factory, device, desc, swapChain);

    if (SUCCEEDED(result) && swapChain && *swapChain) {
        GuardedRegisterSwapChain(device, *swapChain, desc ? desc->OutputWindow : nullptr);
    }
    return result;
}

HRESULT STDMETHODCALLTYPE GUI::HookedCreateSwapChainForHwnd(IDXGIFactory2* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreenDesc, IDXGIOutput* restrictToOutput, IDXGISwapChain1** swapChain) {
    const HRESULT result = originalCreateSwapChainForHwnd(factory, device, hwnd, desc, fullscreenDesc, restrictToOutput, swapChain);

    if (SUCCEEDED(result) && swapChain && *swapChain) {
        GuardedRegisterSwapChain(device, *swapChain, hwnd);
    }
    return result;
}
