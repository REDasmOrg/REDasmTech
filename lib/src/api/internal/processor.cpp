#include "processor.h"
#include "../../context.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void emulator_addref(RDEmulator* self, RDAddress toaddr, usize type) {
    spdlog::trace("emulator_addref({}, {:x}, {})", fmt::ptr(self), toaddr,
                  type);

    state::context->address_to_index(toaddr).map(
        [&](MIndex idx) { api::from_c(self)->add_ref(idx, type); });
}

void emulator_flow(RDEmulator* self, RDAddress flowaddr) {
    spdlog::trace("emulator_flow({}, {:x})", fmt::ptr(self), flowaddr);

    state::context->address_to_index(flowaddr).map(
        [&](MIndex idx) { api::from_c(self)->flow(idx); });
}

u64 emulator_getreg(const RDEmulator* self, int regid) {
    spdlog::trace("emulator_getreg({}, {})", fmt::ptr(self), regid);
    return api::from_c(self)->get_reg(regid);
}

void emulator_setreg(RDEmulator* self, int regid, u64 val) {
    spdlog::trace("emulator_setreg({}, {}, {:x})", fmt::ptr(self), regid, val);
    api::from_c(self)->set_reg(regid, val);
}

u64 emulator_updreg(RDEmulator* self, int regid, u64 val, u64 mask) {
    spdlog::trace("emulator_updreg({}, {}, {:x}, {:x})", fmt::ptr(self), regid,
                  val, mask);
    return api::from_c(self)->upd_reg(regid, val, mask);
}

u64 emulator_getstate(const RDEmulator* self, std::string_view state) {
    spdlog::trace("emulator_getstate({}, '{}')", fmt::ptr(self), state);
    return api::from_c(self)->get_state(state);
}

u64 emulator_takestate(RDEmulator* self, std::string_view state) {
    spdlog::trace("emulator_takestate({}, '{}')", fmt::ptr(self), state);
    return api::from_c(self)->take_state(state);
}

void emulator_delstate(RDEmulator* self, std::string_view state) {
    spdlog::trace("emulator_delstate({}, '{}')", fmt::ptr(self), state);
    api::from_c(self)->del_state(state);
}

void emulator_setstate(RDEmulator* self, const std::string& state, u64 val) {
    spdlog::trace("emulator_setstate({}, '{}', {:x})", fmt::ptr(self), state,
                  val);
    api::from_c(self)->set_state(state, val);
}

u64 emulator_updstate(RDEmulator* self, std::string_view state, u64 val,
                      u64 mask) {
    spdlog::trace("emulator_updstate({}, '{}', {:x}, {:x})", fmt::ptr(self),
                  state, val, mask);
    return api::from_c(self)->upd_state(state, val, mask);
}

const RDProcessor* get_processor() {
    spdlog::trace("get_processor()");

    if(state::context)
        return state::context->processor;

    return nullptr;
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

    if(!processor.address_size) {
        state::error(fmt::format(
            "register_processor: invalid address-size for processor '{}'",
            processor.id));

        return;
    }

    if(!processor.integer_size) {
        state::error(fmt::format(
            "register_processor: invalid integer-size for processor '{}'",
            processor.id));

        return;
    }

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
