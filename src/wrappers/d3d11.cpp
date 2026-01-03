#include "pch.hpp"
#include "d3d11.hpp"

static const char* funcs[] = {
    "D3D11CreateDevice",
    "D3D11CreateDeviceAndSwapChain"
};

constexpr size_t count = ARRAY_LEN(funcs);

UINT_PTR procs[count]{};

static struct D3D11ExportsInit {
    D3D11ExportsInit() {
        g_DllExports = {
            funcs,
            procs,
            count
        };
    }
} g_D3D11ExportsInit;

HRESULT D3D11Wrapper::D3D11CreateDevice_wrapper(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
) {
    static auto D3D11CreateDevice_ptr =
        reinterpret_cast<HRESULT(__cdecl*)(
            IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
            const D3D_FEATURE_LEVEL*, UINT, UINT,
            ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**)>(g_DllExports.procs[0]);

    return D3D11CreateDevice_ptr
        ? D3D11CreateDevice_ptr(
            pAdapter, DriverType, Software, Flags,
            pFeatureLevels, FeatureLevels, SDKVersion,
            ppDevice, pFeatureLevel, ppImmediateContext)
        : E_FAIL;
}

HRESULT D3D11Wrapper::D3D11CreateDeviceAndSwapChain_wrapper(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
) {
    static auto D3D11CreateDeviceAndSwapChain_ptr =
        reinterpret_cast<HRESULT(__cdecl*)(
            IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
            const D3D_FEATURE_LEVEL*, UINT, UINT,
            const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
            ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**)>(g_DllExports.procs[1]);

    return D3D11CreateDeviceAndSwapChain_ptr
        ? D3D11CreateDeviceAndSwapChain_ptr(
            pAdapter, DriverType, Software, Flags,
            pFeatureLevels, FeatureLevels, SDKVersion,
            pSwapChainDesc, ppSwapChain,
            ppDevice, pFeatureLevel, ppImmediateContext)
        : E_FAIL;
}
