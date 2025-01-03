#include "../internal/ui.h"
#include <redasm/ui.h>

void rdui_message(const char* title, const char* text) {
    redasm::api::internal::ui_message(title, text);
}

bool rdui_confirm(const char* title, const char* text) {
    return redasm::api::internal::ui_confirm(title, text);
}

int rdui_getitem(const char* title, const char* text,
                 const RDUIOptions* options, usize c) {
    return redasm::api::internal::ui_getitem(title, text, options, c);
}

bool rdui_getchecked(const char* title, const char* text, RDUIOptions* options,
                     usize c) {
    return redasm::api::internal::ui_getchecked(title, text, options, c);
}

const char* rdui_gettext(const char* title, const char* text, bool* ok) {
    return redasm::api::internal::ui_gettext(title, text, ok);
}

double rdui_getdouble(const char* title, const char* text, bool* ok) {
    return redasm::api::internal::ui_getdouble(title, text, ok);
}

i64 rdui_getsigned(const char* title, const char* text, bool* ok) {
    return redasm::api::internal::ui_getsigned(title, text, ok);
}

u64 rdui_getunsigned(const char* title, const char* text, bool* ok) {
    return redasm::api::internal::ui_getunsigned(title, text, ok);
}
