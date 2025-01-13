#pragma once

#include <redasm/byte.h>
#include <redasm/theme.h>
#include <redasm/types.h>
#include <redasm/typing.h>

RD_HANDLE(RDRenderer);

typedef struct RDRendererParams {
    RDRenderer* renderer;
    RDAddress address;
    RDByte byte;

    union {
        usize segment_index;
    };
} RDRendererParams;

typedef enum RDRendererFlags {
    RC_NEEDSIGN = 1,
    RC_NOSIGN,
} RDRendererFlags;

REDASM_EXPORT void rdrenderer_themed(RDRenderer* self, const char* s,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_mnem(RDRenderer* self, u32 id, RDThemeKind kind);
REDASM_EXPORT void rdrenderer_unkn(RDRenderer* self);
REDASM_EXPORT void rdrenderer_reg(RDRenderer* self, int reg);
REDASM_EXPORT void rdrenderer_text(RDRenderer* self, const char* s);
REDASM_EXPORT void rdrenderer_str(RDRenderer* self, const char* s);
REDASM_EXPORT void rdrenderer_ws(RDRenderer* self);
REDASM_EXPORT void rdrenderer_addr_ex(RDRenderer* self, RDAddress address,
                                      int flags);
REDASM_EXPORT void rdrenderer_addr(RDRenderer* self, RDAddress address);
REDASM_EXPORT void rdrenderer_cnst_ex(RDRenderer* self, usize value, int base,
                                      int flags);
REDASM_EXPORT void rdrenderer_cnst(RDRenderer* self, usize value);
REDASM_EXPORT void rdrenderer_i8_ex(RDRenderer* self, i8 v, int base,
                                    RDThemeKind kind);
REDASM_EXPORT void rdrenderer_i16_ex(RDRenderer* self, i16 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_i32_ex(RDRenderer* self, i32 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_i64_ex(RDRenderer* self, i64 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_u8_ex(RDRenderer* self, u8 v, int base,
                                    RDThemeKind kind);
REDASM_EXPORT void rdrenderer_u16_ex(RDRenderer* self, u16 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_u32_ex(RDRenderer* self, u32 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_u64_ex(RDRenderer* self, u64 v, int base,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_i8(RDRenderer* self, i8 v, int base);
REDASM_EXPORT void rdrenderer_i16(RDRenderer* self, i16 v, int base);
REDASM_EXPORT void rdrenderer_i32(RDRenderer* self, i32 v, int base);
REDASM_EXPORT void rdrenderer_i64(RDRenderer* self, i64 v, int base);
REDASM_EXPORT void rdrenderer_u8(RDRenderer* self, u8 v, int base);
REDASM_EXPORT void rdrenderer_u16(RDRenderer* self, u16 v, int base);
REDASM_EXPORT void rdrenderer_u32(RDRenderer* self, u32 v, int base);
REDASM_EXPORT void rdrenderer_u64(RDRenderer* self, u64 v, int base);
