#pragma once

#include <redasm/renderer.h>
#include <redasm/theme.h>
#include <string_view>

namespace redasm::api::internal {

void renderer_themed(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_mnemonic(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_register(RDRenderer* self, std::string_view s);
void renderer_text(RDRenderer* self, std::string_view s);
void renderer_reference(RDRenderer* self, RDAddress address);
void renderer_constant(RDRenderer* self, usize value, int base);

} // namespace redasm::api::internal
