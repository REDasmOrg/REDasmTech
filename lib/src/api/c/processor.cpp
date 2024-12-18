#include "../internal/processor.h"

void rdemulator_addref(RDEmulator* e, RDAddress toaddr, usize type) {
    redasm::api::internal::emulator_addref(e, toaddr, type);
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
