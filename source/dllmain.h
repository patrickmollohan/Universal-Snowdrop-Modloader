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

__declspec(naked) void _ov_bitrate() { _asm { jmp[vorbisfile.ov_bitrate] } }
__declspec(naked) void _ov_bitrate_instant() { _asm { jmp[vorbisfile.ov_bitrate_instant] } }
__declspec(naked) void _ov_clear() { _asm { jmp[vorbisfile.ov_clear] } }
__declspec(naked) void _ov_comment() { _asm { jmp[vorbisfile.ov_comment] } }
__declspec(naked) void _ov_crosslap() { _asm { jmp[vorbisfile.ov_crosslap] } }
__declspec(naked) void _ov_fopen() { _asm { jmp[vorbisfile.ov_fopen] } }
__declspec(naked) void _ov_halfrate() { _asm { jmp[vorbisfile.ov_halfrate] } }
__declspec(naked) void _ov_halfrate_p() { _asm { jmp[vorbisfile.ov_halfrate_p] } }
__declspec(naked) void _ov_info() { _asm { jmp[vorbisfile.ov_info] } }
__declspec(naked) void _ov_open() { _asm { jmp[vorbisfile.ov_open] } }
__declspec(naked) void _ov_open_callbacks() { _asm { jmp[vorbisfile.ov_open_callbacks] } }
__declspec(naked) void _ov_pcm_seek() { _asm { jmp[vorbisfile.ov_pcm_seek] } }
__declspec(naked) void _ov_pcm_seek_lap() { _asm { jmp[vorbisfile.ov_pcm_seek_lap] } }
__declspec(naked) void _ov_pcm_seek_page() { _asm { jmp[vorbisfile.ov_pcm_seek_page] } }
__declspec(naked) void _ov_pcm_seek_page_lap() { _asm { jmp[vorbisfile.ov_pcm_seek_page_lap] } }
__declspec(naked) void _ov_pcm_tell() { _asm { jmp[vorbisfile.ov_pcm_tell] } }
__declspec(naked) void _ov_pcm_total() { _asm { jmp[vorbisfile.ov_pcm_total] } }
__declspec(naked) void _ov_raw_seek() { _asm { jmp[vorbisfile.ov_raw_seek] } }
__declspec(naked) void _ov_raw_seek_lap() { _asm { jmp[vorbisfile.ov_raw_seek_lap] } }
__declspec(naked) void _ov_raw_tell() { _asm { jmp[vorbisfile.ov_raw_tell] } }
__declspec(naked) void _ov_raw_total() { _asm { jmp[vorbisfile.ov_raw_total] } }
__declspec(naked) void _ov_read() { _asm { jmp[vorbisfile.ov_read] } }
__declspec(naked) void _ov_read_float() { _asm { jmp[vorbisfile.ov_read_float] } }
__declspec(naked) void _ov_seekable() { _asm { jmp[vorbisfile.ov_seekable] } }
__declspec(naked) void _ov_serialnumber() { _asm { jmp[vorbisfile.ov_serialnumber] } }
__declspec(naked) void _ov_streams() { _asm { jmp[vorbisfile.ov_streams] } }
__declspec(naked) void _ov_test() { _asm { jmp[vorbisfile.ov_test] } }
__declspec(naked) void _ov_test_callbacks() { _asm { jmp[vorbisfile.ov_test_callbacks] } }
__declspec(naked) void _ov_test_open() { _asm { jmp[vorbisfile.ov_test_open] } }
__declspec(naked) void _ov_time_seek() { _asm { jmp[vorbisfile.ov_time_seek] } }
__declspec(naked) void _ov_time_seek_lap() { _asm { jmp[vorbisfile.ov_time_seek_lap] } }
__declspec(naked) void _ov_time_seek_page() { _asm { jmp[vorbisfile.ov_time_seek_page] } }
__declspec(naked) void _ov_time_seek_page_lap() { _asm { jmp[vorbisfile.ov_time_seek_page_lap] } }
__declspec(naked) void _ov_time_tell() { _asm { jmp[vorbisfile.ov_time_tell] } }
__declspec(naked) void _ov_time_total() { _asm { jmp[vorbisfile.ov_time_total] } }

__declspec(naked) void _DirectInput8Create() { _asm { jmp[dinput8.DirectInput8Create] } }

__declspec(naked) void _DirectInputCreateA() { _asm { jmp[dinput.DirectInputCreateA] } }
__declspec(naked) void _DirectInputCreateEx() { _asm { jmp[dinput.DirectInputCreateEx] } }
__declspec(naked) void _DirectInputCreateW() { _asm { jmp[dinput.DirectInputCreateW] } }

__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[shared.DllCanUnloadNow] } }
__declspec(naked) void _DllGetClassObject() { _asm { jmp[shared.DllGetClassObject] } }
__declspec(naked) void _DllRegisterServer() { _asm { jmp[shared.DllRegisterServer] } }
__declspec(naked) void _DllUnregisterServer() { _asm { jmp[shared.DllUnregisterServer] } }
__declspec(naked) void _DebugSetMute() { _asm { jmp[shared.DebugSetMute] } }

__declspec(naked) void _DirectSoundCaptureCreate() { _asm { jmp[dsound.DirectSoundCaptureCreate] } }
__declspec(naked) void _DirectSoundCaptureCreate8() { _asm { jmp[dsound.DirectSoundCaptureCreate8] } }
__declspec(naked) void _DirectSoundCaptureEnumerateA() { _asm { jmp[dsound.DirectSoundCaptureEnumerateA] } }
__declspec(naked) void _DirectSoundCaptureEnumerateW() { _asm { jmp[dsound.DirectSoundCaptureEnumerateW] } }
__declspec(naked) void _DirectSoundCreate() { _asm { jmp[dsound.DirectSoundCreate] } }
__declspec(naked) void _DirectSoundCreate8() { _asm { jmp[dsound.DirectSoundCreate8] } }
__declspec(naked) void _DirectSoundEnumerateA() { _asm { jmp[dsound.DirectSoundEnumerateA] } }
__declspec(naked) void _DirectSoundEnumerateW() { _asm { jmp[dsound.DirectSoundEnumerateW] } }
__declspec(naked) void _DirectSoundFullDuplexCreate() { _asm { jmp[dsound.DirectSoundFullDuplexCreate] } }
//__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[dsound.DllCanUnloadNow] } }
//__declspec(naked) void _DllGetClassObject() { _asm { jmp[dsound.DllGetClassObject] } }
__declspec(naked) void _GetDeviceID() { _asm { jmp[dsound.GetDeviceID] } }

//__declspec(naked) void _DebugSetMute() { _asm { jmp[d3d8.DebugSetMute] } }
__declspec(naked) void _Direct3D8EnableMaximizedWindowedModeShim() { _asm { jmp[d3d8.Direct3D8EnableMaximizedWindowedModeShim] } }
__declspec(naked) void _Direct3DCreate8() { _asm { jmp[d3d8.Direct3DCreate8] } }
__declspec(naked) void _ValidatePixelShader() { _asm { jmp[d3d8.ValidatePixelShader] } }
__declspec(naked) void _ValidateVertexShader() { _asm { jmp[d3d8.ValidateVertexShader] } }

__declspec(naked) void _D3DPERF_BeginEvent() { _asm { jmp[d3d9.D3DPERF_BeginEvent] } }
__declspec(naked) void _D3DPERF_EndEvent() { _asm { jmp[d3d9.D3DPERF_EndEvent] } }
__declspec(naked) void _D3DPERF_GetStatus() { _asm { jmp[d3d9.D3DPERF_GetStatus] } }
__declspec(naked) void _D3DPERF_QueryRepeatFrame() { _asm { jmp[d3d9.D3DPERF_QueryRepeatFrame] } }
__declspec(naked) void _D3DPERF_SetMarker() { _asm { jmp[d3d9.D3DPERF_SetMarker] } }
__declspec(naked) void _D3DPERF_SetOptions() { _asm { jmp[d3d9.D3DPERF_SetOptions] } }
__declspec(naked) void _D3DPERF_SetRegion() { _asm { jmp[d3d9.D3DPERF_SetRegion] } }
__declspec(naked) void _DebugSetLevel() { _asm { jmp[d3d9.DebugSetLevel] } }
//__declspec(naked) void _DebugSetMute() { _asm { jmp[d3d9.DebugSetMute] } }
__declspec(naked) void _Direct3D9EnableMaximizedWindowedModeShim() { _asm { jmp[d3d9.Direct3D9EnableMaximizedWindowedModeShim] } }
__declspec(naked) void _Direct3DCreate9() { _asm { jmp[d3d9.Direct3DCreate9] } }
__declspec(naked) void _Direct3DCreate9Ex() { _asm { jmp[d3d9.Direct3DCreate9Ex] } }
__declspec(naked) void _Direct3DCreate9On12() { _asm { jmp[d3d9.Direct3DCreate9On12] } }
__declspec(naked) void _Direct3DCreate9On12Ex() { _asm { jmp[d3d9.Direct3DCreate9On12Ex] } }
__declspec(naked) void _Direct3DShaderValidatorCreate9() { _asm { jmp[d3d9.Direct3DShaderValidatorCreate9] } }
__declspec(naked) void _PSGPError() { _asm { jmp[d3d9.PSGPError] } }
__declspec(naked) void _PSGPSampleTexture() { _asm { jmp[d3d9.PSGPSampleTexture] } }

__declspec(naked) void _D3D10CompileEffectFromMemory() { _asm { jmp[d3d10.D3D10CompileEffectFromMemory] } }
__declspec(naked) void _D3D10CompileShader() { _asm { jmp[d3d10.D3D10CompileShader] } }
__declspec(naked) void _D3D10CreateBlob() { _asm { jmp[d3d10.D3D10CreateBlob] } }
__declspec(naked) void _D3D10CreateDevice() { _asm { jmp[d3d10.D3D10CreateDevice] } }
__declspec(naked) void _D3D10CreateDeviceAndSwapChain() { _asm { jmp[d3d10.D3D10CreateDeviceAndSwapChain] } }
__declspec(naked) void _D3D10CreateEffectFromMemory() { _asm { jmp[d3d10.D3D10CreateEffectFromMemory] } }
__declspec(naked) void _D3D10CreateEffectPoolFromMemory() { _asm { jmp[d3d10.D3D10CreateEffectPoolFromMemory] } }
__declspec(naked) void _D3D10CreateStateBlock() { _asm { jmp[d3d10.D3D10CreateStateBlock] } }
__declspec(naked) void _D3D10DisassembleEffect() { _asm { jmp[d3d10.D3D10DisassembleEffect] } }
__declspec(naked) void _D3D10DisassembleShader() { _asm { jmp[d3d10.D3D10DisassembleShader] } }
__declspec(naked) void _D3D10GetGeometryShaderProfile() { _asm { jmp[d3d10.D3D10GetGeometryShaderProfile] } }
__declspec(naked) void _D3D10GetInputAndOutputSignatureBlob() { _asm { jmp[d3d10.D3D10GetInputAndOutputSignatureBlob] } }
__declspec(naked) void _D3D10GetInputSignatureBlob() { _asm { jmp[d3d10.D3D10GetInputSignatureBlob] } }
__declspec(naked) void _D3D10GetOutputSignatureBlob() { _asm { jmp[d3d10.D3D10GetOutputSignatureBlob] } }
__declspec(naked) void _D3D10GetPixelShaderProfile() { _asm { jmp[d3d10.D3D10GetPixelShaderProfile] } }
__declspec(naked) void _D3D10GetShaderDebugInfo() { _asm { jmp[d3d10.D3D10GetShaderDebugInfo] } }
__declspec(naked) void _D3D10GetVersion() { _asm { jmp[d3d10.D3D10GetVersion] } }
__declspec(naked) void _D3D10GetVertexShaderProfile() { _asm { jmp[d3d10.D3D10GetVertexShaderProfile] } }
__declspec(naked) void _D3D10PreprocessShader() { _asm { jmp[d3d10.D3D10PreprocessShader] } }
__declspec(naked) void _D3D10ReflectShader() { _asm { jmp[d3d10.D3D10ReflectShader] } }
__declspec(naked) void _D3D10RegisterLayers() { _asm { jmp[d3d10.D3D10RegisterLayers] } }
__declspec(naked) void _D3D10StateBlockMaskDifference() { _asm { jmp[d3d10.D3D10StateBlockMaskDifference] } }
__declspec(naked) void _D3D10StateBlockMaskDisableAll() { _asm { jmp[d3d10.D3D10StateBlockMaskDisableAll] } }
__declspec(naked) void _D3D10StateBlockMaskDisableCapture() { _asm { jmp[d3d10.D3D10StateBlockMaskDisableCapture] } }
__declspec(naked) void _D3D10StateBlockMaskEnableAll() { _asm { jmp[d3d10.D3D10StateBlockMaskEnableAll] } }
__declspec(naked) void _D3D10StateBlockMaskEnableCapture() { _asm { jmp[d3d10.D3D10StateBlockMaskEnableCapture] } }
__declspec(naked) void _D3D10StateBlockMaskGetSetting() { _asm { jmp[d3d10.D3D10StateBlockMaskGetSetting] } }
__declspec(naked) void _D3D10StateBlockMaskIntersect() { _asm { jmp[d3d10.D3D10StateBlockMaskIntersect] } }
__declspec(naked) void _D3D10StateBlockMaskUnion() { _asm { jmp[d3d10.D3D10StateBlockMaskUnion] } }

