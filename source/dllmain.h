#pragma once
#include <windows.h>
#include <cctype>
#include <string>
#include <shlobj.h>
#include <stdio.h>
#include <filesystem>
#include <functional>
#include <set>
#include <ModuleList\ModuleList.hpp>
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

struct shared
{
    FARPROC DllCanUnloadNow;
    FARPROC DllGetClassObject;
    FARPROC DllRegisterServer;
    FARPROC DllUnregisterServer;
    FARPROC DebugSetMute;

    void LoadOriginalLibrary(HMODULE dll)
    {
        DllCanUnloadNow = GetProcAddress(dll, "DllCanUnloadNow");
        DllGetClassObject = GetProcAddress(dll, "DllGetClassObject");
        DllRegisterServer = GetProcAddress(dll, "DllRegisterServer");
        DllUnregisterServer = GetProcAddress(dll, "DllUnregisterServer");
        DebugSetMute = GetProcAddress(dll, "DebugSetMute");
    }
} shared;

struct dinput8_dll
{
    HMODULE dll;
    FARPROC DirectInput8Create;

    void LoadOriginalLibrary(HMODULE module)
    {
        dll = module;
        shared.LoadOriginalLibrary(dll);
        DirectInput8Create = GetProcAddress(dll, "DirectInput8Create");
    }
} dinput8;

__declspec(naked) void _DirectInput8Create() { _asm { jmp[dinput8.DirectInput8Create] } }

__declspec(naked) void _DirectInputCreateA() { _asm { jmp[dinput.DirectInputCreateA] } }
__declspec(naked) void _DirectInputCreateEx() { _asm { jmp[dinput.DirectInputCreateEx] } }
__declspec(naked) void _DirectInputCreateW() { _asm { jmp[dinput.DirectInputCreateW] } }

__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[shared.DllCanUnloadNow] } }
__declspec(naked) void _DllGetClassObject() { _asm { jmp[shared.DllGetClassObject] } }
__declspec(naked) void _DllRegisterServer() { _asm { jmp[shared.DllRegisterServer] } }
__declspec(naked) void _DllUnregisterServer() { _asm { jmp[shared.DllUnregisterServer] } }
__declspec(naked) void _DebugSetMute() { _asm { jmp[shared.DebugSetMute] } }

#if X64
#pragma runtime_checks( "", off )

#ifdef _DEBUG
#pragma message ("You are compiling the code in Debug - be warned that wrappers for export functions may not have correct code generated")
#endif

typedef HRESULT(*fn_DirectSoundCaptureCreate)(LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter);
void _DirectSoundCaptureCreate() { (fn_DirectSoundCaptureCreate)dsound.DirectSoundCaptureCreate(); }

typedef HRESULT(*fn_DirectSoundCaptureCreate8)(LPCGUID lpcGUID, LPDIRECTSOUNDCAPTURE8 * lplpDSC, LPUNKNOWN pUnkOuter);
void _DirectSoundCaptureCreate8() { (fn_DirectSoundCaptureCreate8)dsound.DirectSoundCaptureCreate8(); }

typedef HRESULT(*fn_DirectSoundCaptureEnumerateA)(LPDSENUMCALLBACKA lpDSEnumCallback, LPVOID lpContext);
void _DirectSoundCaptureEnumerateA() { (fn_DirectSoundCaptureEnumerateA)dsound.DirectSoundCaptureEnumerateA(); }

typedef HRESULT(*fn_DirectSoundCaptureEnumerateW)(LPDSENUMCALLBACKW lpDSEnumCallback, LPVOID lpContext);
void _DirectSoundCaptureEnumerateW() { (fn_DirectSoundCaptureEnumerateW)dsound.DirectSoundCaptureEnumerateW(); }

typedef HRESULT(*fn_DirectSoundCreate)(LPCGUID lpcGUID, LPDIRECTSOUND* ppDS, IUnknown* pUnkOuter);
void _DirectSoundCreate() { (fn_DirectSoundCreate)dsound.DirectSoundCreate(); }

typedef HRESULT(*fn_DirectSoundCreate8)(LPCGUID lpcGUID, LPDIRECTSOUND8* ppDS, IUnknown* pUnkOuter);
void _DirectSoundCreate8() { (fn_DirectSoundCreate8)dsound.DirectSoundCreate8(); }

typedef HRESULT(*fn_DirectSoundEnumerateA)(LPDSENUMCALLBACKA lpDSEnumCallback, LPVOID lpContext);
void _DirectSoundEnumerateA() { (fn_DirectSoundEnumerateA)dsound.DirectSoundEnumerateA(); }

typedef HRESULT(*fn_DirectSoundEnumerateW)(LPDSENUMCALLBACKW lpDSEnumCallback, LPVOID lpContext);
void _DirectSoundEnumerateW() { (fn_DirectSoundEnumerateW)dsound.DirectSoundEnumerateW(); }

typedef HRESULT(*fn_DirectSoundFullDuplexCreate)(const GUID* capture_dev, const GUID* render_dev, const DSCBUFFERDESC* cbufdesc, const DSBUFFERDESC* bufdesc, HWND  hwnd, DWORD level, IDirectSoundFullDuplex**  dsfd, IDirectSoundCaptureBuffer8** dscb8, IDirectSoundBuffer8** dsb8, IUnknown* outer_unk);
void _DirectSoundFullDuplexCreate() { (fn_DirectSoundFullDuplexCreate)dsound.DirectSoundFullDuplexCreate(); }

typedef HRESULT(*fn_GetDeviceID)(LPCGUID pGuidSrc, LPGUID pGuidDest);
void _GetDeviceID() { (fn_GetDeviceID)dsound.GetDeviceID(); }


typedef HRESULT(*fn_DirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);
void _DirectInput8Create() { (fn_DirectInput8Create)dinput8.DirectInput8Create(); }

void _DllRegisterServer() { shared.DllRegisterServer(); }
void _DllUnregisterServer() { shared.DllUnregisterServer(); }
void _DllCanUnloadNow() { shared.DllCanUnloadNow(); }
void _DllGetClassObject() { shared.DllGetClassObject(); }
void _DebugSetMute() { shared.DebugSetMute(); }

