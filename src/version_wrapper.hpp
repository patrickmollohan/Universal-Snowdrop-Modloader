#pragma once

class VersionWrapper {
public:
    static BOOL Initialise();

    // Wrapper methods for version functions
    static BOOL GetFileVersionInfoA_wrapper(LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoByHandle_wrapper(HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static DWORD GetFileVersionInfoSizeA_wrapper(LPCSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeExA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeExW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeW_wrapper(LPCWSTR lpFileName, LPDWORD lpdwHandle);
    static BOOL GetFileVersionInfoW_wrapper(LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static UINT VerFindFileA_wrapper(DWORD dwFlags, LPCSTR lpFileName, LPCSTR lpWinDir, LPCSTR lpSearchFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen);
    static UINT VerFindFileW_wrapper(DWORD dwFlags, LPCWSTR lpFileName, LPCWSTR lpWinDir, LPCWSTR lpSearchFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen);
    static UINT VerInstallFileA_wrapper(DWORD dwFlags, LPCSTR lpSrcFile, LPCSTR lpDestFile, LPCSTR lpTmpFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen);
    static UINT VerInstallFileW_wrapper(DWORD dwFlags, LPCWSTR lpSrcFile, LPCWSTR lpDestFile, LPCWSTR lpTmpFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen);
    static UINT VerLanguageNameA_wrapper(DWORD dwLang, LPSTR lpName, DWORD nSize);
    static UINT VerLanguageNameW_wrapper(DWORD dwLang, LPWSTR lpName, DWORD nSize);
    static BOOL VerQueryValueA_wrapper(LPVOID lpBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);
    static BOOL VerQueryValueW_wrapper(LPVOID lpBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);

private:
    static HINSTANCE m_hinst_dll;
    static UINT_PTR mProcs[17];
    static const LPCSTR import_names[17];
};

#define GET_FILE_VERSION_INFOA(lpFileName, dwHandle, dwLen, lpData, lpdwLen) \
    static auto GetFileVersionInfoA_ptr = reinterpret_cast<BOOL(__cdecl*)(LPCSTR, DWORD, DWORD, LPVOID, LPDWORD)>(mProcs[0]); \
    if (GetFileVersionInfoA_ptr) return GetFileVersionInfoA_ptr(lpFileName, dwHandle, dwLen, lpData, lpdwLen); \
    return FALSE;

#define GET_FILE_VERSION_INFO_BY_HANDLE(hFile, dwHandle, dwLen, lpData, lpdwLen) \
    static auto GetFileVersionInfoByHandle_ptr = reinterpret_cast<BOOL(__cdecl*)(HANDLE, DWORD, DWORD, LPVOID, LPDWORD)>(mProcs[1]); \
    if (GetFileVersionInfoByHandle_ptr) return GetFileVersionInfoByHandle_ptr(hFile, dwHandle, dwLen, lpData, lpdwLen); \
    return FALSE;

#define GET_FILE_VERSION_INFO_EXA(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen) \
    static auto GetFileVersionInfoExA_ptr = reinterpret_cast<BOOL(__cdecl*)(DWORD, LPCSTR, DWORD, DWORD, LPVOID, LPDWORD)>(mProcs[2]); \
    if (GetFileVersionInfoExA_ptr) return GetFileVersionInfoExA_ptr(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen); \
    return FALSE;

#define GET_FILE_VERSION_INFO_EXW(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen) \
    static auto GetFileVersionInfoExW_ptr = reinterpret_cast<BOOL(__cdecl*)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID, LPDWORD)>(mProcs[3]); \
    if (GetFileVersionInfoExW_ptr) return GetFileVersionInfoExW_ptr(dwFlags, lpFileName, dwHandle, dwLen, lpData, lpdwLen); \
    return FALSE;

#define GET_FILE_VERSION_INFO_SIZEA(lpFileName, lpdwHandle) \
    static auto GetFileVersionInfoSizeA_ptr = reinterpret_cast<DWORD(__cdecl*)(LPCSTR, LPDWORD)>(mProcs[4]); \
    if (GetFileVersionInfoSizeA_ptr) return GetFileVersionInfoSizeA_ptr(lpFileName, lpdwHandle); \
    return 0;

#define GET_FILE_VERSION_INFO_SIZEEXA(dwFlags, lpFileName, lpdwHandle) \
    static auto GetFileVersionInfoSizeExA_ptr = reinterpret_cast<DWORD(__cdecl*)(DWORD, LPCSTR, LPDWORD)>(mProcs[5]); \
    if (GetFileVersionInfoSizeExA_ptr) return GetFileVersionInfoSizeExA_ptr(dwFlags, lpFileName, lpdwHandle); \
    return 0;

#define GET_FILE_VERSION_INFO_SIZEEXW(dwFlags, lpFileName, lpdwHandle) \
    static auto GetFileVersionInfoSizeExW_ptr = reinterpret_cast<DWORD(__cdecl*)(DWORD, LPCWSTR, LPDWORD)>(mProcs[6]); \
    if (GetFileVersionInfoSizeExW_ptr) return GetFileVersionInfoSizeExW_ptr(dwFlags, lpFileName, lpdwHandle); \
    return 0;

#define GET_FILE_VERSION_INFO_SIZEW(lpFileName, lpdwHandle) \
    static auto GetFileVersionInfoSizeW_ptr = reinterpret_cast<DWORD(__cdecl*)(LPCWSTR, LPDWORD)>(mProcs[7]); \
    if (GetFileVersionInfoSizeW_ptr) return GetFileVersionInfoSizeW_ptr(lpFileName, lpdwHandle); \
    return 0;

#define GET_FILE_VERSION_INFOW(lpFileName, dwHandle, dwLen, lpData, lpdwLen) \
    static auto GetFileVersionInfoW_ptr = reinterpret_cast<BOOL(__cdecl*)(LPCWSTR, DWORD, DWORD, LPVOID, LPDWORD)>(mProcs[8]); \
    if (GetFileVersionInfoW_ptr) return GetFileVersionInfoW_ptr(lpFileName, dwHandle, dwLen, lpData, lpdwLen); \
    return FALSE;

#define VER_FIND_FILEA(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen) \
    static auto VerFindFileA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPUINT)>(mProcs[9]); \
    if (VerFindFileA_ptr) return VerFindFileA_ptr(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen); \
    return 0;

#define VER_FIND_FILEW(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen) \
    static auto VerFindFileW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPUINT)>(mProcs[10]); \
    if (VerFindFileW_ptr) return VerFindFileW_ptr(dwFlags, lpFileName, lpWinDir, lpSearchFile, lpResult, dwBufLen, lpBuf, lpdwLen); \
    return 0;

#define VER_INSTALL_FILEA(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen) \
    static auto VerInstallFileA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPUINT)>(mProcs[11]); \
    if (VerInstallFileA_ptr) return VerInstallFileA_ptr(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen); \
    return 0;

