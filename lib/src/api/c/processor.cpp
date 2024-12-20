#include "../internal/processor.h"
#include "../../error.h"

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
    if(state)
        return redasm::api::internal::emulator_getstate(self, state);

    except("rdemulator_getstate(): 'state' argument is null");
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

const RDProcessor* rd_getprocessor(void) {
    return redasm::api::internal::get_processor();
}

usize rd_getprocessors(const RDProcessor** processors) {
    return redasm::api::internal::get_processors(processors);
}

void rd_registerprocessor(const RDProcessor* proc) {
    if(proc)
        redasm::api::internal::register_processor(*proc);
}

void rd_setprocessor(const char* id) {
    if(id)
        redasm::api::internal::set_processor(id);
}
