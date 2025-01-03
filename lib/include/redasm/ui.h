#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef struct RDUIOptions {
    const char* text;
    bool selected;
} RDUIOptions;

// clang-format off
typedef void (*UI_Message)(const char* title, const char* text);
typedef bool (*UI_Confirm)(const char* title, const char* text);
typedef int (*UI_GetItem)(const char* title, const char* text, const RDUIOptions* options, usize c);
typedef bool (*UI_GetChecked)(const char* title, const char* text, RDUIOptions* options, usize c);
typedef const char* (*UI_GetText)(const char* title, const char* text, bool* ok);
typedef double (*UI_GetDouble)(const char* title, const char* text, bool* ok);
typedef i64 (*UI_GetSigned)(const char* title, const char* text, bool* ok);
typedef u64 (*UI_GetUnsigned)(const char* title, const char* text, bool* ok);
// clang-format on

typedef struct RDUI {
    UI_Message message;
    UI_Confirm confirm;
    UI_GetChecked getchecked;
    UI_GetItem getitem;
    UI_GetText gettext;
    UI_GetDouble getdouble;
    UI_GetSigned getsigned;
    UI_GetUnsigned getunsigned;
} RDUI;

REDASM_EXPORT void rdui_message(const char* title, const char* text);
REDASM_EXPORT bool rdui_confirm(const char* title, const char* text);
REDASM_EXPORT int rdui_getitem(const char* title, const char* text,
                               const RDUIOptions* options, usize c);
REDASM_EXPORT bool rdui_getchecked(const char* title, const char* text,
                                   RDUIOptions* options, usize c);
REDASM_EXPORT const char* rdui_gettext(const char* title, const char* text,
                                       bool* ok);
REDASM_EXPORT double rdui_getdouble(const char* title, const char* text,
                                    bool* ok);
REDASM_EXPORT i64 rdui_getsigned(const char* title, const char* text, bool* ok);
REDASM_EXPORT u64 rdui_getunsigned(const char* title, const char* text,
                                   bool* ok);
