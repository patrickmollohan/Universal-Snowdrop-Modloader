#include "version_wrapper.hpp"

HINSTANCE VersionWrapper::m_hinst_dll = nullptr;
UINT_PTR VersionWrapper::mProcs[17] = { 0 };

const LPCSTR VersionWrapper::import_names[] = {
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

// Static method to set up the DLL and function pointers
BOOL VersionWrapper::Initialise() {
    CHAR sys_dir[MAX_PATH];
    GetSystemDirectoryA(sys_dir, MAX_PATH);
    char buffer[MAX_PATH];
    snprintf(buffer, MAX_PATH, "%s\\version.dll", sys_dir);
    m_hinst_dll = LoadLibraryA(buffer);

    if (m_hinst_dll == nullptr) {
        // Handle error: DLL failed to load
        printf("Failed to load version.dll\n");
        return FALSE;
    }

    for (int i = 0; i < 17; i++) {
        mProcs[i] = reinterpret_cast<UINT_PTR>(GetProcAddress(m_hinst_dll, import_names[i]));

        if (mProcs[i] == 0) {
            // Handle error: failed to get the procedure address
            printf("Failed to get address for %s\n", import_names[i]);
            return FALSE;
        }
    }

    return TRUE;
}

// Wrapper function implementations
BOOL VersionWrapper::GetFileVersionInfoA_wrapper(LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    GET_FILE_VERSION_INFOA(lpFileName, dwHandle, dwLen, lpData, lpdwLen)
}

BOOL VersionWrapper::GetFileVersionInfoByHandle_wrapper(HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    GET_FILE_VERSION_INFO_BY_HANDLE(hFile, dwHandle, dwLen, lpData, lpdwLen)
}

BOOL VersionWrapper::GetFileVersionInfoExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    GET_FILE_VERSION_INFO_EXA(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen)
}

BOOL VersionWrapper::GetFileVersionInfoExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    GET_FILE_VERSION_INFO_EXW(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen)
}

DWORD VersionWrapper::GetFileVersionInfoSizeA_wrapper(LPCSTR lpFileName, LPDWORD lpdwHandle) {
    GET_FILE_VERSION_INFO_SIZEA(lpFileName, lpdwHandle)
}

DWORD VersionWrapper::GetFileVersionInfoSizeExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPDWORD lpdwHandle) {
    GET_FILE_VERSION_INFO_SIZEEXA(dwFlags, lpFileName, lpdwHandle)
}

DWORD VersionWrapper::GetFileVersionInfoSizeExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPDWORD lpdwHandle) {
    GET_FILE_VERSION_INFO_SIZEEXW(dwFlags, lpFileName, lpdwHandle)
}

DWORD VersionWrapper::GetFileVersionInfoSizeW_wrapper(LPCWSTR lpFileName, LPDWORD lpdwHandle) {
    GET_FILE_VERSION_INFO_SIZEW(lpFileName, lpdwHandle)
}

BOOL VersionWrapper::GetFileVersionInfoW_wrapper(LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen) {
    GET_FILE_VERSION_INFOW(lpFileName, dwHandle, dwLen, lpData, lpdwLen)
}

UINT VersionWrapper::VerFindFileA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPCSTR lpWinDir, LPCSTR lpSearchFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen) {
    VER_FIND_FILEA(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen)
}

UINT VersionWrapper::VerFindFileW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPCWSTR lpWinDir, LPCWSTR lpSearchFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen) {
    VER_FIND_FILEW(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen)
}

UINT VersionWrapper::VerInstallFileA_wrapper(DWORD dwFlags, LPCSTR lpSrcFile, LPCSTR lpDestFile, LPCSTR lpTmpFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen) {
    VER_INSTALL_FILEA(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen)
}

UINT VersionWrapper::VerInstallFileW_wrapper(DWORD dwFlags, LPCWSTR lpSrcFile, LPCWSTR lpDestFile, LPCWSTR lpTmpFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen) {
    VER_INSTALL_FILEW(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen)
}

UINT VersionWrapper::VerLanguageNameA_wrapper(DWORD dwLang, LPSTR lpName, DWORD nSize) {
    VER_LANGUAGE_NAMEA(dwLang, lpName, nSize)
}

UINT VersionWrapper::VerLanguageNameW_wrapper(DWORD dwLang, LPWSTR lpName, DWORD nSize) {
    VER_LANGUAGE_NAMEW(dwLang, lpName, nSize)
}

BOOL VersionWrapper::VerQueryValueA_wrapper(LPVOID lpBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    VER_QUERY_VALUEA(lpBlock, lpSubBlock, lplpBuffer, puLen)
}

BOOL VersionWrapper::VerQueryValueW_wrapper(LPVOID lpBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    VER_QUERY_VALUEW(lpBlock, lpSubBlock, lplpBuffer, puLen)
}