void _AppCacheCheckManifest() { wininet.AppCacheCheckManifest(); }
void _AppCacheCloseHandle() { wininet.AppCacheCloseHandle(); }
void _AppCacheCreateAndCommitFile() { wininet.AppCacheCreateAndCommitFile(); }
void _AppCacheDeleteGroup() { wininet.AppCacheDeleteGroup(); }
void _AppCacheDeleteIEGroup() { wininet.AppCacheDeleteIEGroup(); }
void _AppCacheDuplicateHandle() { wininet.AppCacheDuplicateHandle(); }
void _AppCacheFinalize() { wininet.AppCacheFinalize(); }
void _AppCacheFreeDownloadList() { wininet.AppCacheFreeDownloadList(); }
void _AppCacheFreeGroupList() { wininet.AppCacheFreeGroupList(); }
void _AppCacheFreeIESpace() { wininet.AppCacheFreeIESpace(); }
void _AppCacheFreeSpace() { wininet.AppCacheFreeSpace(); }
void _AppCacheGetDownloadList() { wininet.AppCacheGetDownloadList(); }
void _AppCacheGetFallbackUrl() { wininet.AppCacheGetFallbackUrl(); }
void _AppCacheGetGroupList() { wininet.AppCacheGetGroupList(); }
void _AppCacheGetIEGroupList() { wininet.AppCacheGetIEGroupList(); }
void _AppCacheGetInfo() { wininet.AppCacheGetInfo(); }
void _AppCacheGetManifestUrl() { wininet.AppCacheGetManifestUrl(); }
void _AppCacheLookup() { wininet.AppCacheLookup(); }
void _CommitUrlCacheEntryA() { wininet.CommitUrlCacheEntryA(); }
void _CommitUrlCacheEntryBinaryBlob() { wininet.CommitUrlCacheEntryBinaryBlob(); }
void _CommitUrlCacheEntryW() { wininet.CommitUrlCacheEntryW(); }
void _CreateMD5SSOHash() { wininet.CreateMD5SSOHash(); }
void _CreateUrlCacheContainerA() { wininet.CreateUrlCacheContainerA(); }
void _CreateUrlCacheContainerW() { wininet.CreateUrlCacheContainerW(); }
void _CreateUrlCacheEntryA() { wininet.CreateUrlCacheEntryA(); }
void _CreateUrlCacheEntryExW() { wininet.CreateUrlCacheEntryExW(); }
void _CreateUrlCacheEntryW() { wininet.CreateUrlCacheEntryW(); }
void _CreateUrlCacheGroup() { wininet.CreateUrlCacheGroup(); }
void _DeleteIE3Cache() { wininet.DeleteIE3Cache(); }
void _DeleteUrlCacheContainerA() { wininet.DeleteUrlCacheContainerA(); }
void _DeleteUrlCacheContainerW() { wininet.DeleteUrlCacheContainerW(); }
void _DeleteUrlCacheEntry() { wininet.DeleteUrlCacheEntry(); }
void _DeleteUrlCacheEntryA() { wininet.DeleteUrlCacheEntryA(); }
void _DeleteUrlCacheEntryW() { wininet.DeleteUrlCacheEntryW(); }
void _DeleteUrlCacheGroup() { wininet.DeleteUrlCacheGroup(); }
void _DeleteWpadCacheForNetworks() { wininet.DeleteWpadCacheForNetworks(); }
void _DetectAutoProxyUrl() { wininet.DetectAutoProxyUrl(); }
void _DispatchAPICall() { wininet.DispatchAPICall(); }
void _DllInstall() { wininet.DllInstall(); }
void _FindCloseUrlCache() { wininet.FindCloseUrlCache(); }
void _FindFirstUrlCacheContainerA() { wininet.FindFirstUrlCacheContainerA(); }
void _FindFirstUrlCacheContainerW() { wininet.FindFirstUrlCacheContainerW(); }
void _FindFirstUrlCacheEntryA() { wininet.FindFirstUrlCacheEntryA(); }
void _FindFirstUrlCacheEntryExA() { wininet.FindFirstUrlCacheEntryExA(); }
void _FindFirstUrlCacheEntryExW() { wininet.FindFirstUrlCacheEntryExW(); }
void _FindFirstUrlCacheEntryW() { wininet.FindFirstUrlCacheEntryW(); }
void _FindFirstUrlCacheGroup() { wininet.FindFirstUrlCacheGroup(); }
void _FindNextUrlCacheContainerA() { wininet.FindNextUrlCacheContainerA(); }
void _FindNextUrlCacheContainerW() { wininet.FindNextUrlCacheContainerW(); }
void _FindNextUrlCacheEntryA() { wininet.FindNextUrlCacheEntryA(); }
void _FindNextUrlCacheEntryExA() { wininet.FindNextUrlCacheEntryExA(); }
void _FindNextUrlCacheEntryExW() { wininet.FindNextUrlCacheEntryExW(); }
void _FindNextUrlCacheEntryW() { wininet.FindNextUrlCacheEntryW(); }
void _FindNextUrlCacheGroup() { wininet.FindNextUrlCacheGroup(); }
void _ForceNexusLookup() { wininet.ForceNexusLookup(); }
void _ForceNexusLookupExW() { wininet.ForceNexusLookupExW(); }
void _FreeUrlCacheSpaceA() { wininet.FreeUrlCacheSpaceA(); }
void _FreeUrlCacheSpaceW() { wininet.FreeUrlCacheSpaceW(); }
void _FtpCommandA() { wininet.FtpCommandA(); }
void _FtpCommandW() { wininet.FtpCommandW(); }
void _FtpCreateDirectoryA() { wininet.FtpCreateDirectoryA(); }
void _FtpCreateDirectoryW() { wininet.FtpCreateDirectoryW(); }
void _FtpDeleteFileA() { wininet.FtpDeleteFileA(); }
void _FtpDeleteFileW() { wininet.FtpDeleteFileW(); }
void _FtpFindFirstFileA() { wininet.FtpFindFirstFileA(); }
void _FtpFindFirstFileW() { wininet.FtpFindFirstFileW(); }
void _FtpGetCurrentDirectoryA() { wininet.FtpGetCurrentDirectoryA(); }
void _FtpGetCurrentDirectoryW() { wininet.FtpGetCurrentDirectoryW(); }
void _FtpGetFileA() { wininet.FtpGetFileA(); }
void _FtpGetFileEx() { wininet.FtpGetFileEx(); }
void _FtpGetFileSize() { wininet.FtpGetFileSize(); }
void _FtpGetFileW() { wininet.FtpGetFileW(); }
void _FtpOpenFileA() { wininet.FtpOpenFileA(); }
void _FtpOpenFileW() { wininet.FtpOpenFileW(); }
void _FtpPutFileA() { wininet.FtpPutFileA(); }
void _FtpPutFileEx() { wininet.FtpPutFileEx(); }
void _FtpPutFileW() { wininet.FtpPutFileW(); }
void _FtpRemoveDirectoryA() { wininet.FtpRemoveDirectoryA(); }
void _FtpRemoveDirectoryW() { wininet.FtpRemoveDirectoryW(); }
void _FtpRenameFileA() { wininet.FtpRenameFileA(); }
void _FtpRenameFileW() { wininet.FtpRenameFileW(); }
void _FtpSetCurrentDirectoryA() { wininet.FtpSetCurrentDirectoryA(); }
void _FtpSetCurrentDirectoryW() { wininet.FtpSetCurrentDirectoryW(); }
void __GetFileExtensionFromUrl() { wininet._GetFileExtensionFromUrl(); }
void _GetProxyDllInfo() { wininet.GetProxyDllInfo(); }
void _GetUrlCacheConfigInfoA() { wininet.GetUrlCacheConfigInfoA(); }
void _GetUrlCacheConfigInfoW() { wininet.GetUrlCacheConfigInfoW(); }
void _GetUrlCacheEntryBinaryBlob() { wininet.GetUrlCacheEntryBinaryBlob(); }
void _GetUrlCacheEntryInfoA() { wininet.GetUrlCacheEntryInfoA(); }
void _GetUrlCacheEntryInfoExA() { wininet.GetUrlCacheEntryInfoExA(); }
void _GetUrlCacheEntryInfoExW() { wininet.GetUrlCacheEntryInfoExW(); }
void _GetUrlCacheEntryInfoW() { wininet.GetUrlCacheEntryInfoW(); }
void _GetUrlCacheGroupAttributeA() { wininet.GetUrlCacheGroupAttributeA(); }
void _GetUrlCacheGroupAttributeW() { wininet.GetUrlCacheGroupAttributeW(); }
void _GetUrlCacheHeaderData() { wininet.GetUrlCacheHeaderData(); }
void _GopherCreateLocatorA() { wininet.GopherCreateLocatorA(); }
void _GopherCreateLocatorW() { wininet.GopherCreateLocatorW(); }
void _GopherFindFirstFileA() { wininet.GopherFindFirstFileA(); }
void _GopherFindFirstFileW() { wininet.GopherFindFirstFileW(); }
void _GopherGetAttributeA() { wininet.GopherGetAttributeA(); }
void _GopherGetAttributeW() { wininet.GopherGetAttributeW(); }
void _GopherGetLocatorTypeA() { wininet.GopherGetLocatorTypeA(); }
void _GopherGetLocatorTypeW() { wininet.GopherGetLocatorTypeW(); }
void _GopherOpenFileA() { wininet.GopherOpenFileA(); }
void _GopherOpenFileW() { wininet.GopherOpenFileW(); }
void _HttpAddRequestHeadersA() { wininet.HttpAddRequestHeadersA(); }
void _HttpAddRequestHeadersW() { wininet.HttpAddRequestHeadersW(); }
void _HttpCheckDavCompliance() { wininet.HttpCheckDavCompliance(); }
void _HttpCloseDependencyHandle() { wininet.HttpCloseDependencyHandle(); }
void _HttpDuplicateDependencyHandle() { wininet.HttpDuplicateDependencyHandle(); }
void _HttpEndRequestA() { wininet.HttpEndRequestA(); }
void _HttpEndRequestW() { wininet.HttpEndRequestW(); }
void _HttpGetServerCredentials() { wininet.HttpGetServerCredentials(); }
void _HttpGetTunnelSocket() { wininet.HttpGetTunnelSocket(); }
void _HttpIsHostHstsEnabled() { wininet.HttpIsHostHstsEnabled(); }
void _HttpOpenDependencyHandle() { wininet.HttpOpenDependencyHandle(); }
void _HttpOpenRequestA() { wininet.HttpOpenRequestA(); }
void _HttpOpenRequestW() { wininet.HttpOpenRequestW(); }
void _HttpPushClose() { wininet.HttpPushClose(); }
void _HttpPushEnable() { wininet.HttpPushEnable(); }
void _HttpPushWait() { wininet.HttpPushWait(); }
void _HttpQueryInfoA() { wininet.HttpQueryInfoA(); }
void _HttpQueryInfoW() { wininet.HttpQueryInfoW(); }
void _HttpSendRequestA() { wininet.HttpSendRequestA(); }
void _HttpSendRequestExA() { wininet.HttpSendRequestExA(); }
void _HttpSendRequestExW() { wininet.HttpSendRequestExW(); }
void _HttpSendRequestW() { wininet.HttpSendRequestW(); }
void _HttpWebSocketClose() { wininet.HttpWebSocketClose(); }
void _HttpWebSocketCompleteUpgrade() { wininet.HttpWebSocketCompleteUpgrade(); }
void _HttpWebSocketQueryCloseStatus() { wininet.HttpWebSocketQueryCloseStatus(); }
void _HttpWebSocketReceive() { wininet.HttpWebSocketReceive(); }
void _HttpWebSocketSend() { wininet.HttpWebSocketSend(); }
void _HttpWebSocketShutdown() { wininet.HttpWebSocketShutdown(); }
void _IncrementUrlCacheHeaderData() { wininet.IncrementUrlCacheHeaderData(); }
void _InternetAlgIdToStringA() { wininet.InternetAlgIdToStringA(); }
void _InternetAlgIdToStringW() { wininet.InternetAlgIdToStringW(); }
void _InternetAttemptConnect() { wininet.InternetAttemptConnect(); }
void _InternetAutodial() { wininet.InternetAutodial(); }
void _InternetAutodialCallback() { wininet.InternetAutodialCallback(); }
void _InternetAutodialHangup() { wininet.InternetAutodialHangup(); }
void _InternetCanonicalizeUrlA() { wininet.InternetCanonicalizeUrlA(); }
void _InternetCanonicalizeUrlW() { wininet.InternetCanonicalizeUrlW(); }
void _InternetCheckConnectionA() { wininet.InternetCheckConnectionA(); }
void _InternetCheckConnectionW() { wininet.InternetCheckConnectionW(); }
void _InternetClearAllPerSiteCookieDecisions() { wininet.InternetClearAllPerSiteCookieDecisions(); }
void _InternetCloseHandle() { wininet.InternetCloseHandle(); }
void _InternetCombineUrlA() { wininet.InternetCombineUrlA(); }
void _InternetCombineUrlW() { wininet.InternetCombineUrlW(); }
void _InternetConfirmZoneCrossing() { wininet.InternetConfirmZoneCrossing(); }
void _InternetConfirmZoneCrossingA() { wininet.InternetConfirmZoneCrossingA(); }
void _InternetConfirmZoneCrossingW() { wininet.InternetConfirmZoneCrossingW(); }
void _InternetConnectA() { wininet.InternetConnectA(); }
void _InternetConnectW() { wininet.InternetConnectW(); }
void _InternetConvertUrlFromWireToWideChar() { wininet.InternetConvertUrlFromWireToWideChar(); }
void _InternetCrackUrlA() { wininet.InternetCrackUrlA(); }
void _InternetCrackUrlW() { wininet.InternetCrackUrlW(); }
void _InternetCreateUrlA() { wininet.InternetCreateUrlA(); }
void _InternetCreateUrlW() { wininet.InternetCreateUrlW(); }
void _InternetDial() { wininet.InternetDial(); }
void _InternetDialA() { wininet.InternetDialA(); }
void _InternetDialW() { wininet.InternetDialW(); }
void _InternetEnumPerSiteCookieDecisionA() { wininet.InternetEnumPerSiteCookieDecisionA(); }
void _InternetEnumPerSiteCookieDecisionW() { wininet.InternetEnumPerSiteCookieDecisionW(); }
void _InternetErrorDlg() { wininet.InternetErrorDlg(); }
void _InternetFindNextFileA() { wininet.InternetFindNextFileA(); }
void _InternetFindNextFileW() { wininet.InternetFindNextFileW(); }
void _InternetFortezzaCommand() { wininet.InternetFortezzaCommand(); }
void _InternetFreeCookies() { wininet.InternetFreeCookies(); }
void _InternetFreeProxyInfoList() { wininet.InternetFreeProxyInfoList(); }
void _InternetGetCertByURL() { wininet.InternetGetCertByURL(); }
void _InternetGetCertByURLA() { wininet.InternetGetCertByURLA(); }
void _InternetGetConnectedState() { wininet.InternetGetConnectedState(); }
void _InternetGetConnectedStateEx() { wininet.InternetGetConnectedStateEx(); }
void _InternetGetConnectedStateExA() { wininet.InternetGetConnectedStateExA(); }
void _InternetGetConnectedStateExW() { wininet.InternetGetConnectedStateExW(); }
void _InternetGetCookieA() { wininet.InternetGetCookieA(); }
void _InternetGetCookieEx2() { wininet.InternetGetCookieEx2(); }
void _InternetGetCookieExA() { wininet.InternetGetCookieExA(); }
void _InternetGetCookieExW() { wininet.InternetGetCookieExW(); }
void _InternetGetCookieW() { wininet.InternetGetCookieW(); }
void _InternetGetLastResponseInfoA() { wininet.InternetGetLastResponseInfoA(); }
void _InternetGetLastResponseInfoW() { wininet.InternetGetLastResponseInfoW(); }
void _InternetGetPerSiteCookieDecisionA() { wininet.InternetGetPerSiteCookieDecisionA(); }
void _InternetGetPerSiteCookieDecisionW() { wininet.InternetGetPerSiteCookieDecisionW(); }
void _InternetGetProxyForUrl() { wininet.InternetGetProxyForUrl(); }
void _InternetGetSecurityInfoByURL() { wininet.InternetGetSecurityInfoByURL(); }
void _InternetGetSecurityInfoByURLA() { wininet.InternetGetSecurityInfoByURLA(); }
void _InternetGetSecurityInfoByURLW() { wininet.InternetGetSecurityInfoByURLW(); }
void _InternetGoOnline() { wininet.InternetGoOnline(); }
void _InternetGoOnlineA() { wininet.InternetGoOnlineA(); }
void _InternetGoOnlineW() { wininet.InternetGoOnlineW(); }
void _InternetHangUp() { wininet.InternetHangUp(); }
void _InternetInitializeAutoProxyDll() { wininet.InternetInitializeAutoProxyDll(); }
void _InternetLockRequestFile() { wininet.InternetLockRequestFile(); }
void _InternetOpenA() { wininet.InternetOpenA(); }
void _InternetOpenUrlA() { wininet.InternetOpenUrlA(); }
void _InternetOpenUrlW() { wininet.InternetOpenUrlW(); }
void _InternetOpenW() { wininet.InternetOpenW(); }
void _InternetQueryDataAvailable() { wininet.InternetQueryDataAvailable(); }
void _InternetQueryFortezzaStatus() { wininet.InternetQueryFortezzaStatus(); }
void _InternetQueryOptionA() { wininet.InternetQueryOptionA(); }
void _InternetQueryOptionW() { wininet.InternetQueryOptionW(); }
void _InternetReadFile() { wininet.InternetReadFile(); }
void _InternetReadFileExA() { wininet.InternetReadFileExA(); }
void _InternetReadFileExW() { wininet.InternetReadFileExW(); }
void _InternetSecurityProtocolToStringA() { wininet.InternetSecurityProtocolToStringA(); }
void _InternetSecurityProtocolToStringW() { wininet.InternetSecurityProtocolToStringW(); }
void _InternetSetCookieA() { wininet.InternetSetCookieA(); }
void _InternetSetCookieEx2() { wininet.InternetSetCookieEx2(); }
void _InternetSetCookieExA() { wininet.InternetSetCookieExA(); }
void _InternetSetCookieExW() { wininet.InternetSetCookieExW(); }
void _InternetSetCookieW() { wininet.InternetSetCookieW(); }
void _InternetSetDialState() { wininet.InternetSetDialState(); }
void _InternetSetDialStateA() { wininet.InternetSetDialStateA(); }
void _InternetSetDialStateW() { wininet.InternetSetDialStateW(); }
void _InternetSetFilePointer() { wininet.InternetSetFilePointer(); }
void _InternetSetOptionA() { wininet.InternetSetOptionA(); }
void _InternetSetOptionExA() { wininet.InternetSetOptionExA(); }
void _InternetSetOptionExW() { wininet.InternetSetOptionExW(); }
void _InternetSetOptionW() { wininet.InternetSetOptionW(); }
void _InternetSetPerSiteCookieDecisionA() { wininet.InternetSetPerSiteCookieDecisionA(); }
void _InternetSetPerSiteCookieDecisionW() { wininet.InternetSetPerSiteCookieDecisionW(); }
void _InternetSetStatusCallback() { wininet.InternetSetStatusCallback(); }
void _InternetSetStatusCallbackA() { wininet.InternetSetStatusCallbackA(); }
void _InternetSetStatusCallbackW() { wininet.InternetSetStatusCallbackW(); }
void _InternetShowSecurityInfoByURL() { wininet.InternetShowSecurityInfoByURL(); }
void _InternetShowSecurityInfoByURLA() { wininet.InternetShowSecurityInfoByURLA(); }
void _InternetShowSecurityInfoByURLW() { wininet.InternetShowSecurityInfoByURLW(); }
void _InternetTimeFromSystemTime() { wininet.InternetTimeFromSystemTime(); }
void _InternetTimeFromSystemTimeA() { wininet.InternetTimeFromSystemTimeA(); }
void _InternetTimeFromSystemTimeW() { wininet.InternetTimeFromSystemTimeW(); }
void _InternetTimeToSystemTime() { wininet.InternetTimeToSystemTime(); }
void _InternetTimeToSystemTimeA() { wininet.InternetTimeToSystemTimeA(); }
void _InternetTimeToSystemTimeW() { wininet.InternetTimeToSystemTimeW(); }
void _InternetUnlockRequestFile() { wininet.InternetUnlockRequestFile(); }
void _InternetWriteFile() { wininet.InternetWriteFile(); }
void _InternetWriteFileExA() { wininet.InternetWriteFileExA(); }
void _InternetWriteFileExW() { wininet.InternetWriteFileExW(); }
void _IsHostInProxyBypassList() { wininet.IsHostInProxyBypassList(); }
void _IsUrlCacheEntryExpiredA() { wininet.IsUrlCacheEntryExpiredA(); }
void _IsUrlCacheEntryExpiredW() { wininet.IsUrlCacheEntryExpiredW(); }
void _LoadUrlCacheContent() { wininet.LoadUrlCacheContent(); }
void _ParseX509EncodedCertificateForListBoxEntry() { wininet.ParseX509EncodedCertificateForListBoxEntry(); }
void _PrivacyGetZonePreferenceW() { wininet.PrivacyGetZonePreferenceW(); }
void _PrivacySetZonePreferenceW() { wininet.PrivacySetZonePreferenceW(); }
void _ReadUrlCacheEntryStream() { wininet.ReadUrlCacheEntryStream(); }
void _ReadUrlCacheEntryStreamEx() { wininet.ReadUrlCacheEntryStreamEx(); }
void _RegisterUrlCacheNotification() { wininet.RegisterUrlCacheNotification(); }
void _ResumeSuspendedDownload() { wininet.ResumeSuspendedDownload(); }
void _RetrieveUrlCacheEntryFileA() { wininet.RetrieveUrlCacheEntryFileA(); }
void _RetrieveUrlCacheEntryFileW() { wininet.RetrieveUrlCacheEntryFileW(); }
void _RetrieveUrlCacheEntryStreamA() { wininet.RetrieveUrlCacheEntryStreamA(); }
void _RetrieveUrlCacheEntryStreamW() { wininet.RetrieveUrlCacheEntryStreamW(); }
void _RunOnceUrlCache() { wininet.RunOnceUrlCache(); }
void _SetUrlCacheConfigInfoA() { wininet.SetUrlCacheConfigInfoA(); }
void _SetUrlCacheConfigInfoW() { wininet.SetUrlCacheConfigInfoW(); }
void _SetUrlCacheEntryGroup() { wininet.SetUrlCacheEntryGroup(); }
void _SetUrlCacheEntryGroupA() { wininet.SetUrlCacheEntryGroupA(); }
void _SetUrlCacheEntryGroupW() { wininet.SetUrlCacheEntryGroupW(); }
void _SetUrlCacheEntryInfoA() { wininet.SetUrlCacheEntryInfoA(); }
void _SetUrlCacheEntryInfoW() { wininet.SetUrlCacheEntryInfoW(); }
void _SetUrlCacheGroupAttributeA() { wininet.SetUrlCacheGroupAttributeA(); }
void _SetUrlCacheGroupAttributeW() { wininet.SetUrlCacheGroupAttributeW(); }
void _SetUrlCacheHeaderData() { wininet.SetUrlCacheHeaderData(); }
void _ShowCertificate() { wininet.ShowCertificate(); }
void _ShowClientAuthCerts() { wininet.ShowClientAuthCerts(); }
void _ShowSecurityInfo() { wininet.ShowSecurityInfo(); }
void _ShowX509EncodedCertificate() { wininet.ShowX509EncodedCertificate(); }
void _UnlockUrlCacheEntryFile() { wininet.UnlockUrlCacheEntryFile(); }
void _UnlockUrlCacheEntryFileA() { wininet.UnlockUrlCacheEntryFileA(); }
void _UnlockUrlCacheEntryFileW() { wininet.UnlockUrlCacheEntryFileW(); }
void _UnlockUrlCacheEntryStream() { wininet.UnlockUrlCacheEntryStream(); }
void _UpdateUrlCacheContentPath() { wininet.UpdateUrlCacheContentPath(); }
void _UrlCacheCheckEntriesExist() { wininet.UrlCacheCheckEntriesExist(); }
void _UrlCacheCloseEntryHandle() { wininet.UrlCacheCloseEntryHandle(); }
void _UrlCacheContainerSetEntryMaximumAge() { wininet.UrlCacheContainerSetEntryMaximumAge(); }
void _UrlCacheCreateContainer() { wininet.UrlCacheCreateContainer(); }
void _UrlCacheFindFirstEntry() { wininet.UrlCacheFindFirstEntry(); }
void _UrlCacheFindNextEntry() { wininet.UrlCacheFindNextEntry(); }
void _UrlCacheFreeEntryInfo() { wininet.UrlCacheFreeEntryInfo(); }
void _UrlCacheFreeGlobalSpace() { wininet.UrlCacheFreeGlobalSpace(); }
void _UrlCacheGetContentPaths() { wininet.UrlCacheGetContentPaths(); }
void _UrlCacheGetEntryInfo() { wininet.UrlCacheGetEntryInfo(); }
void _UrlCacheGetGlobalCacheSize() { wininet.UrlCacheGetGlobalCacheSize(); }
void _UrlCacheGetGlobalLimit() { wininet.UrlCacheGetGlobalLimit(); }
void _UrlCacheReadEntryStream() { wininet.UrlCacheReadEntryStream(); }
void _UrlCacheReloadSettings() { wininet.UrlCacheReloadSettings(); }
void _UrlCacheRetrieveEntryFile() { wininet.UrlCacheRetrieveEntryFile(); }
void _UrlCacheRetrieveEntryStream() { wininet.UrlCacheRetrieveEntryStream(); }
void _UrlCacheServer() { wininet.UrlCacheServer(); }
void _UrlCacheSetGlobalLimit() { wininet.UrlCacheSetGlobalLimit(); }
void _UrlCacheUpdateEntryExtraData() { wininet.UrlCacheUpdateEntryExtraData(); }
void _UrlZonesDetach() { wininet.UrlZonesDetach(); }

