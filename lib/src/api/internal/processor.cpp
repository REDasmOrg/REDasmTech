#include "processor.h"
#include "../../context.h"
#include "../../disasm/emulator.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void emulator_addref(RDEmulator* self, RDAddress address, usize cr) {
    spdlog::trace("emulator_addref({}, {:x}, {})", fmt::ptr(self), address, cr);

    state::context->address_to_index(address).map(
        [&](MIndex idx) { api::from_c(self)->add_ref(idx, cr); });
}

void emulator_settype(RDEmulator* self, RDAddress address,
                      std::string_view tname) {
    spdlog::trace("emulator_settype({}, {:x}, '{}')", fmt::ptr(self), address,
                  tname);

    state::context->address_to_index(address).map(
        [&](MIndex idx) { api::from_c(self)->set_type(idx, tname); });
}

usize get_processors(const RDProcessor** processors) {
    spdlog::trace("get_processors({})", fmt::ptr(processors));

    if(processors)
        *processors = state::processors.data();

    return state::processors.size();
}

void register_processor(const RDProcessor& processor) {
    spdlog::trace("register_processor('{}')", processor.name);

    if(processor.name)
        state::processors.push_back(processor);
    else
        spdlog::error("register_processor: invalid name");
}

void set_processor(std::string_view name) {
    spdlog::trace("set_processor('{}')", name);

    if(!state::context)
        return;

    for(const RDProcessor& p : state::processors) {
        if(p.name == name) {
            state::context->processor = &p;
            return;
        }
    }

    spdlog::error("set_processor: '{}' not found", name);
}

} // namespace redasm::api::internal
