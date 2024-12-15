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

bool emulator_settype(RDEmulator* e, RDAddress address, const RDType* type) {
    spdlog::trace("emulator_settype({}, {:x}, {})", fmt::ptr(e), address,
                  fmt::ptr(type));

    if(type) {
        return state::context->address_to_index(address).map_or(
            [&](MIndex idx) { return api::from_c(e)->set_type(idx, *type); },
            false);
    }

    return false;
}

usize get_processors(const RDProcessor** processors) {
    spdlog::trace("get_processors({})", fmt::ptr(processors));

    if(processors)
        *processors = state::processors.data();

    return state::processors.size();
}

void register_processor(const RDProcessor& processor) {
    spdlog::trace("register_processor('{}', '{}')", processor.id,
                  processor.name);

    if(processor.id)
        state::processors.push_back(processor);
    else
        spdlog::error("register_processor: invalid id");
}

void set_processor(std::string_view id) {
    spdlog::trace("set_processor('{}')", id);

    if(!state::context)
        return;

    assume(!state::processors.empty());

    for(const RDProcessor& p : state::processors) {
        if(p.id == id) {
            state::context->processor = &p;
            return;
        }
    }

    // Select 'null' processor;
    state::context->processor = &state::processors.front();
    spdlog::error("set_processor: '{}' not found", id);
}

} // namespace redasm::api::internal