__declspec(naked) void _CreateDirect3D11DeviceFromDXGIDevice() { _asm { jmp[d3d11.CreateDirect3D11DeviceFromDXGIDevice] } }
__declspec(naked) void _CreateDirect3D11SurfaceFromDXGISurface() { _asm { jmp[d3d11.CreateDirect3D11SurfaceFromDXGISurface] } }
__declspec(naked) void _D3D11CoreCreateDevice() { _asm { jmp[d3d11.D3D11CoreCreateDevice] } }
__declspec(naked) void _D3D11CoreCreateLayeredDevice() { _asm { jmp[d3d11.D3D11CoreCreateLayeredDevice] } }
__declspec(naked) void _D3D11CoreGetLayeredDeviceSize() { _asm { jmp[d3d11.D3D11CoreGetLayeredDeviceSize] } }
__declspec(naked) void _D3D11CoreRegisterLayers() { _asm { jmp[d3d11.D3D11CoreRegisterLayers] } }
__declspec(naked) void _D3D11CreateDevice() { _asm { jmp[d3d11.D3D11CreateDevice] } }
__declspec(naked) void _D3D11CreateDeviceAndSwapChain() { _asm { jmp[d3d11.D3D11CreateDeviceAndSwapChain] } }
__declspec(naked) void _D3D11CreateDeviceForD3D12() { _asm { jmp[d3d11.D3D11CreateDeviceForD3D12] } }
__declspec(naked) void _D3D11On12CreateDevice() { _asm { jmp[d3d11.D3D11On12CreateDevice] } }
__declspec(naked) void _D3DKMTCloseAdapter() { _asm { jmp[d3d11.D3DKMTCloseAdapter] } }
__declspec(naked) void _D3DKMTCreateAllocation() { _asm { jmp[d3d11.D3DKMTCreateAllocation] } }
__declspec(naked) void _D3DKMTCreateContext() { _asm { jmp[d3d11.D3DKMTCreateContext] } }
__declspec(naked) void _D3DKMTCreateDevice() { _asm { jmp[d3d11.D3DKMTCreateDevice] } }
__declspec(naked) void _D3DKMTCreateSynchronizationObject() { _asm { jmp[d3d11.D3DKMTCreateSynchronizationObject] } }
__declspec(naked) void _D3DKMTDestroyAllocation() { _asm { jmp[d3d11.D3DKMTDestroyAllocation] } }
__declspec(naked) void _D3DKMTDestroyContext() { _asm { jmp[d3d11.D3DKMTDestroyContext] } }
__declspec(naked) void _D3DKMTDestroyDevice() { _asm { jmp[d3d11.D3DKMTDestroyDevice] } }
__declspec(naked) void _D3DKMTDestroySynchronizationObject() { _asm { jmp[d3d11.D3DKMTDestroySynchronizationObject] } }
__declspec(naked) void _D3DKMTEscape() { _asm { jmp[d3d11.D3DKMTEscape] } }
__declspec(naked) void _D3DKMTGetContextSchedulingPriority() { _asm { jmp[d3d11.D3DKMTGetContextSchedulingPriority] } }
__declspec(naked) void _D3DKMTGetDeviceState() { _asm { jmp[d3d11.D3DKMTGetDeviceState] } }
__declspec(naked) void _D3DKMTGetDisplayModeList() { _asm { jmp[d3d11.D3DKMTGetDisplayModeList] } }
__declspec(naked) void _D3DKMTGetMultisampleMethodList() { _asm { jmp[d3d11.D3DKMTGetMultisampleMethodList] } }
__declspec(naked) void _D3DKMTGetRuntimeData() { _asm { jmp[d3d11.D3DKMTGetRuntimeData] } }
__declspec(naked) void _D3DKMTGetSharedPrimaryHandle() { _asm { jmp[d3d11.D3DKMTGetSharedPrimaryHandle] } }
__declspec(naked) void _D3DKMTLock() { _asm { jmp[d3d11.D3DKMTLock] } }
__declspec(naked) void _D3DKMTOpenAdapterFromHdc() { _asm { jmp[d3d11.D3DKMTOpenAdapterFromHdc] } }
__declspec(naked) void _D3DKMTOpenResource() { _asm { jmp[d3d11.D3DKMTOpenResource] } }
__declspec(naked) void _D3DKMTPresent() { _asm { jmp[d3d11.D3DKMTPresent] } }
__declspec(naked) void _D3DKMTQueryAdapterInfo() { _asm { jmp[d3d11.D3DKMTQueryAdapterInfo] } }
__declspec(naked) void _D3DKMTQueryAllocationResidency() { _asm { jmp[d3d11.D3DKMTQueryAllocationResidency] } }
__declspec(naked) void _D3DKMTQueryResourceInfo() { _asm { jmp[d3d11.D3DKMTQueryResourceInfo] } }
__declspec(naked) void _D3DKMTRender() { _asm { jmp[d3d11.D3DKMTRender] } }
__declspec(naked) void _D3DKMTSetAllocationPriority() { _asm { jmp[d3d11.D3DKMTSetAllocationPriority] } }
__declspec(naked) void _D3DKMTSetContextSchedulingPriority() { _asm { jmp[d3d11.D3DKMTSetContextSchedulingPriority] } }
__declspec(naked) void _D3DKMTSetDisplayMode() { _asm { jmp[d3d11.D3DKMTSetDisplayMode] } }
__declspec(naked) void _D3DKMTSetDisplayPrivateDriverFormat() { _asm { jmp[d3d11.D3DKMTSetDisplayPrivateDriverFormat] } }
__declspec(naked) void _D3DKMTSetGammaRamp() { _asm { jmp[d3d11.D3DKMTSetGammaRamp] } }
__declspec(naked) void _D3DKMTSetVidPnSourceOwner() { _asm { jmp[d3d11.D3DKMTSetVidPnSourceOwner] } }
__declspec(naked) void _D3DKMTSignalSynchronizationObject() { _asm { jmp[d3d11.D3DKMTSignalSynchronizationObject] } }
__declspec(naked) void _D3DKMTUnlock() { _asm { jmp[d3d11.D3DKMTUnlock] } }
__declspec(naked) void _D3DKMTWaitForSynchronizationObject() { _asm { jmp[d3d11.D3DKMTWaitForSynchronizationObject] } }
__declspec(naked) void _D3DKMTWaitForVerticalBlankEvent() { _asm { jmp[d3d11.D3DKMTWaitForVerticalBlankEvent] } }
__declspec(naked) void _D3DPerformance_BeginEvent() { _asm { jmp[d3d11.D3DPerformance_BeginEvent] } }
__declspec(naked) void _D3DPerformance_EndEvent() { _asm { jmp[d3d11.D3DPerformance_EndEvent] } }
__declspec(naked) void _D3DPerformance_GetStatus() { _asm { jmp[d3d11.D3DPerformance_GetStatus] } }
__declspec(naked) void _D3DPerformance_SetMarker() { _asm { jmp[d3d11.D3DPerformance_SetMarker] } }
__declspec(naked) void _EnableFeatureLevelUpgrade() { _asm { jmp[d3d11.EnableFeatureLevelUpgrade] } }
__declspec(naked) void _OpenAdapter10() { _asm { jmp[d3d11.OpenAdapter10] } }
__declspec(naked) void _OpenAdapter10_2() { _asm { jmp[d3d11.OpenAdapter10_2] } }

__declspec(naked) void _D3D12CoreCreateLayeredDevice() { _asm { jmp[d3d12.D3D12CoreCreateLayeredDevice] } }
__declspec(naked) void _D3D12CoreGetLayeredDeviceSize() { _asm { jmp[d3d12.D3D12CoreGetLayeredDeviceSize] } }
__declspec(naked) void _D3D12CoreRegisterLayers() { _asm { jmp[d3d12.D3D12CoreRegisterLayers] } }
__declspec(naked) void _D3D12CreateDevice() { _asm { jmp[d3d12.D3D12CreateDevice] } }
__declspec(naked) void _D3D12CreateRootSignatureDeserializer() { _asm { jmp[d3d12.D3D12CreateRootSignatureDeserializer] } }
__declspec(naked) void _D3D12CreateVersionedRootSignatureDeserializer() { _asm { jmp[d3d12.D3D12CreateVersionedRootSignatureDeserializer] } }
__declspec(naked) void _D3D12DeviceRemovedExtendedData() { _asm { jmp[d3d12.D3D12DeviceRemovedExtendedData] } }
__declspec(naked) void _D3D12EnableExperimentalFeatures() { _asm { jmp[d3d12.D3D12EnableExperimentalFeatures] } }
__declspec(naked) void _D3D12GetDebugInterface() { _asm { jmp[d3d12.D3D12GetDebugInterface] } }
__declspec(naked) void _D3D12GetInterface() { _asm { jmp[d3d12.D3D12GetInterface] } }
__declspec(naked) void _D3D12PIXEventsReplaceBlock() { _asm { jmp[d3d12.D3D12PIXEventsReplaceBlock] } }
__declspec(naked) void _D3D12PIXGetThreadInfo() { _asm { jmp[d3d12.D3D12PIXGetThreadInfo] } }
__declspec(naked) void _D3D12PIXNotifyWakeFromFenceSignal() { _asm { jmp[d3d12.D3D12PIXNotifyWakeFromFenceSignal] } }
__declspec(naked) void _D3D12PIXReportCounter() { _asm { jmp[d3d12.D3D12PIXReportCounter] } }
__declspec(naked) void _D3D12SerializeRootSignature() { _asm { jmp[d3d12.D3D12SerializeRootSignature] } }
__declspec(naked) void _D3D12SerializeVersionedRootSignature() { _asm { jmp[d3d12.D3D12SerializeVersionedRootSignature] } }
__declspec(naked) void _GetBehaviorValue() { _asm { jmp[d3d12.GetBehaviorValue] } }
__declspec(naked) void _SetAppCompatStringPointer() { _asm { jmp[d3d12.SetAppCompatStringPointer] } }

__declspec(naked) void _AcquireDDThreadLock() { _asm { jmp[ddraw.AcquireDDThreadLock] } }
__declspec(naked) void _CompleteCreateSysmemSurface() { _asm { jmp[ddraw.CompleteCreateSysmemSurface] } }
__declspec(naked) void _D3DParseUnknownCommand() { _asm { jmp[ddraw.D3DParseUnknownCommand] } }
__declspec(naked) void _DDGetAttachedSurfaceLcl() { _asm { jmp[ddraw.DDGetAttachedSurfaceLcl] } }
__declspec(naked) void _DDInternalLock() { _asm { jmp[ddraw.DDInternalLock] } }
__declspec(naked) void _DDInternalUnlock() { _asm { jmp[ddraw.DDInternalUnlock] } }
__declspec(naked) void _DSoundHelp() { _asm { jmp[ddraw.DSoundHelp] } }
__declspec(naked) void _DirectDrawCreate() { _asm { jmp[ddraw.DirectDrawCreate] } }
__declspec(naked) void _DirectDrawCreateClipper() { _asm { jmp[ddraw.DirectDrawCreateClipper] } }
__declspec(naked) void _DirectDrawCreateEx() { _asm { jmp[ddraw.DirectDrawCreateEx] } }
__declspec(naked) void _DirectDrawEnumerateA() { _asm { jmp[ddraw.DirectDrawEnumerateA] } }
__declspec(naked) void _DirectDrawEnumerateExA() { _asm { jmp[ddraw.DirectDrawEnumerateExA] } }
__declspec(naked) void _DirectDrawEnumerateExW() { _asm { jmp[ddraw.DirectDrawEnumerateExW] } }
__declspec(naked) void _DirectDrawEnumerateW() { _asm { jmp[ddraw.DirectDrawEnumerateW] } }
//__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[ddraw.DllCanUnloadNow] } }
//__declspec(naked) void _DllGetClassObject() { _asm { jmp[ddraw.DllGetClassObject] } }
__declspec(naked) void _GetDDSurfaceLocal() { _asm { jmp[ddraw.GetDDSurfaceLocal] } }
__declspec(naked) void _GetOLEThunkData() { _asm { jmp[ddraw.GetOLEThunkData] } }
__declspec(naked) void _GetSurfaceFromDC() { _asm { jmp[ddraw.GetSurfaceFromDC] } }
__declspec(naked) void _RegisterSpecialCase() { _asm { jmp[ddraw.RegisterSpecialCase] } }
__declspec(naked) void _ReleaseDDThreadLock() { _asm { jmp[ddraw.ReleaseDDThreadLock] } }
__declspec(naked) void _SetAppCompatData() { _asm { jmp[ddraw.SetAppCompatData] } }

