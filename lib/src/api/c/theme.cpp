#include "../../theme.h"
#include "../../state.h"
#include <redasm/theme.h>
#include <spdlog/spdlog.h>

bool rd_settheme(RDThemeKind kind, const char* color) {
    spdlog::trace("rd_settheme({}, '{}')", static_cast<int>(kind), color);
    if(color) return redasm::state::theme.set_color(kind, color);
    return false;
}

const char* rd_gettheme(RDThemeKind kind) {
    spdlog::trace("rd_gettheme({})", static_cast<int>(kind));
    std::string_view color = redasm::state::theme.get_color(kind);
    return color.empty() ? nullptr : color.data();
}

RDThemeColors rd_getfulltheme() {
    spdlog::trace("get_fulltheme()");
    static std::array<const char*, THEME_COUNT> res;

    for(usize i = THEME_DEFAULT; i < THEME_COUNT; i++) {
        res[i] =
            redasm::state::theme.get_color(static_cast<RDThemeKind>(i)).data();
    }

    return res.data();
}
