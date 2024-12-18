#pragma once

#include <redasm/byte.h>
#include <redasm/theme.h>
#include <redasm/types.h>

RD_HANDLE(RDRenderer);

typedef struct RDRendererParams {
    RDRenderer* renderer;
    RDAddress address;
    RDByte byte;

    union {
        usize segment_index;
    };
} RDRendererParams;

typedef enum RDRendererConst {
    RC_NEEDSIGN = 1,
    RC_NOSIGN,
} RDRendererConst;

REDASM_EXPORT void rdrenderer_themed(RDRenderer* self, const char* s,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_mnem(RDRenderer* self, const char* s,
                                   RDThemeKind kind);

REDASM_EXPORT void rdrenderer_reg(RDRenderer* self, int reg);
REDASM_EXPORT void rdrenderer_text(RDRenderer* self, const char* s);
REDASM_EXPORT void rdrenderer_ws(RDRenderer* self);
REDASM_EXPORT void rdrenderer_addr_ex(RDRenderer* self, RDAddress address,
                                      int flags);
REDASM_EXPORT void rdrenderer_addr(RDRenderer* self, RDAddress address);
REDASM_EXPORT void rdrenderer_cnst_ex(RDRenderer* self, usize value, int base,
                                      int flags);
REDASM_EXPORT void rdrenderer_cnst(RDRenderer* self, usize value);