__declspec(naked) void _CloseDriver() { _asm { jmp[winmm.CloseDriver] } }
__declspec(naked) void _DefDriverProc() { _asm { jmp[winmm.DefDriverProc] } }
__declspec(naked) void _DriverCallback() { _asm { jmp[winmm.DriverCallback] } }
__declspec(naked) void _DrvGetModuleHandle() { _asm { jmp[winmm.DrvGetModuleHandle] } }
__declspec(naked) void _GetDriverModuleHandle() { _asm { jmp[winmm.GetDriverModuleHandle] } }
__declspec(naked) void _NotifyCallbackData() { _asm { jmp[winmm.NotifyCallbackData] } }
__declspec(naked) void _OpenDriver() { _asm { jmp[winmm.OpenDriver] } }
__declspec(naked) void _PlaySound() { _asm { jmp[winmm.PlaySound] } }
__declspec(naked) void _PlaySoundA() { _asm { jmp[winmm.PlaySoundA] } }
__declspec(naked) void _PlaySoundW() { _asm { jmp[winmm.PlaySoundW] } }
__declspec(naked) void _SendDriverMessage() { _asm { jmp[winmm.SendDriverMessage] } }
__declspec(naked) void _WOW32DriverCallback() { _asm { jmp[winmm.WOW32DriverCallback] } }
__declspec(naked) void _WOW32ResolveMultiMediaHandle() { _asm { jmp[winmm.WOW32ResolveMultiMediaHandle] } }
__declspec(naked) void _WOWAppExit() { _asm { jmp[winmm.WOWAppExit] } }
__declspec(naked) void _aux32Message() { _asm { jmp[winmm.aux32Message] } }
__declspec(naked) void _auxGetDevCapsA() { _asm { jmp[winmm.auxGetDevCapsA] } }
__declspec(naked) void _auxGetDevCapsW() { _asm { jmp[winmm.auxGetDevCapsW] } }
__declspec(naked) void _auxGetNumDevs() { _asm { jmp[winmm.auxGetNumDevs] } }
__declspec(naked) void _auxGetVolume() { _asm { jmp[winmm.auxGetVolume] } }
__declspec(naked) void _auxOutMessage() { _asm { jmp[winmm.auxOutMessage] } }
__declspec(naked) void _auxSetVolume() { _asm { jmp[winmm.auxSetVolume] } }
__declspec(naked) void _joy32Message() { _asm { jmp[winmm.joy32Message] } }
__declspec(naked) void _joyConfigChanged() { _asm { jmp[winmm.joyConfigChanged] } }
__declspec(naked) void _joyGetDevCapsA() { _asm { jmp[winmm.joyGetDevCapsA] } }
__declspec(naked) void _joyGetDevCapsW() { _asm { jmp[winmm.joyGetDevCapsW] } }
__declspec(naked) void _joyGetNumDevs() { _asm { jmp[winmm.joyGetNumDevs] } }
__declspec(naked) void _joyGetPos() { _asm { jmp[winmm.joyGetPos] } }
__declspec(naked) void _joyGetPosEx() { _asm { jmp[winmm.joyGetPosEx] } }
__declspec(naked) void _joyGetThreshold() { _asm { jmp[winmm.joyGetThreshold] } }
__declspec(naked) void _joyReleaseCapture() { _asm { jmp[winmm.joyReleaseCapture] } }
__declspec(naked) void _joySetCapture() { _asm { jmp[winmm.joySetCapture] } }
__declspec(naked) void _joySetThreshold() { _asm { jmp[winmm.joySetThreshold] } }
__declspec(naked) void _mci32Message() { _asm { jmp[winmm.mci32Message] } }
__declspec(naked) void _mciDriverNotify() { _asm { jmp[winmm.mciDriverNotify] } }
__declspec(naked) void _mciDriverYield() { _asm { jmp[winmm.mciDriverYield] } }
__declspec(naked) void _mciExecute() { _asm { jmp[winmm.mciExecute] } }
__declspec(naked) void _mciFreeCommandResource() { _asm { jmp[winmm.mciFreeCommandResource] } }
__declspec(naked) void _mciGetCreatorTask() { _asm { jmp[winmm.mciGetCreatorTask] } }
__declspec(naked) void _mciGetDeviceIDA() { _asm { jmp[winmm.mciGetDeviceIDA] } }
__declspec(naked) void _mciGetDeviceIDFromElementIDA() { _asm { jmp[winmm.mciGetDeviceIDFromElementIDA] } }
__declspec(naked) void _mciGetDeviceIDFromElementIDW() { _asm { jmp[winmm.mciGetDeviceIDFromElementIDW] } }
__declspec(naked) void _mciGetDeviceIDW() { _asm { jmp[winmm.mciGetDeviceIDW] } }
__declspec(naked) void _mciGetDriverData() { _asm { jmp[winmm.mciGetDriverData] } }
__declspec(naked) void _mciGetErrorStringA() { _asm { jmp[winmm.mciGetErrorStringA] } }
__declspec(naked) void _mciGetErrorStringW() { _asm { jmp[winmm.mciGetErrorStringW] } }
__declspec(naked) void _mciGetYieldProc() { _asm { jmp[winmm.mciGetYieldProc] } }
__declspec(naked) void _mciLoadCommandResource() { _asm { jmp[winmm.mciLoadCommandResource] } }
__declspec(naked) void _mciSendCommandA() { _asm { jmp[winmm.mciSendCommandA] } }
__declspec(naked) void _mciSendCommandW() { _asm { jmp[winmm.mciSendCommandW] } }
__declspec(naked) void _mciSendStringA() { _asm { jmp[winmm.mciSendStringA] } }
__declspec(naked) void _mciSendStringW() { _asm { jmp[winmm.mciSendStringW] } }
__declspec(naked) void _mciSetDriverData() { _asm { jmp[winmm.mciSetDriverData] } }
__declspec(naked) void _mciSetYieldProc() { _asm { jmp[winmm.mciSetYieldProc] } }
__declspec(naked) void _mid32Message() { _asm { jmp[winmm.mid32Message] } }
__declspec(naked) void _midiConnect() { _asm { jmp[winmm.midiConnect] } }
__declspec(naked) void _midiDisconnect() { _asm { jmp[winmm.midiDisconnect] } }
__declspec(naked) void _midiInAddBuffer() { _asm { jmp[winmm.midiInAddBuffer] } }
__declspec(naked) void _midiInClose() { _asm { jmp[winmm.midiInClose] } }
__declspec(naked) void _midiInGetDevCapsA() { _asm { jmp[winmm.midiInGetDevCapsA] } }
__declspec(naked) void _midiInGetDevCapsW() { _asm { jmp[winmm.midiInGetDevCapsW] } }
__declspec(naked) void _midiInGetErrorTextA() { _asm { jmp[winmm.midiInGetErrorTextA] } }
__declspec(naked) void _midiInGetErrorTextW() { _asm { jmp[winmm.midiInGetErrorTextW] } }
__declspec(naked) void _midiInGetID() { _asm { jmp[winmm.midiInGetID] } }
__declspec(naked) void _midiInGetNumDevs() { _asm { jmp[winmm.midiInGetNumDevs] } }
__declspec(naked) void _midiInMessage() { _asm { jmp[winmm.midiInMessage] } }
__declspec(naked) void _midiInOpen() { _asm { jmp[winmm.midiInOpen] } }
__declspec(naked) void _midiInPrepareHeader() { _asm { jmp[winmm.midiInPrepareHeader] } }
__declspec(naked) void _midiInReset() { _asm { jmp[winmm.midiInReset] } }
__declspec(naked) void _midiInStart() { _asm { jmp[winmm.midiInStart] } }
__declspec(naked) void _midiInStop() { _asm { jmp[winmm.midiInStop] } }
__declspec(naked) void _midiInUnprepareHeader() { _asm { jmp[winmm.midiInUnprepareHeader] } }
__declspec(naked) void _midiOutCacheDrumPatches() { _asm { jmp[winmm.midiOutCacheDrumPatches] } }
__declspec(naked) void _midiOutCachePatches() { _asm { jmp[winmm.midiOutCachePatches] } }
__declspec(naked) void _midiOutClose() { _asm { jmp[winmm.midiOutClose] } }
__declspec(naked) void _midiOutGetDevCapsA() { _asm { jmp[winmm.midiOutGetDevCapsA] } }
__declspec(naked) void _midiOutGetDevCapsW() { _asm { jmp[winmm.midiOutGetDevCapsW] } }
__declspec(naked) void _midiOutGetErrorTextA() { _asm { jmp[winmm.midiOutGetErrorTextA] } }
__declspec(naked) void _midiOutGetErrorTextW() { _asm { jmp[winmm.midiOutGetErrorTextW] } }
__declspec(naked) void _midiOutGetID() { _asm { jmp[winmm.midiOutGetID] } }
__declspec(naked) void _midiOutGetNumDevs() { _asm { jmp[winmm.midiOutGetNumDevs] } }
__declspec(naked) void _midiOutGetVolume() { _asm { jmp[winmm.midiOutGetVolume] } }
__declspec(naked) void _midiOutLongMsg() { _asm { jmp[winmm.midiOutLongMsg] } }
__declspec(naked) void _midiOutMessage() { _asm { jmp[winmm.midiOutMessage] } }
__declspec(naked) void _midiOutOpen() { _asm { jmp[winmm.midiOutOpen] } }
__declspec(naked) void _midiOutPrepareHeader() { _asm { jmp[winmm.midiOutPrepareHeader] } }
__declspec(naked) void _midiOutReset() { _asm { jmp[winmm.midiOutReset] } }
__declspec(naked) void _midiOutSetVolume() { _asm { jmp[winmm.midiOutSetVolume] } }
__declspec(naked) void _midiOutShortMsg() { _asm { jmp[winmm.midiOutShortMsg] } }
__declspec(naked) void _midiOutUnprepareHeader() { _asm { jmp[winmm.midiOutUnprepareHeader] } }
__declspec(naked) void _midiStreamClose() { _asm { jmp[winmm.midiStreamClose] } }
__declspec(naked) void _midiStreamOpen() { _asm { jmp[winmm.midiStreamOpen] } }
__declspec(naked) void _midiStreamOut() { _asm { jmp[winmm.midiStreamOut] } }
__declspec(naked) void _midiStreamPause() { _asm { jmp[winmm.midiStreamPause] } }
__declspec(naked) void _midiStreamPosition() { _asm { jmp[winmm.midiStreamPosition] } }
__declspec(naked) void _midiStreamProperty() { _asm { jmp[winmm.midiStreamProperty] } }
__declspec(naked) void _midiStreamRestart() { _asm { jmp[winmm.midiStreamRestart] } }
__declspec(naked) void _midiStreamStop() { _asm { jmp[winmm.midiStreamStop] } }
__declspec(naked) void _mixerClose() { _asm { jmp[winmm.mixerClose] } }
__declspec(naked) void _mixerGetControlDetailsA() { _asm { jmp[winmm.mixerGetControlDetailsA] } }
__declspec(naked) void _mixerGetControlDetailsW() { _asm { jmp[winmm.mixerGetControlDetailsW] } }
__declspec(naked) void _mixerGetDevCapsA() { _asm { jmp[winmm.mixerGetDevCapsA] } }
__declspec(naked) void _mixerGetDevCapsW() { _asm { jmp[winmm.mixerGetDevCapsW] } }
__declspec(naked) void _mixerGetID() { _asm { jmp[winmm.mixerGetID] } }
__declspec(naked) void _mixerGetLineControlsA() { _asm { jmp[winmm.mixerGetLineControlsA] } }
__declspec(naked) void _mixerGetLineControlsW() { _asm { jmp[winmm.mixerGetLineControlsW] } }
__declspec(naked) void _mixerGetLineInfoA() { _asm { jmp[winmm.mixerGetLineInfoA] } }
__declspec(naked) void _mixerGetLineInfoW() { _asm { jmp[winmm.mixerGetLineInfoW] } }
__declspec(naked) void _mixerGetNumDevs() { _asm { jmp[winmm.mixerGetNumDevs] } }
__declspec(naked) void _mixerMessage() { _asm { jmp[winmm.mixerMessage] } }
__declspec(naked) void _mixerOpen() { _asm { jmp[winmm.mixerOpen] } }
__declspec(naked) void _mixerSetControlDetails() { _asm { jmp[winmm.mixerSetControlDetails] } }
__declspec(naked) void _mmDrvInstall() { _asm { jmp[winmm.mmDrvInstall] } }
__declspec(naked) void _mmGetCurrentTask() { _asm { jmp[winmm.mmGetCurrentTask] } }
__declspec(naked) void _mmTaskBlock() { _asm { jmp[winmm.mmTaskBlock] } }
__declspec(naked) void _mmTaskCreate() { _asm { jmp[winmm.mmTaskCreate] } }
__declspec(naked) void _mmTaskSignal() { _asm { jmp[winmm.mmTaskSignal] } }
__declspec(naked) void _mmTaskYield() { _asm { jmp[winmm.mmTaskYield] } }
__declspec(naked) void _mmioAdvance() { _asm { jmp[winmm.mmioAdvance] } }
__declspec(naked) void _mmioAscend() { _asm { jmp[winmm.mmioAscend] } }
__declspec(naked) void _mmioClose() { _asm { jmp[winmm.mmioClose] } }
__declspec(naked) void _mmioCreateChunk() { _asm { jmp[winmm.mmioCreateChunk] } }
__declspec(naked) void _mmioDescend() { _asm { jmp[winmm.mmioDescend] } }
__declspec(naked) void _mmioFlush() { _asm { jmp[winmm.mmioFlush] } }
__declspec(naked) void _mmioGetInfo() { _asm { jmp[winmm.mmioGetInfo] } }
__declspec(naked) void _mmioInstallIOProcA() { _asm { jmp[winmm.mmioInstallIOProcA] } }
__declspec(naked) void _mmioInstallIOProcW() { _asm { jmp[winmm.mmioInstallIOProcW] } }
__declspec(naked) void _mmioOpenA() { _asm { jmp[winmm.mmioOpenA] } }
__declspec(naked) void _mmioOpenW() { _asm { jmp[winmm.mmioOpenW] } }
__declspec(naked) void _mmioRead() { _asm { jmp[winmm.mmioRead] } }
__declspec(naked) void _mmioRenameA() { _asm { jmp[winmm.mmioRenameA] } }
__declspec(naked) void _mmioRenameW() { _asm { jmp[winmm.mmioRenameW] } }
__declspec(naked) void _mmioSeek() { _asm { jmp[winmm.mmioSeek] } }
__declspec(naked) void _mmioSendMessage() { _asm { jmp[winmm.mmioSendMessage] } }
__declspec(naked) void _mmioSetBuffer() { _asm { jmp[winmm.mmioSetBuffer] } }
__declspec(naked) void _mmioSetInfo() { _asm { jmp[winmm.mmioSetInfo] } }
__declspec(naked) void _mmioStringToFOURCCA() { _asm { jmp[winmm.mmioStringToFOURCCA] } }
__declspec(naked) void _mmioStringToFOURCCW() { _asm { jmp[winmm.mmioStringToFOURCCW] } }
__declspec(naked) void _mmioWrite() { _asm { jmp[winmm.mmioWrite] } }
__declspec(naked) void _mmsystemGetVersion() { _asm { jmp[winmm.mmsystemGetVersion] } }
__declspec(naked) void _mod32Message() { _asm { jmp[winmm.mod32Message] } }
__declspec(naked) void _mxd32Message() { _asm { jmp[winmm.mxd32Message] } }
__declspec(naked) void _sndPlaySoundA() { _asm { jmp[winmm.sndPlaySoundA] } }
__declspec(naked) void _sndPlaySoundW() { _asm { jmp[winmm.sndPlaySoundW] } }
__declspec(naked) void _tid32Message() { _asm { jmp[winmm.tid32Message] } }
__declspec(naked) void _timeBeginPeriod() { _asm { jmp[winmm.timeBeginPeriod] } }
__declspec(naked) void _timeEndPeriod() { _asm { jmp[winmm.timeEndPeriod] } }
__declspec(naked) void _timeGetDevCaps() { _asm { jmp[winmm.timeGetDevCaps] } }
__declspec(naked) void _timeGetSystemTime() { _asm { jmp[winmm.timeGetSystemTime] } }
__declspec(naked) void _timeGetTime() { _asm { jmp[winmm.timeGetTime] } }
__declspec(naked) void _timeKillEvent() { _asm { jmp[winmm.timeKillEvent] } }
__declspec(naked) void _timeSetEvent() { _asm { jmp[winmm.timeSetEvent] } }
__declspec(naked) void _waveInAddBuffer() { _asm { jmp[winmm.waveInAddBuffer] } }
__declspec(naked) void _waveInClose() { _asm { jmp[winmm.waveInClose] } }
__declspec(naked) void _waveInGetDevCapsA() { _asm { jmp[winmm.waveInGetDevCapsA] } }
__declspec(naked) void _waveInGetDevCapsW() { _asm { jmp[winmm.waveInGetDevCapsW] } }
__declspec(naked) void _waveInGetErrorTextA() { _asm { jmp[winmm.waveInGetErrorTextA] } }
__declspec(naked) void _waveInGetErrorTextW() { _asm { jmp[winmm.waveInGetErrorTextW] } }
__declspec(naked) void _waveInGetID() { _asm { jmp[winmm.waveInGetID] } }
__declspec(naked) void _waveInGetNumDevs() { _asm { jmp[winmm.waveInGetNumDevs] } }
__declspec(naked) void _waveInGetPosition() { _asm { jmp[winmm.waveInGetPosition] } }
__declspec(naked) void _waveInMessage() { _asm { jmp[winmm.waveInMessage] } }
__declspec(naked) void _waveInOpen() { _asm { jmp[winmm.waveInOpen] } }
__declspec(naked) void _waveInPrepareHeader() { _asm { jmp[winmm.waveInPrepareHeader] } }
__declspec(naked) void _waveInReset() { _asm { jmp[winmm.waveInReset] } }
__declspec(naked) void _waveInStart() { _asm { jmp[winmm.waveInStart] } }
__declspec(naked) void _waveInStop() { _asm { jmp[winmm.waveInStop] } }
__declspec(naked) void _waveInUnprepareHeader() { _asm { jmp[winmm.waveInUnprepareHeader] } }
__declspec(naked) void _waveOutBreakLoop() { _asm { jmp[winmm.waveOutBreakLoop] } }
__declspec(naked) void _waveOutClose() { _asm { jmp[winmm.waveOutClose] } }
__declspec(naked) void _waveOutGetDevCapsA() { _asm { jmp[winmm.waveOutGetDevCapsA] } }
__declspec(naked) void _waveOutGetDevCapsW() { _asm { jmp[winmm.waveOutGetDevCapsW] } }
__declspec(naked) void _waveOutGetErrorTextA() { _asm { jmp[winmm.waveOutGetErrorTextA] } }
__declspec(naked) void _waveOutGetErrorTextW() { _asm { jmp[winmm.waveOutGetErrorTextW] } }
__declspec(naked) void _waveOutGetID() { _asm { jmp[winmm.waveOutGetID] } }
__declspec(naked) void _waveOutGetNumDevs() { _asm { jmp[winmm.waveOutGetNumDevs] } }
__declspec(naked) void _waveOutGetPitch() { _asm { jmp[winmm.waveOutGetPitch] } }
__declspec(naked) void _waveOutGetPlaybackRate() { _asm { jmp[winmm.waveOutGetPlaybackRate] } }
__declspec(naked) void _waveOutGetPosition() { _asm { jmp[winmm.waveOutGetPosition] } }
__declspec(naked) void _waveOutGetVolume() { _asm { jmp[winmm.waveOutGetVolume] } }
__declspec(naked) void _waveOutMessage() { _asm { jmp[winmm.waveOutMessage] } }
__declspec(naked) void _waveOutOpen() { _asm { jmp[winmm.waveOutOpen] } }
__declspec(naked) void _waveOutPause() { _asm { jmp[winmm.waveOutPause] } }
__declspec(naked) void _waveOutPrepareHeader() { _asm { jmp[winmm.waveOutPrepareHeader] } }
__declspec(naked) void _waveOutReset() { _asm { jmp[winmm.waveOutReset] } }
__declspec(naked) void _waveOutRestart() { _asm { jmp[winmm.waveOutRestart] } }
__declspec(naked) void _waveOutSetPitch() { _asm { jmp[winmm.waveOutSetPitch] } }
__declspec(naked) void _waveOutSetPlaybackRate() { _asm { jmp[winmm.waveOutSetPlaybackRate] } }
__declspec(naked) void _waveOutSetVolume() { _asm { jmp[winmm.waveOutSetVolume] } }
__declspec(naked) void _waveOutUnprepareHeader() { _asm { jmp[winmm.waveOutUnprepareHeader] } }
__declspec(naked) void _waveOutWrite() { _asm { jmp[winmm.waveOutWrite] } }
__declspec(naked) void _wid32Message() { _asm { jmp[winmm.wid32Message] } }
__declspec(naked) void _wod32Message() { _asm { jmp[winmm.wod32Message] } }