void _GetFileVersionInfoA() { version.GetFileVersionInfoA(); }
void _GetFileVersionInfoByHandle() { version.GetFileVersionInfoByHandle(); }
void _GetFileVersionInfoExA() { version.GetFileVersionInfoExA(); }
void _GetFileVersionInfoExW() { version.GetFileVersionInfoExW(); }
void _GetFileVersionInfoSizeA() { version.GetFileVersionInfoSizeA(); }
void _GetFileVersionInfoSizeExA() { version.GetFileVersionInfoSizeExA(); }
void _GetFileVersionInfoSizeExW() { version.GetFileVersionInfoSizeExW(); }
void _GetFileVersionInfoSizeW() { version.GetFileVersionInfoSizeW(); }
void _GetFileVersionInfoW() { version.GetFileVersionInfoW(); }
void _VerFindFileA() { version.VerFindFileA(); }
void _VerFindFileW() { version.VerFindFileW(); }
void _VerInstallFileA() { version.VerInstallFileA(); }
void _VerInstallFileW() { version.VerInstallFileW(); }
void _VerLanguageNameA() { version.VerLanguageNameA(); }
void _VerLanguageNameW() { version.VerLanguageNameW(); }
void _VerQueryValueA() { version.VerQueryValueA(); }
void _VerQueryValueW() { version.VerQueryValueW(); }

