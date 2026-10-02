#include "pch.hpp"
#include "gui.hpp"

#include <cstring>
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

    if (GetWindowLongPtrW(gameWindow, GWLP_WNDPROC) != reinterpret_cast<LONG_PTR>(&WndProcHook)) {
        originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProcHook)));
    }

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
    if (initialised.load() && Visible) {
        std::lock_guard<std::recursive_mutex> lock(imguiMutex);

        if (initialised.load() && imguiContextCreated) {
            ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

            ImGuiIO& io = ImGui::GetIO();

            const bool isKeyboardMsg = (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR ||
                msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP);
            const bool isMouseMsg = (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST);

            if ((io.WantCaptureKeyboard && isKeyboardMsg) || (io.WantCaptureMouse && isMouseMsg)) {
                return TRUE;
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

void GUI::OnPresent(IDXGISwapChain3* swapChain, UINT flags) {
    if (!Settings::EnableGUI || disabled || !swapChain) return;

    if (flags & DXGI_PRESENT_TEST) return;

    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    PollToggleKey();
    if (!Visible) return;

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
        if (!initialised) return;
    }

    RenderFrame(swapChain);
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
    }

    return originalPresent(swapChain, syncInterval, flags);
}

HRESULT STDMETHODCALLTYPE GUI::HookedPresent1(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags, const DXGI_PRESENT_PARAMETERS* params) {
    PresentScope scope;
    if (scope.owner && !GuardedOnPresent(swapChain, flags)) {
        disabled = true;
        initialised = false;
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