//__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[winhttp.DllCanUnloadNow] } }
//__declspec(naked) void _DllGetClassObject() { _asm { jmp[winhttp.DllGetClassObject] } }
__declspec(naked) void _Private1() { _asm { jmp [winhttp.Private1] } }
__declspec(naked) void _SvchostPushServiceGlobals() { _asm { jmp [winhttp.SvchostPushServiceGlobals] } }
__declspec(naked) void _WinHttpAddRequestHeaders() { _asm { jmp [winhttp.WinHttpAddRequestHeaders] } }
__declspec(naked) void _WinHttpAddRequestHeadersEx() { _asm { jmp [winhttp.WinHttpAddRequestHeadersEx] } }
__declspec(naked) void _WinHttpAutoProxySvcMain() { _asm { jmp [winhttp.WinHttpAutoProxySvcMain] } }
__declspec(naked) void _WinHttpCheckPlatform() { _asm { jmp [winhttp.WinHttpCheckPlatform] } }
__declspec(naked) void _WinHttpCloseHandle() { _asm { jmp [winhttp.WinHttpCloseHandle] } }
__declspec(naked) void _WinHttpConnect() { _asm { jmp [winhttp.WinHttpConnect] } }
__declspec(naked) void _WinHttpConnectionDeletePolicyEntries() { _asm { jmp [winhttp.WinHttpConnectionDeletePolicyEntries] } }
__declspec(naked) void _WinHttpConnectionDeleteProxyInfo() { _asm { jmp [winhttp.WinHttpConnectionDeleteProxyInfo] } }
__declspec(naked) void _WinHttpConnectionFreeNameList() { _asm { jmp [winhttp.WinHttpConnectionFreeNameList] } }
__declspec(naked) void _WinHttpConnectionFreeProxyInfo() { _asm { jmp [winhttp.WinHttpConnectionFreeProxyInfo] } }
__declspec(naked) void _WinHttpConnectionFreeProxyList() { _asm { jmp [winhttp.WinHttpConnectionFreeProxyList] } }
__declspec(naked) void _WinHttpConnectionGetNameList() { _asm { jmp [winhttp.WinHttpConnectionGetNameList] } }
__declspec(naked) void _WinHttpConnectionGetProxyInfo() { _asm { jmp [winhttp.WinHttpConnectionGetProxyInfo] } }
__declspec(naked) void _WinHttpConnectionGetProxyList() { _asm { jmp [winhttp.WinHttpConnectionGetProxyList] } }
__declspec(naked) void _WinHttpConnectionOnlyConvert() { _asm { jmp [winhttp.WinHttpConnectionOnlyConvert] } }
__declspec(naked) void _WinHttpConnectionOnlyReceive() { _asm { jmp [winhttp.WinHttpConnectionOnlyReceive] } }
__declspec(naked) void _WinHttpConnectionOnlySend() { _asm { jmp [winhttp.WinHttpConnectionOnlySend] } }
__declspec(naked) void _WinHttpConnectionSetPolicyEntries() { _asm { jmp [winhttp.WinHttpConnectionSetPolicyEntries] } }
__declspec(naked) void _WinHttpConnectionSetProxyInfo() { _asm { jmp [winhttp.WinHttpConnectionSetProxyInfo] } }
__declspec(naked) void _WinHttpConnectionUpdateIfIndexTable() { _asm { jmp [winhttp.WinHttpConnectionUpdateIfIndexTable] } }
__declspec(naked) void _WinHttpCrackUrl() { _asm { jmp [winhttp.WinHttpCrackUrl] } }
__declspec(naked) void _WinHttpCreateProxyResolver() { _asm { jmp [winhttp.WinHttpCreateProxyResolver] } }
__declspec(naked) void _WinHttpCreateUrl() { _asm { jmp [winhttp.WinHttpCreateUrl] } }
__declspec(naked) void _WinHttpDetectAutoProxyConfigUrl() { _asm { jmp [winhttp.WinHttpDetectAutoProxyConfigUrl] } }
__declspec(naked) void _WinHttpFreeProxyResult() { _asm { jmp [winhttp.WinHttpFreeProxyResult] } }
__declspec(naked) void _WinHttpFreeProxyResultEx() { _asm { jmp [winhttp.WinHttpFreeProxyResultEx] } }
__declspec(naked) void _WinHttpFreeProxySettings() { _asm { jmp [winhttp.WinHttpFreeProxySettings] } }
__declspec(naked) void _WinHttpFreeProxySettingsEx() { _asm { jmp [winhttp.WinHttpFreeProxySettingsEx] } }
__declspec(naked) void _WinHttpFreeQueryConnectionGroupResult() { _asm { jmp [winhttp.WinHttpFreeQueryConnectionGroupResult] } }
__declspec(naked) void _WinHttpGetDefaultProxyConfiguration() { _asm { jmp [winhttp.WinHttpGetDefaultProxyConfiguration] } }
__declspec(naked) void _WinHttpGetIEProxyConfigForCurrentUser() { _asm { jmp [winhttp.WinHttpGetIEProxyConfigForCurrentUser] } }
__declspec(naked) void _WinHttpGetProxyForUrl() { _asm { jmp [winhttp.WinHttpGetProxyForUrl] } }
__declspec(naked) void _WinHttpGetProxyForUrlEx() { _asm { jmp [winhttp.WinHttpGetProxyForUrlEx] } }
__declspec(naked) void _WinHttpGetProxyForUrlEx2() { _asm { jmp [winhttp.WinHttpGetProxyForUrlEx2] } }
__declspec(naked) void _WinHttpGetProxyForUrlHvsi() { _asm { jmp [winhttp.WinHttpGetProxyForUrlHvsi] } }
__declspec(naked) void _WinHttpGetProxyResult() { _asm { jmp [winhttp.WinHttpGetProxyResult] } }
__declspec(naked) void _WinHttpGetProxyResultEx() { _asm { jmp [winhttp.WinHttpGetProxyResultEx] } }
__declspec(naked) void _WinHttpGetProxySettingsEx() { _asm { jmp [winhttp.WinHttpGetProxySettingsEx] } }
__declspec(naked) void _WinHttpGetProxySettingsResultEx() { _asm { jmp [winhttp.WinHttpGetProxySettingsResultEx] } }
__declspec(naked) void _WinHttpGetProxySettingsVersion() { _asm { jmp [winhttp.WinHttpGetProxySettingsVersion] } }
__declspec(naked) void _WinHttpGetTunnelSocket() { _asm { jmp [winhttp.WinHttpGetTunnelSocket] } }
__declspec(naked) void _WinHttpOpen() { _asm { jmp [winhttp.WinHttpOpen] } }
__declspec(naked) void _WinHttpOpenRequest() { _asm { jmp [winhttp.WinHttpOpenRequest] } }
__declspec(naked) void _WinHttpPacJsWorkerMain() { _asm { jmp [winhttp.WinHttpPacJsWorkerMain] } }
__declspec(naked) void _WinHttpProbeConnectivity() { _asm { jmp [winhttp.WinHttpProbeConnectivity] } }
__declspec(naked) void _WinHttpQueryAuthSchemes() { _asm { jmp [winhttp.WinHttpQueryAuthSchemes] } }
__declspec(naked) void _WinHttpQueryConnectionGroup() { _asm { jmp [winhttp.WinHttpQueryConnectionGroup] } }
__declspec(naked) void _WinHttpQueryDataAvailable() { _asm { jmp [winhttp.WinHttpQueryDataAvailable] } }
__declspec(naked) void _WinHttpQueryHeaders() { _asm { jmp [winhttp.WinHttpQueryHeaders] } }
__declspec(naked) void _WinHttpQueryHeadersEx() { _asm { jmp [winhttp.WinHttpQueryHeadersEx] } }
__declspec(naked) void _WinHttpQueryOption() { _asm { jmp [winhttp.WinHttpQueryOption] } }
__declspec(naked) void _WinHttpReadData() { _asm { jmp [winhttp.WinHttpReadData] } }
__declspec(naked) void _WinHttpReadDataEx() { _asm { jmp [winhttp.WinHttpReadDataEx] } }
__declspec(naked) void _WinHttpReadProxySettings() { _asm { jmp [winhttp.WinHttpReadProxySettings] } }
__declspec(naked) void _WinHttpReadProxySettingsHvsi() { _asm { jmp [winhttp.WinHttpReadProxySettingsHvsi] } }
__declspec(naked) void _WinHttpReceiveResponse() { _asm { jmp [winhttp.WinHttpReceiveResponse] } }
__declspec(naked) void _WinHttpRegisterProxyChangeNotification() { _asm { jmp [winhttp.WinHttpRegisterProxyChangeNotification] } }
__declspec(naked) void _WinHttpResetAutoProxy() { _asm { jmp [winhttp.WinHttpResetAutoProxy] } }
__declspec(naked) void _WinHttpSaveProxyCredentials() { _asm { jmp [winhttp.WinHttpSaveProxyCredentials] } }
__declspec(naked) void _WinHttpSendRequest() { _asm { jmp [winhttp.WinHttpSendRequest] } }
__declspec(naked) void _WinHttpSetCredentials() { _asm { jmp [winhttp.WinHttpSetCredentials] } }
__declspec(naked) void _WinHttpSetDefaultProxyConfiguration() { _asm { jmp [winhttp.WinHttpSetDefaultProxyConfiguration] } }
__declspec(naked) void _WinHttpSetOption() { _asm { jmp [winhttp.WinHttpSetOption] } }
__declspec(naked) void _WinHttpSetProxySettingsPerUser() { _asm { jmp [winhttp.WinHttpSetProxySettingsPerUser] } }
__declspec(naked) void _WinHttpSetSecureLegacyServersAppCompat() { _asm { jmp [winhttp.WinHttpSetSecureLegacyServersAppCompat] } }
__declspec(naked) void _WinHttpSetStatusCallback() { _asm { jmp [winhttp.WinHttpSetStatusCallback] } }
__declspec(naked) void _WinHttpSetTimeouts() { _asm { jmp [winhttp.WinHttpSetTimeouts] } }
__declspec(naked) void _WinHttpTimeFromSystemTime() { _asm { jmp [winhttp.WinHttpTimeFromSystemTime] } }
__declspec(naked) void _WinHttpTimeToSystemTime() { _asm { jmp [winhttp.WinHttpTimeToSystemTime] } }
__declspec(naked) void _WinHttpUnregisterProxyChangeNotification() { _asm { jmp [winhttp.WinHttpUnregisterProxyChangeNotification] } }
__declspec(naked) void _WinHttpWebSocketClose() { _asm { jmp [winhttp.WinHttpWebSocketClose] } }
__declspec(naked) void _WinHttpWebSocketCompleteUpgrade() { _asm { jmp [winhttp.WinHttpWebSocketCompleteUpgrade] } }
__declspec(naked) void _WinHttpWebSocketQueryCloseStatus() { _asm { jmp [winhttp.WinHttpWebSocketQueryCloseStatus] } }
__declspec(naked) void _WinHttpWebSocketReceive() { _asm { jmp [winhttp.WinHttpWebSocketReceive] } }
__declspec(naked) void _WinHttpWebSocketSend() { _asm { jmp [winhttp.WinHttpWebSocketSend] } }
__declspec(naked) void _WinHttpWebSocketShutdown() { _asm { jmp [winhttp.WinHttpWebSocketShutdown] } }
__declspec(naked) void _WinHttpWriteData() { _asm { jmp [winhttp.WinHttpWriteData] } }
__declspec(naked) void _WinHttpWriteProxySettings() { _asm { jmp [winhttp.WinHttpWriteProxySettings] } }

__declspec(naked) void _acmDriverAddA() { _asm { jmp[msacm32.acmDriverAddA] } }
__declspec(naked) void _acmDriverAddW() { _asm { jmp[msacm32.acmDriverAddW] } }
__declspec(naked) void _acmDriverClose() { _asm { jmp[msacm32.acmDriverClose] } }
__declspec(naked) void _acmDriverDetailsA() { _asm { jmp[msacm32.acmDriverDetailsA] } }
__declspec(naked) void _acmDriverDetailsW() { _asm { jmp[msacm32.acmDriverDetailsW] } }
__declspec(naked) void _acmDriverEnum() { _asm { jmp[msacm32.acmDriverEnum] } }
__declspec(naked) void _acmDriverID() { _asm { jmp[msacm32.acmDriverID] } }
__declspec(naked) void _acmDriverMessage() { _asm { jmp[msacm32.acmDriverMessage] } }
__declspec(naked) void _acmDriverOpen() { _asm { jmp[msacm32.acmDriverOpen] } }
__declspec(naked) void _acmDriverPriority() { _asm { jmp[msacm32.acmDriverPriority] } }
__declspec(naked) void _acmDriverRemove() { _asm { jmp[msacm32.acmDriverRemove] } }
__declspec(naked) void _acmFilterChooseA() { _asm { jmp[msacm32.acmFilterChooseA] } }
__declspec(naked) void _acmFilterChooseW() { _asm { jmp[msacm32.acmFilterChooseW] } }
__declspec(naked) void _acmFilterDetailsA() { _asm { jmp[msacm32.acmFilterDetailsA] } }
__declspec(naked) void _acmFilterDetailsW() { _asm { jmp[msacm32.acmFilterDetailsW] } }
__declspec(naked) void _acmFilterEnumA() { _asm { jmp[msacm32.acmFilterEnumA] } }
__declspec(naked) void _acmFilterEnumW() { _asm { jmp[msacm32.acmFilterEnumW] } }
__declspec(naked) void _acmFilterTagDetailsA() { _asm { jmp[msacm32.acmFilterTagDetailsA] } }
__declspec(naked) void _acmFilterTagDetailsW() { _asm { jmp[msacm32.acmFilterTagDetailsW] } }
__declspec(naked) void _acmFilterTagEnumA() { _asm { jmp[msacm32.acmFilterTagEnumA] } }
__declspec(naked) void _acmFilterTagEnumW() { _asm { jmp[msacm32.acmFilterTagEnumW] } }
__declspec(naked) void _acmFormatChooseA() { _asm { jmp[msacm32.acmFormatChooseA] } }
__declspec(naked) void _acmFormatChooseW() { _asm { jmp[msacm32.acmFormatChooseW] } }
__declspec(naked) void _acmFormatDetailsA() { _asm { jmp[msacm32.acmFormatDetailsA] } }
__declspec(naked) void _acmFormatDetailsW() { _asm { jmp[msacm32.acmFormatDetailsW] } }
__declspec(naked) void _acmFormatEnumA() { _asm { jmp[msacm32.acmFormatEnumA] } }
__declspec(naked) void _acmFormatEnumW() { _asm { jmp[msacm32.acmFormatEnumW] } }
__declspec(naked) void _acmFormatSuggest() { _asm { jmp[msacm32.acmFormatSuggest] } }
__declspec(naked) void _acmFormatTagDetailsA() { _asm { jmp[msacm32.acmFormatTagDetailsA] } }
__declspec(naked) void _acmFormatTagDetailsW() { _asm { jmp[msacm32.acmFormatTagDetailsW] } }
__declspec(naked) void _acmFormatTagEnumA() { _asm { jmp[msacm32.acmFormatTagEnumA] } }
__declspec(naked) void _acmFormatTagEnumW() { _asm { jmp[msacm32.acmFormatTagEnumW] } }
__declspec(naked) void _acmGetVersion() { _asm { jmp[msacm32.acmGetVersion] } }
__declspec(naked) void _acmMetrics() { _asm { jmp[msacm32.acmMetrics] } }
__declspec(naked) void _acmStreamClose() { _asm { jmp[msacm32.acmStreamClose] } }
__declspec(naked) void _acmStreamConvert() { _asm { jmp[msacm32.acmStreamConvert] } }
__declspec(naked) void _acmStreamMessage() { _asm { jmp[msacm32.acmStreamMessage] } }
__declspec(naked) void _acmStreamOpen() { _asm { jmp[msacm32.acmStreamOpen] } }
__declspec(naked) void _acmStreamPrepareHeader() { _asm { jmp[msacm32.acmStreamPrepareHeader] } }
__declspec(naked) void _acmStreamReset() { _asm { jmp[msacm32.acmStreamReset] } }
__declspec(naked) void _acmStreamSize() { _asm { jmp[msacm32.acmStreamSize] } }
__declspec(naked) void _acmStreamUnprepareHeader() { _asm { jmp[msacm32.acmStreamUnprepareHeader] } }