void _D3DPERF_BeginEvent() { d3d9.D3DPERF_BeginEvent(); }
void _D3DPERF_EndEvent() { d3d9.D3DPERF_EndEvent(); }
void _D3DPERF_GetStatus() { d3d9.D3DPERF_GetStatus(); }
void _D3DPERF_QueryRepeatFrame() { d3d9.D3DPERF_QueryRepeatFrame(); }
void _D3DPERF_SetMarker() { d3d9.D3DPERF_SetMarker(); }
void _D3DPERF_SetOptions() { d3d9.D3DPERF_SetOptions(); }
void _D3DPERF_SetRegion() { d3d9.D3DPERF_SetRegion(); }
void _DebugSetLevel() { d3d9.DebugSetLevel(); }
//void _DebugSetMute() { d3d9.DebugSetMute(); }
void _Direct3D9EnableMaximizedWindowedModeShim() { d3d9.Direct3D9EnableMaximizedWindowedModeShim(); }
void _Direct3DCreate9() { d3d9.Direct3DCreate9(); }
void _Direct3DCreate9Ex() { d3d9.Direct3DCreate9Ex(); }
void _Direct3DCreate9On12() { d3d9.Direct3DCreate9On12(); }
void _Direct3DCreate9On12Ex() { d3d9.Direct3DCreate9On12Ex(); }
void _Direct3DShaderValidatorCreate9() { d3d9.Direct3DShaderValidatorCreate9(); }
void _PSGPError() { d3d9.PSGPError(); }
void _PSGPSampleTexture() { d3d9.PSGPSampleTexture(); }

void _D3D10CompileEffectFromMemory() { d3d10.D3D10CompileEffectFromMemory(); }
void _D3D10CompileShader() { d3d10.D3D10CompileShader(); }
void _D3D10CreateBlob() { d3d10.D3D10CreateBlob(); }
void _D3D10CreateDevice() { d3d10.D3D10CreateDevice(); }
void _D3D10CreateDeviceAndSwapChain() { d3d10.D3D10CreateDeviceAndSwapChain(); }
void _D3D10CreateEffectFromMemory() { d3d10.D3D10CreateEffectFromMemory(); }
void _D3D10CreateEffectPoolFromMemory() { d3d10.D3D10CreateEffectPoolFromMemory(); }
void _D3D10CreateStateBlock() { d3d10.D3D10CreateStateBlock(); }
void _D3D10DisassembleEffect() { d3d10.D3D10DisassembleEffect(); }
void _D3D10DisassembleShader() { d3d10.D3D10DisassembleShader(); }
void _D3D10GetGeometryShaderProfile() { d3d10.D3D10GetGeometryShaderProfile(); }
void _D3D10GetInputAndOutputSignatureBlob() { d3d10.D3D10GetInputAndOutputSignatureBlob(); }
void _D3D10GetInputSignatureBlob() { d3d10.D3D10GetInputSignatureBlob(); }
void _D3D10GetOutputSignatureBlob() { d3d10.D3D10GetOutputSignatureBlob(); }
void _D3D10GetPixelShaderProfile() { d3d10.D3D10GetPixelShaderProfile(); }
void _D3D10GetShaderDebugInfo() { d3d10.D3D10GetShaderDebugInfo(); }
void _D3D10GetVersion() { d3d10.D3D10GetVersion(); }
void _D3D10GetVertexShaderProfile() { d3d10.D3D10GetVertexShaderProfile(); }
void _D3D10PreprocessShader() { d3d10.D3D10PreprocessShader(); }
void _D3D10ReflectShader() { d3d10.D3D10ReflectShader(); }
void _D3D10RegisterLayers() { d3d10.D3D10RegisterLayers(); }
void _D3D10StateBlockMaskDifference() { d3d10.D3D10StateBlockMaskDifference(); }
void _D3D10StateBlockMaskDisableAll() { d3d10.D3D10StateBlockMaskDisableAll(); }
void _D3D10StateBlockMaskDisableCapture() { d3d10.D3D10StateBlockMaskDisableCapture(); }
void _D3D10StateBlockMaskEnableAll() { d3d10.D3D10StateBlockMaskEnableAll(); }
void _D3D10StateBlockMaskEnableCapture() { d3d10.D3D10StateBlockMaskEnableCapture(); }
void _D3D10StateBlockMaskGetSetting() { d3d10.D3D10StateBlockMaskGetSetting(); }
void _D3D10StateBlockMaskIntersect() { d3d10.D3D10StateBlockMaskIntersect(); }
void _D3D10StateBlockMaskUnion() { d3d10.D3D10StateBlockMaskUnion(); }

