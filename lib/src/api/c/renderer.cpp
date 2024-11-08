#include "../internal/renderer.h"
#include <redasm/renderer.h>
#include <spdlog/spdlog.h>

void rdrenderer_themed(RDRenderer* self, const char* s, RDThemeKind kind) {
    if(s)
        redasm::api::internal::renderer_themed(self, s, kind);
    else
        spdlog::error("rdrenderer_themed: invalid string");
}

void rdrenderer_mnemonic(RDRenderer* self, const char* s, RDThemeKind kind) {
    if(s)
        redasm::api::internal::renderer_mnemonic(self, s, kind);
    else
        spdlog::error("rdrenderer_mnemonic: invalid string");
}

void rdrenderer_register(RDRenderer* self, const char* s) {
    if(s)
        redasm::api::internal::renderer_register(self, s);
    else
        spdlog::error("rdrenderer_register: invalid string");
}

void rdrenderer_text(RDRenderer* self, const char* s) {
    if(s)
        redasm::api::internal::renderer_text(self, s);
    else
        spdlog::error("rdrenderer_text: invalid string");
}

void rdrenderer_reference(RDRenderer* self, RDAddress address) {
    redasm::api::internal::renderer_reference(self, address);
}

void rdrenderer_constant(RDRenderer* self, usize value, int base) {
    redasm::api::internal::renderer_constant(self, value, base);
}
