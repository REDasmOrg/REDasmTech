#include "../internal/processor.h"

void rdemulator_addref(RDEmulator* self, RDAddress address, usize cr) {
    redasm::api::internal::emulator_addref(self, address, cr);
}

void rdemulator_settype(RDEmulator* self, RDAddress address,
                        const char* tname) {
    if(tname)
        redasm::api::internal::emulator_settype(self, address, tname);
}

usize rd_getprocessors(const RDProcessor** processors) {
    return redasm::api::internal::get_processors(processors);
}

void rd_registerprocessor(const RDProcessor* proc) {
    if(proc)
        redasm::api::internal::register_processor(*proc);
}

void rd_setprocessor(const char* name) {
    if(name)
        redasm::api::internal::set_processor(name);
}
