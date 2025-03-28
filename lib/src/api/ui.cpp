#include "../state.h"
#include <redasm/ui.h>
#include <spdlog/spdlog.h>

#define return_ui_call(f, ...)                                                 \
    if(!redasm::state::params.ui.f) ct_exceptf("UI method '%s' not set", #f);  \
    return redasm::state::params.ui.f(__VA_ARGS__);

void rdui_message(const char* title, const char* text) {
    spdlog::trace("rdui_message('{}', '{}')", title, text);
    return_ui_call(message, title, text);
}

bool rdui_confirm(const char* title, const char* text) {
    spdlog::trace("rdui_confirm('{}', '{}')", title, text);
    return_ui_call(confirm, title, text);
}

int rdui_getitem(const char* title, const char* text,
                 const RDUIOptions* options, usize c) {
    spdlog::trace("rdui_getitem('{}', '{}', {}, {})", title, text,
                  fmt::ptr(options), c);
    return_ui_call(getitem, title, text, options, c);
}

bool rdui_getchecked(const char* title, const char* text, RDUIOptions* options,
                     usize c) {
    spdlog::trace("rdui_getchecked('{}', '{}', {}, {})", title, text,
                  fmt::ptr(options), c);
    return_ui_call(getchecked, title, text, options, c);
}

const char* rdui_gettext(const char* title, const char* text, bool* ok) {
    spdlog::trace("rdui_gettext('{}', '{}', {})", title, text, fmt::ptr(ok));
    return_ui_call(gettext, title, text, ok);
}

double rdui_getdouble(const char* title, const char* text, bool* ok) {
    spdlog::trace("rdui_getdouble('{}', '{}', {})", title, text, fmt::ptr(ok));
    return_ui_call(getdouble, title, text, ok);
}

i64 rdui_getsigned(const char* title, const char* text, bool* ok) {
    spdlog::trace("rdui_getsigned('{}', '{}', {})", title, text, fmt::ptr(ok));
    return_ui_call(getsigned, title, text, ok);
}

u64 rdui_getunsigned(const char* title, const char* text, bool* ok) {
    spdlog::trace("rdui_getunsigned('{}', '{}', {})", title, text,
                  fmt::ptr(ok));
    return_ui_call(getunsigned, title, text, ok);
}
