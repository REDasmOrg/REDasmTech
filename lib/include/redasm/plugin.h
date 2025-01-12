#pragma once

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
