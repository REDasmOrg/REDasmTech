#pragma once

#include <redasm/ui.h>

namespace redasm::api::internal {
void ui_message(const char* title, const char* text);
bool ui_confirm(const char* title, const char* text);
int ui_getitem(const char* title, const char* text, const RDUIOptions* options,
               usize c);
bool ui_getchecked(const char* title, const char* text, RDUIOptions* options,
                   usize c);
const char* ui_gettext(const char* title, const char* text, bool* ok);
double ui_getdouble(const char* title, const char* text, bool* ok);
i64 ui_getsigned(const char* title, const char* text, bool* ok);
u64 ui_getunsigned(const char* title, const char* text, bool* ok);

} // namespace redasm::api::internal
