#include "../context.h"
#include "../state.h"
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
