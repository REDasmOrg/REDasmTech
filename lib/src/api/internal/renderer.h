#pragma once

#include <redasm/renderer.h>
#include <redasm/theme.h>
#include <string_view>

namespace redasm::api::internal {

void renderer_themed(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_mnem(RDRenderer* self, std::string_view s, RDThemeKind kind);
void renderer_unkn(RDRenderer* self);
void renderer_reg(RDRenderer* self, int reg);
void renderer_text(RDRenderer* self, std::string_view s);
void renderer_ws(RDRenderer* self);
void renderer_addr_ex(RDRenderer* self, RDAddress address, int flags);
void renderer_cnst_ex(RDRenderer* self, u64 value, int base, int flags);
void renderer_i8_ex(RDRenderer* self, i8 v, int base, RDThemeKind kind);
void renderer_i16_ex(RDRenderer* self, i16 v, int base, RDThemeKind kind);
void renderer_i32_ex(RDRenderer* self, i32 v, int base, RDThemeKind kind);
void renderer_i64_ex(RDRenderer* self, i64 v, int base, RDThemeKind kind);
void renderer_u8_ex(RDRenderer* self, u8 v, int base, RDThemeKind kind);
void renderer_u16_ex(RDRenderer* self, u16 v, int base, RDThemeKind kind);
void renderer_u32_ex(RDRenderer* self, u32 v, int base, RDThemeKind kind);
void renderer_u64_ex(RDRenderer* self, u64 v, int base, RDThemeKind kind);

inline void renderer_cnst(RDRenderer* self, u64 value) {
    renderer_cnst_ex(self, value, 0, 0);
}

inline void renderer_addr(RDRenderer* self, RDAddress address) {
    renderer_addr_ex(self, address, 0);
}

inline void renderer_i8(RDRenderer* self, i8 v, int base) {
    renderer_i8_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_i16(RDRenderer* self, i16 v, int base) {
    renderer_i16_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_i32(RDRenderer* self, i32 v, int base) {
    renderer_i32_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_i64(RDRenderer* self, i64 v, int base) {
    renderer_i64_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_u8(RDRenderer* self, u8 v, int base) {
    renderer_u8_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_u16(RDRenderer* self, u16 v, int base) {
    renderer_u16_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_u32(RDRenderer* self, u32 v, int base) {
    renderer_u32_ex(self, v, base, THEME_CONSTANT);
}

inline void renderer_u64(RDRenderer* self, u64 v, int base) {
    renderer_u64_ex(self, v, base, THEME_CONSTANT);
}

} // namespace redasm::api::internal
