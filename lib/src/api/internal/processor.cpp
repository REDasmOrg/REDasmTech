#include "processor.h"
#include "../../context.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void emulator_addref(RDEmulator* e, RDAddress toaddr, usize type) {
    spdlog::trace("emulator_addref({}, {:x}, {})", fmt::ptr(e), toaddr, type);

    state::context->address_to_index(toaddr).map([&](MIndex idx) {
        state::context->disassembler.emulator.add_ref(idx, type);
    });
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
