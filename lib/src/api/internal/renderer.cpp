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

void renderer_mnemonic(RDRenderer* self, std::string_view s, RDThemeKind kind) {
    spdlog::trace("renderer_mnemonic({}, '{}', {})", fmt::ptr(self), s,
                  static_cast<int>(kind));

    api::from_c(self)->chunk(s, kind).ws();
}

void renderer_register(RDRenderer* self, std::string_view s) {
    spdlog::trace("renderer_register({}, '{}')", fmt::ptr(self), s);
    api::from_c(self)->chunk(s, THEME_REG);
}

void renderer_text(RDRenderer* self, std::string_view s) {
    spdlog::trace("renderer_text({}, '{}')", fmt::ptr(self), s);
    api::from_c(self)->chunk(s);
}

void renderer_reference(RDRenderer* self, RDAddress address) {
    spdlog::trace("renderer_reference({}, '{}')", fmt::ptr(self), address);
    api::from_c(self)->ref(address);
}

} // namespace redasm::api::internal
