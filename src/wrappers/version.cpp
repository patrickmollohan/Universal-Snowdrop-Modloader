#include "pch.hpp"
#include "version.hpp"

static const char* funcs[] = {
    "GetFileVersionInfoA",
    "GetFileVersionInfoByHandle",
    "GetFileVersionInfoExA",
    "GetFileVersionInfoExW",
    "GetFileVersionInfoSizeA",
    "GetFileVersionInfoSizeExA",
    "GetFileVersionInfoSizeExW",
    "GetFileVersionInfoSizeW",
    "GetFileVersionInfoW",
    "VerFindFileA",
    "VerFindFileW",
    "VerInstallFileA",
    "VerInstallFileW",
    "VerLanguageNameA",
    "VerLanguageNameW",
    "VerQueryValueA",
    "VerQueryValueW"
};

constexpr size_t count = ARRAY_LEN(funcs);

UINT_PTR procs[count]{};

static struct VersionExportsInit {
    VersionExportsInit() {
        g_DllExports = {
            funcs,
            procs,
            count
        };
    }
} g_VersionExportsInit;

BOOL VersionWrapper::GetFileVersionInfoA_wrapper(LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    static auto GetFileVersionInfoA_ptr = reinterpret_cast<BOOL(__cdecl*)(LPCSTR, DWORD, DWORD, LPVOID, LPDWORD)>(g_DllExports.procs[0]);
    return GetFileVersionInfoA_ptr ? GetFileVersionInfoA_ptr(lpFileName, dwHandle, dwLen, lpData, lpdwLen) : FALSE;
}

BOOL VersionWrapper::GetFileVersionInfoByHandle_wrapper(HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    static auto GetFileVersionInfoByHandle_ptr = reinterpret_cast<BOOL(__cdecl*)(HANDLE, DWORD, DWORD, LPVOID, LPDWORD)>(g_DllExports.procs[1]);
    return GetFileVersionInfoByHandle_ptr ? GetFileVersionInfoByHandle_ptr(hFile, dwHandle, dwLen, lpData, lpdwLen) : FALSE;
}

BOOL VersionWrapper::GetFileVersionInfoExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    static auto GetFileVersionInfoExA_ptr = reinterpret_cast<BOOL(__cdecl*)(DWORD, LPCSTR, DWORD, DWORD, LPVOID, LPDWORD)>(g_DllExports.procs[2]);
    return GetFileVersionInfoExA_ptr ? GetFileVersionInfoExA_ptr(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen) : FALSE;
}

BOOL VersionWrapper::GetFileVersionInfoExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    static auto GetFileVersionInfoExW_ptr = reinterpret_cast<BOOL(__cdecl*)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID, LPDWORD)>(g_DllExports.procs[3]);
    return GetFileVersionInfoExW_ptr ? GetFileVersionInfoExW_ptr(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen) : FALSE;
}

DWORD VersionWrapper::GetFileVersionInfoSizeA_wrapper(LPCSTR lpFileName, LPDWORD lpdwHandle) {
    static auto GetFileVersionInfoSizeA_ptr = reinterpret_cast<DWORD(__cdecl*)(LPCSTR, LPDWORD)>(g_DllExports.procs[4]);
    return GetFileVersionInfoSizeA_ptr ? GetFileVersionInfoSizeA_ptr(lpFileName, lpdwHandle) : 0;
}

DWORD VersionWrapper::GetFileVersionInfoSizeExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPDWORD lpdwHandle) {
    static auto GetFileVersionInfoSizeExA_ptr = reinterpret_cast<DWORD(__cdecl*)(DWORD, LPCSTR, LPDWORD)>(g_DllExports.procs[5]);
    return GetFileVersionInfoSizeExA_ptr ? GetFileVersionInfoSizeExA_ptr(dwFlags, lpFileName, lpdwHandle) : 0;
}

DWORD VersionWrapper::GetFileVersionInfoSizeExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPDWORD lpdwHandle) {
    static auto GetFileVersionInfoSizeExW_ptr = reinterpret_cast<DWORD(__cdecl*)(DWORD, LPCWSTR, LPDWORD)>(g_DllExports.procs[6]);
    return GetFileVersionInfoSizeExW_ptr ? GetFileVersionInfoSizeExW_ptr(dwFlags, lpFileName, lpdwHandle) : 0;
}

