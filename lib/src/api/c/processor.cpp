#include "../internal/processor.h"

void rdemulator_addref(RDEmulator* e, RDAddress toaddr, usize type) {
    redasm::api::internal::emulator_addref(e, toaddr, type);
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
