#include "pch.hpp"
#include "gui.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool GUI::Visible = false;
bool GUI::initialised = false;

ID3D12Device* GUI::device = nullptr;
ID3D12CommandQueue* GUI::commandQueue = nullptr;
ID3D12GraphicsCommandList* GUI::commandList = nullptr;
ID3D12DescriptorHeap* GUI::rtvDescriptorHeap = nullptr;
ID3D12DescriptorHeap* GUI::srvDescriptorHeap = nullptr;
GUI::FrameContext GUI::frameContexts[GUI::BackBufferCount]{};
UINT GUI::frameCount = 0;

HWND GUI::gameWindow = nullptr;
WNDPROC GUI::originalWndProc = nullptr;

GUI::Present_t GUI::originalPresent = nullptr;
GUI::ResizeBuffers_t GUI::originalResizeBuffers = nullptr;
GUI::ExecuteCommandLists_t GUI::originalExecuteCommandLists = nullptr;

bool GUI::FindTargetFunctions(void** presentFn, void** resizeBuffersFn, void** executeCommandListsFn) {
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
        void** queueVTable = *reinterpret_cast<void***>(dummyQueue);

        *presentFn = swapChainVTable[8];
        *resizeBuffersFn = swapChainVTable[13];
        *executeCommandListsFn = queueVTable[10];

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
    void* resizeBuffersFn = nullptr;
    void* executeCommandListsFn = nullptr;

    if (!FindTargetFunctions(&presentFn, &resizeBuffersFn, &executeCommandListsFn)) {
        MessageBoxA(NULL, "Failed to locate DirectX 12 functions. The overlay menu will be disabled.", "Dank farrik!", MB_OK | MB_ICONWARNING);
        return false;
    }

    if (MH_CreateHook(presentFn, reinterpret_cast<LPVOID>(&HookedPresent), reinterpret_cast<LPVOID*>(&originalPresent)) != MH_OK ||
        MH_CreateHook(resizeBuffersFn, reinterpret_cast<LPVOID>(&HookedResizeBuffers), reinterpret_cast<LPVOID*>(&originalResizeBuffers)) != MH_OK ||
        MH_CreateHook(executeCommandListsFn, reinterpret_cast<LPVOID>(&HookedExecuteCommandLists), reinterpret_cast<LPVOID*>(&originalExecuteCommandLists)) != MH_OK) {
        MessageBoxA(NULL, "Failed to hook DirectX 12 functions. The overlay menu will be disabled.", "Dank farrik!", MB_OK | MB_ICONWARNING);
        return false;
    }

    return true;
}

void GUI::UnloadGUI() {
    if (!initialised) return;

    if (gameWindow && originalWndProc) {
        SetWindowLongPtrW(gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
    }

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    ReleaseRenderTargets();

    for (UINT i = 0; i < frameCount; ++i) {
        if (frameContexts[i].commandAllocator) {
            frameContexts[i].commandAllocator->Release();
            frameContexts[i].commandAllocator = nullptr;
        }
    }

    if (commandList) { commandList->Release(); commandList = nullptr; }
    if (rtvDescriptorHeap) { rtvDescriptorHeap->Release(); rtvDescriptorHeap = nullptr; }
    if (srvDescriptorHeap) { srvDescriptorHeap->Release(); srvDescriptorHeap = nullptr; }
    if (device) { device->Release(); device = nullptr; }

    initialised = false;
}

void GUI::InitialiseImGui(IDXGISwapChain3* swapChain) {
    if (FAILED(swapChain->GetDevice(IID_PPV_ARGS(&device)))) return;

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    if (FAILED(swapChain->GetDesc(&swapDesc))) return;

    gameWindow = swapDesc.OutputWindow;
    frameCount = swapDesc.BufferCount;
    if (frameCount == 0 || frameCount > BackBufferCount) frameCount = BackBufferCount;

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = frameCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap)))) return;

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap)))) return;

    const UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < frameCount; ++i) {
        frameContexts[i].rtvHandle = rtvHandle;
        rtvHandle.ptr += rtvDescriptorSize;

        if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameContexts[i].commandAllocator)))) return;
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameContexts[0].commandAllocator, nullptr, IID_PPV_ARGS(&commandList)))) return;
    commandList->Close();

    CreateRenderTargets(swapChain);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(gameWindow)) return;
    if (!ImGui_ImplDX12_Init(device, static_cast<int>(frameCount), DXGI_FORMAT_R8G8B8A8_UNORM, srvDescriptorHeap,
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart())) return;

    originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProcHook)));

    initialised = true;
}

