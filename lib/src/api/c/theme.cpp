#include "../internal/theme.h"
#include <redasm/theme.h>

bool rd_settheme(RDThemeKind kind, const char* color) {
    if(color)
        return redasm::api::internal::set_theme(kind, color);

    return false;
}

const char* rd_gettheme(RDThemeKind kind) {
    std::string_view color = redasm::api::internal::get_theme(kind);

    if(color.empty())
        return nullptr;

    return color.data();
}

RDThemeColors rd_getfulltheme() {
    return redasm::api::internal::get_fulltheme();
}