__declspec(naked) void _DrawDibBegin() { _asm { jmp[msvfw32.DrawDibBegin] } }
__declspec(naked) void _DrawDibChangePalette() { _asm { jmp[msvfw32.DrawDibChangePalette] } }
__declspec(naked) void _DrawDibClose() { _asm { jmp[msvfw32.DrawDibClose] } }
__declspec(naked) void _DrawDibDraw() { _asm { jmp[msvfw32.DrawDibDraw] } }
__declspec(naked) void _DrawDibEnd() { _asm { jmp[msvfw32.DrawDibEnd] } }
__declspec(naked) void _DrawDibGetBuffer() { _asm { jmp[msvfw32.DrawDibGetBuffer] } }
__declspec(naked) void _DrawDibGetPalette() { _asm { jmp[msvfw32.DrawDibGetPalette] } }
__declspec(naked) void _DrawDibOpen() { _asm { jmp[msvfw32.DrawDibOpen] } }
__declspec(naked) void _DrawDibProfileDisplay() { _asm { jmp[msvfw32.DrawDibProfileDisplay] } }
__declspec(naked) void _DrawDibRealize() { _asm { jmp[msvfw32.DrawDibRealize] } }
__declspec(naked) void _DrawDibSetPalette() { _asm { jmp[msvfw32.DrawDibSetPalette] } }
__declspec(naked) void _DrawDibStart() { _asm { jmp[msvfw32.DrawDibStart] } }
__declspec(naked) void _DrawDibStop() { _asm { jmp[msvfw32.DrawDibStop] } }
__declspec(naked) void _DrawDibTime() { _asm { jmp[msvfw32.DrawDibTime] } }
__declspec(naked) void _GetOpenFileNamePreview() { _asm { jmp[msvfw32.GetOpenFileNamePreview] } }
__declspec(naked) void _GetOpenFileNamePreviewA() { _asm { jmp[msvfw32.GetOpenFileNamePreviewA] } }
__declspec(naked) void _GetOpenFileNamePreviewW() { _asm { jmp[msvfw32.GetOpenFileNamePreviewW] } }
__declspec(naked) void _GetSaveFileNamePreviewA() { _asm { jmp[msvfw32.GetSaveFileNamePreviewA] } }
__declspec(naked) void _GetSaveFileNamePreviewW() { _asm { jmp[msvfw32.GetSaveFileNamePreviewW] } }
__declspec(naked) void _ICClose() { _asm { jmp[msvfw32.ICClose] } }
__declspec(naked) void _ICCompress() { _asm { jmp[msvfw32.ICCompress] } }
__declspec(naked) void _ICCompressorChoose() { _asm { jmp[msvfw32.ICCompressorChoose] } }
__declspec(naked) void _ICCompressorFree() { _asm { jmp[msvfw32.ICCompressorFree] } }
__declspec(naked) void _ICDecompress() { _asm { jmp[msvfw32.ICDecompress] } }
__declspec(naked) void _ICDraw() { _asm { jmp[msvfw32.ICDraw] } }
__declspec(naked) void _ICDrawBegin() { _asm { jmp[msvfw32.ICDrawBegin] } }
__declspec(naked) void _ICGetDisplayFormat() { _asm { jmp[msvfw32.ICGetDisplayFormat] } }
__declspec(naked) void _ICGetInfo() { _asm { jmp[msvfw32.ICGetInfo] } }
__declspec(naked) void _ICImageCompress() { _asm { jmp[msvfw32.ICImageCompress] } }
__declspec(naked) void _ICImageDecompress() { _asm { jmp[msvfw32.ICImageDecompress] } }
__declspec(naked) void _ICInfo() { _asm { jmp[msvfw32.ICInfo] } }
__declspec(naked) void _ICInstall() { _asm { jmp[msvfw32.ICInstall] } }
__declspec(naked) void _ICLocate() { _asm { jmp[msvfw32.ICLocate] } }
__declspec(naked) void _ICMThunk32() { _asm { jmp[msvfw32.ICMThunk32] } }
__declspec(naked) void _ICOpen() { _asm { jmp[msvfw32.ICOpen] } }
__declspec(naked) void _ICOpenFunction() { _asm { jmp[msvfw32.ICOpenFunction] } }
__declspec(naked) void _ICRemove() { _asm { jmp[msvfw32.ICRemove] } }
__declspec(naked) void _ICSendMessage() { _asm { jmp[msvfw32.ICSendMessage] } }
__declspec(naked) void _ICSeqCompressFrame() { _asm { jmp[msvfw32.ICSeqCompressFrame] } }
__declspec(naked) void _ICSeqCompressFrameEnd() { _asm { jmp[msvfw32.ICSeqCompressFrameEnd] } }
__declspec(naked) void _ICSeqCompressFrameStart() { _asm { jmp[msvfw32.ICSeqCompressFrameStart] } }
__declspec(naked) void _MCIWndCreate() { _asm { jmp[msvfw32.MCIWndCreate] } }
__declspec(naked) void _MCIWndCreateA() { _asm { jmp[msvfw32.MCIWndCreateA] } }
__declspec(naked) void _MCIWndCreateW() { _asm { jmp[msvfw32.MCIWndCreateW] } }
__declspec(naked) void _MCIWndRegisterClass() { _asm { jmp[msvfw32.MCIWndRegisterClass] } }
__declspec(naked) void _StretchDIB() { _asm { jmp[msvfw32.StretchDIB] } }
__declspec(naked) void _VideoForWindowsVersion() { _asm { jmp[msvfw32.VideoForWindowsVersion] } }

