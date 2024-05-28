#include "processor.h"
#include "../../context.h"
#include "../../disasm/emulateresult.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDAddress emulateresult_getaddress(const RDEmulateResult* self) {
    spdlog::trace("emulateresult_getaddress({})", fmt::ptr(self));
    return api::from_c(self)->address;
}

void emulateresult_setsize(RDEmulateResult* self, usize s) {
    spdlog::trace("emulateresult_setsize({}, {})", fmt::ptr(self), s);
    api::from_c(self)->size = s;
}

void emulateresult_addreturn(RDEmulateResult* self) {
    spdlog::trace("emulateresult_addreturn({})", fmt::ptr(self));
    api::from_c(self)->add_return();
}

void emulateresult_addcall(RDEmulateResult* self, RDAddress address) {
    spdlog::trace("emulateresult_addcall({}, {:x})", fmt::ptr(self), address);
    api::from_c(self)->add_call(address);
}

void emulateresult_addcallunresolved(RDEmulateResult* self) {
    spdlog::trace("emulateresult_addcallunresolved({})", fmt::ptr(self));
    api::from_c(self)->add_call_unresolved();
}

void emulateresult_addbranch(RDEmulateResult* self, RDAddress address) {
    spdlog::trace("emulateresult_addbranch({}, {:x})", fmt::ptr(self), address);
    api::from_c(self)->add_branch(address);
}

void emulateresult_addbranchtrue(RDEmulateResult* self, RDAddress address) {
    spdlog::trace("emulateresult_addbranchtrue({}, {:x})", fmt::ptr(self),
                  address);
    api::from_c(self)->add_branch_true(address);
}

void emulateresult_addbranchfalse(RDEmulateResult* self, RDAddress address) {
    spdlog::trace("emulateresult_addbranchfalse({}, {:x})", fmt::ptr(self),
                  address);
    api::from_c(self)->add_branch_false(address);
}

void emulateresult_addbranchunresolved(RDEmulateResult* self) {
    spdlog::trace("emulateresult_addbranchunresolved({})", fmt::ptr(self));
    api::from_c(self)->add_branch_unresolved();
}

usize get_processors(const RDProcessor** processors) {
    spdlog::trace("get_processors('{}')", fmt::ptr(processors));

    if(processors)
        *processors = state::processors.data();

    return state::processors.size();
}

void register_processor(const RDProcessor& processor) {
    spdlog::trace("register_processor('{}')", processor.name);

    if(processor.name)
        state::processors.push_back(processor);
    else
        spdlog::error("register_processor: invalid name");
}

void set_processor(std::string_view name) {
    spdlog::trace("set_processor('{}')", name);

    if(!state::context)
        return;

    for(const RDProcessor& p : state::processors) {
        if(p.name == name) {
            state::context->processor = &p;
            return;
        }
    }

    spdlog::error("set_processor: '{}' not found", name);
}

} // namespace redasm::api::internal
