#pragma once

#include <redasm/renderer.h>
#include <redasm/theme.h>
#include <string_view>

namespace redasm::api::internal {

void renderer_themed(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_mnem(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_reg(RDRenderer* self, std::string_view s);
void renderer_text(RDRenderer* self, std::string_view s);
void renderer_ws(RDRenderer* self);
void renderer_addr(RDRenderer* self, RDAddress address);
void renderer_cnst(RDRenderer* self, usize value, int base);

} // namespace redasm::api::internal
