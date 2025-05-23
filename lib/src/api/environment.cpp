#include "../context.h"
#include "../state.h"
#include <cstring>
#include <redasm/environment.h>
#include <spdlog/spdlog.h>

bool rdenvironment_init(RDEnvironment* self, const char* name) {
    spdlog::trace("rdenvironment_init({}, '{}')", fmt::ptr(self), name);
    if(!self) return false;

    self->name = name;
    self->update_instruction = [](RDInstruction*) {};
    self->get_operand_symbol = [](const RDInstruction*, usize) -> const char* {
        return nullptr;
    };

    return true;
}

const RDCallingConvention* rd_getcallingconvention() {
    spdlog::trace("rd_getcallingconvention()");
    redasm::Context* ctx = redasm::state::context;

    if(!ctx || !ctx->loaderplugin || !ctx->processorplugin ||
       !ctx->loaderplugin->get_callingconvention ||
       !ctx->processorplugin->get_callingconventions)
        return nullptr;

    const char* ldrcc = ctx->loaderplugin->get_callingconvention(ctx->loader);
    const RDCallingConvention** cc =
        ctx->processorplugin->get_callingconventions(ctx->processor);

    if(ldrcc && cc) {
        while(*cc) {
            if(!std::strcmp(ldrcc, (*cc)->name)) return *cc;
            cc++;
        }
    }

    return nullptr;
}

const RDEnvironment* rd_getenvironment() {
    spdlog::trace("rd_getenvironment()");

    static RDEnvironment dummy;
    redasm::Context* ctx = redasm::state::context;

    if(!ctx || !ctx->loaderplugin || !ctx->loaderplugin->get_environment) {
        if(!dummy.name) rdenvironment_init(&dummy, nullptr);
        return &dummy;
    }

    return ctx->loaderplugin->get_environment(ctx->loader);
}
