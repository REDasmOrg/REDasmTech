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

void rdrenderer_reg(RDRenderer* self, const char* s) {
    if(s)
        redasm::api::internal::renderer_reg(self, s);
    else
        spdlog::error("rdrenderer_register: invalid string");
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