__declspec(naked) void _AppCacheCheckManifest() { _asm { jmp[wininet.AppCacheCheckManifest] } }
__declspec(naked) void _AppCacheCloseHandle() { _asm { jmp[wininet.AppCacheCloseHandle] } }
__declspec(naked) void _AppCacheCreateAndCommitFile() { _asm { jmp[wininet.AppCacheCreateAndCommitFile] } }
__declspec(naked) void _AppCacheDeleteGroup() { _asm { jmp[wininet.AppCacheDeleteGroup] } }
__declspec(naked) void _AppCacheDeleteIEGroup() { _asm { jmp[wininet.AppCacheDeleteIEGroup] } }
__declspec(naked) void _AppCacheDuplicateHandle() { _asm { jmp[wininet.AppCacheDuplicateHandle] } }
__declspec(naked) void _AppCacheFinalize() { _asm { jmp[wininet.AppCacheFinalize] } }
__declspec(naked) void _AppCacheFreeDownloadList() { _asm { jmp[wininet.AppCacheFreeDownloadList] } }
__declspec(naked) void _AppCacheFreeGroupList() { _asm { jmp[wininet.AppCacheFreeGroupList] } }
__declspec(naked) void _AppCacheFreeIESpace() { _asm { jmp[wininet.AppCacheFreeIESpace] } }
__declspec(naked) void _AppCacheFreeSpace() { _asm { jmp[wininet.AppCacheFreeSpace] } }
__declspec(naked) void _AppCacheGetDownloadList() { _asm { jmp[wininet.AppCacheGetDownloadList] } }
__declspec(naked) void _AppCacheGetFallbackUrl() { _asm { jmp[wininet.AppCacheGetFallbackUrl] } }
__declspec(naked) void _AppCacheGetGroupList() { _asm { jmp[wininet.AppCacheGetGroupList] } }
__declspec(naked) void _AppCacheGetIEGroupList() { _asm { jmp[wininet.AppCacheGetIEGroupList] } }
__declspec(naked) void _AppCacheGetInfo() { _asm { jmp[wininet.AppCacheGetInfo] } }
__declspec(naked) void _AppCacheGetManifestUrl() { _asm { jmp[wininet.AppCacheGetManifestUrl] } }
__declspec(naked) void _AppCacheLookup() { _asm { jmp[wininet.AppCacheLookup] } }
__declspec(naked) void _CommitUrlCacheEntryA() { _asm { jmp[wininet.CommitUrlCacheEntryA] } }
__declspec(naked) void _CommitUrlCacheEntryBinaryBlob() { _asm { jmp[wininet.CommitUrlCacheEntryBinaryBlob] } }
__declspec(naked) void _CommitUrlCacheEntryW() { _asm { jmp[wininet.CommitUrlCacheEntryW] } }
__declspec(naked) void _CreateMD5SSOHash() { _asm { jmp[wininet.CreateMD5SSOHash] } }
__declspec(naked) void _CreateUrlCacheContainerA() { _asm { jmp[wininet.CreateUrlCacheContainerA] } }
__declspec(naked) void _CreateUrlCacheContainerW() { _asm { jmp[wininet.CreateUrlCacheContainerW] } }
__declspec(naked) void _CreateUrlCacheEntryA() { _asm { jmp[wininet.CreateUrlCacheEntryA] } }
__declspec(naked) void _CreateUrlCacheEntryExW() { _asm { jmp[wininet.CreateUrlCacheEntryExW] } }
__declspec(naked) void _CreateUrlCacheEntryW() { _asm { jmp[wininet.CreateUrlCacheEntryW] } }
__declspec(naked) void _CreateUrlCacheGroup() { _asm { jmp[wininet.CreateUrlCacheGroup] } }
__declspec(naked) void _DeleteIE3Cache() { _asm { jmp[wininet.DeleteIE3Cache] } }
__declspec(naked) void _DeleteUrlCacheContainerA() { _asm { jmp[wininet.DeleteUrlCacheContainerA] } }
__declspec(naked) void _DeleteUrlCacheContainerW() { _asm { jmp[wininet.DeleteUrlCacheContainerW] } }
__declspec(naked) void _DeleteUrlCacheEntry() { _asm { jmp[wininet.DeleteUrlCacheEntry] } }
__declspec(naked) void _DeleteUrlCacheEntryA() { _asm { jmp[wininet.DeleteUrlCacheEntryA] } }
__declspec(naked) void _DeleteUrlCacheEntryW() { _asm { jmp[wininet.DeleteUrlCacheEntryW] } }
__declspec(naked) void _DeleteUrlCacheGroup() { _asm { jmp[wininet.DeleteUrlCacheGroup] } }
__declspec(naked) void _DeleteWpadCacheForNetworks() { _asm { jmp[wininet.DeleteWpadCacheForNetworks] } }
__declspec(naked) void _DetectAutoProxyUrl() { _asm { jmp[wininet.DetectAutoProxyUrl] } }
__declspec(naked) void _DispatchAPICall() { _asm { jmp[wininet.DispatchAPICall] } }
//__declspec(naked) void _DllCanUnloadNow() { _asm { jmp[wininet.DllCanUnloadNow] } }
//__declspec(naked) void _DllGetClassObject() { _asm { jmp[wininet.DllGetClassObject] } }
__declspec(naked) void _DllInstall() { _asm { jmp[wininet.DllInstall] } }
//__declspec(naked) void _DllRegisterServer() { _asm { jmp[wininet.DllRegisterServer] } }
//__declspec(naked) void _DllUnregisterServer() { _asm { jmp[wininet.DllUnregisterServer] } }
__declspec(naked) void _FindCloseUrlCache() { _asm { jmp[wininet.FindCloseUrlCache] } }
__declspec(naked) void _FindFirstUrlCacheContainerA() { _asm { jmp[wininet.FindFirstUrlCacheContainerA] } }
__declspec(naked) void _FindFirstUrlCacheContainerW() { _asm { jmp[wininet.FindFirstUrlCacheContainerW] } }
__declspec(naked) void _FindFirstUrlCacheEntryA() { _asm { jmp[wininet.FindFirstUrlCacheEntryA] } }
__declspec(naked) void _FindFirstUrlCacheEntryExA() { _asm { jmp[wininet.FindFirstUrlCacheEntryExA] } }
__declspec(naked) void _FindFirstUrlCacheEntryExW() { _asm { jmp[wininet.FindFirstUrlCacheEntryExW] } }
__declspec(naked) void _FindFirstUrlCacheEntryW() { _asm { jmp[wininet.FindFirstUrlCacheEntryW] } }
__declspec(naked) void _FindFirstUrlCacheGroup() { _asm { jmp[wininet.FindFirstUrlCacheGroup] } }
__declspec(naked) void _FindNextUrlCacheContainerA() { _asm { jmp[wininet.FindNextUrlCacheContainerA] } }
__declspec(naked) void _FindNextUrlCacheContainerW() { _asm { jmp[wininet.FindNextUrlCacheContainerW] } }
__declspec(naked) void _FindNextUrlCacheEntryA() { _asm { jmp[wininet.FindNextUrlCacheEntryA] } }
__declspec(naked) void _FindNextUrlCacheEntryExA() { _asm { jmp[wininet.FindNextUrlCacheEntryExA] } }
__declspec(naked) void _FindNextUrlCacheEntryExW() { _asm { jmp[wininet.FindNextUrlCacheEntryExW] } }
__declspec(naked) void _FindNextUrlCacheEntryW() { _asm { jmp[wininet.FindNextUrlCacheEntryW] } }
__declspec(naked) void _FindNextUrlCacheGroup() { _asm { jmp[wininet.FindNextUrlCacheGroup] } }
__declspec(naked) void _ForceNexusLookup() { _asm { jmp[wininet.ForceNexusLookup] } }
__declspec(naked) void _ForceNexusLookupExW() { _asm { jmp[wininet.ForceNexusLookupExW] } }
__declspec(naked) void _FreeUrlCacheSpaceA() { _asm { jmp[wininet.FreeUrlCacheSpaceA] } }
__declspec(naked) void _FreeUrlCacheSpaceW() { _asm { jmp[wininet.FreeUrlCacheSpaceW] } }
__declspec(naked) void _FtpCommandA() { _asm { jmp[wininet.FtpCommandA] } }
__declspec(naked) void _FtpCommandW() { _asm { jmp[wininet.FtpCommandW] } }
__declspec(naked) void _FtpCreateDirectoryA() { _asm { jmp[wininet.FtpCreateDirectoryA] } }
__declspec(naked) void _FtpCreateDirectoryW() { _asm { jmp[wininet.FtpCreateDirectoryW] } }
__declspec(naked) void _FtpDeleteFileA() { _asm { jmp[wininet.FtpDeleteFileA] } }
__declspec(naked) void _FtpDeleteFileW() { _asm { jmp[wininet.FtpDeleteFileW] } }
__declspec(naked) void _FtpFindFirstFileA() { _asm { jmp[wininet.FtpFindFirstFileA] } }
__declspec(naked) void _FtpFindFirstFileW() { _asm { jmp[wininet.FtpFindFirstFileW] } }
__declspec(naked) void _FtpGetCurrentDirectoryA() { _asm { jmp[wininet.FtpGetCurrentDirectoryA] } }
__declspec(naked) void _FtpGetCurrentDirectoryW() { _asm { jmp[wininet.FtpGetCurrentDirectoryW] } }
__declspec(naked) void _FtpGetFileA() { _asm { jmp[wininet.FtpGetFileA] } }
__declspec(naked) void _FtpGetFileEx() { _asm { jmp[wininet.FtpGetFileEx] } }
__declspec(naked) void _FtpGetFileSize() { _asm { jmp[wininet.FtpGetFileSize] } }
__declspec(naked) void _FtpGetFileW() { _asm { jmp[wininet.FtpGetFileW] } }
__declspec(naked) void _FtpOpenFileA() { _asm { jmp[wininet.FtpOpenFileA] } }
__declspec(naked) void _FtpOpenFileW() { _asm { jmp[wininet.FtpOpenFileW] } }
__declspec(naked) void _FtpPutFileA() { _asm { jmp[wininet.FtpPutFileA] } }
__declspec(naked) void _FtpPutFileEx() { _asm { jmp[wininet.FtpPutFileEx] } }
__declspec(naked) void _FtpPutFileW() { _asm { jmp[wininet.FtpPutFileW] } }
__declspec(naked) void _FtpRemoveDirectoryA() { _asm { jmp[wininet.FtpRemoveDirectoryA] } }
__declspec(naked) void _FtpRemoveDirectoryW() { _asm { jmp[wininet.FtpRemoveDirectoryW] } }
__declspec(naked) void _FtpRenameFileA() { _asm { jmp[wininet.FtpRenameFileA] } }
__declspec(naked) void _FtpRenameFileW() { _asm { jmp[wininet.FtpRenameFileW] } }
__declspec(naked) void _FtpSetCurrentDirectoryA() { _asm { jmp[wininet.FtpSetCurrentDirectoryA] } }
__declspec(naked) void _FtpSetCurrentDirectoryW() { _asm { jmp[wininet.FtpSetCurrentDirectoryW] } }
__declspec(naked) void __GetFileExtensionFromUrl() { _asm { jmp[wininet._GetFileExtensionFromUrl] } }
__declspec(naked) void _GetProxyDllInfo() { _asm { jmp[wininet.GetProxyDllInfo] } }
__declspec(naked) void _GetUrlCacheConfigInfoA() { _asm { jmp[wininet.GetUrlCacheConfigInfoA] } }
__declspec(naked) void _GetUrlCacheConfigInfoW() { _asm { jmp[wininet.GetUrlCacheConfigInfoW] } }
__declspec(naked) void _GetUrlCacheEntryBinaryBlob() { _asm { jmp[wininet.GetUrlCacheEntryBinaryBlob] } }
__declspec(naked) void _GetUrlCacheEntryInfoA() { _asm { jmp[wininet.GetUrlCacheEntryInfoA] } }
__declspec(naked) void _GetUrlCacheEntryInfoExA() { _asm { jmp[wininet.GetUrlCacheEntryInfoExA] } }
__declspec(naked) void _GetUrlCacheEntryInfoExW() { _asm { jmp[wininet.GetUrlCacheEntryInfoExW] } }
__declspec(naked) void _GetUrlCacheEntryInfoW() { _asm { jmp[wininet.GetUrlCacheEntryInfoW] } }
__declspec(naked) void _GetUrlCacheGroupAttributeA() { _asm { jmp[wininet.GetUrlCacheGroupAttributeA] } }
__declspec(naked) void _GetUrlCacheGroupAttributeW() { _asm { jmp[wininet.GetUrlCacheGroupAttributeW] } }
__declspec(naked) void _GetUrlCacheHeaderData() { _asm { jmp[wininet.GetUrlCacheHeaderData] } }
__declspec(naked) void _GopherCreateLocatorA() { _asm { jmp[wininet.GopherCreateLocatorA] } }
__declspec(naked) void _GopherCreateLocatorW() { _asm { jmp[wininet.GopherCreateLocatorW] } }
__declspec(naked) void _GopherFindFirstFileA() { _asm { jmp[wininet.GopherFindFirstFileA] } }
__declspec(naked) void _GopherFindFirstFileW() { _asm { jmp[wininet.GopherFindFirstFileW] } }
__declspec(naked) void _GopherGetAttributeA() { _asm { jmp[wininet.GopherGetAttributeA] } }
__declspec(naked) void _GopherGetAttributeW() { _asm { jmp[wininet.GopherGetAttributeW] } }
__declspec(naked) void _GopherGetLocatorTypeA() { _asm { jmp[wininet.GopherGetLocatorTypeA] } }
__declspec(naked) void _GopherGetLocatorTypeW() { _asm { jmp[wininet.GopherGetLocatorTypeW] } }
__declspec(naked) void _GopherOpenFileA() { _asm { jmp[wininet.GopherOpenFileA] } }
__declspec(naked) void _GopherOpenFileW() { _asm { jmp[wininet.GopherOpenFileW] } }
__declspec(naked) void _HttpAddRequestHeadersA() { _asm { jmp[wininet.HttpAddRequestHeadersA] } }
__declspec(naked) void _HttpAddRequestHeadersW() { _asm { jmp[wininet.HttpAddRequestHeadersW] } }
__declspec(naked) void _HttpCheckDavCompliance() { _asm { jmp[wininet.HttpCheckDavCompliance] } }
__declspec(naked) void _HttpCloseDependencyHandle() { _asm { jmp[wininet.HttpCloseDependencyHandle] } }
__declspec(naked) void _HttpDuplicateDependencyHandle() { _asm { jmp[wininet.HttpDuplicateDependencyHandle] } }
__declspec(naked) void _HttpEndRequestA() { _asm { jmp[wininet.HttpEndRequestA] } }
__declspec(naked) void _HttpEndRequestW() { _asm { jmp[wininet.HttpEndRequestW] } }
__declspec(naked) void _HttpGetServerCredentials() { _asm { jmp[wininet.HttpGetServerCredentials] } }
__declspec(naked) void _HttpGetTunnelSocket() { _asm { jmp[wininet.HttpGetTunnelSocket] } }
__declspec(naked) void _HttpIsHostHstsEnabled() { _asm { jmp[wininet.HttpIsHostHstsEnabled] } }
__declspec(naked) void _HttpOpenDependencyHandle() { _asm { jmp[wininet.HttpOpenDependencyHandle] } }
__declspec(naked) void _HttpOpenRequestA() { _asm { jmp[wininet.HttpOpenRequestA] } }
__declspec(naked) void _HttpOpenRequestW() { _asm { jmp[wininet.HttpOpenRequestW] } }
__declspec(naked) void _HttpPushClose() { _asm { jmp[wininet.HttpPushClose] } }
__declspec(naked) void _HttpPushEnable() { _asm { jmp[wininet.HttpPushEnable] } }
__declspec(naked) void _HttpPushWait() { _asm { jmp[wininet.HttpPushWait] } }
__declspec(naked) void _HttpQueryInfoA() { _asm { jmp[wininet.HttpQueryInfoA] } }
__declspec(naked) void _HttpQueryInfoW() { _asm { jmp[wininet.HttpQueryInfoW] } }
__declspec(naked) void _HttpSendRequestA() { _asm { jmp[wininet.HttpSendRequestA] } }
__declspec(naked) void _HttpSendRequestExA() { _asm { jmp[wininet.HttpSendRequestExA] } }
__declspec(naked) void _HttpSendRequestExW() { _asm { jmp[wininet.HttpSendRequestExW] } }
__declspec(naked) void _HttpSendRequestW() { _asm { jmp[wininet.HttpSendRequestW] } }
__declspec(naked) void _HttpWebSocketClose() { _asm { jmp[wininet.HttpWebSocketClose] } }
__declspec(naked) void _HttpWebSocketCompleteUpgrade() { _asm { jmp[wininet.HttpWebSocketCompleteUpgrade] } }
__declspec(naked) void _HttpWebSocketQueryCloseStatus() { _asm { jmp[wininet.HttpWebSocketQueryCloseStatus] } }
__declspec(naked) void _HttpWebSocketReceive() { _asm { jmp[wininet.HttpWebSocketReceive] } }
__declspec(naked) void _HttpWebSocketSend() { _asm { jmp[wininet.HttpWebSocketSend] } }
__declspec(naked) void _HttpWebSocketShutdown() { _asm { jmp[wininet.HttpWebSocketShutdown] } }
__declspec(naked) void _IncrementUrlCacheHeaderData() { _asm { jmp[wininet.IncrementUrlCacheHeaderData] } }
__declspec(naked) void _InternetAlgIdToStringA() { _asm { jmp[wininet.InternetAlgIdToStringA] } }
__declspec(naked) void _InternetAlgIdToStringW() { _asm { jmp[wininet.InternetAlgIdToStringW] } }
__declspec(naked) void _InternetAttemptConnect() { _asm { jmp[wininet.InternetAttemptConnect] } }
__declspec(naked) void _InternetAutodial() { _asm { jmp[wininet.InternetAutodial] } }
__declspec(naked) void _InternetAutodialCallback() { _asm { jmp[wininet.InternetAutodialCallback] } }
__declspec(naked) void _InternetAutodialHangup() { _asm { jmp[wininet.InternetAutodialHangup] } }
__declspec(naked) void _InternetCanonicalizeUrlA() { _asm { jmp[wininet.InternetCanonicalizeUrlA] } }
__declspec(naked) void _InternetCanonicalizeUrlW() { _asm { jmp[wininet.InternetCanonicalizeUrlW] } }
__declspec(naked) void _InternetCheckConnectionA() { _asm { jmp[wininet.InternetCheckConnectionA] } }
__declspec(naked) void _InternetCheckConnectionW() { _asm { jmp[wininet.InternetCheckConnectionW] } }
__declspec(naked) void _InternetClearAllPerSiteCookieDecisions() { _asm { jmp[wininet.InternetClearAllPerSiteCookieDecisions] } }
__declspec(naked) void _InternetCloseHandle() { _asm { jmp[wininet.InternetCloseHandle] } }
__declspec(naked) void _InternetCombineUrlA() { _asm { jmp[wininet.InternetCombineUrlA] } }
__declspec(naked) void _InternetCombineUrlW() { _asm { jmp[wininet.InternetCombineUrlW] } }
__declspec(naked) void _InternetConfirmZoneCrossing() { _asm { jmp[wininet.InternetConfirmZoneCrossing] } }
__declspec(naked) void _InternetConfirmZoneCrossingA() { _asm { jmp[wininet.InternetConfirmZoneCrossingA] } }
__declspec(naked) void _InternetConfirmZoneCrossingW() { _asm { jmp[wininet.InternetConfirmZoneCrossingW] } }
__declspec(naked) void _InternetConnectA() { _asm { jmp[wininet.InternetConnectA] } }
__declspec(naked) void _InternetConnectW() { _asm { jmp[wininet.InternetConnectW] } }
__declspec(naked) void _InternetConvertUrlFromWireToWideChar() { _asm { jmp[wininet.InternetConvertUrlFromWireToWideChar] } }
__declspec(naked) void _InternetCrackUrlA() { _asm { jmp[wininet.InternetCrackUrlA] } }
__declspec(naked) void _InternetCrackUrlW() { _asm { jmp[wininet.InternetCrackUrlW] } }
__declspec(naked) void _InternetCreateUrlA() { _asm { jmp[wininet.InternetCreateUrlA] } }
__declspec(naked) void _InternetCreateUrlW() { _asm { jmp[wininet.InternetCreateUrlW] } }
__declspec(naked) void _InternetDial() { _asm { jmp[wininet.InternetDial] } }
__declspec(naked) void _InternetDialA() { _asm { jmp[wininet.InternetDialA] } }
__declspec(naked) void _InternetDialW() { _asm { jmp[wininet.InternetDialW] } }
__declspec(naked) void _InternetEnumPerSiteCookieDecisionA() { _asm { jmp[wininet.InternetEnumPerSiteCookieDecisionA] } }
__declspec(naked) void _InternetEnumPerSiteCookieDecisionW() { _asm { jmp[wininet.InternetEnumPerSiteCookieDecisionW] } }
__declspec(naked) void _InternetErrorDlg() { _asm { jmp[wininet.InternetErrorDlg] } }
__declspec(naked) void _InternetFindNextFileA() { _asm { jmp[wininet.InternetFindNextFileA] } }
__declspec(naked) void _InternetFindNextFileW() { _asm { jmp[wininet.InternetFindNextFileW] } }
__declspec(naked) void _InternetFortezzaCommand() { _asm { jmp[wininet.InternetFortezzaCommand] } }
__declspec(naked) void _InternetFreeCookies() { _asm { jmp[wininet.InternetFreeCookies] } }
__declspec(naked) void _InternetFreeProxyInfoList() { _asm { jmp[wininet.InternetFreeProxyInfoList] } }
__declspec(naked) void _InternetGetCertByURL() { _asm { jmp[wininet.InternetGetCertByURL] } }
__declspec(naked) void _InternetGetCertByURLA() { _asm { jmp[wininet.InternetGetCertByURLA] } }
__declspec(naked) void _InternetGetConnectedState() { _asm { jmp[wininet.InternetGetConnectedState] } }
__declspec(naked) void _InternetGetConnectedStateEx() { _asm { jmp[wininet.InternetGetConnectedStateEx] } }
__declspec(naked) void _InternetGetConnectedStateExA() { _asm { jmp[wininet.InternetGetConnectedStateExA] } }
__declspec(naked) void _InternetGetConnectedStateExW() { _asm { jmp[wininet.InternetGetConnectedStateExW] } }
__declspec(naked) void _InternetGetCookieA() { _asm { jmp[wininet.InternetGetCookieA] } }
__declspec(naked) void _InternetGetCookieEx2() { _asm { jmp[wininet.InternetGetCookieEx2] } }
__declspec(naked) void _InternetGetCookieExA() { _asm { jmp[wininet.InternetGetCookieExA] } }
__declspec(naked) void _InternetGetCookieExW() { _asm { jmp[wininet.InternetGetCookieExW] } }
__declspec(naked) void _InternetGetCookieW() { _asm { jmp[wininet.InternetGetCookieW] } }
__declspec(naked) void _InternetGetLastResponseInfoA() { _asm { jmp[wininet.InternetGetLastResponseInfoA] } }
__declspec(naked) void _InternetGetLastResponseInfoW() { _asm { jmp[wininet.InternetGetLastResponseInfoW] } }
__declspec(naked) void _InternetGetPerSiteCookieDecisionA() { _asm { jmp[wininet.InternetGetPerSiteCookieDecisionA] } }
__declspec(naked) void _InternetGetPerSiteCookieDecisionW() { _asm { jmp[wininet.InternetGetPerSiteCookieDecisionW] } }
__declspec(naked) void _InternetGetProxyForUrl() { _asm { jmp[wininet.InternetGetProxyForUrl] } }
__declspec(naked) void _InternetGetSecurityInfoByURL() { _asm { jmp[wininet.InternetGetSecurityInfoByURL] } }
__declspec(naked) void _InternetGetSecurityInfoByURLA() { _asm { jmp[wininet.InternetGetSecurityInfoByURLA] } }
__declspec(naked) void _InternetGetSecurityInfoByURLW() { _asm { jmp[wininet.InternetGetSecurityInfoByURLW] } }
__declspec(naked) void _InternetGoOnline() { _asm { jmp[wininet.InternetGoOnline] } }
__declspec(naked) void _InternetGoOnlineA() { _asm { jmp[wininet.InternetGoOnlineA] } }
__declspec(naked) void _InternetGoOnlineW() { _asm { jmp[wininet.InternetGoOnlineW] } }
__declspec(naked) void _InternetHangUp() { _asm { jmp[wininet.InternetHangUp] } }
__declspec(naked) void _InternetInitializeAutoProxyDll() { _asm { jmp[wininet.InternetInitializeAutoProxyDll] } }
__declspec(naked) void _InternetLockRequestFile() { _asm { jmp[wininet.InternetLockRequestFile] } }
__declspec(naked) void _InternetOpenA() { _asm { jmp[wininet.InternetOpenA] } }
__declspec(naked) void _InternetOpenUrlA() { _asm { jmp[wininet.InternetOpenUrlA] } }
__declspec(naked) void _InternetOpenUrlW() { _asm { jmp[wininet.InternetOpenUrlW] } }
__declspec(naked) void _InternetOpenW() { _asm { jmp[wininet.InternetOpenW] } }
__declspec(naked) void _InternetQueryDataAvailable() { _asm { jmp[wininet.InternetQueryDataAvailable] } }
__declspec(naked) void _InternetQueryFortezzaStatus() { _asm { jmp[wininet.InternetQueryFortezzaStatus] } }
__declspec(naked) void _InternetQueryOptionA() { _asm { jmp[wininet.InternetQueryOptionA] } }
__declspec(naked) void _InternetQueryOptionW() { _asm { jmp[wininet.InternetQueryOptionW] } }
__declspec(naked) void _InternetReadFile() { _asm { jmp[wininet.InternetReadFile] } }
__declspec(naked) void _InternetReadFileExA() { _asm { jmp[wininet.InternetReadFileExA] } }
__declspec(naked) void _InternetReadFileExW() { _asm { jmp[wininet.InternetReadFileExW] } }
__declspec(naked) void _InternetSecurityProtocolToStringA() { _asm { jmp[wininet.InternetSecurityProtocolToStringA] } }
__declspec(naked) void _InternetSecurityProtocolToStringW() { _asm { jmp[wininet.InternetSecurityProtocolToStringW] } }
__declspec(naked) void _InternetSetCookieA() { _asm { jmp[wininet.InternetSetCookieA] } }
__declspec(naked) void _InternetSetCookieEx2() { _asm { jmp[wininet.InternetSetCookieEx2] } }
__declspec(naked) void _InternetSetCookieExA() { _asm { jmp[wininet.InternetSetCookieExA] } }
__declspec(naked) void _InternetSetCookieExW() { _asm { jmp[wininet.InternetSetCookieExW] } }
__declspec(naked) void _InternetSetCookieW() { _asm { jmp[wininet.InternetSetCookieW] } }
__declspec(naked) void _InternetSetDialState() { _asm { jmp[wininet.InternetSetDialState] } }
__declspec(naked) void _InternetSetDialStateA() { _asm { jmp[wininet.InternetSetDialStateA] } }
__declspec(naked) void _InternetSetDialStateW() { _asm { jmp[wininet.InternetSetDialStateW] } }
__declspec(naked) void _InternetSetFilePointer() { _asm { jmp[wininet.InternetSetFilePointer] } }
__declspec(naked) void _InternetSetOptionA() { _asm { jmp[wininet.InternetSetOptionA] } }
__declspec(naked) void _InternetSetOptionExA() { _asm { jmp[wininet.InternetSetOptionExA] } }
__declspec(naked) void _InternetSetOptionExW() { _asm { jmp[wininet.InternetSetOptionExW] } }
__declspec(naked) void _InternetSetOptionW() { _asm { jmp[wininet.InternetSetOptionW] } }
__declspec(naked) void _InternetSetPerSiteCookieDecisionA() { _asm { jmp[wininet.InternetSetPerSiteCookieDecisionA] } }
__declspec(naked) void _InternetSetPerSiteCookieDecisionW() { _asm { jmp[wininet.InternetSetPerSiteCookieDecisionW] } }
__declspec(naked) void _InternetSetStatusCallback() { _asm { jmp[wininet.InternetSetStatusCallback] } }
__declspec(naked) void _InternetSetStatusCallbackA() { _asm { jmp[wininet.InternetSetStatusCallbackA] } }
__declspec(naked) void _InternetSetStatusCallbackW() { _asm { jmp[wininet.InternetSetStatusCallbackW] } }
__declspec(naked) void _InternetShowSecurityInfoByURL() { _asm { jmp[wininet.InternetShowSecurityInfoByURL] } }
__declspec(naked) void _InternetShowSecurityInfoByURLA() { _asm { jmp[wininet.InternetShowSecurityInfoByURLA] } }
__declspec(naked) void _InternetShowSecurityInfoByURLW() { _asm { jmp[wininet.InternetShowSecurityInfoByURLW] } }
__declspec(naked) void _InternetTimeFromSystemTime() { _asm { jmp[wininet.InternetTimeFromSystemTime] } }
__declspec(naked) void _InternetTimeFromSystemTimeA() { _asm { jmp[wininet.InternetTimeFromSystemTimeA] } }
__declspec(naked) void _InternetTimeFromSystemTimeW() { _asm { jmp[wininet.InternetTimeFromSystemTimeW] } }
__declspec(naked) void _InternetTimeToSystemTime() { _asm { jmp[wininet.InternetTimeToSystemTime] } }
__declspec(naked) void _InternetTimeToSystemTimeA() { _asm { jmp[wininet.InternetTimeToSystemTimeA] } }
__declspec(naked) void _InternetTimeToSystemTimeW() { _asm { jmp[wininet.InternetTimeToSystemTimeW] } }
__declspec(naked) void _InternetUnlockRequestFile() { _asm { jmp[wininet.InternetUnlockRequestFile] } }
__declspec(naked) void _InternetWriteFile() { _asm { jmp[wininet.InternetWriteFile] } }
__declspec(naked) void _InternetWriteFileExA() { _asm { jmp[wininet.InternetWriteFileExA] } }
__declspec(naked) void _InternetWriteFileExW() { _asm { jmp[wininet.InternetWriteFileExW] } }
__declspec(naked) void _IsHostInProxyBypassList() { _asm { jmp[wininet.IsHostInProxyBypassList] } }
__declspec(naked) void _IsUrlCacheEntryExpiredA() { _asm { jmp[wininet.IsUrlCacheEntryExpiredA] } }
__declspec(naked) void _IsUrlCacheEntryExpiredW() { _asm { jmp[wininet.IsUrlCacheEntryExpiredW] } }
__declspec(naked) void _LoadUrlCacheContent() { _asm { jmp[wininet.LoadUrlCacheContent] } }
__declspec(naked) void _ParseX509EncodedCertificateForListBoxEntry() { _asm { jmp[wininet.ParseX509EncodedCertificateForListBoxEntry] } }
__declspec(naked) void _PrivacyGetZonePreferenceW() { _asm { jmp[wininet.PrivacyGetZonePreferenceW] } }
__declspec(naked) void _PrivacySetZonePreferenceW() { _asm { jmp[wininet.PrivacySetZonePreferenceW] } }
__declspec(naked) void _ReadUrlCacheEntryStream() { _asm { jmp[wininet.ReadUrlCacheEntryStream] } }
__declspec(naked) void _ReadUrlCacheEntryStreamEx() { _asm { jmp[wininet.ReadUrlCacheEntryStreamEx] } }
__declspec(naked) void _RegisterUrlCacheNotification() { _asm { jmp[wininet.RegisterUrlCacheNotification] } }
__declspec(naked) void _ResumeSuspendedDownload() { _asm { jmp[wininet.ResumeSuspendedDownload] } }
__declspec(naked) void _RetrieveUrlCacheEntryFileA() { _asm { jmp[wininet.RetrieveUrlCacheEntryFileA] } }
__declspec(naked) void _RetrieveUrlCacheEntryFileW() { _asm { jmp[wininet.RetrieveUrlCacheEntryFileW] } }
__declspec(naked) void _RetrieveUrlCacheEntryStreamA() { _asm { jmp[wininet.RetrieveUrlCacheEntryStreamA] } }
__declspec(naked) void _RetrieveUrlCacheEntryStreamW() { _asm { jmp[wininet.RetrieveUrlCacheEntryStreamW] } }
__declspec(naked) void _RunOnceUrlCache() { _asm { jmp[wininet.RunOnceUrlCache] } }
__declspec(naked) void _SetUrlCacheConfigInfoA() { _asm { jmp[wininet.SetUrlCacheConfigInfoA] } }
__declspec(naked) void _SetUrlCacheConfigInfoW() { _asm { jmp[wininet.SetUrlCacheConfigInfoW] } }
__declspec(naked) void _SetUrlCacheEntryGroup() { _asm { jmp[wininet.SetUrlCacheEntryGroup] } }
__declspec(naked) void _SetUrlCacheEntryGroupA() { _asm { jmp[wininet.SetUrlCacheEntryGroupA] } }
__declspec(naked) void _SetUrlCacheEntryGroupW() { _asm { jmp[wininet.SetUrlCacheEntryGroupW] } }
__declspec(naked) void _SetUrlCacheEntryInfoA() { _asm { jmp[wininet.SetUrlCacheEntryInfoA] } }
__declspec(naked) void _SetUrlCacheEntryInfoW() { _asm { jmp[wininet.SetUrlCacheEntryInfoW] } }
__declspec(naked) void _SetUrlCacheGroupAttributeA() { _asm { jmp[wininet.SetUrlCacheGroupAttributeA] } }
__declspec(naked) void _SetUrlCacheGroupAttributeW() { _asm { jmp[wininet.SetUrlCacheGroupAttributeW] } }
__declspec(naked) void _SetUrlCacheHeaderData() { _asm { jmp[wininet.SetUrlCacheHeaderData] } }
__declspec(naked) void _ShowCertificate() { _asm { jmp[wininet.ShowCertificate] } }
__declspec(naked) void _ShowClientAuthCerts() { _asm { jmp[wininet.ShowClientAuthCerts] } }
__declspec(naked) void _ShowSecurityInfo() { _asm { jmp[wininet.ShowSecurityInfo] } }
__declspec(naked) void _ShowX509EncodedCertificate() { _asm { jmp[wininet.ShowX509EncodedCertificate] } }
__declspec(naked) void _UnlockUrlCacheEntryFile() { _asm { jmp[wininet.UnlockUrlCacheEntryFile] } }
__declspec(naked) void _UnlockUrlCacheEntryFileA() { _asm { jmp[wininet.UnlockUrlCacheEntryFileA] } }
__declspec(naked) void _UnlockUrlCacheEntryFileW() { _asm { jmp[wininet.UnlockUrlCacheEntryFileW] } }
__declspec(naked) void _UnlockUrlCacheEntryStream() { _asm { jmp[wininet.UnlockUrlCacheEntryStream] } }
__declspec(naked) void _UpdateUrlCacheContentPath() { _asm { jmp[wininet.UpdateUrlCacheContentPath] } }
__declspec(naked) void _UrlCacheCheckEntriesExist() { _asm { jmp[wininet.UrlCacheCheckEntriesExist] } }
__declspec(naked) void _UrlCacheCloseEntryHandle() { _asm { jmp[wininet.UrlCacheCloseEntryHandle] } }
__declspec(naked) void _UrlCacheContainerSetEntryMaximumAge() { _asm { jmp[wininet.UrlCacheContainerSetEntryMaximumAge] } }
__declspec(naked) void _UrlCacheCreateContainer() { _asm { jmp[wininet.UrlCacheCreateContainer] } }
__declspec(naked) void _UrlCacheFindFirstEntry() { _asm { jmp[wininet.UrlCacheFindFirstEntry] } }
__declspec(naked) void _UrlCacheFindNextEntry() { _asm { jmp[wininet.UrlCacheFindNextEntry] } }
__declspec(naked) void _UrlCacheFreeEntryInfo() { _asm { jmp[wininet.UrlCacheFreeEntryInfo] } }
__declspec(naked) void _UrlCacheFreeGlobalSpace() { _asm { jmp[wininet.UrlCacheFreeGlobalSpace] } }
__declspec(naked) void _UrlCacheGetContentPaths() { _asm { jmp[wininet.UrlCacheGetContentPaths] } }
__declspec(naked) void _UrlCacheGetEntryInfo() { _asm { jmp[wininet.UrlCacheGetEntryInfo] } }
__declspec(naked) void _UrlCacheGetGlobalCacheSize() { _asm { jmp[wininet.UrlCacheGetGlobalCacheSize] } }
__declspec(naked) void _UrlCacheGetGlobalLimit() { _asm { jmp[wininet.UrlCacheGetGlobalLimit] } }
__declspec(naked) void _UrlCacheReadEntryStream() { _asm { jmp[wininet.UrlCacheReadEntryStream] } }
__declspec(naked) void _UrlCacheReloadSettings() { _asm { jmp[wininet.UrlCacheReloadSettings] } }
__declspec(naked) void _UrlCacheRetrieveEntryFile() { _asm { jmp[wininet.UrlCacheRetrieveEntryFile] } }
__declspec(naked) void _UrlCacheRetrieveEntryStream() { _asm { jmp[wininet.UrlCacheRetrieveEntryStream] } }
__declspec(naked) void _UrlCacheServer() { _asm { jmp[wininet.UrlCacheServer] } }
__declspec(naked) void _UrlCacheSetGlobalLimit() { _asm { jmp[wininet.UrlCacheSetGlobalLimit] } }
__declspec(naked) void _UrlCacheUpdateEntryExtraData() { _asm { jmp[wininet.UrlCacheUpdateEntryExtraData] } }
__declspec(naked) void _UrlZonesDetach() { _asm { jmp[wininet.UrlZonesDetach] } }

