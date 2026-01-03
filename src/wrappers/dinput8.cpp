#include "pch.hpp"
#include "dinput8.hpp"

static const char* funcs[] = {
    "DirectInput8Create",
    "DllCanUnloadNow",
    "DllGetClassObject",
    "DllRegisterServer",
    "DllUnregisterServer"
};

constexpr size_t count = ARRAY_LEN(funcs);
UINT_PTR procs[count]{};

static struct DInput8ExportsInit {
    DInput8ExportsInit() {
        g_DllExports = {
            funcs,
            procs,
            count
        };
    }
} g_DInput8ExportsInit;

HRESULT DInput8Wrapper::DirectInput8Create_wrapper(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
    static auto DirectInput8Create_ptr = reinterpret_cast<HRESULT(__cdecl*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN)>(g_DllExports.procs[0]);
    return DirectInput8Create_ptr ? DirectInput8Create_ptr(hinst, dwVersion, riidltf, ppvOut, punkOuter) : E_FAIL;
}

HRESULT DInput8Wrapper::DllCanUnloadNow_wrapper() {
    static auto DllCanUnloadNow_ptr = reinterpret_cast<HRESULT(__cdecl*)()>(g_DllExports.procs[1]);
    return DllCanUnloadNow_ptr ? DllCanUnloadNow_ptr() : E_FAIL;
}

HRESULT DInput8Wrapper::DllGetClassObject_wrapper(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    static auto DllGetClassObject_ptr = reinterpret_cast<HRESULT(__cdecl*)(REFCLSID, REFIID, LPVOID*)>(g_DllExports.procs[2]);
    return DllGetClassObject_ptr ? DllGetClassObject_ptr(rclsid, riid, ppv) : E_FAIL;
}

HRESULT DInput8Wrapper::DllRegisterServer_wrapper() {
    static auto DllRegisterServer_ptr = reinterpret_cast<HRESULT(__cdecl*)()>(g_DllExports.procs[3]);
    return DllRegisterServer_ptr ? DllRegisterServer_ptr() : E_FAIL;
}

HRESULT DInput8Wrapper::DllUnregisterServer_wrapper() {
    static auto DllUnregisterServer_ptr = reinterpret_cast<HRESULT(__cdecl*)()>(g_DllExports.procs[4]);
    return DllUnregisterServer_ptr ? DllUnregisterServer_ptr() : E_FAIL;
}
