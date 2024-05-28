#pragma once

#include <redasm/processor.h>
#include <string_view>

namespace redasm::api::internal {

RDAddress emulateresult_getaddress(const RDEmulateResult* self);
void emulateresult_setsize(RDEmulateResult* self, usize s);
void emulateresult_addreturn(RDEmulateResult* self);
void emulateresult_addcall(RDEmulateResult* self, RDAddress address);
void emulateresult_addcallunresolved(RDEmulateResult* self);
void emulateresult_addbranch(RDEmulateResult* self, RDAddress address);
void emulateresult_addbranchtrue(RDEmulateResult* self, RDAddress address);
void emulateresult_addbranchfalse(RDEmulateResult* self, RDAddress address);
void emulateresult_addbranchunresolved(RDEmulateResult* self);

usize get_processors(const RDProcessor** processors);
void register_processor(const RDProcessor& processor);
void set_processor(std::string_view name);

} // namespace redasm::api::internal
