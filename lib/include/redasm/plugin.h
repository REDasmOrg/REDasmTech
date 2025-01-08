#pragma once

#define RDPLUGIN_LEVEL 1

#define RDPLUGIN_HEADER(Self)                                                  \
    u32 level;                                                                 \
    const char* id;                                                            \
    const char* name;                                                          \
    void (*oninit)(const struct Self##Plugin*);                                \
    void (*onshutdown)(const struct Self##Plugin*);                            \
    Self* (*create)(const struct Self##Plugin*);                               \
    void (*destroy)(Self*);                                                    \
    u32 flags;

#define PF_LAST (1u << 31)
