#pragma once

#include "proxies.hpp"

class VersionProxy {
public:
    static BOOL GetFileVersionInfoA_proxy(LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoByHandle_proxy(HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoExA_proxy(DWORD dwFlags, LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static BOOL GetFileVersionInfoExW_proxy(DWORD dwFlags, LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static DWORD GetFileVersionInfoSizeA_proxy(LPCSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeExA_proxy(DWORD dwFlags, LPCSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeExW_proxy(DWORD dwFlags, LPCWSTR lpFileName, LPDWORD lpdwHandle);
    static DWORD GetFileVersionInfoSizeW_proxy(LPCWSTR lpFileName, LPDWORD lpdwHandle);
    static BOOL GetFileVersionInfoW_proxy(LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData, LPDWORD lpdwLen);
    static UINT VerFindFileA_proxy(DWORD dwFlags, LPCSTR lpFileName, LPCSTR lpWinDir, LPCSTR lpSearchFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen);
    static UINT VerFindFileW_proxy(DWORD dwFlags, LPCWSTR lpFileName, LPCWSTR lpWinDir, LPCWSTR lpSearchFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen);
    static UINT VerInstallFileA_proxy(DWORD dwFlags, LPCSTR lpSrcFile, LPCSTR lpDestFile, LPCSTR lpTmpFile, LPCSTR lpResult, DWORD dwBufLen, LPSTR lpBuf, LPUINT lpdwLen);
    static UINT VerInstallFileW_proxy(DWORD dwFlags, LPCWSTR lpSrcFile, LPCWSTR lpDestFile, LPCWSTR lpTmpFile, LPCWSTR lpResult, DWORD dwBufLen, LPWSTR lpBuf, LPUINT lpdwLen);
    static UINT VerLanguageNameA_proxy(DWORD dwLang, LPSTR lpName, DWORD nSize);
    static UINT VerLanguageNameW_proxy(DWORD dwLang, LPWSTR lpName, DWORD nSize);
    static BOOL VerQueryValueA_proxy(LPVOID lpBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);
    static BOOL VerQueryValueW_proxy(LPVOID lpBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen);
};
