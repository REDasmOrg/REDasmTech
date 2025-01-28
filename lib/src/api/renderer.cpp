#include "../surface/renderer.h"
#include "marshal.h"
#include <redasm/renderer.h>
#include <spdlog/spdlog.h>

void rdrenderer_themed(RDRenderer* self, const char* s, RDThemeKind kind) {
    spdlog::trace("rd_renderer_themed({}, '{}', {})", fmt::ptr(self), s,
                  static_cast<int>(kind));

    if(s)
        redasm::api::from_c(self)->chunk(s, kind);
    else
        spdlog::error("rdrenderer_themed: invalid string");
}

void rdrenderer_mnem(RDRenderer* self, u32 id, RDThemeKind kind) {
    spdlog::trace("rdrenderer_mnem({}, {}, {})", fmt::ptr(self), id,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->mnem(id, kind);
}

void rdrenderer_unkn(RDRenderer* self) {
    spdlog::trace("rdrenderer_unkn({})", fmt::ptr(self));
    redasm::api::from_c(self)->unknown();
}

void rdrenderer_reg(RDRenderer* self, int reg) {
    spdlog::trace("rdrenderer_reg({}, {})", fmt::ptr(self), reg);
    redasm::api::from_c(self)->reg(reg);
}

void rdrenderer_text(RDRenderer* self, const char* s) {
    spdlog::trace("rdrenderer_text({}, '{}')", fmt::ptr(self), s);
    if(s)
        redasm::api::from_c(self)->chunk(s);
    else
        spdlog::error("rdrenderer_text: invalid string");
}

void rdrenderer_str(RDRenderer* self, const char* s) {
    spdlog::trace("rdrenderer_str({}, '{}')", fmt::ptr(self), s);
    if(s)
        redasm::api::from_c(self)->string(s);
    else
        spdlog::error("rdrenderer_str: invalid string");
}

void rdrenderer_ws(RDRenderer* self) {
    spdlog::trace("rdrenderer_ws({})", fmt::ptr(self));
    redasm::api::from_c(self)->ws();
}

void rdrenderer_addr_ex(RDRenderer* self, RDAddress address, int flags) {
    spdlog::trace("rdrenderer_addr_ex({}, {})", fmt::ptr(self), address);
    redasm::api::from_c(self)->addr(address, flags);
}

void rdrenderer_addr(RDRenderer* self, RDAddress address) {
    rdrenderer_addr_ex(self, address, 0);
}

void rdrenderer_cnst_ex(RDRenderer* self, usize value, int base, int flags) {
    spdlog::trace("rdrenderer_cnst_ex({}, {}, {})", fmt::ptr(self), value,
                  base);
    redasm::api::from_c(self)->constant(value, base, flags);
}

void rdrenderer_cnst(RDRenderer* self, usize value) {
    rdrenderer_cnst_ex(self, value, 0, 0);
}

void rdrenderer_i8_ex(RDRenderer* self, i8 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_i8_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_i8(v, base, kind);
}

void rdrenderer_i16_ex(RDRenderer* self, i16 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_i16_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_i16(v, base, kind);
}

void rdrenderer_i32_ex(RDRenderer* self, i32 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_i32_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_i32(v, base, kind);
}

void rdrenderer_i64_ex(RDRenderer* self, i64 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_i64_ex({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_i64(v, base, kind);
}

void rdrenderer_u8_ex(RDRenderer* self, u8 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_u8({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_u8(v, base, kind);
}

void rdrenderer_u16_ex(RDRenderer* self, u16 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_u16({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_u16(v, base, kind);
}

void rdrenderer_u32_ex(RDRenderer* self, u32 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_u32({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_u32(v, base, kind);
}

void rdrenderer_u64_ex(RDRenderer* self, u64 v, int base, RDThemeKind kind) {
    spdlog::trace("rdrenderer_u64({}, {}, {}, {}", fmt::ptr(self), v, base,
                  static_cast<int>(kind));
    redasm::api::from_c(self)->int_u64(v, base, kind);
}

// clang-format off
void rdrenderer_i8(RDRenderer* self, i8 v, int base) { rdrenderer_i8_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_i16(RDRenderer* self, i16 v, int base) { rdrenderer_i16_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_i32(RDRenderer* self, i32 v, int base) { rdrenderer_i32_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_i64(RDRenderer* self, i64 v, int base) { rdrenderer_i64_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_u8(RDRenderer* self, u8 v, int base) { rdrenderer_u8_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_u16(RDRenderer* self, u16 v, int base) { rdrenderer_u16_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_u32(RDRenderer* self, u32 v, int base) { rdrenderer_u32_ex(self, v, base, THEME_CONSTANT); }
void rdrenderer_u64(RDRenderer* self, u64 v, int base) { rdrenderer_u64_ex(self, v, base, THEME_CONSTANT); }
// clang-format on