void _CreateDirect3D11DeviceFromDXGIDevice() { d3d11.CreateDirect3D11DeviceFromDXGIDevice(); }
void _CreateDirect3D11SurfaceFromDXGISurface() { d3d11.CreateDirect3D11SurfaceFromDXGISurface(); }
void _D3D11CoreCreateDevice() { d3d11.D3D11CoreCreateDevice(); }
void _D3D11CoreCreateLayeredDevice() { d3d11.D3D11CoreCreateLayeredDevice(); }
void _D3D11CoreGetLayeredDeviceSize() { d3d11.D3D11CoreGetLayeredDeviceSize(); }
void _D3D11CoreRegisterLayers() { d3d11.D3D11CoreRegisterLayers(); }
void _D3D11CreateDevice() { d3d11.D3D11CreateDevice(); }
void _D3D11CreateDeviceAndSwapChain() { d3d11.D3D11CreateDeviceAndSwapChain(); }
void _D3D11CreateDeviceForD3D12() { d3d11.D3D11CreateDeviceForD3D12(); }
void _D3D11On12CreateDevice() { d3d11.D3D11On12CreateDevice(); }
void _D3DKMTCloseAdapter() { d3d11.D3DKMTCloseAdapter(); }
void _D3DKMTCreateAllocation() { d3d11.D3DKMTCreateAllocation(); }
void _D3DKMTCreateContext() { d3d11.D3DKMTCreateContext(); }
void _D3DKMTCreateDevice() { d3d11.D3DKMTCreateDevice(); }
void _D3DKMTCreateSynchronizationObject() { d3d11.D3DKMTCreateSynchronizationObject(); }
void _D3DKMTDestroyAllocation() { d3d11.D3DKMTDestroyAllocation(); }
void _D3DKMTDestroyContext() { d3d11.D3DKMTDestroyContext(); }
void _D3DKMTDestroyDevice() { d3d11.D3DKMTDestroyDevice(); }
void _D3DKMTDestroySynchronizationObject() { d3d11.D3DKMTDestroySynchronizationObject(); }
void _D3DKMTEscape() { d3d11.D3DKMTEscape(); }
void _D3DKMTGetContextSchedulingPriority() { d3d11.D3DKMTGetContextSchedulingPriority(); }
void _D3DKMTGetDeviceState() { d3d11.D3DKMTGetDeviceState(); }
void _D3DKMTGetDisplayModeList() { d3d11.D3DKMTGetDisplayModeList(); }
void _D3DKMTGetMultisampleMethodList() { d3d11.D3DKMTGetMultisampleMethodList(); }
void _D3DKMTGetRuntimeData() { d3d11.D3DKMTGetRuntimeData(); }
void _D3DKMTGetSharedPrimaryHandle() { d3d11.D3DKMTGetSharedPrimaryHandle(); }
void _D3DKMTLock() { d3d11.D3DKMTLock(); }
void _D3DKMTOpenAdapterFromHdc() { d3d11.D3DKMTOpenAdapterFromHdc(); }
void _D3DKMTOpenResource() { d3d11.D3DKMTOpenResource(); }
void _D3DKMTPresent() { d3d11.D3DKMTPresent(); }
void _D3DKMTQueryAdapterInfo() { d3d11.D3DKMTQueryAdapterInfo(); }
void _D3DKMTQueryAllocationResidency() { d3d11.D3DKMTQueryAllocationResidency(); }
void _D3DKMTQueryResourceInfo() { d3d11.D3DKMTQueryResourceInfo(); }
void _D3DKMTRender() { d3d11.D3DKMTRender(); }
void _D3DKMTSetAllocationPriority() { d3d11.D3DKMTSetAllocationPriority(); }
void _D3DKMTSetContextSchedulingPriority() { d3d11.D3DKMTSetContextSchedulingPriority(); }
void _D3DKMTSetDisplayMode() { d3d11.D3DKMTSetDisplayMode(); }
void _D3DKMTSetDisplayPrivateDriverFormat() { d3d11.D3DKMTSetDisplayPrivateDriverFormat(); }
void _D3DKMTSetGammaRamp() { d3d11.D3DKMTSetGammaRamp(); }
void _D3DKMTSetVidPnSourceOwner() { d3d11.D3DKMTSetVidPnSourceOwner(); }
void _D3DKMTSignalSynchronizationObject() { d3d11.D3DKMTSignalSynchronizationObject(); }
void _D3DKMTUnlock() { d3d11.D3DKMTUnlock(); }
void _D3DKMTWaitForSynchronizationObject() { d3d11.D3DKMTWaitForSynchronizationObject(); }
void _D3DKMTWaitForVerticalBlankEvent() { d3d11.D3DKMTWaitForVerticalBlankEvent(); }
void _D3DPerformance_BeginEvent() { d3d11.D3DPerformance_BeginEvent(); }
void _D3DPerformance_EndEvent() { d3d11.D3DPerformance_EndEvent(); }
void _D3DPerformance_GetStatus() { d3d11.D3DPerformance_GetStatus(); }
void _D3DPerformance_SetMarker() { d3d11.D3DPerformance_SetMarker(); }
void _EnableFeatureLevelUpgrade() { d3d11.EnableFeatureLevelUpgrade(); }
void _OpenAdapter10() { d3d11.OpenAdapter10(); }
void _OpenAdapter10_2() { d3d11.OpenAdapter10_2(); }

void _D3D12CoreCreateLayeredDevice() { d3d12.D3D12CoreCreateLayeredDevice(); }
void _D3D12CoreGetLayeredDeviceSize() { d3d12.D3D12CoreGetLayeredDeviceSize(); }
void _D3D12CoreRegisterLayers() { d3d12.D3D12CoreRegisterLayers(); }
void _D3D12CreateDevice() { d3d12.D3D12CreateDevice(); }
void _D3D12CreateRootSignatureDeserializer() { d3d12.D3D12CreateRootSignatureDeserializer(); }
void _D3D12CreateVersionedRootSignatureDeserializer() { d3d12.D3D12CreateVersionedRootSignatureDeserializer(); }
void _D3D12DeviceRemovedExtendedData() { d3d12.D3D12DeviceRemovedExtendedData(); }
void _D3D12EnableExperimentalFeatures() { d3d12.D3D12EnableExperimentalFeatures(); }
void _D3D12GetDebugInterface() { d3d12.D3D12GetDebugInterface(); }
void _D3D12GetInterface() { d3d12.D3D12GetInterface(); }
void _D3D12PIXEventsReplaceBlock() { d3d12.D3D12PIXEventsReplaceBlock(); }
void _D3D12PIXGetThreadInfo() { d3d12.D3D12PIXGetThreadInfo(); }
void _D3D12PIXNotifyWakeFromFenceSignal() { d3d12.D3D12PIXNotifyWakeFromFenceSignal(); }
void _D3D12PIXReportCounter() { d3d12.D3D12PIXReportCounter(); }
void _D3D12SerializeRootSignature() { d3d12.D3D12SerializeRootSignature(); }
void _D3D12SerializeVersionedRootSignature() { d3d12.D3D12SerializeVersionedRootSignature(); }
void _GetBehaviorValue() { d3d12.GetBehaviorValue(); }
void _SetAppCompatStringPointer() { d3d12.SetAppCompatStringPointer(); }

