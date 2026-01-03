#pragma once

#include "wrappers.hpp"

class VersionWrapper {
public:
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
};