__declspec(naked) void _GetFileVersionInfoA() { _asm { jmp[version.GetFileVersionInfoA] } }
__declspec(naked) void _GetFileVersionInfoByHandle() { _asm { jmp[version.GetFileVersionInfoByHandle] } }
__declspec(naked) void _GetFileVersionInfoExA() { _asm { jmp[version.GetFileVersionInfoExA] } }
__declspec(naked) void _GetFileVersionInfoExW() { _asm { jmp[version.GetFileVersionInfoExW] } }
__declspec(naked) void _GetFileVersionInfoSizeA() { _asm { jmp[version.GetFileVersionInfoSizeA] } }
__declspec(naked) void _GetFileVersionInfoSizeExA() { _asm { jmp[version.GetFileVersionInfoSizeExA] } }
__declspec(naked) void _GetFileVersionInfoSizeExW() { _asm { jmp[version.GetFileVersionInfoSizeExW] } }
__declspec(naked) void _GetFileVersionInfoSizeW() { _asm { jmp[version.GetFileVersionInfoSizeW] } }
__declspec(naked) void _GetFileVersionInfoW() { _asm { jmp[version.GetFileVersionInfoW] } }
__declspec(naked) void _VerFindFileA() { _asm { jmp[version.VerFindFileA] } }
__declspec(naked) void _VerFindFileW() { _asm { jmp[version.VerFindFileW] } }
__declspec(naked) void _VerInstallFileA() { _asm { jmp[version.VerInstallFileA] } }
__declspec(naked) void _VerInstallFileW() { _asm { jmp[version.VerInstallFileW] } }
__declspec(naked) void _VerLanguageNameA() { _asm { jmp[version.VerLanguageNameA] } }
__declspec(naked) void _VerLanguageNameW() { _asm { jmp[version.VerLanguageNameW] } }
__declspec(naked) void _VerQueryValueA() { _asm { jmp[version.VerQueryValueA] } }
__declspec(naked) void _VerQueryValueW() { _asm { jmp[version.VerQueryValueW] } }

