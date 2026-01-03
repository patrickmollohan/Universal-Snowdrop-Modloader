#pragma once

#include "wrappers.hpp"

class DInput8Wrapper {
public:
    static HRESULT DirectInput8Create_wrapper(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
    static HRESULT DllCanUnloadNow_wrapper();
    static HRESULT DllGetClassObject_wrapper(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
    static HRESULT DllRegisterServer_wrapper();
    static HRESULT DllUnregisterServer_wrapper();
};
