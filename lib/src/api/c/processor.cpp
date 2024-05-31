#include "../internal/processor.h"

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