extern "C" __declspec(naked) void __stdcall _BinkAllocateFrameBuffers(int, int, int) { _asm { jmp[bink2w32.BinkAllocateFrameBuffers] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferBlit(int, int, int) { _asm { jmp[bink2w32.BinkBufferBlit] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferCheckWinPos(int, int, int) { _asm { jmp[bink2w32.BinkBufferCheckWinPos] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferClear(int, int) { _asm { jmp[bink2w32.BinkBufferClear] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferClose(int) { _asm { jmp[bink2w32.BinkBufferClose] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferGetDescription(int) { _asm { jmp[bink2w32.BinkBufferGetDescription] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferGetError() { _asm { jmp[bink2w32.BinkBufferGetError] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferLock(int) { _asm { jmp[bink2w32.BinkBufferLock] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferOpen(int, int, int, int) { _asm { jmp[bink2w32.BinkBufferOpen] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferSetDirectDraw(int, int) { _asm { jmp[bink2w32.BinkBufferSetDirectDraw] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferSetHWND(int, int) { _asm { jmp[bink2w32.BinkBufferSetHWND] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferSetOffset(int, int, int) { _asm { jmp[bink2w32.BinkBufferSetOffset] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferSetResolution(int, int, int) { _asm { jmp[bink2w32.BinkBufferSetResolution] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferSetScale(int, int, int) { _asm { jmp[bink2w32.BinkBufferSetScale] } }
extern "C" __declspec(naked) void __stdcall _BinkBufferUnlock(int) { _asm { jmp[bink2w32.BinkBufferUnlock] } }
extern "C" __declspec(naked) void __stdcall _BinkCheckCursor(int, int, int, int, int) { _asm { jmp[bink2w32.BinkCheckCursor] } }
extern "C" __declspec(naked) void __stdcall _BinkClose(int) { _asm { jmp[bink2w32.BinkClose] } }
extern "C" __declspec(naked) void __stdcall _BinkCloseTrack(int) { _asm { jmp[bink2w32.BinkCloseTrack] } }
extern "C" __declspec(naked) void __stdcall _BinkControlBackgroundIO(int, int) { _asm { jmp[bink2w32.BinkControlBackgroundIO] } }
extern "C" __declspec(naked) void __stdcall _BinkControlPlatformFeatures(int, int) { _asm { jmp[bink2w32.BinkControlPlatformFeatures] } }
extern "C" __declspec(naked) void __stdcall _BinkCopyToBuffer(int, int, int, int, int, int, int) { _asm { jmp[bink2w32.BinkCopyToBuffer] } }
extern "C" __declspec(naked) void __stdcall _BinkCopyToBufferRect(int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.BinkCopyToBufferRect] } }
extern "C" __declspec(naked) void __stdcall _BinkCurrentSubtitle(int, int, int, int) { _asm { jmp[bink2w32.BinkCurrentSubtitle] } }
extern "C" __declspec(naked) void __stdcall _BinkDDSurfaceType(int) { _asm { jmp[bink2w32.BinkDDSurfaceType] } }
extern "C" __declspec(naked) void __stdcall _BinkDX8SurfaceType(int) { _asm { jmp[bink2w32.BinkDX8SurfaceType] } }
extern "C" __declspec(naked) void __stdcall _BinkDX9SurfaceType(int) { _asm { jmp[bink2w32.BinkDX9SurfaceType] } }
extern "C" __declspec(naked) void __stdcall _BinkDoFrame(int) { _asm { jmp[bink2w32.BinkDoFrame] } }
extern "C" __declspec(naked) void __stdcall _BinkDoFrameAsync(int, int, int) { _asm { jmp[bink2w32.BinkDoFrameAsync] } }
extern "C" __declspec(naked) void __stdcall _BinkDoFrameAsyncMulti(int, int, int) { _asm { jmp[bink2w32.BinkDoFrameAsyncMulti] } }
extern "C" __declspec(naked) void __stdcall _BinkDoFrameAsyncWait(int, int) { _asm { jmp[bink2w32.BinkDoFrameAsyncWait] } }
extern "C" __declspec(naked) void __stdcall _BinkDoFramePlane(int, int) { _asm { jmp[bink2w32.BinkDoFramePlane] } }
extern "C" __declspec(naked) void __stdcall _BinkFindXAudio2WinDevice(int, int) { _asm { jmp[bink2w32.BinkFindXAudio2WinDevice] } }
extern "C" __declspec(naked) void __stdcall _BinkFreeGlobals() { _asm { jmp[bink2w32.BinkFreeGlobals] } }
extern "C" __declspec(naked) void __stdcall _BinkGetError() { _asm { jmp[bink2w32.BinkGetError] } }
extern "C" __declspec(naked) void __stdcall _BinkGetFrameBuffersInfo(int, int) { _asm { jmp[bink2w32.BinkGetFrameBuffersInfo] } }
extern "C" __declspec(naked) void __stdcall _BinkGetGPUDataBuffersInfo(int, int) { _asm { jmp[bink2w32.BinkGetGPUDataBuffersInfo] } }
extern "C" __declspec(naked) void __stdcall _BinkGetKeyFrame(int, int, int) { _asm { jmp[bink2w32.BinkGetKeyFrame] } }
extern "C" __declspec(naked) void __stdcall _BinkGetPalette(int) { _asm { jmp[bink2w32.BinkGetPalette] } }
extern "C" __declspec(naked) void __stdcall _BinkGetPlatformInfo(int, int) { _asm { jmp[bink2w32.BinkGetPlatformInfo] } }
extern "C" __declspec(naked) void __stdcall _BinkGetRealtime(int, int, int) { _asm { jmp[bink2w32.BinkGetRealtime] } }
extern "C" __declspec(naked) void __stdcall _BinkGetRects(int, int) { _asm { jmp[bink2w32.BinkGetRects] } }
extern "C" __declspec(naked) void __stdcall _BinkGetRects4(int) { _asm { jmp[bink2w32.BinkGetRects4] } }
extern "C" __declspec(naked) void __stdcall _BinkGetSubtitleByIndex(int, int, int, int) { _asm { jmp[bink2w32.BinkGetSubtitleByIndex] } }
extern "C" __declspec(naked) void __stdcall _BinkGetSummary(int, int) { _asm { jmp[bink2w32.BinkGetSummary] } }
extern "C" __declspec(naked) void __stdcall _BinkGetTrackData(int, int) { _asm { jmp[bink2w32.BinkGetTrackData] } }
extern "C" __declspec(naked) void __stdcall _BinkGetTrackData12(int, int, int) { _asm { jmp[bink2w32.BinkGetTrackData12] } }
extern "C" __declspec(naked) void __stdcall _BinkGetTrackID(int, int) { _asm { jmp[bink2w32.BinkGetTrackID] } }
extern "C" __declspec(naked) void __stdcall _BinkGetTrackMaxSize(int, int) { _asm { jmp[bink2w32.BinkGetTrackMaxSize] } }
extern "C" __declspec(naked) void __stdcall _BinkGetTrackType(int, int) { _asm { jmp[bink2w32.BinkGetTrackType] } }
extern "C" __declspec(naked) void __stdcall _BinkGoto(int, int, int) { _asm { jmp[bink2w32.BinkGoto] } }
extern "C" __declspec(naked) void __stdcall _BinkIsSoftwareCursor(int, int) { _asm { jmp[bink2w32.BinkIsSoftwareCursor] } }
extern "C" __declspec(naked) void __stdcall _BinkLoadSubtitles(int, int) { _asm { jmp[bink2w32.BinkLoadSubtitles] } }
extern "C" __declspec(naked) void __stdcall _BinkLogoAddress() { _asm { jmp[bink2w32.BinkLogoAddress] } }
extern "C" __declspec(naked) void __stdcall _BinkNextFrame(int) { _asm { jmp[bink2w32.BinkNextFrame] } }
extern "C" __declspec(naked) void __stdcall _BinkOpen(int, int) { _asm { jmp[bink2w32.BinkOpen] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenDirectSound(int) { _asm { jmp[bink2w32.BinkOpenDirectSound] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenMiles(int) { _asm { jmp[bink2w32.BinkOpenMiles] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenTrack(int, int) { _asm { jmp[bink2w32.BinkOpenTrack] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenWaveOut(int) { _asm { jmp[bink2w32.BinkOpenWaveOut] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenWithOptions(int, int, int) { _asm { jmp[bink2w32.BinkOpenWithOptions] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenXAudio2(int) { _asm { jmp[bink2w32.BinkOpenXAudio2] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenXAudio2_8(int, int) { _asm { jmp[bink2w32.BinkOpenXAudio2_8] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenXAudio27(int, int) { _asm { jmp[bink2w32.BinkOpenXAudio27] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenXAudio28(int, int) { _asm { jmp[bink2w32.BinkOpenXAudio28] } }
extern "C" __declspec(naked) void __stdcall _BinkOpenXAudio29(int, int) { _asm { jmp[bink2w32.BinkOpenXAudio29] } }
extern "C" __declspec(naked) void __stdcall _BinkPause(int, int) { _asm { jmp[bink2w32.BinkPause] } }
extern "C" __declspec(naked) void __stdcall _BinkRegisterFrameBuffers(int, int) { _asm { jmp[bink2w32.BinkRegisterFrameBuffers] } }
extern "C" __declspec(naked) void __stdcall _BinkRegisterGPUDataBuffers(int, int) { _asm { jmp[bink2w32.BinkRegisterGPUDataBuffers] } }
extern "C" __declspec(naked) void __stdcall _BinkRequestStopAsyncThread(int) { _asm { jmp[bink2w32.BinkRequestStopAsyncThread] } }
extern "C" __declspec(naked) void __stdcall _BinkRequestStopAsyncThreadsMulti(int, int) { _asm { jmp[bink2w32.BinkRequestStopAsyncThreadsMulti] } }
extern "C" __declspec(naked) void __stdcall _BinkRestoreCursor(int) { _asm { jmp[bink2w32.BinkRestoreCursor] } }
extern "C" __declspec(naked) void __stdcall _BinkService(int) { _asm { jmp[bink2w32.BinkService] } }
extern "C" __declspec(naked) void __stdcall _BinkServiceSound() { _asm { jmp[bink2w32.BinkServiceSound] } }
extern "C" __declspec(naked) void __stdcall _BinkSetError(int) { _asm { jmp[bink2w32.BinkSetError] } }
extern "C" __declspec(naked) void __stdcall _BinkSetFileOffset(int, int) { _asm { jmp[bink2w32.BinkSetFileOffset] } }
extern "C" __declspec(naked) void __stdcall _BinkSetFrameRate(int, int) { _asm { jmp[bink2w32.BinkSetFrameRate] } }
extern "C" __declspec(naked) void __stdcall _BinkSetIO(int) { _asm { jmp[bink2w32.BinkSetIO] } }
extern "C" __declspec(naked) void __stdcall _BinkSetIOSize(int) { _asm { jmp[bink2w32.BinkSetIOSize] } }
extern "C" __declspec(naked) void __stdcall _BinkSetIOSize8(int, int) { _asm { jmp[bink2w32.BinkSetIOSize8] } }
extern "C" __declspec(naked) void __stdcall _BinkSetMemory(int, int) { _asm { jmp[bink2w32.BinkSetMemory] } }
extern "C" __declspec(naked) void __stdcall _BinkSetMixBinVolumes(int, int, int, int, int) { _asm { jmp[bink2w32.BinkSetMixBinVolumes] } }
extern "C" __declspec(naked) void __stdcall _BinkSetMixBins(int, int, int, int) { _asm { jmp[bink2w32.BinkSetMixBins] } }
extern "C" __declspec(naked) void __stdcall _BinkSetOSFileCallbacks(int, int, int, int) { _asm { jmp[bink2w32.BinkSetOSFileCallbacks] } }
extern "C" __declspec(naked) void __stdcall _BinkSetPan(int, int, int) { _asm { jmp[bink2w32.BinkSetPan] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSimulate(int) { _asm { jmp[bink2w32.BinkSetSimulate] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSoundOnOff(int, int) { _asm { jmp[bink2w32.BinkSetSoundOnOff] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSoundSystem(int, int) { _asm { jmp[bink2w32.BinkSetSoundSystem] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSoundSystem2(int, int, int) { _asm { jmp[bink2w32.BinkSetSoundSystem2] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSoundTrack(int, int) { _asm { jmp[bink2w32.BinkSetSoundTrack] } }
extern "C" __declspec(naked) void __stdcall _BinkSetSpeakerVolumes(int, int, int, int, int) { _asm { jmp[bink2w32.BinkSetSpeakerVolumes] } }
extern "C" __declspec(naked) void __stdcall _BinkSetVideoOnOff(int, int) { _asm { jmp[bink2w32.BinkSetVideoOnOff] } }
extern "C" __declspec(naked) void __stdcall _BinkSetVolume(int, int, int) { _asm { jmp[bink2w32.BinkSetVolume] } }
extern "C" __declspec(naked) void __stdcall _BinkSetWillLoop(int, int) { _asm { jmp[bink2w32.BinkSetWillLoop] } }
extern "C" __declspec(naked) void __stdcall _BinkShouldSkip(int) { _asm { jmp[bink2w32.BinkShouldSkip] } }
extern "C" __declspec(naked) void __stdcall _BinkStartAsyncThread(int, int) { _asm { jmp[bink2w32.BinkStartAsyncThread] } }
extern "C" __declspec(naked) void __stdcall _BinkUseTelemetry(int) { _asm { jmp[bink2w32.BinkUseTelemetry] } }
extern "C" __declspec(naked) void __stdcall _BinkUseTmLite(int) { _asm { jmp[bink2w32.BinkUseTmLite] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilCPUs() { _asm { jmp[bink2w32.BinkUtilCPUs] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilFree(int) { _asm { jmp[bink2w32.BinkUtilFree] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMalloc(int, int) { _asm { jmp[bink2w32.BinkUtilMalloc] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMutexCreate(int, int) { _asm { jmp[bink2w32.BinkUtilMutexCreate] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMutexDestroy(int) { _asm { jmp[bink2w32.BinkUtilMutexDestroy] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMutexLock(int) { _asm { jmp[bink2w32.BinkUtilMutexLock] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMutexLockTimeOut(int, int) { _asm { jmp[bink2w32.BinkUtilMutexLockTimeOut] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilMutexUnlock(int) { _asm { jmp[bink2w32.BinkUtilMutexUnlock] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilSoundGlobalLock() { _asm { jmp[bink2w32.BinkUtilSoundGlobalLock] } }
extern "C" __declspec(naked) void __stdcall _BinkUtilSoundGlobalUnlock() { _asm { jmp[bink2w32.BinkUtilSoundGlobalUnlock] } }
extern "C" __declspec(naked) void __stdcall _BinkWait(int) { _asm { jmp[bink2w32.BinkWait] } }
extern "C" __declspec(naked) void __stdcall _BinkWaitStopAsyncThread(int) { _asm { jmp[bink2w32.BinkWaitStopAsyncThread] } }
extern "C" __declspec(naked) void __stdcall _BinkWaitStopAsyncThreadsMulti(int, int) { _asm { jmp[bink2w32.BinkWaitStopAsyncThreadsMulti] } }
extern "C" __declspec(naked) void __stdcall _RADSetMemory(int, int) { _asm { jmp[bink2w32.RADSetMemory] } }
extern "C" __declspec(naked) void __stdcall _RADTimerRead() { _asm { jmp[bink2w32.RADTimerRead] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a1bpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a1bpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a1bpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a1bpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a1bpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a1bpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a1bpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a1bpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a4bpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a4bpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a4bpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a4bpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a4bpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a4bpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16a4bpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16a4bpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_16bpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_16bpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24bpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24bpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_24rbpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_24rbpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32abpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32abpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32abpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32abpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32abpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32abpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32abpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32abpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32bpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32bpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rabpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rabpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rabpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rabpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rabpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rabpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rabpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rabpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_32rbpp_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_32rbpp_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY40(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY40] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY52(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY52] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_UYVY_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_UYVY_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY240(int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY240] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY248(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY248] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY252(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY252] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY2_mask48(int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY2_mask48] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY2_mask56(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY2_mask56] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YUY2_mask60(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YUY2_mask60] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YV1244(int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YV1244] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YV1252(int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YV1252] } }
extern "C" __declspec(naked) void __stdcall _YUV_blit_YV1256(int, int, int, int, int, int, int, int, int, int, int, int, int, int) { _asm { jmp[bink2w32.YUV_blit_YV1256] } }
extern "C" __declspec(naked) void __stdcall _YUV_init4(int) { _asm { jmp[bink2w32.YUV_init4] } }
extern "C" __declspec(naked) void __stdcall _radfree(int) { _asm { jmp[bink2w32.radfree] } }
extern "C" __declspec(naked) void __stdcall _radmalloc(int) { _asm { jmp[bink2w32.radmalloc] } }
#endif

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