#include "theme.h"
#include "../../state.h"
#include <spdlog/spdlog.h>
#include <string>

namespace redasm::api::internal {

bool set_theme(RDThemeKind kind, const std::string& color) {
    spdlog::trace("set_theme({}, '{}')", static_cast<int>(kind), color);
    return state::theme.set_color(kind, color);
}

std::string_view get_theme(RDThemeKind kind) {
    spdlog::trace("get_theme({})", static_cast<int>(kind));
    return state::theme.get_color(kind);
}

RDThemeColors get_fulltheme() {
    spdlog::trace("get_theme_colors()");

    static std::array<const char*, THEME_COUNT> res;

    for(usize i = THEME_DEFAULT; i < THEME_COUNT; i++)
        res[i] = state::theme.get_color(static_cast<RDThemeKind>(i)).data();

    return res.data();
}

} // namespace redasm::api::internal
