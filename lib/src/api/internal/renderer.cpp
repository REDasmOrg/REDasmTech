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

    Renderer* renderer = api::from_c(self);
    renderer->chunk(s, kind);
    renderer->prevmnemonic = true;
}

void renderer_reg(RDRenderer* self, int reg) {
    spdlog::trace("renderer_reg({}, {})", fmt::ptr(self), reg);
    api::from_c(self)->reg(reg);
}

void renderer_text(RDRenderer* self, std::string_view s) {
    spdlog::trace("renderer_text({}, '{}')", fmt::ptr(self), s);
    api::from_c(self)->chunk(s);
}

void renderer_ws(RDRenderer* self) {
    spdlog::trace("renderer_ws({})", fmt::ptr(self));
    api::from_c(self)->ws();
}

void renderer_addr_ex(RDRenderer* self, RDAddress address, int flags) {
    spdlog::trace("renderer_addr_ex({}, {})", fmt::ptr(self), address);
    api::from_c(self)->addr(address, flags);
}

void renderer_cnst_ex(RDRenderer* self, u64 value, int base, int flags) {
    spdlog::trace("renderer_cnst_ex({}, {}, {})", fmt::ptr(self), value, base);
    api::from_c(self)->constant(value, base, flags);
}

} // namespace redasm::api::internal
