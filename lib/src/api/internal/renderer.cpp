#include "renderer.h"
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

void renderer_unkn(RDRenderer* self) {
    spdlog::trace("renderer_unkn({})", fmt::ptr(self));
    api::from_c(self)->unknown();
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

void renderer_i8_ex(RDRenderer* self, i8 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_i8_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_i8(v, base, kind);
}

void renderer_i16_ex(RDRenderer* self, i16 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_i16_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_i16(v, base, kind);
}

void renderer_i32_ex(RDRenderer* self, i32 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_i32_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_i32(v, base, kind);
}

void renderer_i64_ex(RDRenderer* self, i64 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_i64_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_i64(v, base, kind);
}

void renderer_u8_ex(RDRenderer* self, u8 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_u8_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_u8(v, base, kind);
}

void renderer_u16_ex(RDRenderer* self, u16 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_u16_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_u16(v, base, kind);
}

void renderer_u32_ex(RDRenderer* self, u32 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_u32_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_u32(v, base, kind);
}

void renderer_u64_ex(RDRenderer* self, u64 v, int base, RDThemeKind kind) {
    spdlog::trace("renderer_u64_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    api::from_c(self)->int_u64(v, base, kind);
}

} // namespace redasm::api::internal
