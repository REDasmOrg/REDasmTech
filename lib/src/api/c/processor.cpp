#include "../internal/processor.h"

void rdemulator_addref(RDEmulator* e, RDAddress toaddr, usize type) {
    redasm::api::internal::emulator_addref(e, toaddr, type);
}

void rdemulator_settype(RDEmulator* e, RDAddress addr, const RDType* type) {
    redasm::api::internal::emulator_settype(e, addr, type);
}

void rdemulator_flow(RDEmulator* e, RDAddress flowaddr) {
    redasm::api::internal::emulator_flow(e, flowaddr);
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