void _BinkAllocateFrameBuffers() { bink2w64.BinkAllocateFrameBuffers(); }
void _BinkBufferBlit() { bink2w64.BinkBufferBlit(); }
void _BinkBufferCheckWinPos() { bink2w64.BinkBufferCheckWinPos(); }
void _BinkBufferClear() { bink2w64.BinkBufferClear(); }
void _BinkBufferClose() { bink2w64.BinkBufferClose(); }
void _BinkBufferGetDescription() { bink2w64.BinkBufferGetDescription(); }
void _BinkBufferGetError() { bink2w64.BinkBufferGetError(); }
void _BinkBufferLock() { bink2w64.BinkBufferLock(); }
void _BinkBufferOpen() { bink2w64.BinkBufferOpen(); }
void _BinkBufferSetDirectDraw() { bink2w64.BinkBufferSetDirectDraw(); }
void _BinkBufferSetHWND() { bink2w64.BinkBufferSetHWND(); }
void _BinkBufferSetOffset() { bink2w64.BinkBufferSetOffset(); }
void _BinkBufferSetResolution() { bink2w64.BinkBufferSetResolution(); }
void _BinkBufferSetScale() { bink2w64.BinkBufferSetScale(); }
void _BinkBufferUnlock() { bink2w64.BinkBufferUnlock(); }
void _BinkCheckCursor() { bink2w64.BinkCheckCursor(); }
void _BinkClose() { bink2w64.BinkClose(); }
void _BinkCloseTrack() { bink2w64.BinkCloseTrack(); }
void _BinkControlBackgroundIO() { bink2w64.BinkControlBackgroundIO(); }
void _BinkControlPlatformFeatures() { bink2w64.BinkControlPlatformFeatures(); }
void _BinkCopyToBuffer() { bink2w64.BinkCopyToBuffer(); }
void _BinkCopyToBufferRect() { bink2w64.BinkCopyToBufferRect(); }
void _BinkCurrentSubtitle() { bink2w64.BinkCurrentSubtitle(); }
void _BinkDDSurfaceType() { bink2w64.BinkDDSurfaceType(); }
void _BinkDX8SurfaceType() { bink2w64.BinkDX8SurfaceType(); }
void _BinkDX9SurfaceType() { bink2w64.BinkDX9SurfaceType(); }
void _BinkDoFrame() { bink2w64.BinkDoFrame(); }
void _BinkDoFrameAsync() { bink2w64.BinkDoFrameAsync(); }
void _BinkDoFrameAsyncMulti() { bink2w64.BinkDoFrameAsyncMulti(); }
void _BinkDoFrameAsyncWait() { bink2w64.BinkDoFrameAsyncWait(); }
void _BinkDoFramePlane() { bink2w64.BinkDoFramePlane(); }
void _BinkFindXAudio2WinDevice() { bink2w64.BinkFindXAudio2WinDevice(); }
void _BinkFreeGlobals() { bink2w64.BinkFreeGlobals(); }
void _BinkGetError() { bink2w64.BinkGetError(); }
void _BinkGetFrameBuffersInfo() { bink2w64.BinkGetFrameBuffersInfo(); }
void _BinkGetGPUDataBuffersInfo() { bink2w64.BinkGetGPUDataBuffersInfo(); }
void _BinkGetKeyFrame() { bink2w64.BinkGetKeyFrame(); }
void _BinkGetPalette() { bink2w64.BinkGetPalette(); }
void _BinkGetPlatformInfo() { bink2w64.BinkGetPlatformInfo(); }
void _BinkGetRealtime() { bink2w64.BinkGetRealtime(); }
void _BinkGetRects() { bink2w64.BinkGetRects(); }
void _BinkGetSubtitleByIndex() { bink2w64.BinkGetSubtitleByIndex(); }
void _BinkGetSummary() { bink2w64.BinkGetSummary(); }
void _BinkGetTrackData() { bink2w64.BinkGetTrackData(); }
void _BinkGetTrackID() { bink2w64.BinkGetTrackID(); }
void _BinkGetTrackMaxSize() { bink2w64.BinkGetTrackMaxSize(); }
void _BinkGetTrackType() { bink2w64.BinkGetTrackType(); }
void _BinkGoto() { bink2w64.BinkGoto(); }
void _BinkIsSoftwareCursor() { bink2w64.BinkIsSoftwareCursor(); }
void _BinkLoadSubtitles() { bink2w64.BinkLoadSubtitles(); }
void _BinkLogoAddress() { bink2w64.BinkLogoAddress(); }
void _BinkNextFrame() { bink2w64.BinkNextFrame(); }
void _BinkOpen() { bink2w64.BinkOpen(); }
void _BinkOpenDirectSound() { bink2w64.BinkOpenDirectSound(); }
void _BinkOpenMiles() { bink2w64.BinkOpenMiles(); }
void _BinkOpenTrack() { bink2w64.BinkOpenTrack(); }
void _BinkOpenWaveOut() { bink2w64.BinkOpenWaveOut(); }
void _BinkOpenWithOptions() { bink2w64.BinkOpenWithOptions(); }
void _BinkOpenXAudio2() { bink2w64.BinkOpenXAudio2(); }
void _BinkOpenXAudio27() { bink2w64.BinkOpenXAudio27(); }
void _BinkOpenXAudio28() { bink2w64.BinkOpenXAudio28(); }
void _BinkOpenXAudio29() { bink2w64.BinkOpenXAudio29(); }
void _BinkPause() { bink2w64.BinkPause(); }
void _BinkRegisterFrameBuffers() { bink2w64.BinkRegisterFrameBuffers(); }
void _BinkRegisterGPUDataBuffers() { bink2w64.BinkRegisterGPUDataBuffers(); }
void _BinkRequestStopAsyncThread() { bink2w64.BinkRequestStopAsyncThread(); }
void _BinkRequestStopAsyncThreadsMulti() { bink2w64.BinkRequestStopAsyncThreadsMulti(); }
void _BinkRestoreCursor() { bink2w64.BinkRestoreCursor(); }
void _BinkService() { bink2w64.BinkService(); }
void _BinkServiceSound() { bink2w64.BinkServiceSound(); }
void _BinkSetError() { bink2w64.BinkSetError(); }
void _BinkSetFileOffset() { bink2w64.BinkSetFileOffset(); }
void _BinkSetFrameRate() { bink2w64.BinkSetFrameRate(); }
void _BinkSetIO() { bink2w64.BinkSetIO(); }
void _BinkSetIOSize() { bink2w64.BinkSetIOSize(); }
void _BinkSetMemory() { bink2w64.BinkSetMemory(); }
void _BinkSetOSFileCallbacks() { bink2w64.BinkSetOSFileCallbacks(); }
void _BinkSetPan() { bink2w64.BinkSetPan(); }
void _BinkSetSimulate() { bink2w64.BinkSetSimulate(); }
void _BinkSetSoundOnOff() { bink2w64.BinkSetSoundOnOff(); }
void _BinkSetSoundSystem() { bink2w64.BinkSetSoundSystem(); }
void _BinkSetSoundSystem2() { bink2w64.BinkSetSoundSystem2(); }
void _BinkSetSoundTrack() { bink2w64.BinkSetSoundTrack(); }
void _BinkSetSpeakerVolumes() { bink2w64.BinkSetSpeakerVolumes(); }
void _BinkSetVideoOnOff() { bink2w64.BinkSetVideoOnOff(); }
void _BinkSetVolume() { bink2w64.BinkSetVolume(); }
void _BinkSetWillLoop() { bink2w64.BinkSetWillLoop(); }
void _BinkShouldSkip() { bink2w64.BinkShouldSkip(); }
void _BinkStartAsyncThread() { bink2w64.BinkStartAsyncThread(); }
void _BinkUtilCPUs() { bink2w64.BinkUtilCPUs(); }
void _BinkUtilFree() { bink2w64.BinkUtilFree(); }
void _BinkUtilMalloc() { bink2w64.BinkUtilMalloc(); }
void _BinkUtilMutexCreate() { bink2w64.BinkUtilMutexCreate(); }
void _BinkUtilMutexDestroy() { bink2w64.BinkUtilMutexDestroy(); }
void _BinkUtilMutexLock() { bink2w64.BinkUtilMutexLock(); }
void _BinkUtilMutexLockTimeOut() { bink2w64.BinkUtilMutexLockTimeOut(); }
void _BinkUtilMutexUnlock() { bink2w64.BinkUtilMutexUnlock(); }
void _BinkUtilSoundGlobalLock() { bink2w64.BinkUtilSoundGlobalLock(); }
void _BinkUtilSoundGlobalUnlock() { bink2w64.BinkUtilSoundGlobalUnlock(); }
void _BinkWait() { bink2w64.BinkWait(); }
void _BinkWaitStopAsyncThread() { bink2w64.BinkWaitStopAsyncThread(); }
void _BinkWaitStopAsyncThreadsMulti() { bink2w64.BinkWaitStopAsyncThreadsMulti(); }
void _RADTimerRead() { bink2w64.RADTimerRead(); }

