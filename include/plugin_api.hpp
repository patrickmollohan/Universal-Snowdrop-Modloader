#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MODLOADER_PLUGIN_API_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ModLoaderPluginCtx ModLoaderPluginCtx;

typedef struct ModLoaderHostAPI {
    uint32_t apiVersion;

    // --- Widgets -------------------------------------------------------
    void (*Text)(ModLoaderPluginCtx* ctx, const char* fmt, ...);
    bool (*Checkbox)(ModLoaderPluginCtx* ctx, const char* label, bool* value);
    bool (*SliderInt)(ModLoaderPluginCtx* ctx, const char* label, int* value, int min, int max);
    bool (*SliderFloat)(ModLoaderPluginCtx* ctx, const char* label, float* value, float min, float max);
    bool (*InputText)(ModLoaderPluginCtx* ctx, const char* label, char* buf, size_t bufSize);
    bool (*Button)(ModLoaderPluginCtx* ctx, const char* label);
    void (*Separator)(ModLoaderPluginCtx* ctx);

    // --- Config ----------------------------------------------------------
    bool (*GetConfigBool)(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool defaultValue);
    int  (*GetConfigInt)(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int defaultValue);
    void (*SetConfigBool)(ModLoaderPluginCtx* ctx, const char* key, const char* comment, bool value);
    void (*SetConfigInt)(ModLoaderPluginCtx* ctx, const char* key, const char* comment, int value);

    // --- Commands ----------------------------------------------------------
    bool (*SendCommand)(ModLoaderPluginCtx* ctx, const char* targetPlugin, const char* command);

    // --- Misc ------------------------------------------------------------
    void (*Log)(ModLoaderPluginCtx* ctx, const char* fmt, ...);
} ModLoaderHostAPI;

typedef void (*ModLoaderDrawMenuFn)(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx);
typedef void (*ModLoaderCommandFn)(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx, const char* command);

typedef struct ModLoaderPluginInfo {
    const char* name;
    const char* version;
    const char* author;
    ModLoaderDrawMenuFn DrawMenu;
    ModLoaderCommandFn OnCommand;
} ModLoaderPluginInfo;

typedef bool (*ModLoaderInitPluginFn)(const ModLoaderHostAPI* host, ModLoaderPluginCtx* ctx, ModLoaderPluginInfo* outInfo);

#ifdef __cplusplus
}
#endif