void GUI::CreateRenderTargets(IDXGISwapChain3* swapChain) {
    for (UINT i = 0; i < frameCount; ++i) {
        ID3D12Resource* backBuffer = nullptr;
        if (SUCCEEDED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))) {
            device->CreateRenderTargetView(backBuffer, nullptr, frameContexts[i].rtvHandle);
            frameContexts[i].backBuffer = backBuffer;
        }
    }
}

void GUI::ReleaseRenderTargets() {
    for (UINT i = 0; i < frameCount; ++i) {
        if (frameContexts[i].backBuffer) {
            frameContexts[i].backBuffer->Release();
            frameContexts[i].backBuffer = nullptr;
        }
    }
}

void GUI::RenderFrame(IDXGISwapChain3* swapChain) {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Visible) {
        DrawMenu();
    }

    ImGui::Render();

    const UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    FrameContext& frame = frameContexts[backBufferIndex];

    frame.commandAllocator->Reset();
    commandList->Reset(frame.commandAllocator, nullptr);

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = frame.backBuffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandList->ResourceBarrier(1, &barrier);

    commandList->OMSetRenderTargets(1, &frame.rtvHandle, FALSE, nullptr);
    commandList->SetDescriptorHeaps(1, &srvDescriptorHeap);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();

    ID3D12CommandList* lists[] = { commandList };
    commandQueue->ExecuteCommandLists(1, lists);
}

// ---------------------------------------------------------------------------
// Plugin host API
// ---------------------------------------------------------------------------

void GUI::Host_Text(ModLoaderPluginCtx*, const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    ImGui::TextUnformatted(buffer);
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

static const ModLoaderHostAPI g_PluginHostAPI = {
    MODLOADER_PLUGIN_API_VERSION,
    &GUI::Host_Text,
    &GUI::Host_Checkbox,
    &GUI::Host_SliderInt,
    &GUI::Host_SliderFloat,
    &GUI::Host_InputText,
    &GUI::Host_Button,
    &GUI::Host_Separator,
    &GUI::Host_GetConfigBool,
    &GUI::Host_GetConfigInt,
    &GUI::Host_SetConfigBool,
    &GUI::Host_SetConfigInt,
    &GUI::Host_SendCommand,
    &GUI::Host_Log,
};

const ModLoaderHostAPI* GUI::GetPluginHostAPI() {
    return &g_PluginHostAPI;
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
                const bool hasMenu = isLoaded && plugin.apiInitialised && plugin.info.DrawMenu != nullptr;

                std::string label = (plugin.apiInitialised && plugin.info.name) ? plugin.info.name : plugin.fileName;
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
                    plugin.info.DrawMenu(&g_PluginHostAPI, plugin.ctx.get());
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }
    }

    ImGui::End();
}

LRESULT CALLBACK GUI::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == Settings::MenuToggleKey) {
        Visible = !Visible;
    }

    if (Visible) {
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        ImGuiIO& io = ImGui::GetIO();

        const bool isKeyboardMsg = (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR ||
            msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP);
        const bool isMouseMsg = (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST);

        if ((io.WantCaptureKeyboard && isKeyboardMsg) || (io.WantCaptureMouse && isMouseMsg)) {
            return TRUE;
        }
    }

    return CallWindowProcW(originalWndProc, hWnd, msg, wParam, lParam);
}

HRESULT STDMETHODCALLTYPE GUI::HookedPresent(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags) {
    if (Settings::EnableGUI) {
        if (!initialised && commandQueue) {
            InitialiseImGui(swapChain);
        }

        if (initialised) {
            RenderFrame(swapChain);
        }
    }

    return originalPresent(swapChain, syncInterval, flags);
}

HRESULT STDMETHODCALLTYPE GUI::HookedResizeBuffers(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
    if (initialised) {
        ReleaseRenderTargets();
    }

    HRESULT result = originalResizeBuffers(swapChain, bufferCount, width, height, newFormat, swapChainFlags);

    if (initialised && SUCCEEDED(result)) {
        CreateRenderTargets(swapChain);
    }

    return result;
}

void STDMETHODCALLTYPE GUI::HookedExecuteCommandLists(ID3D12CommandQueue* queue, UINT numCommandLists, ID3D12CommandList* const* commandLists) {
    if (!commandQueue && queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
        commandQueue = queue;
    }

    originalExecuteCommandLists(queue, numCommandLists, commandLists);
}
