#include "../internal/processor.h"
#include "../../error.h"

u32 rdemulator_getdslotinfo(const RDEmulator* self,
                            const RDInstruction** dslot) {
    return redasm::api::internal::emulator_getdslotinfo(self, dslot);
}

void rdemulator_flow(RDEmulator* self, RDAddress flowaddr) {
    redasm::api::internal::emulator_flow(self, flowaddr);
}

void rdemulator_addref(RDEmulator* self, RDAddress toaddr, usize type) {
    redasm::api::internal::emulator_addref(self, toaddr, type);
}

u64 rdemulator_getreg(const RDEmulator* self, int regid) {
    return redasm::api::internal::emulator_getreg(self, regid);
}

void rdemulator_setreg(RDEmulator* self, int regid, u64 val) {
    redasm::api::internal::emulator_setreg(self, regid, val);
}

u64 rdemulator_updreg(RDEmulator* self, int regid, u64 val, u64 mask) {
    return redasm::api::internal::emulator_updreg(self, regid, val, mask);
}

u64 rdemulator_getstate(const RDEmulator* self, const char* state) {
    if(state) return redasm::api::internal::emulator_getstate(self, state);

    except("rdemulator_getstate(): 'state' argument is null");
}

u64 rdemulator_takestate(RDEmulator* self, const char* state) {
    if(state) return redasm::api::internal::emulator_takestate(self, state);

    except("rdemulator_takestate(): 'state' argument is null");
}

void rdemulator_delstate(RDEmulator* self, const char* state) {
    if(state)
        redasm::api::internal::emulator_delstate(self, state);
    else
        except("rdemulator_delstate(): 'state' argument is null");
}

void rdemulator_setstate(RDEmulator* self, const char* state, u64 val) {
    if(state)
        redasm::api::internal::emulator_setstate(self, state, val);
    else
        except("rdemulator_setstate(): 'state' argument is null");
}

u64 rdemulator_updstate(RDEmulator* self, const char* state, u64 val,
                        u64 mask) {
    if(state)
        return redasm::api::internal::emulator_updstate(self, state, val, mask);

    except("rdemulator_setstate(): 'state' argument is null");
}

bool rd_registerprocessor(const RDProcessorPlugin* plugin) {
    return redasm::api::internal::register_processor(plugin);
}

const RDProcessorPlugin** rd_getprocessorplugins(usize* n) {
    return redasm::api::internal::get_processorplugins(n);
}

const RDProcessorPlugin* rd_getprocessorplugin() {
    return redasm::api::internal::get_processorplugin();
}

const RDProcessor* rd_getprocessor(void) {
    return redasm::api::internal::get_processor();
}

bool rd_setprocessor(const char* id) {
    if(id) return redasm::api::internal::set_processor(id);
    return false;
}

bool rd_decode(RDAddress address, RDInstruction* instr) {
    return redasm::api::internal::decode(address, instr);
}