void _CloseDriver() { winmm.CloseDriver(); }
void _DefDriverProc() { winmm.DefDriverProc(); }
void _DriverCallback() { winmm.DriverCallback(); }
void _DrvGetModuleHandle() { winmm.DrvGetModuleHandle(); }
void _GetDriverModuleHandle() { winmm.GetDriverModuleHandle(); }
void _NotifyCallbackData() { winmm.NotifyCallbackData(); }
void _OpenDriver() { winmm.OpenDriver(); }
void _PlaySound() { winmm.PlaySound(); }
void _PlaySoundA() { winmm.PlaySoundA(); }
void _PlaySoundW() { winmm.PlaySoundW(); }
void _SendDriverMessage() { winmm.SendDriverMessage(); }
void _WOW32DriverCallback() { winmm.WOW32DriverCallback(); }
void _WOW32ResolveMultiMediaHandle() { winmm.WOW32ResolveMultiMediaHandle(); }
void _WOWAppExit() { winmm.WOWAppExit(); }
void _aux32Message() { winmm.aux32Message(); }
void _auxGetDevCapsA() { winmm.auxGetDevCapsA(); }
void _auxGetDevCapsW() { winmm.auxGetDevCapsW(); }
void _auxGetNumDevs() { winmm.auxGetNumDevs(); }
void _auxGetVolume() { winmm.auxGetVolume(); }
void _auxOutMessage() { winmm.auxOutMessage(); }
void _auxSetVolume() { winmm.auxSetVolume(); }
void _joy32Message() { winmm.joy32Message(); }
void _joyConfigChanged() { winmm.joyConfigChanged(); }
void _joyGetDevCapsA() { winmm.joyGetDevCapsA(); }
void _joyGetDevCapsW() { winmm.joyGetDevCapsW(); }
void _joyGetNumDevs() { winmm.joyGetNumDevs(); }
void _joyGetPos() { winmm.joyGetPos(); }
void _joyGetPosEx() { winmm.joyGetPosEx(); }
void _joyGetThreshold() { winmm.joyGetThreshold(); }
void _joyReleaseCapture() { winmm.joyReleaseCapture(); }
void _joySetCapture() { winmm.joySetCapture(); }
void _joySetThreshold() { winmm.joySetThreshold(); }
void _mci32Message() { winmm.mci32Message(); }
void _mciDriverNotify() { winmm.mciDriverNotify(); }
void _mciDriverYield() { winmm.mciDriverYield(); }
void _mciExecute() { winmm.mciExecute(); }
void _mciFreeCommandResource() { winmm.mciFreeCommandResource(); }
void _mciGetCreatorTask() { winmm.mciGetCreatorTask(); }
void _mciGetDeviceIDA() { winmm.mciGetDeviceIDA(); }
void _mciGetDeviceIDFromElementIDA() { winmm.mciGetDeviceIDFromElementIDA(); }
void _mciGetDeviceIDFromElementIDW() { winmm.mciGetDeviceIDFromElementIDW(); }
void _mciGetDeviceIDW() { winmm.mciGetDeviceIDW(); }
void _mciGetDriverData() { winmm.mciGetDriverData(); }
void _mciGetErrorStringA() { winmm.mciGetErrorStringA(); }
void _mciGetErrorStringW() { winmm.mciGetErrorStringW(); }
void _mciGetYieldProc() { winmm.mciGetYieldProc(); }
void _mciLoadCommandResource() { winmm.mciLoadCommandResource(); }
void _mciSendCommandA() { winmm.mciSendCommandA(); }
void _mciSendCommandW() { winmm.mciSendCommandW(); }
void _mciSendStringA() { winmm.mciSendStringA(); }
void _mciSendStringW() { winmm.mciSendStringW(); }
void _mciSetDriverData() { winmm.mciSetDriverData(); }
void _mciSetYieldProc() { winmm.mciSetYieldProc(); }
void _mid32Message() { winmm.mid32Message(); }
void _midiConnect() { winmm.midiConnect(); }
void _midiDisconnect() { winmm.midiDisconnect(); }
void _midiInAddBuffer() { winmm.midiInAddBuffer(); }
void _midiInClose() { winmm.midiInClose(); }
void _midiInGetDevCapsA() { winmm.midiInGetDevCapsA(); }
void _midiInGetDevCapsW() { winmm.midiInGetDevCapsW(); }
void _midiInGetErrorTextA() { winmm.midiInGetErrorTextA(); }
void _midiInGetErrorTextW() { winmm.midiInGetErrorTextW(); }
void _midiInGetID() { winmm.midiInGetID(); }
void _midiInGetNumDevs() { winmm.midiInGetNumDevs(); }
void _midiInMessage() { winmm.midiInMessage(); }
void _midiInOpen() { winmm.midiInOpen(); }
void _midiInPrepareHeader() { winmm.midiInPrepareHeader(); }
void _midiInReset() { winmm.midiInReset(); }
void _midiInStart() { winmm.midiInStart(); }
void _midiInStop() { winmm.midiInStop(); }
void _midiInUnprepareHeader() { winmm.midiInUnprepareHeader(); }
void _midiOutCacheDrumPatches() { winmm.midiOutCacheDrumPatches(); }
void _midiOutCachePatches() { winmm.midiOutCachePatches(); }
void _midiOutClose() { winmm.midiOutClose(); }
void _midiOutGetDevCapsA() { winmm.midiOutGetDevCapsA(); }
void _midiOutGetDevCapsW() { winmm.midiOutGetDevCapsW(); }
void _midiOutGetErrorTextA() { winmm.midiOutGetErrorTextA(); }
void _midiOutGetErrorTextW() { winmm.midiOutGetErrorTextW(); }
void _midiOutGetID() { winmm.midiOutGetID(); }
void _midiOutGetNumDevs() { winmm.midiOutGetNumDevs(); }
void _midiOutGetVolume() { winmm.midiOutGetVolume(); }
void _midiOutLongMsg() { winmm.midiOutLongMsg(); }
void _midiOutMessage() { winmm.midiOutMessage(); }
void _midiOutOpen() { winmm.midiOutOpen(); }
void _midiOutPrepareHeader() { winmm.midiOutPrepareHeader(); }
void _midiOutReset() { winmm.midiOutReset(); }
void _midiOutSetVolume() { winmm.midiOutSetVolume(); }
void _midiOutShortMsg() { winmm.midiOutShortMsg(); }
void _midiOutUnprepareHeader() { winmm.midiOutUnprepareHeader(); }
void _midiStreamClose() { winmm.midiStreamClose(); }
void _midiStreamOpen() { winmm.midiStreamOpen(); }
void _midiStreamOut() { winmm.midiStreamOut(); }
void _midiStreamPause() { winmm.midiStreamPause(); }
void _midiStreamPosition() { winmm.midiStreamPosition(); }
void _midiStreamProperty() { winmm.midiStreamProperty(); }
void _midiStreamRestart() { winmm.midiStreamRestart(); }
void _midiStreamStop() { winmm.midiStreamStop(); }
void _mixerClose() { winmm.mixerClose(); }
void _mixerGetControlDetailsA() { winmm.mixerGetControlDetailsA(); }
void _mixerGetControlDetailsW() { winmm.mixerGetControlDetailsW(); }
void _mixerGetDevCapsA() { winmm.mixerGetDevCapsA(); }
void _mixerGetDevCapsW() { winmm.mixerGetDevCapsW(); }
void _mixerGetID() { winmm.mixerGetID(); }
void _mixerGetLineControlsA() { winmm.mixerGetLineControlsA(); }
void _mixerGetLineControlsW() { winmm.mixerGetLineControlsW(); }
void _mixerGetLineInfoA() { winmm.mixerGetLineInfoA(); }
void _mixerGetLineInfoW() { winmm.mixerGetLineInfoW(); }
void _mixerGetNumDevs() { winmm.mixerGetNumDevs(); }
void _mixerMessage() { winmm.mixerMessage(); }
void _mixerOpen() { winmm.mixerOpen(); }
void _mixerSetControlDetails() { winmm.mixerSetControlDetails(); }
void _mmDrvInstall() { winmm.mmDrvInstall(); }
void _mmGetCurrentTask() { winmm.mmGetCurrentTask(); }
void _mmTaskBlock() { winmm.mmTaskBlock(); }
void _mmTaskCreate() { winmm.mmTaskCreate(); }
void _mmTaskSignal() { winmm.mmTaskSignal(); }
void _mmTaskYield() { winmm.mmTaskYield(); }
void _mmioAdvance() { winmm.mmioAdvance(); }
void _mmioAscend() { winmm.mmioAscend(); }
void _mmioClose() { winmm.mmioClose(); }
void _mmioCreateChunk() { winmm.mmioCreateChunk(); }
void _mmioDescend() { winmm.mmioDescend(); }
void _mmioFlush() { winmm.mmioFlush(); }
void _mmioGetInfo() { winmm.mmioGetInfo(); }
void _mmioInstallIOProcA() { winmm.mmioInstallIOProcA(); }
void _mmioInstallIOProcW() { winmm.mmioInstallIOProcW(); }
void _mmioOpenA() { winmm.mmioOpenA(); }
void _mmioOpenW() { winmm.mmioOpenW(); }
void _mmioRead() { winmm.mmioRead(); }
void _mmioRenameA() { winmm.mmioRenameA(); }
void _mmioRenameW() { winmm.mmioRenameW(); }
void _mmioSeek() { winmm.mmioSeek(); }
void _mmioSendMessage() { winmm.mmioSendMessage(); }
void _mmioSetBuffer() { winmm.mmioSetBuffer(); }
void _mmioSetInfo() { winmm.mmioSetInfo(); }
void _mmioStringToFOURCCA() { winmm.mmioStringToFOURCCA(); }
void _mmioStringToFOURCCW() { winmm.mmioStringToFOURCCW(); }
void _mmioWrite() { winmm.mmioWrite(); }
void _mmsystemGetVersion() { winmm.mmsystemGetVersion(); }
void _mod32Message() { winmm.mod32Message(); }
void _mxd32Message() { winmm.mxd32Message(); }
void _sndPlaySoundA() { winmm.sndPlaySoundA(); }
void _sndPlaySoundW() { winmm.sndPlaySoundW(); }
void _tid32Message() { winmm.tid32Message(); }
void _timeBeginPeriod() { winmm.timeBeginPeriod(); }
void _timeEndPeriod() { winmm.timeEndPeriod(); }
void _timeGetDevCaps() { winmm.timeGetDevCaps(); }
void _timeGetSystemTime() { winmm.timeGetSystemTime(); }
void _timeGetTime() { winmm.timeGetTime(); }
void _timeKillEvent() { winmm.timeKillEvent(); }
void _timeSetEvent() { winmm.timeSetEvent(); }
void _waveInAddBuffer() { winmm.waveInAddBuffer(); }
void _waveInClose() { winmm.waveInClose(); }
void _waveInGetDevCapsA() { winmm.waveInGetDevCapsA(); }
void _waveInGetDevCapsW() { winmm.waveInGetDevCapsW(); }
void _waveInGetErrorTextA() { winmm.waveInGetErrorTextA(); }
void _waveInGetErrorTextW() { winmm.waveInGetErrorTextW(); }
void _waveInGetID() { winmm.waveInGetID(); }
void _waveInGetNumDevs() { winmm.waveInGetNumDevs(); }
void _waveInGetPosition() { winmm.waveInGetPosition(); }
void _waveInMessage() { winmm.waveInMessage(); }
void _waveInOpen() { winmm.waveInOpen(); }
void _waveInPrepareHeader() { winmm.waveInPrepareHeader(); }
void _waveInReset() { winmm.waveInReset(); }
void _waveInStart() { winmm.waveInStart(); }
void _waveInStop() { winmm.waveInStop(); }
void _waveInUnprepareHeader() { winmm.waveInUnprepareHeader(); }
void _waveOutBreakLoop() { winmm.waveOutBreakLoop(); }
void _waveOutClose() { winmm.waveOutClose(); }
void _waveOutGetDevCapsA() { winmm.waveOutGetDevCapsA(); }
void _waveOutGetDevCapsW() { winmm.waveOutGetDevCapsW(); }
void _waveOutGetErrorTextA() { winmm.waveOutGetErrorTextA(); }
void _waveOutGetErrorTextW() { winmm.waveOutGetErrorTextW(); }
void _waveOutGetID() { winmm.waveOutGetID(); }
void _waveOutGetNumDevs() { winmm.waveOutGetNumDevs(); }
void _waveOutGetPitch() { winmm.waveOutGetPitch(); }
void _waveOutGetPlaybackRate() { winmm.waveOutGetPlaybackRate(); }
void _waveOutGetPosition() { winmm.waveOutGetPosition(); }
void _waveOutGetVolume() { winmm.waveOutGetVolume(); }
void _waveOutMessage() { winmm.waveOutMessage(); }
void _waveOutOpen() { winmm.waveOutOpen(); }
void _waveOutPause() { winmm.waveOutPause(); }
void _waveOutPrepareHeader() { winmm.waveOutPrepareHeader(); }
void _waveOutReset() { winmm.waveOutReset(); }
void _waveOutRestart() { winmm.waveOutRestart(); }
void _waveOutSetPitch() { winmm.waveOutSetPitch(); }
void _waveOutSetPlaybackRate() { winmm.waveOutSetPlaybackRate(); }
void _waveOutSetVolume() { winmm.waveOutSetVolume(); }
void _waveOutUnprepareHeader() { winmm.waveOutUnprepareHeader(); }
void _waveOutWrite() { winmm.waveOutWrite(); }
void _wid32Message() { winmm.wid32Message(); }
void _wod32Message() { winmm.wod32Message(); }

