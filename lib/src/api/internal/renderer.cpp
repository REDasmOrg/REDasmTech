#include "renderer.h"
#include "../../context.h"
#include "../../state.h"
#include "../../surface/renderer.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void renderer_themed(RDRenderer* self, std::string_view s, RDThemeKind kind) {
    spdlog::trace("renderer_themed({}, '{}', {})", fmt::ptr(self), s,
                  static_cast<int>(kind));

    api::from_c(self)->chunk(s, kind);
}

void renderer_mnem(RDRenderer* self, std::string_view s, RDThemeKind kind) {
    spdlog::trace("renderer_mnem({}, '{}', {})", fmt::ptr(self), s,
                  static_cast<int>(kind));

    api::from_c(self)->chunk(s, kind).ws();
}

void renderer_reg(RDRenderer* self, std::string_view s) {
    spdlog::trace("renderer_reg({}, '{}')", fmt::ptr(self), s);
    api::from_c(self)->chunk(s, THEME_REG);
}

void renderer_text(RDRenderer* self, std::string_view s) {
    spdlog::trace("renderer_text({}, '{}')", fmt::ptr(self), s);
    api::from_c(self)->chunk(s);
}

void renderer_ws(RDRenderer* self) {
    spdlog::trace("renderer_ws({})", fmt::ptr(self));
    api::from_c(self)->ws();
}

void renderer_addr(RDRenderer* self, RDAddress address) {
    spdlog::trace("renderer_addr({}, {})", fmt::ptr(self), address);
    api::from_c(self)->addr(address);
}

void renderer_cnst(RDRenderer* self, usize value, int base) {
    spdlog::trace("renderer_cnst({}, {}, {})", fmt::ptr(self), value, base);
    api::from_c(self)->constant(value, base);
}

} // namespace redasm::api::internal
