#include "../internal/renderer.h"
#include <redasm/renderer.h>
#include <spdlog/spdlog.h>

void rdrenderer_themed(RDRenderer* self, const char* s, RDThemeKind kind) {
    if(s)
        redasm::api::internal::renderer_themed(self, s, kind);
    else
        spdlog::error("rdrenderer_themed: invalid string");
}

void rdrenderer_mnem(RDRenderer* self, const char* s, RDThemeKind kind) {
    if(s)
        redasm::api::internal::renderer_mnem(self, s, kind);
    else
        spdlog::error("rdrenderer_mnemonic: invalid string");
}

void rdrenderer_unkn(RDRenderer* self) {
    redasm::api::internal::renderer_unkn(self);
}

void rdrenderer_reg(RDRenderer* self, int reg) {
    redasm::api::internal::renderer_reg(self, reg);
}

void rdrenderer_text(RDRenderer* self, const char* s) {
    if(s)
        redasm::api::internal::renderer_text(self, s);
    else
        spdlog::error("rdrenderer_text: invalid string");
}

void rdrenderer_ws(RDRenderer* self) {
    redasm::api::internal::renderer_ws(self);
}

void rdrenderer_addr_ex(RDRenderer* self, RDAddress address, int flags) {
    redasm::api::internal::renderer_addr_ex(self, address, flags);
}

void rdrenderer_addr(RDRenderer* self, RDAddress address) {
    redasm::api::internal::renderer_addr(self, address);
}

void rdrenderer_cnst_ex(RDRenderer* self, usize value, int base, int flags) {
    redasm::api::internal::renderer_cnst_ex(self, value, base, flags);
}

void rdrenderer_cnst(RDRenderer* self, usize value) {
    redasm::api::internal::renderer_cnst(self, value);
}

void rdrenderer_i8_ex(RDRenderer* self, i8 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_i8_ex(self, v, base, kind);
}
void rdrenderer_i16_ex(RDRenderer* self, i16 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_i16_ex(self, v, base, kind);
}
void rdrenderer_i32_ex(RDRenderer* self, i32 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_i32_ex(self, v, base, kind);
}
void rdrenderer_i64_ex(RDRenderer* self, i64 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_i64_ex(self, v, base, kind);
}
void rdrenderer_u8_ex(RDRenderer* self, u8 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_u8_ex(self, v, base, kind);
    redasm::api::internal::renderer_u8_ex(self, v, base, kind);
}
void rdrenderer_u16_ex(RDRenderer* self, u16 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_u16_ex(self, v, base, kind);
}
void rdrenderer_u32_ex(RDRenderer* self, u32 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_u32_ex(self, v, base, kind);
}
void rdrenderer_u64_ex(RDRenderer* self, u64 v, int base, RDThemeKind kind) {
    redasm::api::internal::renderer_u64_ex(self, v, base, kind);
}
void rdrenderer_i8(RDRenderer* self, i8 v, int base) {
    redasm::api::internal::renderer_i8(self, v, base);
}
void rdrenderer_i16(RDRenderer* self, i16 v, int base) {
    redasm::api::internal::renderer_i16(self, v, base);
}
void rdrenderer_i32(RDRenderer* self, i32 v, int base) {
    redasm::api::internal::renderer_i32(self, v, base);
}
void rdrenderer_i64(RDRenderer* self, i64 v, int base) {
    redasm::api::internal::renderer_i64(self, v, base);
}
void rdrenderer_u8(RDRenderer* self, u8 v, int base) {
    redasm::api::internal::renderer_u8(self, v, base);
}
void rdrenderer_u16(RDRenderer* self, u16 v, int base) {
    redasm::api::internal::renderer_u16(self, v, base);
}
void rdrenderer_u32(RDRenderer* self, u32 v, int base) {
    redasm::api::internal::renderer_u32(self, v, base);
}
void rdrenderer_u64(RDRenderer* self, u64 v, int base) {
    redasm::api::internal::renderer_u64(self, v, base);
}