DWORD VersionWrapper::GetFileVersionInfoSizeW_wrapper(LPCWSTR lpFileName, LPDWORD lpdwHandle) {
    static auto GetFileVersionInfoSizeW_ptr = reinterpret_cast<DWORD(__cdecl*)(LPCWSTR, LPDWORD)>(g_DllExports.procs[7]);
    return GetFileVersionInfoSizeW_ptr ? GetFileVersionInfoSizeW_ptr(lpFileName, lpdwHandle) : 0;
}

BOOL VersionWrapper::GetFileVersionInfoW_wrapper(LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    static auto GetFileVersionInfoW_ptr = reinterpret_cast<BOOL(__cdecl*)(LPCWSTR, DWORD, DWORD, LPVOID, LPDWORD)>(g_DllExports.procs[8]);
    return GetFileVersionInfoW_ptr ? GetFileVersionInfoW_ptr(lpFileName, dwHandle, dwLen, lpData, lpdwLen) : FALSE;
}

UINT VersionWrapper::VerFindFileA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPCSTR lpWinDir, LPCSTR lpSearchFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen) {
    static auto VerFindFileA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPUINT)>(g_DllExports.procs[9]);
    return VerFindFileA_ptr ? VerFindFileA_ptr(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen) : 0;
}

UINT VersionWrapper::VerFindFileW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPCWSTR lpWinDir, LPCWSTR lpSearchFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen) {
    static auto VerFindFileW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPUINT)>(g_DllExports.procs[10]);
    return VerFindFileW_ptr ? VerFindFileW_ptr(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen) : 0;
}

UINT VersionWrapper::VerInstallFileA_wrapper(DWORD dwFlags, LPCSTR lpSrcFile, LPCSTR lpDestFile, LPCSTR lpTmpFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen) {
    static auto VerInstallFileA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPUINT)>(g_DllExports.procs[11]);
    return VerInstallFileA_ptr ? VerInstallFileA_ptr(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen) : 0;
}

UINT VersionWrapper::VerInstallFileW_wrapper(DWORD dwFlags, LPCWSTR lpSrcFile, LPCWSTR lpDestFile, LPCWSTR lpTmpFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen) {
    static auto VerInstallFileW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPUINT)>(g_DllExports.procs[12]);
    return VerInstallFileW_ptr ? VerInstallFileW_ptr(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen) : 0;
}

UINT VersionWrapper::VerLanguageNameA_wrapper(DWORD dwLang, LPSTR lpName, DWORD nSize) {
    static auto VerLanguageNameA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPSTR, DWORD)>(g_DllExports.procs[13]);
    return VerLanguageNameA_ptr ? VerLanguageNameA_ptr(dwLang, lpName, nSize) : 0;
}

UINT VersionWrapper::VerLanguageNameW_wrapper(DWORD dwLang, LPWSTR lpName, DWORD nSize) {
    static auto VerLanguageNameW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPWSTR, DWORD)>(g_DllExports.procs[14]);
    return VerLanguageNameW_ptr ? VerLanguageNameW_ptr(dwLang, lpName, nSize) : 0;
}

BOOL VersionWrapper::VerQueryValueA_wrapper(LPVOID lpBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    static auto VerQueryValueA_ptr = reinterpret_cast<BOOL(__cdecl*)(LPVOID, LPCSTR, LPVOID*, PUINT)>(g_DllExports.procs[15]);
    return VerQueryValueA_ptr ? VerQueryValueA_ptr(lpBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}

BOOL VersionWrapper::VerQueryValueW_wrapper(LPVOID lpBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    static auto VerQueryValueW_ptr = reinterpret_cast<BOOL(__cdecl*)(LPVOID, LPCWSTR, LPVOID*, PUINT)>(g_DllExports.procs[16]);
    return VerQueryValueW_ptr ? VerQueryValueW_ptr(lpBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}