#define VER_INSTALL_FILEW(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen) \
    static auto VerInstallFileW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPUINT)>(mProcs[12]); \
    if (VerInstallFileW_ptr) return VerInstallFileW_ptr(dwFlags, lpSrcFile, lpDestFile, lpTmpFile, lpResult, dwBufLen, lpBuf, lpdwLen); \
    return 0;

#define VER_LANGUAGE_NAMEA(dwLang, lpName, nSize) \
    static auto VerLanguageNameA_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPSTR, DWORD)>(mProcs[13]); \
    if (VerLanguageNameA_ptr) return VerLanguageNameA_ptr(dwLang, lpName, nSize); \
    return 0;

#define VER_LANGUAGE_NAMEW(dwLang, lpName, nSize) \
    static auto VerLanguageNameW_ptr = reinterpret_cast<UINT(__cdecl*)(DWORD, LPWSTR, DWORD)>(mProcs[14]); \
    if (VerLanguageNameW_ptr) return VerLanguageNameW_ptr(dwLang, lpName, nSize); \
    return 0;

#define VER_QUERY_VALUEA(lpBlock, lpSubBlock, lplpBuffer, puLen) \
    static auto VerQueryValueA_ptr = reinterpret_cast<BOOL(__cdecl*)(LPVOID, LPCSTR, LPVOID*, PUINT)>(mProcs[15]); \
    if (VerQueryValueA_ptr) return VerQueryValueA_ptr(lpBlock, lpSubBlock, lplpBuffer, puLen); \
    return FALSE;

#define VER_QUERY_VALUEW(lpBlock, lpSubBlock, lplpBuffer, puLen) \
    static auto VerQueryValueW_ptr = reinterpret_cast<BOOL(__cdecl*)(LPVOID, LPCWSTR, LPVOID*, PUINT)>(mProcs[16]); \
    if (VerQueryValueW_ptr) return VerQueryValueW_ptr(lpBlock, lpSubBlock, lplpBuffer, puLen); \
    return FALSE;