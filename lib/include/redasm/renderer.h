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

REDASM_EXPORT void rdrenderer_themed(RDRenderer* self, const char* s,
                                     RDThemeKind kind);
REDASM_EXPORT void rdrenderer_mnemonic(RDRenderer* self, const char* s,
                                       RDThemeKind kind);

REDASM_EXPORT void rdrenderer_register(RDRenderer* self, const char* s);
REDASM_EXPORT void rdrenderer_text(RDRenderer* self, const char* s);
REDASM_EXPORT void rdrenderer_reference(RDRenderer* self, RDAddress address);
REDASM_EXPORT void rdrenderer_constant(RDRenderer* self, usize value, int base);
