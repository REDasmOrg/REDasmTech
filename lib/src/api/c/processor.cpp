#include "../internal/processor.h"

void rdemulator_addcoderef(RDEmulator* self, RDAddress address, usize cr) {
    redasm::api::internal::emulator_addcoderef(self, address, cr);
}
void rdemulator_adddataref(RDEmulator* self, RDAddress address, usize dr) {
    redasm::api::internal::emulator_adddataref(self, address, dr);
}

usize rd_getprocessors(const RDProcessor** processors) {
    return redasm::api::internal::get_processors(processors);
}

void rd_registerprocessor(const RDProcessor* proc) {
    if(proc)
        redasm::api::internal::register_processor(*proc);
}

void rd_selectprocessor(const char* name) {
    if(name)
        redasm::api::internal::set_processor(name);
}
