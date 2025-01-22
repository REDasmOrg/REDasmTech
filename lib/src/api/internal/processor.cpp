#include "processor.h"
#include "../../context.h"
#include "../../plugins/pluginmanager.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

u32 emulator_getdslotinfo(const RDEmulator* self, const RDInstruction** dslot) {
    const Emulator* e = api::from_c(self);
    if(dslot && e->ndslot) *dslot = e->dslotinstr.get();
    return e->ndslot;
}

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

bool register_processor(const RDProcessorPlugin* plugin, pm::Origin o) {
    spdlog::trace("register_processor({}, {})", fmt::ptr(plugin), o);
    return pm::register_processor(plugin, o);
}

const RDProcessorPlugin** get_processorplugins(usize* n) {
    spdlog::trace("get_processorplugins({})", fmt::ptr(n));
    return pm::get_processors(n);
}

const RDProcessorPlugin* get_processorplugin() {
    spdlog::trace("get_processorplugin()");
    if(state::context) return state::context->processorplugin;
    return nullptr;
}

const RDProcessor* get_processor() {
    spdlog::trace("get_processor()");
    if(state::context) return state::context->processor;
    return nullptr;
}

bool decode(RDAddress address, RDInstruction* instr) {
    spdlog::trace("decode({:x}, {})", address, fmt::ptr(instr));

    const Context* ctx = state::context;
    if(!instr || (ctx && !ctx->is_address(address))) return false;

    *instr = {
        .address = address,
    };

    ctx->processorplugin->decode(ctx->processor, instr);
    return instr->length > 0;
}

} // namespace redasm::api::internal
