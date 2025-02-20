#include "../context.h"
#include "../error.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include "marshal.h"
#include <spdlog/spdlog.h>

const RDSegment* rdemulator_getsegment(const RDEmulator* self) {
    spdlog::trace("rdemulator_getsegment({})", fmt::ptr(self));
    return redasm::api::from_c(self)->segment;
}

u32 rdemulator_getdslotinfo(const RDEmulator* self,
                            const RDInstruction** dslot) {
    spdlog::trace("rdemulator_getdslotinfo({}, {})", fmt::ptr(self),
                  fmt::ptr(dslot));
    const redasm::Emulator* e = redasm::api::from_c(self);
    if(dslot && e->ndslot) *dslot = e->dslotinstr.get();
    return e->ndslot;
}

void rdemulator_addregchange(RDEmulator* self, RDAddress addr, int reg,
                             u64 val) {
    spdlog::trace("rdemulator_addregchange({}, {}, {}, {})", fmt::ptr(self),
                  addr, reg, val);
    redasm::api::from_c(self)->add_regchange(addr, reg, val);
}

void rdemulator_flow(RDEmulator* self, RDAddress flowaddr) {
    spdlog::trace("rdemulator_flow({}, {:x})", fmt::ptr(self), flowaddr);
    if(redasm::state::context) redasm::api::from_c(self)->flow(flowaddr);
}

void rdemulator_addref(RDEmulator* self, RDAddress toaddr, usize type) {
    spdlog::trace("rdemulator_addref({}, {:x}, {})", fmt::ptr(self), toaddr,
                  type);

    if(redasm::state::context) redasm::api::from_c(self)->add_ref(toaddr, type);
}

u64 rdemulator_getreg(const RDEmulator* self, int regid) {
    spdlog::trace("rdemulator_getreg({}, {})", fmt::ptr(self), regid);
    return redasm::api::from_c(self)->get_reg(regid);
}

void rdemulator_setreg(RDEmulator* self, int regid, u64 val) {
    spdlog::trace("rdemulator_setreg({}, {}, {:x})", fmt::ptr(self), regid,
                  val);
    redasm::api::from_c(self)->set_reg(regid, val);
}

u64 rdemulator_updreg(RDEmulator* self, int regid, u64 val, u64 mask) {
    spdlog::trace("rd_emulator_updreg({}, {}, {:x}, {:x})", fmt::ptr(self),
                  regid, val, mask);
    return redasm::api::from_c(self)->upd_reg(regid, val, mask);
}

u64 rdemulator_getstate(const RDEmulator* self, const char* state) {
    spdlog::trace("rdemulator_getstate({}, '{}')", fmt::ptr(self), state);
    if(state) return redasm::api::from_c(self)->get_state(state);
    except("rdemulator_getstate(): 'state' argument is null");
}

u64 rdemulator_takestate(RDEmulator* self, const char* state) {
    spdlog::trace("rdemulator_takestate({}, '{}')", fmt::ptr(self), state);
    if(state) return redasm::api::from_c(self)->take_state(state);
    except("rdemulator_takestate(): 'state' argument is null");
}

void rdemulator_delstate(RDEmulator* self, const char* state) {
    spdlog::trace("rdemulator_delstate({}, '{}')", fmt::ptr(self), state);
    if(state)
        redasm::api::from_c(self)->del_state(state);
    else
        except("rdemulator_delstate(): 'state' argument is null");
}

void rdemulator_setstate(RDEmulator* self, const char* state, u64 val) {
    spdlog::trace("rdemulator_setstate({}, '{}', {:x})", fmt::ptr(self), state,
                  val);
    if(state)
        redasm::api::from_c(self)->set_state(state, val);
    else
        except("rdemulator_setstate(): 'state' argument is null");
}

u64 rdemulator_updstate(RDEmulator* self, const char* state, u64 val,
                        u64 mask) {
    spdlog::trace("rdemulator_updstate({}, '{}', {:x}, {:x})", fmt::ptr(self),
                  state, val, mask);
    if(state) return redasm::api::from_c(self)->upd_state(state, val, mask);
    except("rdemulator_setstate(): 'state' argument is null");
}

bool rd_registerprocessor(const RDProcessorPlugin* plugin) {
    spdlog::trace("rd_registerprocessor({})", fmt::ptr(plugin));
    return redasm::pm::register_processor(plugin, redasm::pm::NATIVE);
}

bool rd_registerprocessor_ex(const RDProcessorPlugin* plugin,
                             const char* origin) {
    spdlog::trace("rd_registerprocessor_ex({}, '{}')", fmt::ptr(plugin),
                  origin);
    return redasm::pm::register_processor(plugin, origin);
}

Vect(const RDProcessorPlugin*) rd_getprocessorplugins() {
    spdlog::trace("rd_getprocessorplugins()");
    return redasm::pm::processors;
}

const RDProcessorPlugin* rd_getprocessorplugin() {
    spdlog::trace("rd_getprocessorplugin()");
    if(redasm::state::context) return redasm::state::context->processorplugin;
    return nullptr;
}

const RDProcessor* rd_getprocessor() {
    spdlog::trace("rd_getprocessor()");
    if(redasm::state::context) return redasm::state::context->processor;
    return nullptr;
}

bool rd_decode(RDAddress address, RDInstruction* instr) {
    spdlog::trace("rd_decode({:x}, {})", address, fmt::ptr(instr));

    const redasm::Context* ctx = redasm::state::context;
    if(!ctx || !instr || !rd_isaddress(address)) return false;

    *instr = {
        .address = address,
    };

    ctx->processorplugin->decode(ctx->processor, instr);
    return instr->length > 0;
}

const char* rd_getregistername(int regid) {
    spdlog::trace("rd_getregistername({})", regid);
    static std::string res;

    const redasm::Context* ctx = redasm::state::context;
    const char* reg = nullptr;

    if(ctx && ctx->processorplugin && ctx->processorplugin->get_registername)
        reg = ctx->processorplugin->get_registername(ctx->processor, regid);

    if(reg) return reg;

    res = "$" + std::to_string(regid);
    return res.c_str();
}
