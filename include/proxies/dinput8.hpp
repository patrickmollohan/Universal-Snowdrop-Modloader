#pragma once

#include "proxies.hpp"

class DInput8Proxy {
public:
    static HRESULT DirectInput8Create_proxy(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
    static HRESULT DllCanUnloadNow_proxy();
    static HRESULT DllGetClassObject_proxy(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
    static HRESULT DllRegisterServer_proxy();
    static HRESULT DllUnregisterServer_proxy();
};
