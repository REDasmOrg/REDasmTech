#include "ui.h"
#include "../../error.h"
#include "../../state.h"

#define return_ui_call(f, ...)                                                 \
    if(!state::params.ui.f) except("UI method '{}' not set", #f);              \
    return state::params.ui.f(__VA_ARGS__);

namespace redasm::api::internal {

void ui_message(const char* title, const char* text) {
    return_ui_call(message, title, text);
}

bool ui_confirm(const char* title, const char* text) {
    return_ui_call(confirm, title, text);
}

int ui_getitem(const char* title, const char* text, const RDUIOptions* options,
               usize c) {
    return_ui_call(getitem, title, text, options, c);
}

bool ui_getchecked(const char* title, const char* text, RDUIOptions* options,
                   usize c) {
    return_ui_call(getchecked, title, text, options, c);
}

const char* ui_gettext(const char* title, const char* text, bool* ok) {
    return_ui_call(gettext, title, text, ok);
}

double ui_getdouble(const char* title, const char* text, bool* ok) {
    return_ui_call(getdouble, title, text, ok);
}

i64 ui_getsigned(const char* title, const char* text, bool* ok) {
    return_ui_call(getsigned, title, text, ok);
}

u64 ui_getunsigned(const char* title, const char* text, bool* ok) {
    return_ui_call(getunsigned, title, text, ok);
}

} // namespace redasm::api::internal
