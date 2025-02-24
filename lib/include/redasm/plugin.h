#pragma once

#include <redasm/common.h>

#define RDPLUGIN_HEADER(Self)                                                  \
    u32 level;                                                                 \
    const char* id;                                                            \
    const char* name;                                                          \
    void (*on_init)(const struct Self##Plugin*);                               \
    void (*on_shutdown)(const struct Self##Plugin*);                           \
    Self* (*create)(const struct Self##Plugin*);                               \
    void (*destroy)(Self*);                                                    \
    u32 flags;

#define PF_LAST (1u << 31)

REDASM_EXPORT const char* rdplugin_getorigin(const void* plugin);
REDASM_EXPORT bool rdplugin_isorigin(const void* plugin, const char* origin);

// Plugin entry points
REDASM_EXPORT void rdplugin_create(void);
REDASM_EXPORT void rdplugin_destroy(void);
