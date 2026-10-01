#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ModLoaderPluginCtx ModLoaderPluginCtx;
typedef struct ModLoaderPatch ModLoaderPatch;

typedef void (*ModLoaderDrawMenuFn)(ModLoaderPluginCtx* ctx);
typedef void (*ModLoaderCommandFn)(ModLoaderPluginCtx* ctx, const char* command);

typedef void* (*ModLoaderGetProcFn)(const char* name);

typedef bool (*ModLoaderInitPluginFn)(ModLoaderGetProcFn getProc, ModLoaderPluginCtx* ctx);

#define MODLOADER_API(X) \
    /* Registration */ \
    X(void, SetPluginInfo,       (ModLoaderPluginCtx* ctx, const char* name, const char* version, const char* author)) \
    X(void, SetDrawMenuCallback, (ModLoaderPluginCtx* ctx, ModLoaderDrawMenuFn fn)) \
    X(void, SetCommandCallback,  (ModLoaderPluginCtx* ctx, ModLoaderCommandFn fn)) \
    /* Widgets */ \
    X(void, Text,        (ModLoaderPluginCtx* ctx, const char* fmt, ...)) \
    X(void, TextWrapped, (ModLoaderPluginCtx* ctx, const char* fmt, ...)) \
    X(bool, Checkbox,    (ModLoaderPluginCtx* ctx, const char* label, bool* value)) \
    X(bool, SliderInt,   (ModLoaderPluginCtx* ctx, const char* label, int* value, int min, int max)) \
    X(bool, SliderFloat, (ModLoaderPluginCtx* ctx, const char* label, float* value, float min, float max)) \
    X(bool, InputText,   (ModLoaderPluginCtx* ctx, const char* label, char* buf, size_t bufSize)) \
    X(bool, Button,      (ModLoaderPluginCtx* ctx, const char* label)) \
    X(void, Separator,   (ModLoaderPluginCtx* ctx)) \
    /* Config */ \
    X(bool, GetConfigBool, (ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool defaultValue)) \
    X(int,  GetConfigInt,  (ModLoaderPluginCtx* ctx, const char* key, const char* comment, int defaultValue)) \
    X(void, SetConfigBool, (ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool value)) \
    X(void, SetConfigInt,  (ModLoaderPluginCtx* ctx, const char* key, const char* comment, int value)) \
    /* Commands */ \
    X(bool, SendCommand, (ModLoaderPluginCtx* ctx, const char* targetPlugin, const char* command)) \
    /* Misc */ \
    X(void, Log, (ModLoaderPluginCtx* ctx, const char* fmt, ...)) \
    /* Memory */ \
    X(uintptr_t, FindPattern, (ModLoaderPluginCtx* ctx, const char* pattern)) \
    /* Patches */ \
    X(ModLoaderPatch*, CreatePatch,     (ModLoaderPluginCtx* ctx, const char* pattern, size_t offset, const uint8_t* bytes, size_t size)) \
    X(ModLoaderPatch*, CreatePatchAt,   (ModLoaderPluginCtx* ctx, uintptr_t address, const uint8_t* bytes, size_t size)) \
    X(bool,            SetPatchEnabled, (ModLoaderPluginCtx* ctx, ModLoaderPatch* patch, bool enabled)) \
    X(bool,            IsPatchEnabled,  (ModLoaderPluginCtx* ctx, ModLoaderPatch* patch)) \
    X(uintptr_t,       GetPatchAddress, (ModLoaderPluginCtx* ctx, ModLoaderPatch* patch)) \
    X(void,            DestroyPatch,    (ModLoaderPluginCtx* ctx, ModLoaderPatch* patch))

typedef struct ModLoaderHostAPI {
#define MODLOADER_X_MEMBER(ret, name, params) ret (*name) params;
    MODLOADER_API(MODLOADER_X_MEMBER)
#undef MODLOADER_X_MEMBER
} ModLoaderHostAPI;

static inline int ModLoader_BindAPI(ModLoaderGetProcFn getProc, ModLoaderHostAPI* out) {
    int missing = 0;
#define MODLOADER_X_BIND(ret, name, params) \
    out->name = (ret (*) params)getProc(#name); \
    if (!out->name) ++missing;
    MODLOADER_API(MODLOADER_X_BIND)
#undef MODLOADER_X_BIND
    return missing;
}

#ifdef __cplusplus
}

template <typename... Fn>
static inline bool ModLoader_AllBound(Fn... fns) {
    return ((fns != nullptr) && ...);
}
#endif