void _Private1() { winhttp.Private1(); }
void _SvchostPushServiceGlobals() { winhttp.SvchostPushServiceGlobals(); }
void _WinHttpAddRequestHeaders() { winhttp.WinHttpAddRequestHeaders(); }
void _WinHttpAddRequestHeadersEx() { winhttp.WinHttpAddRequestHeadersEx(); }
void _WinHttpAutoProxySvcMain() { winhttp.WinHttpAutoProxySvcMain(); }
void _WinHttpCheckPlatform() { winhttp.WinHttpCheckPlatform(); }
void _WinHttpCloseHandle() { winhttp.WinHttpCloseHandle(); }
void _WinHttpConnect() { winhttp.WinHttpConnect(); }
void _WinHttpConnectionDeletePolicyEntries() { winhttp.WinHttpConnectionDeletePolicyEntries(); }
void _WinHttpConnectionDeleteProxyInfo() { winhttp.WinHttpConnectionDeleteProxyInfo(); }
void _WinHttpConnectionFreeNameList() { winhttp.WinHttpConnectionFreeNameList(); }
void _WinHttpConnectionFreeProxyInfo() { winhttp.WinHttpConnectionFreeProxyInfo(); }
void _WinHttpConnectionFreeProxyList() { winhttp.WinHttpConnectionFreeProxyList(); }
void _WinHttpConnectionGetNameList() { winhttp.WinHttpConnectionGetNameList(); }
void _WinHttpConnectionGetProxyInfo() { winhttp.WinHttpConnectionGetProxyInfo(); }
void _WinHttpConnectionGetProxyList() { winhttp.WinHttpConnectionGetProxyList(); }
void _WinHttpConnectionOnlyConvert() { winhttp.WinHttpConnectionOnlyConvert(); }
void _WinHttpConnectionOnlyReceive() { winhttp.WinHttpConnectionOnlyReceive(); }
void _WinHttpConnectionOnlySend() { winhttp.WinHttpConnectionOnlySend(); }
void _WinHttpConnectionSetPolicyEntries() { winhttp.WinHttpConnectionSetPolicyEntries(); }
void _WinHttpConnectionSetProxyInfo() { winhttp.WinHttpConnectionSetProxyInfo(); }
void _WinHttpConnectionUpdateIfIndexTable() { winhttp.WinHttpConnectionUpdateIfIndexTable(); }
void _WinHttpCrackUrl() { winhttp.WinHttpCrackUrl(); }
void _WinHttpCreateProxyResolver() { winhttp.WinHttpCreateProxyResolver(); }
void _WinHttpCreateUrl() { winhttp.WinHttpCreateUrl(); }
void _WinHttpDetectAutoProxyConfigUrl() { winhttp.WinHttpDetectAutoProxyConfigUrl(); }
void _WinHttpFreeProxyResult() { winhttp.WinHttpFreeProxyResult(); }
void _WinHttpFreeProxyResultEx() { winhttp.WinHttpFreeProxyResultEx(); }
void _WinHttpFreeProxySettings() { winhttp.WinHttpFreeProxySettings(); }
void _WinHttpFreeProxySettingsEx() { winhttp.WinHttpFreeProxySettingsEx(); }
void _WinHttpFreeQueryConnectionGroupResult() { winhttp.WinHttpFreeQueryConnectionGroupResult(); }
void _WinHttpGetDefaultProxyConfiguration() { winhttp.WinHttpGetDefaultProxyConfiguration(); }
void _WinHttpGetIEProxyConfigForCurrentUser() { winhttp.WinHttpGetIEProxyConfigForCurrentUser(); }
void _WinHttpGetProxyForUrl() { winhttp.WinHttpGetProxyForUrl(); }
void _WinHttpGetProxyForUrlEx() { winhttp.WinHttpGetProxyForUrlEx(); }
void _WinHttpGetProxyForUrlEx2() { winhttp.WinHttpGetProxyForUrlEx2(); }
void _WinHttpGetProxyForUrlHvsi() { winhttp.WinHttpGetProxyForUrlHvsi(); }
void _WinHttpGetProxyResult() { winhttp.WinHttpGetProxyResult(); }
void _WinHttpGetProxyResultEx() { winhttp.WinHttpGetProxyResultEx(); }
void _WinHttpGetProxySettingsEx() { winhttp.WinHttpGetProxySettingsEx(); }
void _WinHttpGetProxySettingsResultEx() { winhttp.WinHttpGetProxySettingsResultEx(); }
void _WinHttpGetProxySettingsVersion() { winhttp.WinHttpGetProxySettingsVersion(); }
void _WinHttpGetTunnelSocket() { winhttp.WinHttpGetTunnelSocket(); }
void _WinHttpOpen() { winhttp.WinHttpOpen(); }
void _WinHttpOpenRequest() { winhttp.WinHttpOpenRequest(); }
void _WinHttpPacJsWorkerMain() { winhttp.WinHttpPacJsWorkerMain(); }
void _WinHttpProbeConnectivity() { winhttp.WinHttpProbeConnectivity(); }
void _WinHttpQueryAuthSchemes() { winhttp.WinHttpQueryAuthSchemes(); }
void _WinHttpQueryConnectionGroup() { winhttp.WinHttpQueryConnectionGroup(); }
void _WinHttpQueryDataAvailable() { winhttp.WinHttpQueryDataAvailable(); }
void _WinHttpQueryHeaders() { winhttp.WinHttpQueryHeaders(); }
void _WinHttpQueryHeadersEx() { winhttp.WinHttpQueryHeadersEx(); }
void _WinHttpQueryOption() { winhttp.WinHttpQueryOption(); }
void _WinHttpReadData() { winhttp.WinHttpReadData(); }
void _WinHttpReadDataEx() { winhttp.WinHttpReadDataEx(); }
void _WinHttpReadProxySettings() { winhttp.WinHttpReadProxySettings(); }
void _WinHttpReadProxySettingsHvsi() { winhttp.WinHttpReadProxySettingsHvsi(); }
void _WinHttpReceiveResponse() { winhttp.WinHttpReceiveResponse(); }
void _WinHttpRegisterProxyChangeNotification() { winhttp.WinHttpRegisterProxyChangeNotification(); }
void _WinHttpResetAutoProxy() { winhttp.WinHttpResetAutoProxy(); }
void _WinHttpSaveProxyCredentials() { winhttp.WinHttpSaveProxyCredentials(); }
void _WinHttpSendRequest() { winhttp.WinHttpSendRequest(); }
void _WinHttpSetCredentials() { winhttp.WinHttpSetCredentials(); }
void _WinHttpSetDefaultProxyConfiguration() { winhttp.WinHttpSetDefaultProxyConfiguration(); }
void _WinHttpSetOption() { winhttp.WinHttpSetOption(); }
void _WinHttpSetProxySettingsPerUser() { winhttp.WinHttpSetProxySettingsPerUser(); }
void _WinHttpSetSecureLegacyServersAppCompat() { winhttp.WinHttpSetSecureLegacyServersAppCompat(); }
void _WinHttpSetStatusCallback() { winhttp.WinHttpSetStatusCallback(); }
void _WinHttpSetTimeouts() { winhttp.WinHttpSetTimeouts(); }
void _WinHttpTimeFromSystemTime() { winhttp.WinHttpTimeFromSystemTime(); }
void _WinHttpTimeToSystemTime() { winhttp.WinHttpTimeToSystemTime(); }
void _WinHttpUnregisterProxyChangeNotification() { winhttp.WinHttpUnregisterProxyChangeNotification(); }
void _WinHttpWebSocketClose() { winhttp.WinHttpWebSocketClose(); }
void _WinHttpWebSocketCompleteUpgrade() { winhttp.WinHttpWebSocketCompleteUpgrade(); }
void _WinHttpWebSocketQueryCloseStatus() { winhttp.WinHttpWebSocketQueryCloseStatus(); }
void _WinHttpWebSocketReceive() { winhttp.WinHttpWebSocketReceive(); }
void _WinHttpWebSocketSend() { winhttp.WinHttpWebSocketSend(); }
void _WinHttpWebSocketShutdown() { winhttp.WinHttpWebSocketShutdown(); }
void _WinHttpWriteData() { winhttp.WinHttpWriteData(); }
void _WinHttpWriteProxySettings() { winhttp.WinHttpWriteProxySettings(); }

#pragma runtime_checks( "", restore )
#endif