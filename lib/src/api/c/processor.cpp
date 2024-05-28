#include "../internal/processor.h"
#include <spdlog/spdlog.h>

RDAddress rdemulateresult_getaddress(const RDEmulateResult* self) {
    return redasm::api::internal::emulateresult_getaddress(self);
}

void rdemulateresult_setsize(RDEmulateResult* self, usize s) {
    redasm::api::internal::emulateresult_setsize(self, s);
}

void rdemulateresult_addreturn(RDEmulateResult* self) {
    redasm::api::internal::emulateresult_addreturn(self);
}

void rdemulateresult_addcall(RDEmulateResult* self, RDAddress address) {
    redasm::api::internal::emulateresult_addcall(self, address);
}

void rdemulateresult_addcallunresolved(RDEmulateResult* self) {
    redasm::api::internal::emulateresult_addcallunresolved(self);
}

void rdemulateresult_addbranch(RDEmulateResult* self, RDAddress address) {
    redasm::api::internal::emulateresult_addbranch(self, address);
}

void rdemulateresult_addbranchtrue(RDEmulateResult* self, RDAddress address) {
    redasm::api::internal::emulateresult_addbranchtrue(self, address);
}

void rdemulateresult_addbranchfalse(RDEmulateResult* self, RDAddress address) {
    redasm::api::internal::emulateresult_addbranchfalse(self, address);
}

void rdemulateresult_addbranchunresolved(RDEmulateResult* self) {
    redasm::api::internal::emulateresult_addbranchunresolved(self);
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
