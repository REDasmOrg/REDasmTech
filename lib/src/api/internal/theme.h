#pragma once

#include <redasm/theme.h>
#include <string>
#include <string_view>

namespace redasm::api::internal {

bool set_theme(RDThemeKind kind, const std::string& color);
std::string_view get_theme(RDThemeKind kind);
RDThemeColors get_fulltheme();

} // namespace redasm::api::internal
